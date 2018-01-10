#ifndef TRACKS_TRACK_H
#define TRACKS_TRACK_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <jsoncpp/json/json.h>

using namespace std;
ifstream ifs("/home/agata/Documents/tracks/track.json");

class Track{
    unsigned int VBO, VAO;
    bool isInited;
    bool gammaCorrection;


public:
    void init(){
        vector<GLfloat> tracks;
        getTracks(tracks);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, tracks.size() * sizeof(GLfloat),  &tracks[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(1);

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

        isInited = false;
        VAO = 0;
        VBO = 0;
    }

    void draw()
    {
        if (!isInited) {
            std::cout << "please call init() before draw()" << std::endl;
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 348);
    }
private:

    void getTracks(vector<GLfloat> &tracks){
        Json::Value objectTracks;
        Json::Reader readerTracks;
        readerTracks.parse(ifs,objectTracks);
        for (Json::Value::iterator it = objectTracks["fTracks"].begin(); it != objectTracks["fTracks"].end(); ++it){

            for (int i = 0; i < (*it)["fPolyX"].size(); i++){
                tracks.push_back((*it)["fPolyX"][i].asFloat());
                tracks.push_back((*it)["fPolyY"][i].asFloat());
                tracks.push_back((*it)["fPolyZ"][i].asFloat());
                tracks.push_back((*it)["fMass"].asFloat());
                tracks.push_back(1.0f);
            }
        }
    };
};
#endif

