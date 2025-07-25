#include "Window.h"
#include <chrono>
#include <Cloth.h>

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";
double prevTime = 0;
double currTime = 0;

int fixpoints[5];
int currIndex = 0;
int dirHeld = 0;

glm::vec3 wind = glm::vec3(0);

// Objects to render
//Cube* Window::cube;
Cloth* Window::cloth;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    //cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

    cloth = new Cloth();
    cloth->InitSimpleCloth();
    cloth->Update(1.0/60.0);

    int dimx = cloth->GetDimensionX();
    int dimz = cloth->GetDimensionZ();

    fixpoints[0] = 0;
    fixpoints[1] = dimx + dimx / 2;
    fixpoints[2] = dimz / 2 * dimx + dimx / 2;
    fixpoints[3] = dimz / 3 * 2 * dimx + dimx / 3;
    fixpoints[4] = dimz / 4 * 3 * dimx + dimx / 3 * 2;


    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    //delete cube;

    // Delete the shader program.
    glDeleteProgram(shaderProgram);
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.

    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration<double, std::milli>(now.time_since_epoch()).count();

    prevTime = currTime;
    currTime = time;

    if (currTime != 0 && prevTime != 0) {
        double deltaTime = currTime - prevTime;
        cloth->Update(deltaTime);
    }

    switch (dirHeld) {
    case 1:
        cloth->MoveFixed(glm::vec3(0.0, 0.0, 2.0 * (currTime - prevTime)/1000.0));
        break;
    case 2:
        cloth->MoveFixed(glm::vec3(0.0, 0.0, -2.0 * (currTime - prevTime)/1000.0));
        break;
    case 3:
        cloth->MoveFixed(glm::vec3(2.0 * (currTime - prevTime)/1000.0, 0.0, 0.0));
        break;
    case 4:
        cloth->MoveFixed(glm::vec3(-2.0 * (currTime - prevTime)/1000.0, 0.0, 0.0));
        break;
    case 5:
        cloth->MoveFixed(glm::vec3(0.0, 2.0 * (currTime - prevTime)/1000.0, 0.0));
        break;
    case 6:
        cloth->MoveFixed(glm::vec3(0.0, -2.0 * (currTime - prevTime)/1000.0, 0.0));
        break;
    }

    Cam->Update();

    //cube->update();
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    //cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    cloth->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            case GLFW_KEY_V:
                wind += glm::vec3(1.0, 0.0, 0.0);
                break;

            case GLFW_KEY_B:
                wind += glm::vec3(-1.0, 0.0, 0.0);
                break;

            case GLFW_KEY_C:
                wind = glm::vec3(0);
                break;

            case GLFW_KEY_G:
                if (currIndex < 4) {
                    currIndex++;
                }
                std::cout << "Current fixpoint: " << currIndex;
                break;

            case GLFW_KEY_H:
                if (currIndex > 0) {
                    currIndex--;
                }
                std::cout << "Current fixpoint: " << currIndex;
                break;

            case GLFW_KEY_F:
                cloth->SetFixed(fixpoints[currIndex]);
                std::cout << "Fixed: " << fixpoints[currIndex] << std::endl;
                break;

            case GLFW_KEY_U:
                cloth->UnsetFixed(fixpoints[currIndex]);
                std::cout << "Unfixed: " << fixpoints[currIndex] << std::endl;
                break;

            case GLFW_KEY_W:
                dirHeld = 1;
                break;

            case GLFW_KEY_S:
                dirHeld = 2;
                break;

            case GLFW_KEY_A:
                dirHeld = 3;
                break;

            case GLFW_KEY_D:
                dirHeld = 4;
                break;

            case GLFW_KEY_K:
                dirHeld = 5;
                std::cout << dirHeld << std::endl;
                break;

            case GLFW_KEY_L:
                dirHeld = 6;
                std::cout << dirHeld << std::endl;
                break;

            default:
                break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:
                if (dirHeld == 1) {
                    dirHeld = 0;
                
                }
                break;

            case GLFW_KEY_S:
                if (dirHeld == 2) {
                    dirHeld = 0;
                    
                }
                break;

            case GLFW_KEY_A:
                if (dirHeld == 3) {
                    dirHeld = 0;
                    
                }
                break;

            case GLFW_KEY_D:
                if (dirHeld == 4) {
                    dirHeld = 0;
                    break;
                }

            case GLFW_KEY_K:
                if (dirHeld == 5) {
                    dirHeld = 0;
                    
                }
                std::cout << dirHeld << std::endl;
                break;

            case GLFW_KEY_L:
                if (dirHeld == 6) {
                    dirHeld = 0;
                    
                }
                std::cout << dirHeld << std::endl;
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}