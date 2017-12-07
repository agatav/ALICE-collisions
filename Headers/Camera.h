#ifndef TRACKS_CAMERA_H
#define TRACKS_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW        = -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  1.5f;
const float SENSITIVTY =  0.01f;
const float ZOOM       =  45.0f;
const float nearZ      =  0.1f;
const float farZ       =  100.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float g_initial_fov = glm::pi<float>()*0.99f;
    glm::vec3 g_position = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 g_view_matrix;
    glm::mat4 g_projection_matrix;


    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 g_position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = g_position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 getViewMatrix(){
        return g_view_matrix;
    }

    glm::mat4 getProjectionMatrix(){
        return g_projection_matrix;
    }

    void computeStereoView(float aspect_ratio, float IOD, float depthZ, bool left_eye, glm::vec3 up = glm::vec3(0, 1, 0), glm::vec3 direction_z = glm::vec3(0, 0, 1) ) {
        //mirror the parameters with the right eye
        float left_right_direction = -1.0f;
        if (left_eye)
            left_right_direction = 1.0f;

        double frustumshift = (IOD / 2) * nearZ / depthZ;
        float top = tan(g_initial_fov / 2.0f) * nearZ;
        float right = aspect_ratio * top + frustumshift * left_right_direction;
        float left = -aspect_ratio * top + frustumshift * left_right_direction;
        float bottom = -top;
        g_projection_matrix = glm::frustum(left, right, bottom, top, nearZ, farZ);
        g_view_matrix = glm::lookAt( g_position - direction_z + glm::vec3(left_right_direction * IOD / 2, 0, 0),
                                     g_position + glm::vec3(left_right_direction * IOD / 2, 0, 0), up);
    }


    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:

    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif