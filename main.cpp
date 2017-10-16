// Local Headers
#include "glitter.hpp"
#include <GLFW/glfw3.h>
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Shader.h"

#include <iostream>
#include <fstream>

using namespace std;

const unsigned int SCR_WIDTH = 1020;
const unsigned int SCR_HEIGHT = 800;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    Shader shader("/home/agata/Documents/tracks/Shaders/vertex.vert",
                  "/home/agata/Documents/tracks/Shaders/fragment.frag",
                  "/home/agata/Documents/tracks/Shaders/geometry.geom");

    float* tracks = new float[300]();
    ifstream ifs("/home/agata/Documents/tracks/track.json");

    Json::Value obj;

    ifs >> obj; //cLion bug, works perfectly.

    for (Json::Value::iterator it = obj["fTracks"].begin(); it != obj["fTracks"].end(); ++it){

        int polyCoordinateSize = (*it)["fPolyX"].size();
        for (int i = 0; i < polyCoordinateSize; i++){
            tracks[i*5] = (*it)["fPolyX"][i].asFloat();
            tracks[i*5 + 1] = (*it)["fPolyY"][i].asFloat();
            tracks[i*5 + 2] = (*it)["fPolyZ"][i].asFloat();
            tracks[i*5 + 3] = 1.0;
            tracks[i*5 + 4] = 0.0;
        }
        for (int i = 0; i< polyCoordinateSize; i++){
            cout<< tracks[i*5] << " ";
            cout<< tracks[i*5+1] << " ";
            cout<< tracks[i*5+2] << " ";
            cout<< tracks[i*5+3] << " ";
            cout<< tracks[i*5+4] << " ";
            cout<<"\n";
        }
//
//        cout << "" << (*it)["fType"].asString();
//        cout << " chapter: " << (*it)["fBits"].asUInt();
//        cout << endl;
    }

    ifs.close();

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tracks), &tracks[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 transform;

        transform = glm::scale(transform, glm::vec3(0.01, 0.01, 0.01));
        shader.use();
        GLint transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}