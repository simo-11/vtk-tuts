# MathExample

 * calls lapacke and cuda Sgesv using thread pool [CTPL](https://github.com/vit-vit/CTPL)
 * matrix sizes and size of thread pools are controlled using imgui 
 * uses col_major storage
 * visualize performance using imgui and vtk
 * environment setting for using mkl in oneAPI installation
```
   math [main ≡]> cmd.exe "/K" '"C:\Program Files (x86)\Intel\oneAPI\setvars.bat" && pwsh'
```
Tunings
 * CMakeLists.txt tuning
   * make target hot reloadble (/ZI) as default - https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.html
     * does not affect cuda but cuda can be set using project properties/Configuration properties/CUDA C/C++/Host/Debug 

# Resuls

## 7.1.2024

 * work spaces are preallocated berofe starting solves
 * mkl does not need allocations during solves as col_major is used

![image](https://github.com/simo-11/vtk-tuts/assets/1210784/c0705750-2029-4ebf-8f8d-d5dd583148fe)


## 15.1.2024
For matrix size 150

![image](https://github.com/simo-11/vtk-tuts/assets/1210784/e9b84bbf-f76a-48a4-831e-aad1178aa1d8)

Method|thread-count | sps | Notes
---   |---| --- | ----
CUDA|1|1151|GPU fully loaded
MKL|1|3200| CPU 20 % load
MKL|4|5500| CPU 40 % load
MKL|8|4600| CPU 45 % load
VTK|1|350| CPU 8 % load
VTK|4|800| CPU 17 % load
VTK|8|1100| CPU 24 % load

