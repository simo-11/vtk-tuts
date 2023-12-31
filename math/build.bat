rem Use this batch file to build MathExample for Visual Studio
rmdir /s /q build
mkdir build
cd build
cmake --fresh "-DVTK_DIR:PATH=/Users/simon/build/vtk" ..
cmake --build .
rem start MathExample.sln
