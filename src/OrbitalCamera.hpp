#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Orbital camera
//Using Euclidean angles (= has gimbal lock)
class OrbitalCamera
{
public:
	OrbitalCamera();

	glm::mat4 getViewProjectionMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
    glm::vec3 getPosition();

	void setupCameraView(float zoom, glm::vec3 center,  float angleXDeg, float angleYDeg);
    void setCenterPoint(const glm::vec3& center);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void adjustZoom(int zoom);
	
	void adjustOrientation(float dx, float dy);
	
	void update();

private:
    glm::vec3 _pos;
    glm::vec3 _view;
    glm::vec3 _left;
	glm::vec3 _up;
    glm::vec3 _center;

	float _fovy;
	float _nearZ;
	float _farZ;
	float _aspect;

	float _zoom;
	float _angleXDeg;
	float _angleYDeg;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;
};