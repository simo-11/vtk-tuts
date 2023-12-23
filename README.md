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

## Integrate Cuda


