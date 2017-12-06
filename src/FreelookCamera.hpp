#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "HighResolutionTimer.hpp"
#include "BitOperations.h"


//Button indicator
//F.e. I have a "W" bound as a camera forward movement
//When "W" is pressed, call handleInputMessage(CAMERA_FORWARD_DOWN)
//When "W" is released (user put off the finger from "W" key), call handleInputMessage(CAMERA_FORWARD_UP)
//The UP and DOWN indicates whether the button was pressed or released
//Camera update method will handle the message
enum class CameraMessage : unsigned char
{
    CAMERA_FORWARD_UP = 0,
    CAMERA_FORWARD_DOWN,
    CAMERA_BACKWARD_UP,
    CAMERA_BACKWARD_DOWN,
    CAMERA_LEFT_UP,
    CAMERA_LEFT_DOWN,
    CAMERA_RIGHT_UP,
    CAMERA_RIGHT_DOWN,
    CAMERA_UP_UP,
    CAMERA_UP_DOWN,
    CAMERA_DOWN_UP,
    CAMERA_DOWN_DOWN
};

//Free look camera
//Works similarly to FPS camera
//Timer and quaternion-based
//Uses quaternions for rotations, which can be local
//More suitable for animations - just by calling positionCamera

class FreelookCamera
{
public:
	FreelookCamera();

    //Use these 2 function to setup the camera
    //FOVY in degrees
    void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void positionCamera(const glm::vec3 &camPosition, const glm::vec3& focusPoint, const glm::vec3 &upDirection = glm::vec3(0, 1, 0));

	void handleInputMessage( CameraMessage m );
	void adjustOrientation(float hRad, float vRad);
	void setMovementSpeed(float s );
    void resetButtons();

    //Update method - call every time when camera data needs updating
	void update();

    glm::vec3 getPosition() const;
    glm::mat4 getViewProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    float     getMovementSpeed() const;

private:
	float               _cameraMovementSpeed;
    float               _yaw, _pitch;   //camera angles
    unsigned int        _flags;

    HighResolutionTimer _timer;

    glm::vec3           _position;
    glm::vec3           _up;
    glm::vec3           _left;
    glm::vec3           _view;

	glm::mat4           _viewMatrix;
    glm::mat4           _projectionMatrix;

    enum
    {
        CAMERA_FORWARD_BIT = 0,
        CAMERA_BACKWARD_BIT,
        CAMERA_LEFT_BIT,
        CAMERA_RIGHT_BIT,
        CAMERA_UP_BIT,
        CAMERA_DOWN_BIT
    };
};