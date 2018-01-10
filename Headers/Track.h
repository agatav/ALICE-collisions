#ifndef TRACKS_TRACK_H
#define TRACKS_TRACK_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <jsoncpp/json/json.h>

using namespace std;
ifstream ifs("/home/agata/Documents/tracks/track.json");

class Track{
    unsigned int VBO, VAO, EBO;
    bool isInited;

private: int numsToDraw;

public:
    void init(){
        vector<GLfloat> tracks;
        vector<GLuint> indices;
        int indicator = 0;

        Json::Value objectTracks;
        Json::Reader readerTracks;
        readerTracks.parse(ifs,objectTracks);

        for (Json::Value::iterator it = objectTracks["fTracks"].begin(); it != objectTracks["fTracks"].end(); ++it){

            for (int i = 0; i < (*it)["fPolyX"].size(); i++){
                tracks.push_back((*it)["fPolyX"][i].asFloat());
                tracks.push_back((*it)["fPolyY"][i].asFloat());
                tracks.push_back((*it)["fPolyZ"][i].asFloat());
                tracks.push_back((*it)["fMass"].asFloat());
                tracks.push_back((*it)["fMass"].asFloat());
                indices.push_back(indicator);
                indicator++;
            }
            indices.push_back(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, tracks.size() * sizeof(GLfloat),  &tracks[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(1);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        numsToDraw = indices.size();
        isInited = true;
    }

    void cleanUp(){
        if (!isInited) {
            return;
        }
        if(VBO) {
            glDeleteBuffers(1, &VBO);
        }
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
        }
        if(EBO) {
            glDeleteBuffers(1, &EBO);
        }
        isInited = false;
        VAO = 0;
        VBO = 0;
        EBO = 0;
    }

    void draw()
    {
        if (!isInited) {
            std::cout << "please call init() before draw()" << std::endl;
        }

        glBindVertexArray(VAO);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_LINE_STRIP_ADJACENCY, numsToDraw, GL_UNSIGNED_INT,  (void*)0 );
    };
};
#endif

