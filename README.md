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

Start visual studio x64 Native tools command propmt 
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
added sleeps in mainLoopCallback which lowered gpu usage but fan is still active

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

## Integrate Cuda


