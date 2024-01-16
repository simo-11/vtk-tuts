# vtk-build 
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

## rebuilds

### enable cli11
C:\Users\simon\build\vtk\CMakeCache.txt change from DEFAULT to WANT

VTK_MODULE_ENABLE_VTK_cli11:STRING=WANT

Or as noted in https://examples.vtk.org/site/Cxx/GeometricObjects/ParametricObjectsDemo/
give as parameter while building
-DVTK_MODULE_ENABLE_VTK_cli11=WANT or
-DVTK_MODULE_ENABLE_VTK_fmt=WANT
-DVTK_BUILD_TESTING=ON

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
