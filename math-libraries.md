# Integrate (high performance) math libraries

## mkl

 * https://www.intel.com/content/www/us/en/docs/onemkl/developer-guide-windows/2023-1/overview.html
 * https://www.intel.com/content/www/us/en/docs/onemkl/code-samples-lapack/2023-1/overview.html
 * https://github.com/simo-11/oneapi-tuts/ own installation

## Cuda
 * https://docs.nvidia.com/cuda/cusolver/index.html
 * https://github.com/NVIDIA/CUDALibrarySamples
 * https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html
```
Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.3 $ ls bin/cusolver64_11.dll include/*cusolver*
bin/cusolver64_11.dll*  include/cusolverMg.h  include/cusolverSp.h                   include/cusolver_common.h
include/cusolverDn.h    include/cusolverRf.h  include/cusolverSp_LOWLEVEL_PREVIEW.h
```

```
C:\Users\simon\github> git clone https://github.com/NVIDIA/CUDALibrarySamples
CUDALibrarySamples\cuSOLVER\gesv\build [master ≡]> cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..
CUDALibrarySamples\cuSOLVER\gesv\build [master ≡]> cmake --build .
CUDALibrarySamples\cuSOLVER\gesv\build [master ≡]> C:\Users\simon\github\CUDALibrarySamples\cuSOLVER\gesv\build\Debug\cusolver_irs_expert.exe
...
Workspace is 12609152 bytes
Solving matrix on device...
Solve info is: 0, iter is: 2
Solved matrix 1024x1024 with 1 right hand sides in 73.6532ms
CUDALibrarySamples\cuSOLVER\gesv\build [master ≡]> start cusolver_examples.sln
```


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

  
##  lapacke
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
