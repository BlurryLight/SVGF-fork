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

# Set C++ and CUDA standards - apply only to main project, not to subprojects
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(API_GL 0)
set(API_CU 1)
set(API ${API_CU})

message(STATUS "[DEBUG] API Configuration: API_GL=${API_GL}, API_CU=${API_CU}, USE_API=${API}")

# Use target_compile_definitions instead of add_definitions for project-specific definitions
set(PROJECT_DEFINES "-DAPI_GL=${API_GL}" "-DAPI_CU=${API_CU}" "-DAPI=${API}")

set(USE_OPTIX 0)
message(STATUS "[DEBUG] USE_OPTIX=${USE_OPTIX}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)

# ========== Configure ASSIMP (before other includes) ==========
message(STATUS "[DEBUG] Configuring ASSIMP...")
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build ASSIMP tests" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build ZLIB" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all exporters" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all importers" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "Build GLTF importer" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "Build OBJ importer" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "Build FBX importer" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build ASSIMP tools" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable all export functionality" FORCE)
message(STATUS "[DEBUG] ASSIMP options set successfully")

add_subdirectory(submodules/assimp)
message(STATUS "[DEBUG] ASSIMP subdirectory added")


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

# GLM
set(GLM_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glm)

# TinyGLTF
set(TINYGLTF_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinygltf)

# STB
set(STB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/stb)

# FileDialog
set(FILEDIALOG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/include)
set(FILEDIALOG_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/nfd_common.c)
set(FILEDIALOG_SOURCE ${FILEDIALOG_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fileDialog/src/nfd_win.cpp)


# Glfw
set(GLFW_INSTALL FALSE)
add_subdirectory(submodules/glfw)

# Glad
set(GLAD_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glad/src/glad.c)

# # OIDN
SET(OIDN_DEVICE_CPU OFF)
SET(OIDN_DEVICE_CUDA OFF)
# add_subdirectory(submodules/oidn)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CUDA_ARCHITECTURES "52;60;70;75")

# ========== OptiX Configuration ==========
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

set(OPTIX_DIR $ENV{OptiX_DIR}/include)
message(STATUS "[DEBUG] OPTIX_DIR: ${OPTIX_DIR}")

include(cmake/FindOptix.cmake)
message(STATUS "[DEBUG] OptiX Include Dir: ${OptiX_INCLUDE_DIR}")

# ========== CUDA Configuration ==========
message(STATUS "[DEBUG] Searching for CUDA...")
find_package(CUDAToolkit REQUIRED)
message(STATUS "[DEBUG] CUDA_VERSION: ${CUDAToolkit_VERSION}")
message(STATUS "[DEBUG] CUDA_TOOLKIT_ROOT: ${CUDAToolkit_ROOT}")
message(STATUS "[DEBUG] CUDA_NVCC_EXECUTABLE: ${CUDAToolkit_NVCC_EXECUTABLE}")

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -lineinfo --generate-line-info)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -diag-suppress 20012) # Suppress warning about host/device label on default constructor
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -diag-suppress 20013) # Suppress constexpr functoin
message(STATUS "[DEBUG] CUDA_NVCC_FLAGS: ${CUDA_NVCC_FLAGS}")
message(STATUS "[DEBUG] CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")

set(CUDA_LIB_DIR "$ENV{CUDA_PATH}/lib/x64")
message(STATUS "[DEBUG] CUDA_LIB_DIR: ${CUDA_LIB_DIR}")

# ========== Compiler-specific options ==========
# These will be applied only to the main target, not to subprojects
set(PATHTRACER_COMPILE_OPTS)
set(PATHTRACER_COMPILE_DEFS ${PROJECT_DEFINES} "USE_OPTIX=${USE_OPTIX}")

if(MSVC)
    message(STATUS "[DEBUG] MSVC Compiler detected")
    message(STATUS "[DEBUG] Compiler: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "[DEBUG] Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
    
    list(APPEND PATHTRACER_COMPILE_OPTS "/utf-8" "/wd4819")
    list(APPEND PATHTRACER_COMPILE_DEFS "_CRT_SECURE_NO_WARNINGS")
    
    # For CUDA compile with MSVC
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -Xcompiler "/wd4819,/utf-8")
endif()

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
        COMMAND ${CUDAToolkit_NVCC_EXECUTABLE}
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
add_executable(PathTracer ${sourceFiles})

# ========== Configure PathTracer Target ==========
# Add target-specific include directories
target_include_directories(PathTracer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IMGUI_INC_DIR}
    ${GLM_INC_DIR}
    ${TINYGLTF_INC_DIR}
    ${STB_INC_DIR}
    ${FILEDIALOG_INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glad/include
    ${CMAKE_CURRENT_BINARY_DIR}/submodules/assimp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/assimp/include
    ${OptiX_INCLUDE_DIR}
)

# Add target-specific compile options
target_compile_options(PathTracer PRIVATE ${PATHTRACER_COMPILE_OPTS})

# Add target-specific compile definitions
target_compile_definitions(PathTracer PRIVATE ${PATHTRACER_COMPILE_DEFS})

# Add target-specific link directories
target_link_directories(PathTracer PRIVATE ${CUDA_LIB_DIR})

add_custom_target(GeneratePTXFiles ALL DEPENDS ${PTX_FILES})
add_dependencies(PathTracer GeneratePTXFiles)

message(STATUS "[DEBUG] Linking libraries: glfw, OpenGL, assimp, CUDA, OptiX")
target_link_libraries(PathTracer PRIVATE glfw OpenGL::GL assimp CUDA::cudart ${OptiX_LIBRARIES})
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