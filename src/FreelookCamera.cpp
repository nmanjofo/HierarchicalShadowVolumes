#include "FreelookCamera.hpp"

FreelookCamera::FreelookCamera()
{
    _cameraMovementSpeed = 5;
    _yaw = _pitch = 0;
    _flags = 0;

    _position = _left = _up = glm::vec3(0);

    _viewMatrix = _projectionMatrix = glm::mat4(1);

    _timer.reset();
}

void FreelookCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg)
{
    _projectionMatrix = glm::perspective(glm::radians(fovYDeg), aspectRatio, nearZ, farZ);
}

void FreelookCamera::setMovementSpeed(float s)
{
    if(s>0)
        _cameraMovementSpeed = s;
}

void FreelookCamera::adjustOrientation(float hDeg, float vDeg)
{
    _yaw += glm::radians(hDeg);
    _pitch += glm::radians(vDeg);
    
	//value clamping - keep heading and pitch between 0 and 2 pi
    if ( _yaw > glm::two_pi<float>() ) 
		_yaw -= glm::two_pi<float>();
	else if ( _yaw < 0 ) 
		_yaw = glm::two_pi<float>() + _yaw;
	
	if ( _pitch > glm::two_pi<float>() ) 
		_pitch -= glm::two_pi<float>();
	else if ( _pitch < 0 ) 
		_pitch = glm::two_pi<float>() + _pitch;
    //*/
}

void FreelookCamera::resetButtons()
{
    _flags = 0;
}

void FreelookCamera::positionCamera(const glm::vec3 &camPosition, const glm::vec3& focusPoint, const glm::vec3 &upDirection)
{
    _position = camPosition;
    _up = upDirection;
    _view = glm::normalize(focusPoint - _position);

    _left = glm::normalize(glm::cross(_up, _view));
}

void FreelookCamera::handleInputMessage( CameraMessage msg )
{
    switch (msg)
    {
    case CameraMessage::CAMERA_FORWARD_DOWN:
        SetBit(_flags, CAMERA_FORWARD_BIT);
        break;
    case CameraMessage::CAMERA_FORWARD_UP:
        ClearBit(_flags, CAMERA_FORWARD_BIT);
        break;

    case CameraMessage::CAMERA_BACKWARD_DOWN:
        SetBit(_flags, CAMERA_BACKWARD_BIT);
        break;
    case CameraMessage::CAMERA_BACKWARD_UP:
        ClearBit(_flags, CAMERA_BACKWARD_BIT);
        break;

    case CameraMessage::CAMERA_LEFT_DOWN:
        SetBit(_flags, CAMERA_LEFT_BIT);
        break;
    case CameraMessage::CAMERA_LEFT_UP:
        ClearBit(_flags, CAMERA_LEFT_BIT);
        break;

    case CameraMessage::CAMERA_RIGHT_DOWN:
        SetBit(_flags, CAMERA_RIGHT_BIT);
        break;
    case CameraMessage::CAMERA_RIGHT_UP:
        ClearBit(_flags, CAMERA_RIGHT_BIT);
        break;

    case CameraMessage::CAMERA_UP_DOWN:
        SetBit(_flags, CAMERA_UP_BIT);
        break;
    case CameraMessage::CAMERA_UP_UP:
        ClearBit(_flags, CAMERA_UP_BIT);
        break;

    case CameraMessage::CAMERA_DOWN_DOWN:
        SetBit(_flags, CAMERA_DOWN_BIT);
        break;
    case CameraMessage::CAMERA_DOWN_UP:
        ClearBit(_flags, CAMERA_DOWN_BIT);
        break;

    default:
        break;
    }
}

void FreelookCamera::update()
{
    float t = (float)_timer.getElapsedTimeFromLastQuerySeconds();
    
    glm::mat4 m = glm::mat4(1);

    if(_yaw !=0 || _pitch!=0)
    {
        glm::quat p = glm::angleAxis(_pitch, _left);
        glm::quat q = glm::angleAxis(-_yaw, glm::vec3(0, 1, 0));

        m = glm::mat4_cast(glm::normalize(q*p));
    }

    _view = glm::normalize(glm::vec3(m * glm::vec4(_view, 0)));
    _up = glm::normalize(glm::vec3(m * glm::vec4(_up, 0)));
    _left = glm::cross(_up, _view);

    //Move camera position
    if (IsBitSet(_flags, CAMERA_FORWARD_BIT) ) 
        _position += _cameraMovementSpeed * t * _view;	//forward
    if (IsBitSet(_flags, CAMERA_BACKWARD_BIT)) 
        _position -= _cameraMovementSpeed * t * _view;	//back
    if (IsBitSet(_flags, CAMERA_LEFT_BIT))     
        _position += _cameraMovementSpeed * t * _left; //left
    if (IsBitSet(_flags, CAMERA_RIGHT_BIT))    
        _position -= _cameraMovementSpeed * t * _left; //right
    if (IsBitSet(_flags, CAMERA_UP_BIT))       
        _position += _cameraMovementSpeed * t * glm::vec3(0, 1, 0); //up
    if (IsBitSet(_flags, CAMERA_DOWN_BIT))     
        _position -= _cameraMovementSpeed * t * glm::vec3(0, 1, 0); //down

    //Update view matrix
    _viewMatrix = glm::lookAt(_position, _position + _view, _up);

    //Reset angles
    _pitch = 0.0f;
	_yaw = 0.0f;
}

glm::vec3 FreelookCamera::getPosition() const
{
    return _position;
}

glm::mat4 FreelookCamera::getViewProjectionMatrix() const
{
    return _projectionMatrix * _viewMatrix;
}

glm::mat4 FreelookCamera::getViewMatrix() const
{
    return _viewMatrix;
}

glm::mat4 FreelookCamera::getProjectionMatrix() const
{
    return _projectionMatrix;
}

float FreelookCamera::getMovementSpeed() const
{
    return _cameraMovementSpeed;
}