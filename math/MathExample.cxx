#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
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
extern int LapackSgesv(bool printSolution);
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
                auto interactor = reinterpret_cast<vtkRenderWindowInteractor*>(caller);
                auto renderWindow = interactor->GetRenderWindow();
                auto glWindow = static_cast
                    <vtkWin32OpenGLRenderWindow*>(renderWindow);
                if (glWindow->SupportsOpenGL()) {
                    glWindow->ReportCapabilities();
                    const char* glRenderer = (const char*)glGetString(GL_RENDERER);
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
        static mathCallback* New()
        {
            return new mathCallback;
        }
        void Execute(vtkObject* caller, unsigned long, void*) override
        {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            LapackSgesv(callCount>0);
            callCount++;
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            long elapsedUs = std::chrono::duration_cast
                <std::chrono::microseconds>(end - start).count();
            std::clog << "Call #"<<callCount<< "LapackSgesv took "
                << elapsedUs << "us" << std::endl;

        }
        mathCallback() = default;
    };

} // namespace

int main(int, char*[])
{
  vtkNew<vtkNamedColors> colors;

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkNew<vtkCylinderSource> cylinder;
  int faceCount = 100;
  cylinder->SetResolution(faceCount);

  // The mapper is responsible for pushing the geometry into the graphics
  // library. It may also do color mapping, if scalars or other attributes are
  // defined.
  vtkNew<vtkPolyDataMapper> cylinderMapper;
  cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

  // The actor is a grouping mechanism: besides the geometry (mapper), it
  // also has a property, transformation matrix, and/or texture map.
  // Here we set its color and rotate it around the X and Y axes.
  vtkNew<vtkActor> cylinderActor;
  cylinderActor->SetMapper(cylinderMapper);
  cylinderActor->GetProperty()->SetColor(
      colors->GetColor4d("Tomato").GetData());
  cylinderActor->RotateX(30.0);
  cylinderActor->RotateY(-45.0);

  // The renderer generates the image
  // which is then displayed on the render window.
  // It can be thought of as a scene to which the actor is added
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(cylinderActor);
  renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.5);

  // The render window is the actual GUI window
  // that appears on the computer screen
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Cylinder");

  // The render window interactor captures mouse events
  // and will perform appropriate camera or actor manipulation
  // depending on the nature of the events.
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  unsigned long event=vtkCommand::EnterEvent;
  vtkNew<tutCallback> cb;
  renderWindowInteractor->AddObserver(event, cb);
  vtkNew<mathCallback> mathCb;
  renderWindowInteractor->AddObserver
  (vtkCommand::MouseMoveEvent, mathCb);

  // This starts the event loop and as a side effect causes an initial render.
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
