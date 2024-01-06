cmake_minimum_required(VERSION 3.28)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")
set(NAME MathExample)
set(GROOT /users/simon/github)
project(${NAME} LANGUAGES C CXX CUDA)
set(IMGUI_DIR ${GROOT}/imgui)
set(VTK_IMGUI_DIR ${GROOT}/vtkDearImGUIInjector/src)
set(CTPL_DIR ${GROOT}/CTPL)
# sources and headers
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI_DIR}/*.h)
file(GLOB VTK_IMGUI_SOURCES ${VTK_IMGUI_DIR}/*.cxx)
add_compile_definitions(NO_VTK_IMGUI_EXPORT)
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
  message(FATAL_ERROR "${NAME}: Unable to find the VTK build folder.")
endif()

find_package(LAPACK)

if (NOT LAPACK_FOUND)
  message(FATAL_ERROR "${NAME}: Unable to find LAPACK_LIBRARIES")
endif()

find_package(MKL)
if (NOT MKL_FOUND)
  message(FATAL_ERROR "${NAME}: Unable to find MKL")
endif()

add_executable(${NAME} MACOSX_BUNDLE
${NAME}.cxx 
Math.cxx
LapackSolve.cxx
CudaSolve.cu
${IMGUI_SOURCES}
"${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
${VTK_IMGUI_SOURCES}
)
target_link_libraries(${NAME} PRIVATE
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

target_include_directories(${NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  ${MKL_INCLUDE}
  ${CUDA_INCLUDE_DIRS}
  ${IMGUI_DIR}
  "${IMGUI_DIR}/backends"
  ${VTK_IMGUI_DIR}  
  ${CTPL_DIR}
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS ${NAME}
  MODULES ${VTK_LIBRARIES}
)
