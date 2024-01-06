# MathExample

 * Initially Calls lapacke and cuda Sgesv on mouse move
 * uses col_major storage
 * add matrix size input using imgui (x)
 * visualize performance using imgui and vtk
 * environment setting for using mkl in oneAPI installation
```
   math [main ≡]> cmd.exe "/K" '"C:\Program Files (x86)\Intel\oneAPI\setvars.bat" && pwsh'
```
Tunings
 * CMakeLists.txt tuning
   * make target hot reloadble as default - https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_DEBUG_INFORMATION_FORMAT.html
     * does not work with cuda

