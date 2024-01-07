#include "vtkDearImGuiInjector.h"

#include <vtkActor.h>
#include "vtkCallbackCommand.h"
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <windows.h>
#include <gl/GL.h>

#include <chrono>
#include <thread>
#include <string>
#include <array>
#include <sstream>
#include <vtkWin32OpenGLRenderWindow.h>
#include "MathExample.h"
#include "ctpl_stl.h"
#include "imgui.h"
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleSwitch.h>
namespace {
	void solveTask(int id, SolverImpl solverImpl);
	void startWorkThreads();
	float* a = nullptr, * b = nullptr;
	std::atomic_long mklCount, cudaCount;
	std::atomic_long mklUs, cudaUs;
	ctpl::thread_pool threadPool;
	int matrixSize = 1, threadCount = 0, verbose = 0, sleepAfterSolve=100;
	bool useMkl, useCuda, running, stopRequested;
	void restart() {
		if (a != nullptr) {
			free(a);
		}
		a = getRandomA(matrixSize);
		if (b != nullptr) {
			free(b);
		}
		b = getRandomB(matrixSize);
		startWorkThreads();
	};
	void startWorkThreads()
	{
		mklCount = 0;
		mklUs = 0;
		cudaCount = 0;
		cudaUs = 0;
		int methodCount = 0;
		if (useMkl) methodCount++;
		if (useCuda) methodCount++;
		int size = methodCount * threadCount;
		threadPool.resize(size);
		stopRequested = false;
		for (int i = 0; i < threadCount; i++) {
			if (useMkl) {
				threadPool.push(solveTask,mkl);
			}
			if (useCuda) {
				threadPool.push(solveTask, cuda);
			}
		}
		running = true;
	}
	void solveTask(int id, SolverImpl solverImpl) {
		size_t rhs_size = matrixSize * sizeof(float);
		float* rhs = (float*)malloc(rhs_size);
		size_t lhs_size = rhs_size*matrixSize;
		float* lhs = (float*)malloc(lhs_size);
		if (rhs != nullptr && lhs != nullptr) {
			while (!stopRequested) {
				memcpy(lhs, a, lhs_size);
				memcpy(rhs, b, rhs_size);
				std::chrono::steady_clock::time_point start =
					std::chrono::steady_clock::now();
				MathSolve(verbose, solverImpl, matrixSize, lhs, rhs);
				std::chrono::steady_clock::time_point end =
					std::chrono::steady_clock::now();
				long elapsedUs = std::chrono::duration_cast
					<std::chrono::microseconds>(end - start).count();
				switch (solverImpl) {
				case mkl:
					mklUs += elapsedUs;
					mklCount++;
					break;
				case cuda:
					cudaUs += elapsedUs;
					cudaCount++;
					break;
				}
				std::chrono::milliseconds duration(sleepAfterSolve);
				std::this_thread::sleep_for(duration);
			}
		}
		if (lhs != nullptr) {
			free(lhs);
		}
		if (rhs != nullptr) {
			free(rhs);
		}
	}
	void addStats(int count, long us) {
		if (count == 0) {
			return;
		}
		ImGui::SameLine();
		char* unit = "us";
		long average = us / count;
		if (average > 100000) {
			average /= 1000;
			unit = "ms";
		}
		ImGui::Text("%d solves, average=%d %s", count, average, unit);
	}
} // namespace

int main(int, char* [])
{
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkRenderWindowInteractor> iren;
	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->SetSize(300, 300);
	renderWindow->AddRenderer(renderer);
	renderWindow->SetWindowName("MathExample");
	iren->SetRenderWindow(renderWindow);
	renderWindow->Render();
	vtkNew<vtkDearImGuiInjector> dearImGuiOverlay;
	dearImGuiOverlay->DebugOff();
	dearImGuiOverlay->Inject(iren);
	vtkNew<vtkCallbackCommand> uiSetupCmd;
	uiSetupCmd->SetCallback(MathExampleUI::setup);
	dearImGuiOverlay->AddObserver(vtkDearImGuiInjector::ImGuiSetupEvent,
		uiSetupCmd);
	vtkNew<vtkCallbackCommand> uiDrawCmd;
	uiDrawCmd->SetCallback(MathExampleUI::draw);
	dearImGuiOverlay->AddObserver(vtkDearImGuiInjector::ImGuiDrawEvent,
		uiDrawCmd);
	// Start event loop
	renderWindow->SetSize(1200, 800);
	iren->EnableRenderOff();
	iren->Start();
	return EXIT_SUCCESS;
}

void MathExampleUI::setup(vtkObject* caller, unsigned long, void*,
	void* callData)
{
	vtkDearImGuiInjector* injector = reinterpret_cast
		<vtkDearImGuiInjector*>(caller);
	if (!callData)
	{
		return;
	}
	bool imguiInitStatus = *(reinterpret_cast<bool*>(callData));
	if (imguiInitStatus)
	{
	}
	else
	{
		vtkErrorWithObjectMacro(
			injector, R"(
Failed to setup overlay UI because 
ImGUI failed to initialize!)");
	}
}

namespace
{
	void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}

void MathExampleUI::draw(vtkObject* caller,
	unsigned long, void*, void* callData)
{
	vtkDearImGuiInjector* injector = reinterpret_cast
		<vtkDearImGuiInjector*>(caller);

	ImGui::SetNextWindowBgAlpha(0.5);
	// MathExample
	ImGui::SetNextWindowPos(ImVec2(5, 25 + 10 + 550), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 550), ImGuiCond_Once);
	ImGui::Begin("MathExample");
	if (ImGui::CollapsingHeader("threadPool",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (running) {
			if (threadPool.size() == threadPool.n_idle()) {
				running = false;
			}
		}
		bool showAsRunning = running;
		if (showAsRunning) {
			bool val = stopRequested;
			if (ImGui::Checkbox("Stop", &val)) {
				if (val) {
					stopRequested = true;
					ImGui::SameLine();
					ImGui::Text("requested, waiting tasks to complete");
				}
			}
			ImGui::BeginDisabled();
		}
		else {
			bool start=false;
			if (ImGui::Checkbox("Start", &start)) {
				if (start) {
					restart();
				}
			}
		}
		ImGui::SliderInt
			("Thread count", &threadCount, 0, 10,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderInt("Matrix size", &matrixSize, 1, 1024,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::Checkbox("MKL", &useMkl);
		addStats(mklCount, mklUs);
		ImGui::Checkbox("CUDA", &useCuda);
		addStats(cudaCount, cudaUs);
		if(showAsRunning) ImGui::EndDisabled();
		ImGui::SliderInt("verbosity", &verbose, 0, 5,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderInt("Sleep after solve [ms]", &sleepAfterSolve, 0, 1000,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
	}
	ImGui::End();
	// VTK
	ImGui::SetNextWindowPos(ImVec2(5, 25), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(450, 550), ImGuiCond_Once);
	ImGui::Begin("VTK");
	if (ImGui::CollapsingHeader("vtkRenderWindow",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto rw = injector->Interactor->GetRenderWindow();
		ImGui::Text("MTime: %ld", rw->GetMTime());
		ImGui::Text("Name: %s", rw->GetClassName());
		if (ImGui::TreeNode("Capabilities"))
		{
			ImGui::TextWrapped("OpenGL: %s", rw->ReportCapabilities());
			ImGui::TreePop();
		}
	}
	if (ImGui::CollapsingHeader("vtkRenderWindowInteractor",
		ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto& iren = injector->Interactor;
		ImGui::Text("MTime: %ld", iren->GetMTime());
		ImGui::Text("Name: %s", iren->GetClassName());
		if (ImGui::TreeNode("Style"))
		{
			auto styleBase = iren->GetInteractorStyle();
			vtkInteractorObserver* iStyle = nullptr;
			if (styleBase->IsA("vtkInteractorStyleSwitchBase"))
			{
				iStyle = vtkInteractorStyleSwitch::SafeDownCast
				(styleBase)->GetCurrentStyle();
			}
			else
			{
				iStyle = styleBase;
			}
			ImGui::Text("MTime: %ld", iStyle->GetMTime());
			ImGui::Text("Name: %s", iStyle->GetClassName());
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Mouse"))
		{
			int* xy = iren->GetEventPosition();
			ImGui::Text("X: %d", xy[0]);
			ImGui::Text("Y: %d", xy[1]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Keyboard"))
		{
			ImGui::Text("KeySym: %s", iren->GetKeySym());
			ImGui::SameLine();
			HelpMarker("VTK does not flush KeySym per frame.");
			ImGui::Text("KeyCode: %c", iren->GetKeyCode());
			ImGui::Text("Mods: %s %s %s", (iren->GetAltKey() ? "ALT" : " "),
				(iren->GetControlKey() ? "CTRL" : " "),
				(iren->GetShiftKey() ? "SHIFT" : " "));
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
