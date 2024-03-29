#include "glitter.hpp"
#include <GLFW/glfw3.h>
#include <jsoncpp/json/json.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Track.h"
#include <Model.h>

using namespace std;

int SCR_WIDTH = 1020;
int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void drawStereoTracks(glm::mat4 transform, Shader shader, Track track);
void drawStereoDetector(glm::mat4 transform, Shader shader, Model ourModel);

Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char* tracksVertexShaderPath = "/home/agata/Documents/tracks/Shaders/tracks.vert";
const char* tracksFragmentShaderPath = "/home/agata/Documents/tracks/Shaders/tracks.frag";
const char* tracksGeometryShaderPath = "/home/agata/Documents/tracks/Shaders/tracks.geom";

const char* materialsVertexShaderPath = "/home/agata/Documents/tracks/Shaders/materials.vert";
const char* materialsFragmentShaderPath = "/home/agata/Documents/tracks/Shaders/materials.frag";

const char* modelPath = "/home/agata/Documents/tracks/detector.dae";

float rotateY = 0.0f;
float rotateX = 0.0f;
float IOD = 0.0001f;
float depthZ = -1000.0f;

bool stereo = false;
bool tracks =true;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    Shader shader(tracksVertexShaderPath, tracksFragmentShaderPath, tracksGeometryShaderPath );
    Shader materialShader(materialsVertexShaderPath, materialsFragmentShaderPath);

    Track track;
    track.init();

    Model ourModel(modelPath);

    GLuint program_id = glCreateProgram();
    GLuint matrix_id = glGetUniformLocation(program_id,"MVP");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.1f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //shader.use();
        if (stereo) {
            glm::mat4 transform;

            glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
            camera.computeStereoView((float) SCR_WIDTH / (float) SCR_HEIGHT, IOD, depthZ, true);

            if(tracks)
               drawStereoTracks(transform, shader, track);
            else
                drawStereoDetector(transform, materialShader, ourModel);

            //Draw the RIGHT eye, right half of the screen
            glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);
            camera.computeStereoView((float) SCR_WIDTH / (float) SCR_HEIGHT, IOD, depthZ, false);

            if(tracks)
                drawStereoTracks(transform, shader, track);
            else
                drawStereoDetector(transform, materialShader, ourModel);
        }
        else {
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            if(tracks) {
                shader.use();
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                        (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
                shader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                shader.setMat4("view", view);
                glm::mat4 transform;

                transform = glm::scale(transform, glm::vec3(0.01, 0.01, 0.01));
                GLint transformLoc = glGetUniformLocation(shader.ID, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

                track.draw();

            }
            else {
                materialShader.use();
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

                materialShader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                materialShader.setMat4("view", view);

                glm::mat4 model;
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's
                // at the center of the scene
                model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// it's a bit too big for our scene,
                // so scale it down
                materialShader.setMat4("model", model);

                ourModel.Draw(materialShader);
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    track.cleanUp();
    glfwTerminate();
    return 0;
}

void drawStereoDetector (glm::mat4 transform, Shader materialShader, Model ourModel) {
    materialShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    materialShader.setMat4("projection", projection);
    glm::mat4 view = camera.GetViewMatrix();
    materialShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0);

    transform = glm::scale(transform, glm::vec3(0.001, 0.001, 0.001));
    transform = glm::translate(model, glm::vec3(0.0f, 0.0f, -depthZ));
    transform = glm::rotate(model, glm::pi<float>() * rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(model, glm::pi<float>() * rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
    GLint transformLoc = glGetUniformLocation(materialShader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    ourModel.Draw(materialShader);
}

void drawStereoTracks(glm::mat4 transform, Shader shader, Track track) {
    shader.use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0);

    transform = glm::translate(model, glm::vec3(0.0f, 0.0f, -depthZ));
    transform = glm::rotate(model, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(model, glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    GLint transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    track.draw();
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        stereo=true;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        stereo= false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        tracks= true;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tracks= false;
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