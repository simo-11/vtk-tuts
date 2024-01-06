# ImGui Integration

## links

 * https://github.com/ocornut/imgui/wiki/Bindings
   * https://github.com/jspanchu/vtkDearImGUIInjector
 * https://discourse.vtk.org/t/vtk-dearimgui-integration-through-vtkgenericopenglrenderwindow/4424/2

## integrate vtkDearImGUIInjector using clone

 * https://github.com/simo-11/vtkDearImGUIInjector
    * CMakeLists.txt ON -> OFF
       * option(USE_ADOBE_SPECTRUM_STYLE "Style ImGui with Adobe spectrum look" OFF)
   * modify  src/CMakeLists.txt to use
       * set(IMGUI_DIR /users/simon/github/imgui) 
 * https://github.com/simo-11/imgui

Start visual studio x64 Native tools command prompt 
```
C:\Users\simon\github\vtkDearImGUIInjector>cmake "-DVTK_DIR=/users/simon/build/vtk"  -S . -B build
C:\Users\simon\github\vtkDearImGUIInjector>cmake --build build
C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(205,13): error C2065:
'ImGuiKey_KeyPadEnter': undeclared identifier [C:\Users\simon\github\vtkDearImGUIInjector\build\src\vtkDearImGuiInjector.vcxproj]
$ grep ImGuiKey_KeyPadEnter *.h
imgui.h:    //ImGuiKey_KeyPadEnter = ImGuiKey_KeypadEnter,              // Renamed in 1.87

:\Users\simon\github\vtkDearImGUIInjector\sample\OverlayUI.cxx(49,52): error C2065:
'Karla_Regular_compressed_data_base85': undeclared identifier [C:\Users\simon\github\vtkDearImGUIInjector\build\sample\main.vcxproj]
Move include two lines earlier so that include is done without spectrum version in use
C:\Users\simon\github\vtkDearImGUIInjector\build\sample\Debug\main.exe
```
idle sample takes about 16 % of GPU which causes constant fan noise.
added sleeps in mainLoopCallback which lowered gpu usage to 2-5 % but fan is still active

Check out unneeded calls based on profiling data
|Function Name|Total CPU \[unit, %\]|Self CPU \[unit, %\]|Module|
|-|-|-|-|
|\| - vtkFreeTypeTools::RenderCharacter|346 \(10,81 %\)|276 \(8,62 %\)|vtkrenderingfreetype-9.3d|
|\| - vtkAOSDataArrayTemplate\<unsigned char\>::GetTuple|239 \(7,46 %\)|190 \(5,93 %\)|vtkcommoncore-9.3d|
|\| - \[External Call\]gdi32full.dll0x00007fff4dc8d338|161 \(5,03 %\)|161 \(5,03 %\)|gdi32full|
|\| - \[External Call\]ucrtbased.dll0x00007ffe7b6d0dee|121 \(3,78 %\)|121 \(3,78 %\)|ucrtbased|
|\| - vtkFreeTypeTools::RenderBackground|157 \(4,90 %\)|94 \(2,94 %\)|vtkrenderingfreetype-9.3d|
|\| - vtkTexture::IsTranslucent|611 \(19,08 %\)|88 \(2,75 %\)|vtkrenderingcore-9.3d|
|\| - \[External Call\]msvcp140d.dll0x00007ffee31edcd6|76 \(2,37 %\)|76 \(2,37 %\)|msvcp140d|
|\| - \[External Call\]msvcp140d.dll0x00007ffee31edbb5|74 \(2,31 %\)|74 \(2,31 %\)|msvcp140d|
|\| - gray\_set\_cell|64 \(2,00 %\)|64 \(2,00 %\)|vtkfreetype-9.3d|
|\| - gray\_render\_line|126 \(3,94 %\)|63 \(1,97 %\)|vtkfreetype-9.3d|
|\| - \[External Call\]ucrtbased.dll0x00007ffe7b6d0335|58 \(1,81 %\)|58 \(1,81 %\)|ucrtbased|
|\| - vtkAbstractArray::GetNumberOfTuples|55 \(1,72 %\)|54 \(1,69 %\)|vtkcommoncore-9.3d|
|\| - \[External Call\]nvoglv64.dll0x00007ffe566d26d0|51 \(1,59 %\)|51 \(1,59 %\)|nvoglv64|

Window Size 1920x1000, GPU memory size is reflected by window size.

|camManipulator| imGui | multiSamples | GPU %|CPU %|Committed GPU memory|
|-|-|-|-|-|-|
|  off | on |8| 2-3 | 0 | 251| 
|  on | on |8| 2-3 | 0 | 257|
|  on | on |0| 5 | 0 | 145|

tkDearImGuiInjector::PumpEvents makes a busy loop
```
main.exe!vtkDearImGuiInjector::BeginDearImGuiOverlay(vtkObject * caller, unsigned long eid, void * callData) Line 242
	at C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(242)
main.exe!vtkObject::vtkClassMemberCallback<vtkDearImGuiInjector>::operator()(vtkObject * caller, unsigned long event, void * calldata) Line 382
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.h(382)
vtkCommonCore-9.3d.dll!vtkObjectCommandInternal::Execute(vtkObject * caller, unsigned long eventId, void * callData) Line 944
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(944)
vtkCommonCore-9.3d.dll!vtkSubjectHelper::InvokeEvent(unsigned long event, void * callData, vtkObject * self) Line 641
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(641)
vtkCommonCore-9.3d.dll!vtkObject::InvokeEvent(unsigned long event, void * callData) Line 807
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(807)
vtkRenderingCore-9.3d.dll!vtkRenderWindow::Render() Line 289
	at C:\Users\simon\github\vtk\Rendering\Core\vtkRenderWindow.cxx(289)
vtkRenderingOpenGL2-9.3d.dll!vtkOpenGLRenderWindow::Render() Line 2771
	at C:\Users\simon\github\vtk\Rendering\OpenGL2\vtkOpenGLRenderWindow.cxx(2771)
main.exe!`anonymous namespace'::mainLoopCallback(void * arg) Line 536
	at C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(536)
main.exe!vtkDearImGuiInjector::PumpEvents(vtkObject * caller, unsigned long eid, void * callData) Line 565
	at C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(565)
main.exe!vtkObject::vtkClassMemberCallback<vtkDearImGuiInjector>::operator()(vtkObject * caller, unsigned long event, void * calldata) Line 382
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.h(382)
vtkCommonCore-9.3d.dll!vtkObjectCommandInternal::Execute(vtkObject * caller, unsigned long eventId, void * callData) Line 944
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(944)
vtkCommonCore-9.3d.dll!vtkSubjectHelper::InvokeEvent(unsigned long event, void * callData, vtkObject * self) Line 641
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(641)
vtkCommonCore-9.3d.dll!vtkObject::InvokeEvent(unsigned long event, void * callData) Line 807
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(807)
vtkRenderingCore-9.3d.dll!vtkRenderWindowInteractor::Start() Line 199
	at C:\Users\simon\github\vtk\Rendering\Core\vtkRenderWindowInteractor.cxx(199)
main.exe!main(int argc, char * * argv) Line 89
	at C:\Users\simon\github\vtkDearImGUIInjector\sample\main.cxx(89)
```

Events are handled like this
```
main.exe!vtkDearImGuiInjector::InterceptEvent(vtkObject * caller, unsigned long eid, void * clientData, void * callData) Line 572
	at C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(572)
vtkCommonCore-9.3d.dll!vtkCallbackCommand::Execute(vtkObject * caller, unsigned long event, void * callData) Line 31
	at C:\Users\simon\github\vtk\Common\Core\vtkCallbackCommand.cxx(31)
vtkCommonCore-9.3d.dll!vtkSubjectHelper::InvokeEvent(unsigned long event, void * callData, vtkObject * self) Line 641
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(641)
vtkCommonCore-9.3d.dll!vtkObject::InvokeEvent(unsigned long event, void * callData) Line 807
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(807)
vtkRenderingCore-9.3d.dll!vtkInteractorStyle::ProcessEvents(vtkObject * __formal, unsigned long event, void * clientdata, void * calldata) Line 1083
	at C:\Users\simon\github\vtk\Rendering\Core\vtkInteractorStyle.cxx(1083)
vtkCommonCore-9.3d.dll!vtkCallbackCommand::Execute(vtkObject * caller, unsigned long event, void * callData) Line 31
	at C:\Users\simon\github\vtk\Common\Core\vtkCallbackCommand.cxx(31)
vtkCommonCore-9.3d.dll!vtkSubjectHelper::InvokeEvent(unsigned long event, void * callData, vtkObject * self) Line 641
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(641)
vtkCommonCore-9.3d.dll!vtkObject::InvokeEvent(unsigned long event, void * callData) Line 807
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(807)
vtkRenderingUI-9.3d.dll!vtkWin32RenderWindowInteractor::OnMouseMove(HWND__ * hWnd, unsigned int nFlags, int X, int Y) Line 487
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(487)
vtkRenderingUI-9.3d.dll!vtkHandleMessage2(HWND__ * hWnd, unsigned int uMsg, unsigned __int64 wParam, __int64 lParam, vtkWin32RenderWindowInteractor * me) Line 958
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(958)
vtkRenderingUI-9.3d.dll!vtkHandleMessage(HWND__ * hWnd, unsigned int uMsg, unsigned __int64 wParam, __int64 lParam) Line 879
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(879)
[External Code]
vtkRenderingUI-9.3d.dll!vtkWin32RenderWindowInteractor::ProcessEvents() Line 260
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(260)
main.exe!`anonymous namespace'::mainLoopCallback(void * arg) Line 533
	at C:\Users\simon\github\vtkDearImGUIInjector\src\vtkDearImGuiInjector.cxx(533)
```

Added condition to call render only if event-processing takes at least 1 ms to vtkDearImGuiInjector.cxx#mainLoopCallback.
GPU usage drops to less than 1 % if there are no events.
