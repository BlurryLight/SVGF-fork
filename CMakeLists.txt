CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(gpuPathTracing CXX CUDA C)

# ========== DEBUG INFO ==========
message(STATUS "========== Project Configuration Start ==========")
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "========== End Debug Info ==========")
# ================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD  17)
set(CUDA_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(API_GL 0)
set(API_CU 1)
set(API ${API_CU})

message(STATUS "[DEBUG] API Configuration: API_GL=${API_GL}, API_CU=${API_CU}, USE_API=${API}")

add_definitions(-DAPI_GL=${API_GL})
add_definitions(-DAPI_CU=${API_CU})
add_definitions(-DAPI=${API})

set(USE_OPTIX 1)
message(STATUS "[DEBUG] USE_OPTIX=${USE_OPTIX}")
add_definitions(-DUSE_OPTIX=${USE_OPTIX})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)


set(IMGUI_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/backends ${CMAKE_CURRENT_SOURCE_DIR}/submodules/ImGuizmo/)
set(IMGUI_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/imgui_demo.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/backends/imgui_impl_glfw.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/imgui.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/imgui_draw.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/imgui_widgets.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/imgui_tables.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/dearimgui/backends/imgui_impl_opengl3.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/submodules/ImGuizmo/ImGuizmo.cpp
)
include_directories(${IMGUI_INC_DIR})

# GLM
set(GLM_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glm)
include_directories(${GLM_INC_DIR})

# TinyGLTF
set(TINYGLTF_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinygltf)
include_directories(${TINYGLTF_INC_DIR})

# STB
set(STB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/stb)
include_directories(${STB_INC_DIR})

# FileDialog
set(FILEDIALOG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/include)
set(FILEDIALOG_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/nfd_common.c)
set(FILEDIALOG_SOURCE ${FILEDIALOG_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/nfd_win.cpp)
include_directories(${FILEDIALOG_INC_DIR})


# Glfw
set(GLFW_INSTALL FALSE)
add_subdirectory(submodules/glfw)
include_directories(submodules/glfw/include)

# # Glad
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/submodules/glad/include")
set(GLAD_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glad/src/glad.c)

# ASSIMP
set(ASSIMP_INSTALL_PDB OFF)
add_subdirectory(submodules/assimp)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/submodules/assimp/include")
include_directories(submodules/assimp/include)

# # OIDN
SET(OIDN_DEVICE_CPU OFF)
SET(OIDN_DEVICE_CUDA OFF)
# add_subdirectory(submodules/oidn)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CUDA_ARCHITECTURES "52;60;70;75")

# OptiX
message(STATUS "[DEBUG] Searching for OptiX...")
if(NOT DEFINED ENV{OptiX_DIR})
    message(FATAL_ERROR "NO OPTIX_DIR SET")
endif()
message(STATUS "[DEBUG] OptiX_DIR: $ENV{OptiX_DIR}")

if(NOT EXISTS "$ENV{OptiX_DIR}/include/optix.h")
    message(FATAL_ERROR "NO optix.h FOUND IN $ENV{OptiX_DIR}/include")
endif()

set(OptiX_ROOT_DIR $ENV{OptiX_DIR})
message(STATUS "[DEBUG] OptiX Header found successfully")

# Optix
set(OPTIX_DIR $ENV{OptiX_DIR}/include)
message(STATUS "[DEBUG] OPTIX_DIR: ${OPTIX_DIR}")

include(cmake/FindOptix.cmake)
include_directories(${OptiX_INCLUDE_DIR})
message(STATUS "[DEBUG] OptiX Include Dir: ${OptiX_INCLUDE_DIR}")

message(STATUS "[DEBUG] Searching for CUDA...")
find_package(CUDA REQUIRED)
enable_language(CUDA)
message(STATUS "[DEBUG] CUDA_VERSION: ${CUDA_VERSION}")
message(STATUS "[DEBUG] CUDA_TOOLKIT_ROOT_DIR: ${CUDA_TOOLKIT_ROOT_DIR}")
message(STATUS "[DEBUG] CUDA_LIBRARIES: ${CUDA_LIBRARIES}")
message(STATUS "[DEBUG] CUDA_NVCC_EXECUTABLE: ${CUDA_NVCC_EXECUTABLE}")

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -lineinfo --generate-line-info)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -diag-suppress 20012) # Suppress warning about host/device label on default constructor
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -diag-suppress 20013) # Suppress constexpr functoin
message(STATUS "[DEBUG] CUDA_NVCC_FLAGS: ${CUDA_NVCC_FLAGS}")
message(STATUS "[DEBUG] CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")

if(MSVC)
    message(STATUS "[DEBUG] MSVC Compiler detected")
    # utf8
    # ddisbale 4819
    add_compile_options("/utf-8")
    add_compile_options("/wd4819")
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -Xcompiler "/wd4819,/utf-8")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    message(STATUS "[DEBUG] Compiler: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "[DEBUG] Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()


set(CUDA_LIB_DIR "$ENV{CUDA_PATH}/lib/x64")
message(STATUS "[DEBUG] CUDA_LIB_DIR: ${CUDA_LIB_DIR}")
link_directories(${CUDA_LIB_DIR})

set (sourceFiles
        src/Main.cpp
        src/Window.cpp
        src/App.cu
        src/ShaderGL.cpp
        src/TextureGL.cpp
        src/Buffer.cpp
        src/CameraController.cpp
        src/GLTFLoader.cpp
        src/AssimpLoader.cpp
        src/AssetLoader.cpp
        src/ImageLoader.cpp
        src/TextureArrayGL.cpp
        src/TextureArrayCu.cu
        src/Framebuffer.cpp
        src/VertexBuffer.cpp
        src/Scene.cpp
        src/Tracing.cpp
        src/GUI.cpp
        src/BVH.cpp
        ${IMGUI_SOURCE}
        ${FILEDIALOG_SOURCE}
        ${GLAD_SOURCE}
        )
        
set(optixSource 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Raygen.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Miss.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Closesthit.cu
)

if(${USE_OPTIX} EQUAL 1)
foreach(source_file ${optixSource})
    get_filename_component(source_file_we ${source_file} NAME_WE)
    set(ptx_file "${CMAKE_CURRENT_SOURCE_DIR}/resources/ptx/${source_file_we}.ptx")
    add_custom_command(
        OUTPUT ${ptx_file}
        COMMAND ${CUDA_NVCC_EXECUTABLE}
        ARGS ${CUDA_NVCC_FLAGS} -ptx ${source_file} -o ${ptx_file} -I ${OPTIX_DIR} -I ${GLM_INC_DIR} -DUSE_OPTIX=${USE_OPTIX}
        # ARGS ${CUDA_NVCC_FLAGS} -ptx ${source_file} -o ${ptx_file} -I ${OPTIX_DIR} -I ${GLM_INC_DIR}
        DEPENDS ${source_file}
    )
    list(APPEND PTX_FILES ${ptx_file})
endforeach()
endif()


find_package(OpenGL REQUIRED)
message(STATUS "[DEBUG] OpenGL found: ${OPENGL_FOUND}")
message(STATUS "[DEBUG] OpenGL Library: ${OPENGL_LIBRARIES}")

message(STATUS "[DEBUG] Source files: ${sourceFiles}")
message(STATUS "[DEBUG] Building PathTracer executable...")
cuda_add_executable(PathTracer ${sourceFiles})


add_custom_target(GeneratePTXFiles ALL DEPENDS ${PTX_FILES})
add_dependencies(PathTracer GeneratePTXFiles)

message(STATUS "[DEBUG] Linking libraries: glfw, OpenGL, assimp, CUDA, OptiX")
target_link_libraries(PathTracer glfw OpenGL::GL assimp)
target_link_libraries(PathTracer ${CUDA_LIBRARIES} ${OptiX_LIBRARIES})
message(STATUS "[DEBUG] Libraries linked successfully")

set_property(TARGET PathTracer PROPERTY CXX_STANDARD 17)
set_property(TARGET PathTracer PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET PathTracer PROPERTY CUDA_STANDARD 17)
target_compile_definitions(PathTracer PRIVATE CMAKE_EXPORT_COMPILE_COMMANDS=1)

message(STATUS "========== Project Configuration Complete ==========")
message(STATUS "[DEBUG] Target: PathTracer")
message(STATUS "[DEBUG] Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "[DEBUG] Build Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "[DEBUG] Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========== Ready to Build ==========")

install(TARGETS PathTracer RUNTIME)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION bin/resources)