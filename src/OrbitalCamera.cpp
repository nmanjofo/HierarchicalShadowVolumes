#include "OrbitalCamera.hpp"

OrbitalCamera::OrbitalCamera()
{
	_pos = _view = _up = _left = glm::vec3(0);
	_zoom = _angleXDeg = _angleYDeg = 0.0f;
}

void OrbitalCamera::setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg)
{
	_zoom = zoom;

    _center = center;
	_angleXDeg = angleXDeg;
	_angleYDeg = angleYDeg;
}

void OrbitalCamera::setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovY)
{
	_nearZ = nearZ;
	_farZ = farZ;
	_aspect = aspectRatio;
     _fovy = glm::radians(fovY);
}

void OrbitalCamera::adjustOrientation(float dx, float dy)
{
	_angleXDeg += dx;
	_angleYDeg += dy;
    _angleYDeg = glm::max(glm::min(_angleYDeg, 89.0f), -89.0f);
}

void OrbitalCamera::adjustZoom(int d)
{
    float c = 1.1f;
    _zoom = d < 0 ? _zoom*c : _zoom / c;
}

void OrbitalCamera::setCenterPoint(const glm::vec3& center)
{
    _center = center;
}

void OrbitalCamera::update()
{
    float radx = glm::radians(_angleXDeg);
    float rady = glm::radians(_angleYDeg);

    float x = _zoom * cos(rady) * cos(radx);
	float y = _zoom * sin(rady);
    float z = _zoom * cos(rady) * sin(radx);
    
    _pos = glm::vec3(x, y, z) + _center;
    _view = _center - _pos;
	_left = glm::cross(glm::vec3(0, 1, 0), glm::normalize(_view));
	_up = glm::cross(_view, _left);
    
    /*
    _pos = glm::vec3(x, y, z);
	_pos += _center;
    _up = glm::vec3(0, 1, 0);
    */
	_viewMatrix = glm::lookAt(_pos, _center, _up);
	_projectionMatrix = glm::perspective(_fovy, _aspect, _nearZ, _farZ);
}

glm::mat4 OrbitalCamera::getViewProjectionMatrix()
{
     return _projectionMatrix * _viewMatrix;
}

glm::mat4 OrbitalCamera::getProjectionMatrix()
{
     return _projectionMatrix;
}

glm::mat4 OrbitalCamera::getViewMatrix()
{
	return _viewMatrix;
}

glm::vec3 OrbitalCamera::getPosition()
{
    return _pos;
}


