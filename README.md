# vtk-tuts
Getting familiar with vtk

# build 
https://docs.vtk.org/en/latest/build_instructions/index.html
but use own directories and comment out line causing configure error
```
simon@MSI MINGW64 ~/github/vtk/ThirdParty/hdf5/vtkhdf5/config/cmake (master)
$ git diff ConfigureChecks.cmake
diff --git a/ThirdParty/hdf5/vtkhdf5/config/cmake/ConfigureChecks.cmake b/ThirdParty/hdf5/vtkhdf5/config/cmake/ConfigureChecks.cmake
index 1ae31d86a6..b954316585 100644
--- a/ThirdParty/hdf5/vtkhdf5/config/cmake/ConfigureChecks.cmake
+++ b/ThirdParty/hdf5/vtkhdf5/config/cmake/ConfigureChecks.cmake
@@ -353,7 +353,7 @@ C_RUN ("maximum decimal precision for C" ${PROG_SRC} PROG_RES PROG_OUTPUT4)
 # dnl  -- long double decimal precision  --  __float128 decimal precision

 list (GET PROG_OUTPUT4 0 H5_LDBL_DIG)
-list (GET PROG_OUTPUT4 1 H5_FLT128_DIG)
+#list (GET PROG_OUTPUT4 1 H5_FLT128_DIG)
 endif ()
```
Build Debug configuration for Debugging
```
>cmake-gui -GNinja -S %HOMEPATH%\github\vtk -B %HOMEPATH%\build\vtk
```
 * build took about hour and results use 12 GB, bin 2 GB
```
>cmake --build %HOMEPATH%\build\vtk --config Debug
```

# prepare
add the VTK bin directory to your path. This will resolve the VTK dll's at run time.
C:\Users\simon\build\vtk\bin\Debug

# Building examples

## CylinderExample
https://examples.vtk.org/site/Cxx/GeometricObjects/CylinderExample

 * build configs
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> cmake "-DVTK_DIR:PATH=/Users/simon/build/vtk" ..
```
 * build exe
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> cmake --build .
```
 * run - end with q or e keys or through window manager.
   * see vtkRenderingCore-9.3d.dll!vtkInteractorStyle::OnChar() Line 936
	at C:\Users\simon\github\vtk\Rendering\Core\vtkInteractorStyle.cxx(936) for default char handling
     * m - stop or start animate - no meaning in this example
     * f - fly to picked point - no meaning in this example
     * u - userCallback - no meaning in this example
     * r - reset camera
     * w - wireframe
     * s - surface
     * 3 - stereo - error if not suitable window
     * p - pick - no meaning in this example
```
C:\Users\simon\github\vtk-tuts\CylinderExample\build\Debug\CylinderExample.exe
```
 * start visual studio project
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> start CylinderExample.sln
```
 * After starting
     * set Example as startup project
     * start debugging

Having window open does not cause GPU usage
```
CylinderExample.exe!`anonymous namespace'::tutCallback::Execute(vtkObject * caller, unsigned long __formal, void * __formal) Line 30
	at C:\Users\simon\github\vtk-tuts\CylinderExample\CylinderExample.cxx(30)
vtkCommonCore-9.3d.dll!vtkSubjectHelper::InvokeEvent(unsigned long event, void * callData, vtkObject * self) Line 641
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(641)
vtkCommonCore-9.3d.dll!vtkObject::InvokeEvent(unsigned long event, void * callData) Line 807
	at C:\Users\simon\github\vtk\Common\Core\vtkObject.cxx(807)
vtkRenderingUI-9.3d.dll!vtkWin32RenderWindowInteractor::OnMouseMove(HWND__ * hWnd, unsigned int nFlags, int X, int Y) Line 477
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(477)
vtkRenderingUI-9.3d.dll!vtkHandleMessage2(HWND__ * hWnd, unsigned int uMsg, unsigned __int64 wParam, __int64 lParam, vtkWin32RenderWindowInteractor * me) Line 958
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(958)
vtkRenderingUI-9.3d.dll!vtkHandleMessage(HWND__ * hWnd, unsigned int uMsg, unsigned __int64 wParam, __int64 lParam) Line 879
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(879)
[External Code]
vtkRenderingUI-9.3d.dll!vtkWin32RenderWindowInteractor::StartEventLoop() Line 279
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(279)
```


# Tasks

## How to select Display Adapter

```
vtkRenderingOpenGL2-9.3d.dll!vtkWin32OpenGLRenderWindow::WindowInitialize() Line 1024
	at C:\Users\simon\github\vtk\Rendering\OpenGL2\vtkWin32OpenGLRenderWindow.cxx(1024)
vtkRenderingOpenGL2-9.3d.dll!vtkWin32OpenGLRenderWindow::Initialize() Line 1042
vtkRenderingOpenGL2-9.3d.dll!vtkOpenGLRenderWindow::Start() Line 1639
	at C:\Users\simon\github\vtk\Rendering\OpenGL2\vtkOpenGLRenderWindow.cxx(1639)
vtkRenderingUI-9.3d.dll!vtkWin32RenderWindowInteractor::Initialize() Line 303
	at C:\Users\simon\github\vtk\Rendering\UI\vtkWin32RenderWindowInteractor.cxx(303)
vtkRenderingCore-9.3d.dll!vtkRenderWindow::Render() Line 296
	at C:\Users\simon\github\vtk\Rendering\Core\vtkRenderWindow.cxx(296)
vtkRenderingOpenGL2-9.3d.dll!vtkOpenGLRenderWindow::Render() Line 2771
	at C:\Users\simon\github\vtk\Rendering\OpenGL2\vtkOpenGLRenderWindow.cxx(2771)
CylinderExample.exe!main(int __formal, char * * __formal) Line 67
	at C:\Users\simon\github\vtk-tuts\CylinderExample\CylinderExample.cxx(67)
```
![image](https://github.com/simo-11/vtk-tuts/assets/1210784/643e0390-723e-45a8-bddb-8ef7df33916a)

Let Windows decide or Power saving -> UHD
![image](https://github.com/simo-11/vtk-tuts/assets/1210784/dc3907a3-3901-4c9f-90f4-e595888a8d46)

High performance Nvidia RTX
![image](https://github.com/simo-11/vtk-tuts/assets/1210784/8fb2c062-dbe3-4efe-a213-59ec18d021c7)



## Try ImGui Integration

### links

 * https://github.com/ocornut/imgui/wiki/Bindings
   * https://github.com/jspanchu/vtkDearImGUIInjector
 * https://discourse.vtk.org/t/vtk-dearimgui-integration-through-vtkgenericopenglrenderwindow/4424/2

### integrate vtkDearImGUIInjector using clone

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

## Integrate (high performance) math libraries

### mkl

 * https://www.intel.com/content/www/us/en/docs/onemkl/developer-guide-windows/2023-1/overview.html
 * https://www.intel.com/content/www/us/en/docs/onemkl/code-samples-lapack/2023-1/overview.html

```
nuget install intelmkl.devel.win-x64
> dir C:\Users\simon\nuget dir C:\Users\simon\.nuget\packages\
C:\Users\simon\nuget> dir intelmkl.devel.win-x64.2024.0.0.49657\build\native\include\
C:\Users\simon\nuget> dir intelmkl.devel.win-x64.2024.0.0.49657\runtimes\win-x64\native\
```
#### MathExample
Calls lapacke Sgesv on mouse move. 

Tunings
 * CMakeLists.txt tuning
   * make target hot reloadble as default - https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.html

TODO
 * CMakeLists.txt tuning
   * target_include_directories in proper way, now is hardcoded
 * optionally call netlib version   

```
'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\mkl_intel_thread.2.dll'. Module was built without symbols.

'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\mkl_core.2.dll'. Module was built without symbols.
'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\libimalloc.dll'. Module was built without symbols.
'MathExample.exe' (Win32): Unloaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\libimalloc.dll'
'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\libimalloc.dll'. Module was built without symbols.
'MathExample.exe' (Win32): Unloaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\libimalloc.dll'
'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\mkl_avx2.2.dll'. Module was built without symbols.
'MathExample.exe' (Win32): Loaded 'C:\Program Files (x86)\Intel\oneAPI\mkl\2024.0\bin\mkl_vml_avx2.2.dll'. Module was built without symbols.
```
https://learn.microsoft.com/en-us/cpp/build/reference/dumpbin-reference?view=msvc-170
```
PS C:\Program Files (x86)\Intel\oneAPI\mkl\latest\bin> dumpbin /exports .\mkl_intel_thread.2.dll  | findstr sgesv
       1956  7A3 01EEAE70 mkl_lapack_sgesv

PS C:\Program Files (x86)\Intel\oneAPI\mkl\latest\bin> dumpbin /exports mkl_core.2.dll  | findstr sgesv
       2417  970 007B1280 mkl_lapack_dsgesv
```

### Cuda
 * https://docs.nvidia.com/cuda/cusolver/index.html
```
Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.3 $ ls bin/cusolver64_11.dll include/*cusolver*
bin/cusolver64_11.dll*  include/cusolverMg.h  include/cusolverSp.h                   include/cusolver_common.h
include/cusolverDn.h    include/cusolverRf.h  include/cusolverSp_LOWLEVEL_PREVIEW.h
```
  
### initial tests with lapacke
https://icl.utk.edu/lapack-for-windows/lapack/index.html#lapacke

```
C:\Users\simon\build\LAPACKE_examples\Release> dir
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a---          26/12/2023    21.30         439942 libblas.dll
-a---          26/12/2023    21.30         107520 libgcc_s_dw2-1.dll
-a---          26/12/2023    21.30         802318 libgfortran-3.dll
-a---          26/12/2023    21.30        5983228 liblapack.dll
-a---          26/12/2023    21.30        2104615 liblapacke.dll
C:\Users\simon\build\LAPACKE_examples\Release> C:\Users\simon\build\LAPACKE_examples\Debug\example_DGESV_rowmajor.exe

C:\Users\simon\build\LAPACKE_examples\include> dir
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a---          26/12/2023    21.30           4235 lapacke_config.h
-a---          26/12/2023    21.30            474 lapacke_mangling.h
-a---          26/12/2023    21.30          32375 lapacke_utils.h
-a---          26/12/2023    21.30        1061210 lapacke.h
C:\Users\simon\build\LAPACKE_examples\lib> dir
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a---          26/12/2023    21.30         439942 libblas.dll
-a---          26/12/2023    21.30          25800 libblas.lib
-a---          26/12/2023    21.30        5983228 liblapack.dll
-a---          26/12/2023    21.30         261136 liblapack.lib
-a---          26/12/2023    21.30        2104615 liblapacke.dll
-a---          26/12/2023    21.30         457926 liblapacke.lib
```


