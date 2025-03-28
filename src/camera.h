#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"

class Camera {
public:
	Camera() {
		m_Position = glm::vec3(Settings::CHUNK_WIDTH/2, Settings::CHUNK_HEIGHT, -Settings::CHUNK_LENGTH/2);
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_Pitch = 0.0f;
		m_Yaw = -90.0f;
	}
	~Camera() = default;
	const glm::mat4& GetView() const {
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}
	void Move(glm::vec3 move) {
		m_Position += move;
	}

	void Look(int x, int y) {
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
		m_Yaw += x * 0.1f;
		m_Pitch -= y * 0.1f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		direction.y = sin(glm::radians(m_Pitch));
		direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(direction);
	}
public:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	float m_Pitch;
	float m_Yaw;
};

#endif