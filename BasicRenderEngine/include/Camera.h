/*
 * Camera.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Collin
 */

#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
protected:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 upDirection;
	float fov;
	float aspectRatio;
	float nearZ, farZ;

public:
	Camera(): position(0.0, 0.0, 1.0), target(0.0, 0.0, 0.0), upDirection(0.0, 1.0, 0.0), fov(70.0), aspectRatio(1.0), nearZ(1.0), farZ(10.0) {}

	glm::mat4 calcViewMatrix() const noexcept
	{
		return glm::lookAt(position, target, upDirection);
	}
	glm::mat4 calcPerspectiveMatrix() const noexcept
	{
		return glm::perspective(fov, aspectRatio, nearZ, farZ);
	}

	float getFov() const
	{
		return fov;
	}
	void setFovDeg(float fov)
	{
		this->fov = glm::radians(fov);
	}
	void setFov(float fov)
	{
		this->fov = fov;
	}

	glm::vec3 getPosition() const
	{
		return position;
	}
	void setPosition(glm::vec3 position)
	{
		this->position = position;
	}
	void offsetPosition(glm::vec3 offset)
	{
		this->position += offset;
	}

	glm::vec3 getTarget() const
	{
		return target;
	}
	void setTarget(glm::vec3 target)
	{
		this->target = target;
	}
	void offsetTarget(glm::vec3 offset)
	{
		this->target += offset;
	}

	void offsetCamera(glm::vec3 offset)
	{
		offsetPosition(offset);
		offsetTarget(offset);
	}

	void orientCamera(glm::quat orientation)
	{
		target = position + orientation * (target - position);
		upDirection = orientation * upDirection;
		glm::vec3 rightDirection = glm::cross(target - position, upDirection);
		upDirection = glm::normalize(glm::cross(rightDirection, target - position));
	}

	glm::vec3 calcForwardDirection() const
	{
		return glm::normalize(target - position);
	}

	glm::vec3 getUpDirection() const
	{
		return upDirection;
	}
	void setUpDirection(glm::vec3 upDirection)
	{
		this->upDirection = glm::normalize(upDirection);
	}
	
	float getAspectRatio() const
	{
		return aspectRatio;
	}
	void setAspectRatio(float aspectRatio)
	{
		this->aspectRatio = aspectRatio;
	}
	
	float getFarZ() const
	{
		return farZ;
	}
	void setFarZ(float farZ)
	{
		this->farZ = farZ;
	}
	
	float getNearZ() const
	{
		return nearZ;
	}
	void setNearZ(float nearZ)
	{
		this->nearZ = nearZ;
	}
};

#endif /* CAMERA_H_INCLUDED */
