#ifndef TRACKS_TRACK_H
#define TRACKS_TRACK_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <jsoncpp/json/json.h>


using namespace std;
ifstream ifs("/home/agata/Documents/tracks/track.json");
ifstream ifs1("/home/agata/Documents/tracks/track.json");
ifstream ifs2("/home/agata/Documents/tracks/track.json");

class Track{
public:
    int trackSize = getTrackSize();
    int maxPolyXValue = getMaxPolyXValue();
    float tracks[360][300]={};

    int getTrackSize(){
        Json::Reader readerSize;
        Json::Value objSize;
        readerSize.parse(ifs1,objSize);
        int trackSize = objSize["fTracks"].size();
        return trackSize;
    }

    int getMaxPolyXValue() {
        Json::Value objPoly;
        Json::Reader readerPoly;
        readerPoly.parse(ifs,objPoly);
        for (Json::Value::iterator it = objPoly["fTracks"].begin(); it != objPoly["fTracks"].end(); ++it) {

            if ((*it)["fPolyX"].size() > maxPolyXValue) {
                maxPolyXValue = (*it)["fPolyX"].size();
            }
        }
        return maxPolyXValue;
    }

    void initiateBuffers(unsigned int *VAO, unsigned int *VBO){
        Json::Value objTracks;
        Json::Reader readerTracks;
        readerTracks.parse(ifs2,objTracks);

        for (Json::Value::iterator it = objTracks["fTracks"].begin(); it != objTracks["fTracks"].end(); ++it){
            int TrackIndex = it.key().asInt();

            for (int i = 0; i < (*it)["fPolyX"].size(); i++){
                tracks[TrackIndex][i*5] = (*it)["fPolyX"][i].asFloat();
                tracks[TrackIndex][i*5 + 1] = (*it)["fPolyY"][i].asFloat();
                tracks[TrackIndex][i*5 + 2] = (*it)["fPolyZ"][i].asFloat();
                tracks[TrackIndex][i*5 + 3] = (*it)["fMass"].asFloat();
                tracks[TrackIndex][i*5 + 4] = 1.0f;
            }

            glGenBuffers(1, &VBO[TrackIndex]);
            glGenVertexArrays(1, &VAO[TrackIndex]);
            glBindVertexArray(VAO[TrackIndex]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[TrackIndex]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tracks[TrackIndex]), &tracks[TrackIndex], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
        }
    }
};
#endif

