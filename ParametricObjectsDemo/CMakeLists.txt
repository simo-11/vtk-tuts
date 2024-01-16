
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(ParametricObjectsDemo)

find_package(VTK COMPONENTS 
  CommonColor
  CommonComputationalGeometry
  CommonCore
  CommonDataModel
  FiltersCore
  FiltersSources
  IOImage
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  cli11
  fmt
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "ParametricObjectsDemo: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(ParametricObjectsDemo MACOSX_BUNDLE ParametricObjectsDemo.cxx )
  target_link_libraries(ParametricObjectsDemo PRIVATE ${VTK_LIBRARIES}
)
# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS ParametricObjectsDemo
  MODULES ${VTK_LIBRARIES}
)
