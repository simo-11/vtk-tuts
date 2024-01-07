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
   * make target hot reloadble as default - https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.html
     * does not work with cuda

# Resuls

## 7.1.2024

 * work spaces allocated for each solve for cuda
 * mkl does not need allocations during solves as col_major is used

![image](https://github.com/simo-11/vtk-tuts/assets/1210784/c0705750-2029-4ebf-8f8d-d5dd583148fe)
