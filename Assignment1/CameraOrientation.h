#pragma once

#include <iostream>
#include<stdio.h>
#include<string.h>

#include <iostream> 

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat2x2.hpp>

/****
  CameraOrientation.h  header file to adjust the position of camer object and supports
  rotation operations against X, Y an Z axis as well get's us the view matrix and projection matrix.
***/

enum Camera_Orientation {
	Ahead, Back, Left, Right, Up, Down, RotateX, RotateXOpposite, RotateY, RotateYOpposite, RotateZ, RotateZOpposite
};

class CameraOrientation {
public:

	glm::vec3 originalPos;
	glm::vec3 originalFront;
	glm::vec3 originalUp;
	glm::vec3 originalRight;

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;


	float originalZoom;
	float zoom;
	float angle;
	float zoomIn;
	float zoomOut;

	int width;
	int height;


	glm::mat4 projectionMatrix;

	glm::mat4 viewMatrix;

	CameraOrientation(
		glm::vec3 pos1 = glm::vec3(-0.7, 1.5, 4.7),
		glm::vec3 front1 = glm::vec3(0., 0, -1),
		glm::vec3 up1 = glm::vec3(0, 1, 0),
		glm::vec3 right1 = glm::vec3(1, 0, 0),
		float zoom1 = 45.0,
		float near1 = 4.0f,
		float far1 = 11.0f,
		float angle = 0.0,
		int width1 = 1000,
		int height1 = 800
	)
	{
		this->originalPos = pos1;
		this->originalFront = front1;
		this->originalUp = up1;
		this->originalRight = right1;
		this->originalZoom = zoom1;
		this->zoomIn = near1;
		this->zoomOut = far1;
		this->width = width1;
		this->height = height1;
	}


	void reset() {
		this->pos = originalPos;
		this->front = originalFront;
		this->up = originalUp;
		this->right = originalRight;
		this->zoom = originalZoom;
	}

	void init() {
		reset();
	};

	void rotX(GLfloat angle)
	{
		glm::vec3 up = this->up;
		glm::mat4 rotation_mat(1);
		rotation_mat = glm::rotate(rotation_mat, angle, this->right);
		this->up = glm::normalize(glm::vec3(rotation_mat * glm::vec4(up, 1.0)));
		this->front = glm::normalize(glm::cross(this->up, this->right));
	}

	void rotY(GLfloat angle)
	{
		glm::vec3 front = this->front;
		glm::mat4 rotation_mat(1);
		rotation_mat = glm::rotate(rotation_mat, angle, glm::vec3(0.0, 1.0, 0.0));
		this->front = glm::normalize(glm::vec3(rotation_mat * glm::vec4(front, 1.0)));
	}

	void rotZ(GLfloat angle)
	{
		glm::vec3 right = this->right;
		glm::mat4 rotation_mat(1);
		rotation_mat = glm::rotate(rotation_mat, angle, this->front);
		this->right = glm::normalize(glm::vec3(rotation_mat * glm::vec4(right, 1.0)));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

	glm::mat4 loadCameraViewMatrix()
	{
		this->viewMatrix = glm::lookAt(this->pos, this->pos + this->front, this->up);
		return this->viewMatrix;
	}

	glm::mat4 loadCameraProjectionMatrix()
	{
		this->projectionMatrix = glm::perspective(this->zoom, (GLfloat)this->width / (GLfloat)this->height, this->zoomIn, this->zoomOut);
		return this->projectionMatrix;
	}

	void keyboardListener(Camera_Orientation cameraOrientation, GLfloat timeSlice)
	{
		GLfloat speed = timeSlice;
		if (cameraOrientation == Ahead)
			this->pos += this->front * speed;
		if (cameraOrientation == Back)
			this->pos -= this->front * speed;
		if (cameraOrientation == Left)
			this->pos -= this->right * speed;
		if (cameraOrientation == Right)
			this->pos += this->right * speed;
		if (cameraOrientation == Up)
			this->pos += this->up * speed;
		if (cameraOrientation == Down)
			this->pos -= this->up * speed;
		if (cameraOrientation == RotateX)
			rotX(speed);
		if (cameraOrientation == RotateXOpposite)
			rotX((-1) * speed);
		if (cameraOrientation == RotateY)
			rotY(speed);
		if (cameraOrientation == RotateYOpposite)
			rotY((-1) * speed);
		if (cameraOrientation == RotateZ)
			rotZ(speed);
		if (cameraOrientation == RotateZOpposite)
			rotZ((-1) * speed);
	}

};