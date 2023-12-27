cmake_minimum_required(VERSION 3.28)
file(GLOB MKL_INC_DIR c:/users/simon/nuget/intelmkl.devel*/build/*/include)
file(GLOB MKL_LIB_DIR c:/users/simon/nuget/intelmkl.devel*/run*/*/native)
message(INFO "MKL_LIB_DIR=${MKL_LIB_DIR}")

project(MathExample)

find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "MathExample: Unable to find the VTK build folder.")
endif()


# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(MathExample MACOSX_BUNDLE MathExample.cxx LapackSgesv.cxx)
  target_link_libraries(MathExample PRIVATE 
  ${VTK_LIBRARIES}
)
target_include_directories(MathExample PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${MKL_INC_DIR}>
  $<BUILD_INTERFACE:${IMGUI_DIR}/backends>
  $<INSTALL_INTERFACE:include>
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS MathExample
  MODULES ${VTK_LIBRARIES}
)
