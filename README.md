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
>cmake --build %HOMEPATH%\build\vtk --config Debug
```

# prepare
add the VTK bin directory to your path. This will resolve the VTK dll's at run time.
C:\Users\simon\build\vtk\bin\Debug

# Building examples

## CylinderExample

 * build configs
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> cmake "-DVTK_DIR:PATH=/Users/simon/build/vtk" ..
```
 * build exe
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> cmake --build .
```
 * run - end qith q or e keys or through window manager
```
C:\Users\simon\github\vtk-tuts\CylinderExample\build\Debug\CylinderExample.exe
```
 * start visual studio project
```
PS C:\Users\simon\github\vtk-tuts\CylinderExample\build> start CylinderExample.sln
```
 ** set Example as startup project
 ** start debugging


# Tasks

## How to select Display Adapter

## Try ImGui Integration

## Integrate Cuda


