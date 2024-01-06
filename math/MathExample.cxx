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
#include "imgui.h"
#include <vtkInteractorObserver.h>
#include <vtkInteractorStyleSwitch.h>
namespace {
    static const char* windowTitle=nullptr;
    // Callback for the interaction.
    class tutCallback : public vtkCommand
    {
    public:
        static tutCallback* New()
        {
            return new tutCallback;
        }
        void Execute(vtkObject* caller, unsigned long, void*) override
        {
            if (windowTitle != nullptr) {
                return;
            }
            try {
                auto interactor = reinterpret_cast
                    <vtkRenderWindowInteractor*>(caller);
                auto renderWindow = interactor->GetRenderWindow();
                auto glWindow = static_cast
                    <vtkWin32OpenGLRenderWindow*>(renderWindow);
                if (glWindow->SupportsOpenGL()) {
                    glWindow->ReportCapabilities();
                    const char* glRenderer = (const char*)
                        glGetString(GL_RENDERER);
                    std::ostringstream strm;
                    strm << "Rectangle";
                    if (glRenderer)
                        strm << "@" << glRenderer;
                    std::string s = std::string{ strm.str() };
                    if(windowTitle!=nullptr){
                        free((void*)windowTitle);
                    }
                    windowTitle = _strdup(s.c_str());
                    renderWindow->SetWindowName(windowTitle);
                }
            }
            catch (...)
            {
                std::exception_ptr p = std::current_exception();
                std::clog << (p ? "got exception" : "null") << std::endl;
            }
        }
        tutCallback() = default;
    };
    static int callCount;
    class mathCallback : public vtkCommand
    {
    public:
        float* a = nullptr;
        float* b = nullptr;
        static mathCallback* New()
        {
            return new mathCallback;
        }
        void Execute(vtkObject* caller, unsigned long, void*) override
        {
            std::chrono::steady_clock::time_point start = 
                std::chrono::steady_clock::now();
            SolverImpl solverImpl = mkl;
            char* solverName="mkl";
            int n = callCount / 2 + 1;
            switch (callCount % 2) {
            case 0:
                if (a != nullptr) {
                    free(a);
                }
                a=getRandomA(n);
                if (b != nullptr) {
                    free(b);
                }
                b = getRandomB(n);
                solverImpl = mkl;
                solverName = "mkl";
                break;
            case 1:
                solverImpl = cuda;
                solverName = "cuda";
                break;
            }
            size_t size = n * sizeof(float);
            float* rhs = (float*)malloc(size);
            if (rhs != nullptr) {
                memcpy(rhs, b, size);
            }
            size *= n;
            float* lhs = (float*)malloc(size);
            if (lhs != nullptr) {
                memcpy(lhs, a, size);
            }
            if (rhs != nullptr && lhs!=nullptr) {
                
                MathSolve(n <= 10 ? 1 : 0, solverImpl, n, lhs, rhs);
            }
            if (lhs != nullptr) {
                free(lhs);
            }
            if (rhs != nullptr) {
                free(rhs);
            }
            callCount++;
            std::chrono::steady_clock::time_point end = 
                std::chrono::steady_clock::now();
            long elapsedUs = std::chrono::duration_cast
                <std::chrono::microseconds>(end - start).count();
            std::clog << "Call #"<<callCount<< 
                " to MathSolve("<<solverName<<") took "
                << elapsedUs << " us, n="<<n<< std::endl;

        }
        mathCallback() = default;
    };

} // namespace

int main(int, char*[])
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
  renderWindow->SetSize(800, 800);
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
