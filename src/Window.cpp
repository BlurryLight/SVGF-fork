#include "Window.h"
#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>


void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
 GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    const char* src = "UNKNOWN";
    const char* tp = "UNKNOWN";
    const char* sev = "UNKNOWN";

    switch (source) {
    case GL_DEBUG_SOURCE_API: src = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src = "WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: src = "SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: src = "THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION: src = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER: src = "OTHER"; break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR: tp = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tp = "DEPRECATED"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tp = "UNDEFINED"; break;
    case GL_DEBUG_TYPE_PORTABILITY: tp = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: tp = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER: tp = "MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: tp = "PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP: tp = "POP_GROUP"; break;
    case GL_DEBUG_TYPE_OTHER: tp = "OTHER"; break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: sev = "HIGH"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: sev = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW: sev = "LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: sev = "NOTIFY"; break;
    }

    if ((type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        || (severity == GL_DEBUG_SEVERITY_HIGH))
    {
        std::printf("[GL DEBUG][%s][%s][%s][id=%u] %s\n", src, tp, sev, id, message);
    }
}

namespace gpupt
{

window::window(uint32_t Width, uint32_t Height)
{
    this->Width = Width;
    this->Height = Height;
    
    if(glfwInit() != GLFW_TRUE)
    {
        std::cout << "Error initializing GLFW" << std::endl;
        exit(0);
    }

    GLFWmonitor* PrimaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* VideoMode = glfwGetVideoMode(PrimaryMonitor);
    if(this->Width == 0 || this->Height == 0)
    {
        this->Width = VideoMode->width;
        this->Height = VideoMode->height;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwWindowHint(GLFW_DECORATED, true);
    glfwWindowHint(GLFW_RESIZABLE, true);

#if API==API_GL
    this->Handle = glfwCreateWindow(this->Width, this->Height, "GPU Path Tracing - OpenGL", nullptr, nullptr);
#elif API==API_CU
    this->Handle = glfwCreateWindow(this->Width, this->Height, "GPU Path Tracing - Cuda", nullptr, nullptr);
#endif 

    if(this->Handle == nullptr)
    {
        std::cout << "Error Creating Window" << std::endl;
        exit(0);
    }

    glfwMakeContextCurrent(this->Handle);
    glfwSwapInterval(0);

    glfwSetWindowSizeCallback(this->Handle, [](GLFWwindow *handle, int width, int height){
        auto &Window = *(window*)glfwGetWindowUserPointer(handle);
        if(Window.OnResize) Window.OnResize(Window, glm::ivec2(width, height));
    });    

    glfwSetWindowUserPointer(this->Handle, (void*)this);

    // 这个API必须要glad生成的时候带上loader
    int version = gladLoadGL(glfwGetProcAddress);
#if !defined(NDEBUG)
    gladInstallGLDebug();
#endif


    // int version = gladLoaderLoadGL((GLADloadproc) glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        exit(0);
    }

#if !defined(NDEBUG)
    if (GLAD_GL_VERSION_4_3 || GLAD_GL_KHR_debug)
    {
        GLint flags =0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        }
    }
#endif
}


bool window::ShouldClose() const
{
    return glfwWindowShouldClose(this->Handle);
}

void window::PollEvents() const
{
    glfwPollEvents();
}


void window::Present()
{
    glfwSwapBuffers(this->Handle);
}

window::~window()
{
    glfwDestroyWindow(Handle);
    glfwTerminate();
}

}