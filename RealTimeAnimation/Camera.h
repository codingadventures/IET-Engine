#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

static const float SPEED_STEP = 0.3f;
// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::quat Rotation;
	glm::quat ModelRotation;
	glm::vec3 Direction;
	// Euler Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom; 
	GLboolean HasMoved;
	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = -90.0f, GLfloat pitch = 0.0f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.25f), Zoom(45.0f)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();  
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)  : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.25f), Zoom(45.0f)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position , this->Position + Front, this->WorldUp);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime; 
		HasMoved = true;
		if(direction == FORWARD)
			this->Position += this->Front * velocity * glm::vec3(1.0f,0.0f,1.0f);
		if(direction == BACKWARD)
			this->Position -= this->Front * velocity * glm::vec3(1.0f,0.0f,1.0f);
		if(direction == LEFT)
			this->Position -= this->Right * velocity;
		if(direction == RIGHT)
			this->Position += this->Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset )
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;


		if(this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if(this->Pitch < 30.0f)
			this->Pitch = 30.0f;


		// Update Front, Right and Up Vectors using the updated Euler angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if(this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if(this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}


	// Moves/alters the camera positions based on user input
	void MoveCamera(GLfloat deltaTime)
	{
		HasMoved = false;

		this->Position = Direction * Rotation + Target;
		this->Front = glm::normalize(Target - Position);
		this->deltaTime = deltaTime;

		// Camera controls
		//if(keys[KEY_w])
		//	this->ProcessKeyboard(FORWARD, SPEED_STEP);
		//if(keys[KEY_s])
		//	this->ProcessKeyboard(BACKWARD, SPEED_STEP);
		//if(keys[KEY_a])
		//	this->ProcessKeyboard(LEFT, SPEED_STEP);
		//if(keys[KEY_d])
		//	this->ProcessKeyboard(RIGHT, SPEED_STEP);

	}

	void SetTarget(glm::vec3 target)
	{
		this->Target = target;
	}
private:

	glm::vec3 Target;
	GLfloat deltaTime;
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new 
		//glm::vec3 front;
		//front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		//front.y = sin(glm::radians(this->Pitch));
		//front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

		this->Rotation =   glm::quat(glm::vec3(0.0f,glm::radians(Yaw),glm::radians(Pitch)));
		this->ModelRotation =  glm::quat(glm::vec3(0.0f,glm::radians(-Yaw),0.0f)); //to obtain this very effect you can simply invert the quaternion
		this->Front = glm::normalize(Front);
		// Make sure that when pitch is out of bounds, screen doesn't get flipped 
		//this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = glm::normalize(glm::cross(this->Front, this->Right));
	}
};