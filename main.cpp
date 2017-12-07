#include "glitter.hpp"
#include <GLFW/glfw3.h>
#include <jsoncpp/json/json.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Track.h"
#include <iostream>
#include <fstream>

using namespace std;

int SCR_WIDTH = 1020;
int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void drawStereo(glm::mat4 transform, Shader shader, int trackSize, unsigned int VAO[]);

Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const char* vertexShaderPath = "/home/agata/Documents/tracks/Shaders/vertex.vert";
const char* fragmentShaderPath = "/home/agata/Documents/tracks/Shaders/fragment.frag";
const char* geometryShaderPath = "/home/agata/Documents/tracks/Shaders/geometry.geom";

float rotateY = 0.0f;
float rotateX = 0.0f;
float IOD = 0.0001f;
float depthZ = 100.0f;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tracks", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    Shader shader(vertexShaderPath, fragmentShaderPath, geometryShaderPath );
    Track track;

    static int trackSize = track.trackSize;
    unsigned int VBO[trackSize], VAO[trackSize];
    track.initiateBuffers(VAO, VBO);

    GLuint program_id = glCreateProgram();
    GLuint matrix_id = glGetUniformLocation(program_id,"MVP");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        bool stereo = true;
        if (stereo) {
            glm::mat4 transform;

            glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
            camera.computeStereoView((float) SCR_WIDTH / (float) SCR_HEIGHT, IOD, depthZ, true);
            drawStereo(transform, shader, trackSize, &VAO[trackSize]);

            //Draw the RIGHT eye, right half of the screen
            glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);
            camera.computeStereoView((float) SCR_WIDTH / (float) SCR_HEIGHT, IOD, depthZ, false);
            drawStereo(transform, shader, trackSize, &VAO[trackSize]);
        }
        else {
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            shader.setMat4("projection", projection);

            // camera/view transformation
            glm::mat4 view = camera.GetViewMatrix();
            shader.setMat4("view", view);

            glm::mat4 transform;
            transform = glm::scale(transform, glm::vec3(0.01, 0.01, 0.01));
            GLint transformLoc = glGetUniformLocation(shader.ID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

            for (int iterator =0; iterator < trackSize; iterator++) {
                glBindVertexArray(VAO[iterator]);
                glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (int iterator =0; iterator < trackSize; iterator++) {
        glDeleteVertexArrays(1, &VAO[iterator]);
        glDeleteBuffers(1, &VBO[iterator]);
    }

    glfwTerminate();
    return 0;
}

void drawStereo (glm::mat4 transform, Shader shader, int trackSize, unsigned int VAO[]) {

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0);

    transform = glm::scale(transform, glm::vec3(0.01, 0.01, 0.01));
    transform = glm::translate(model, glm::vec3(0.0f, 0.0f, -depthZ));
    transform = glm::rotate(model, glm::pi<float>() * rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(model, glm::pi<float>() * rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
    GLint transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    for (int iterator = 0; iterator < trackSize; iterator++) {
        glBindVertexArray(VAO[iterator]);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}