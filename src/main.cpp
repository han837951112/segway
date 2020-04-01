#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "../lib/scene.h"
#include "../lib/global.h"

using std::string;
static void key_callback(GLFWwindow* window, int key, int scancode, int action,int mode);
static void mouse_callback(GLFWwindow* window,int button, int action,int mode);
static void cursor_callback(GLFWwindow* window,double x, double y);

Scene scene(SCR_WIDTH,SCR_HEIGHT);

int main(int argc, char **argv)
{
    string exePath = argv[0];
    string RootPath = exePath.substr(0,exePath.find_last_of('\\')) + "/../";
    std::cout << RootPath << '\n';

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "segway", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window,cursor_callback);
    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glViewport(0,0,SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    scene.Init();
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        scene.ProcessInput(deltaTime);

        scene.Update(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        scene.Render();
        // render
        // ------

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    Scene::Destroy();
    glfwTerminate();
    return 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            scene.Keys[key] = GL_TRUE;
        else if(action == GLFW_RELEASE)
            scene.Keys[key] = GL_FALSE;
    }
}

static void mouse_callback(GLFWwindow* window,int button, int action,int mode)
{
    double xd,yd;
    glfwGetCursorPos(window,&xd,&yd);
    b2Vec2 ps((float)xd,(float)yd);

    if(button == GLFW_MOUSE_BUTTON_1)
    {
        if(action == GLFW_PRESS)
        {
            if(mode != GLFW_MOD_SHIFT)
            {
                scene.MouseDown(ps);
            }
        }
        if(action == GLFW_RELEASE)
        {
            scene.MouseUp(ps);
        }
    }
}

static void cursor_callback(GLFWwindow*, double xd, double yd)
{
    b2Vec2 ps((float)xd, (float)yd);
    scene.MouseMove(ps);
}