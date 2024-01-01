cmake_minimum_required(VERSION 3.28)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
project(MathExample LANGUAGES C CXX CUDA)

# Global CUDA CXX flags/options
set(CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_EXTENSIONS OFF)

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

find_package(LAPACK)

if (NOT LAPACK_FOUND)
  message(FATAL_ERROR "MathExample: Unable to find LAPACK_LIBRARIES")
endif()

find_package(MKL)
if (NOT MKL_FOUND)
  message(FATAL_ERROR "MathExample: Unable to find MKL")
endif()

add_executable(MathExample MACOSX_BUNDLE 
MathExample.cxx 
Math.cxx
LapackSolve.cxx
CudaSolve.cu
)
target_link_libraries(MathExample PRIVATE 
  ${VTK_LIBRARIES}
  ${LAPACK_LIBRARIES}
    PUBLIC
        cusolver
        cublas
        cublasLt
        cusparse
        cusolverMg
)
set_target_properties(${EXAMPLE_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(MathExample PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  ${MKL_INCLUDE}
  ${CUDA_INCLUDE_DIRS}
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS MathExample
  MODULES ${VTK_LIBRARIES}
)
