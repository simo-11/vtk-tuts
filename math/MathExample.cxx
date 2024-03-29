﻿#include "vtkDearImGuiInjector.h"

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
	/* */
	void solveTask(int id, SolverImpl solverImpl);
	void startWorkThreads();
	float* a = nullptr, * b = nullptr;
	std::atomic_long mklCount, cudaCount, vtkCount;
	std::atomic_long mklUs, cudaUs, vtkUs;
	ctpl::thread_pool threadPool;
	std::chrono::steady_clock::time_point wallStart, wallEnd;
	int matrixSize = 150, threadCount = 1, verbose = 0, sleepAfterSolve=0;
	int meErrorCode = 0;
	bool useMkl=true, useCuda=true, useVtk=true, running, stopRequested;
	enum MeErrorCodeBase { 
		LAPACK_ALLOCATE, CUDA_ALLOCATE, VTK_MATH_ALLOCATE
	};
	int getMeErrorCode(MeErrorCodeBase base, int local) {
		return base * 100 + local;
	}
	const char* getErrorSource() {
		switch (meErrorCode / 100) {
		case LAPACK_ALLOCATE: return "Lapack allocation";
		case CUDA_ALLOCATE: return "Cuda allocation";
		case VTK_MATH_ALLOCATE: return "VtkSolve allocation";
		}
		return "fix getErrorSource";
	}
	const char* getErrorReason() {
		switch (meErrorCode / 100) {
		case LAPACK_ALLOCATE: return get_lapack_error_reason(meErrorCode%100);
		case CUDA_ALLOCATE: return get_cuda_error_reason(meErrorCode % 100);
		case VTK_MATH_ALLOCATE: return get_vtk_error_reason(meErrorCode % 100);
		}
		return "fix getErrorReason";
	}
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
		wallStart=std::chrono::steady_clock::now();
		mklCount = 0;
		mklUs = 0;
		cudaCount = 0;
		cudaUs = 0;
		meErrorCode = 0;
		vtkCount = 0;
		vtkUs = 0;
		int methodCount = 0;
		if (useMkl) methodCount++;
		if (useCuda) methodCount++;
		if (useVtk) methodCount++;
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
			if (useVtk) {
				threadPool.push(solveTask, vtkMath);
			}
		}
		running = true;
	}
	void solveTask(int id, SolverImpl solverImpl) {
		size_t rhs_size = matrixSize * sizeof(float);
		Workspaces ws;
		ws.n = matrixSize;
		int errNo=0;
		switch (solverImpl) {
		case mkl:
			errNo = lapack_allocate(&ws);
			switch (errNo) {
			case 0:
				break;
			default: meErrorCode = getMeErrorCode(LAPACK_ALLOCATE, errNo);
				return;
			}
			break;
		case cuda:
			errNo = cuda_allocate(&ws);
			switch (errNo) {
			case 0:
				break;
			default: meErrorCode = getMeErrorCode(CUDA_ALLOCATE, errNo);
				return;
			}
			break;
		case vtkMath:
			errNo = vtk_allocate(&ws);
			switch (errNo) {
			case 0:
				break;
			default: meErrorCode = getMeErrorCode(VTK_MATH_ALLOCATE, errNo);
				return;
			}
			break;
		}
		float* rhs = (float*)malloc(rhs_size);
		size_t lhs_size = rhs_size*matrixSize;
		float* lhs = (float*)malloc(lhs_size);
		if (rhs != nullptr && lhs != nullptr) {
			while (!stopRequested && meErrorCode==0) {
				memcpy(lhs, a, lhs_size);
				memcpy(rhs, b, rhs_size);
				std::chrono::steady_clock::time_point start =
					std::chrono::steady_clock::now();
				MathSolve(verbose, solverImpl, matrixSize, lhs, rhs, &ws);
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
				case vtkMath:
					vtkUs += elapsedUs;
					vtkCount++;
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
		if (running) {
			wallEnd = std::chrono::steady_clock::now();
		}
		long wallUs = std::chrono::duration_cast
			<std::chrono::microseconds>(wallEnd - wallStart).count();
		float sps = (1e6*count)/((float)wallUs);
		ImGui::Text("%d solves, average=%d %s, %.4g sps", 
			count, average, unit, sps);
	}
	void addWallTotal() {
		long wt = mklCount + cudaCount + vtkCount;
		if (wt == 0) {
			return;
		}
		if (running) {
			wallEnd = std::chrono::steady_clock::now();
		}
		float wallS = 1e-6*std::chrono::duration_cast
			<std::chrono::microseconds>(wallEnd - wallStart).count();
		float sps = wt / wallS;
		ImGui::Text("Total: %d solves in %.3g s, %.4g sps",
			wt, wallS, sps);
	}
} // empty namespace

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
	ImGui::SetNextWindowPos(ImVec2(5, 25 + 10 + 250), ImGuiCond_Once);
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
		if (meErrorCode != 0) {
			stopRequested = true;
			ImGui::Text("Stop due to errorCode %d %s %s",
				meErrorCode, getErrorSource(), getErrorReason());
		}
		bool showAsRunning = running;
		if (showAsRunning) {
			bool buttonShown = false;
			if (stopRequested) {
				ImGui::BeginDisabled();
			}
			else {
				buttonShown = true;
				if (ImGui::Button("Stop")) {
					stopRequested = true;
				}
				ImGui::BeginDisabled();
			}
			if (stopRequested) {
				if (!buttonShown) {
					ImGui::Text("Stop");
				}
				ImGui::SameLine();
				ImGui::Text("requested, waiting tasks to complete");
			}
		}
		else {
			if (ImGui::Button("Start")) {
				restart();
			}
		}
		ImGui::SliderInt
			("Thread count", &threadCount, 1, 10,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderInt("Matrix size", &matrixSize, 1, 1024,
			nullptr, ImGuiSliderFlags_AlwaysClamp);
		ImGui::Checkbox("MKL", &useMkl);
		addStats(mklCount, mklUs);
		ImGui::Checkbox("CUDA", &useCuda);
		addStats(cudaCount, cudaUs);
		ImGui::Checkbox("VTK", &useVtk);
		addStats(vtkCount, vtkUs);
		if(showAsRunning) ImGui::EndDisabled();
		addWallTotal();
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
