#include "Keyboard.h"

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"

#define SIM_KEY_STATUS_FORWARD		0
#define SIM_KEY_STATUS_BACKWARD		1
#define SIM_KEY_STATUS_LEFT			2
#define SIM_KEY_STATUS_RIGHT		3

#define SIM_KEY_STATUS_PITCH_LEFT	4	// x
#define SIM_KEY_STATUS_PITCH_RIGHT	5	// x

#define SIM_KEY_STATUS_YAW_LEFT		6	// y
#define SIM_KEY_STATUS_YAW_RIGHT	7	// y

#define SIM_KEY_STATUS_ROLL_LEFT	8	// z
#define SIM_KEY_STATUS_ROLL_RIGHT	9	// z

#define SIM_KEY_STATUS_UP			10
#define SIM_KEY_STATUS_DOWN			11
#define SIM_MODE_FREE_CAM			true
#define SIM_MODE_PRESENTATION		false

bool keyStatus[12] = { false };

bool mode = SIM_MODE_FREE_CAM;

void keyboardPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		keyStatus[SIM_KEY_STATUS_FORWARD] = true; break;

	case 's':
	case 'S':
		keyStatus[SIM_KEY_STATUS_BACKWARD] = true; break;

	case 'a':
	case 'A':
		keyStatus[SIM_KEY_STATUS_LEFT] = true; break;

	case 'd':
	case 'D':
		keyStatus[SIM_KEY_STATUS_RIGHT] = true; break;

	case 'q':
	case 'Q':
		keyStatus[SIM_KEY_STATUS_YAW_LEFT] = true; break; // vertical rotation

	case 'e':
	case 'E':
		keyStatus[SIM_KEY_STATUS_YAW_RIGHT] = true; break; // vertical rotation

	case 'b':
	case 'B':
		keyStatus[SIM_KEY_STATUS_DOWN] = true; break;

	case 'v':
	case 'V':
		keyStatus[SIM_KEY_STATUS_UP] = true; break;
	case 'p':
	case 'P':
		if (mode == SIM_MODE_FREE_CAM)
			mode = SIM_MODE_PRESENTATION;
		else
			mode = SIM_MODE_FREE_CAM;

		break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
		keyStatus[SIM_KEY_STATUS_FORWARD] = false; break;

	case 's':
	case 'S':
		keyStatus[SIM_KEY_STATUS_BACKWARD] = false; break;

	case 'a':
	case 'A':
		keyStatus[SIM_KEY_STATUS_LEFT] = false; break;

	case 'd':
	case 'D':
		keyStatus[SIM_KEY_STATUS_RIGHT] = false; break;

	case 'q':
	case 'Q':
		keyStatus[SIM_KEY_STATUS_YAW_LEFT] = false; break; // vertical rotation

	case 'e':
	case 'E':
		keyStatus[SIM_KEY_STATUS_YAW_RIGHT] = false; break; // vertical rotation

	case 'b':
	case 'B':
		keyStatus[SIM_KEY_STATUS_DOWN] = false; break;
	case 'v':
	case 'V':
		keyStatus[SIM_KEY_STATUS_UP] = false; break;
	}
}

void keyboardSpecialPressed(int key, int x, int y)
{
	switch (key)
	{

	case GLUT_KEY_UP:
		keyStatus[SIM_KEY_STATUS_PITCH_LEFT] = true; break; // lateral

	case GLUT_KEY_DOWN:
		keyStatus[SIM_KEY_STATUS_PITCH_RIGHT] = true; break; // lateral

	case GLUT_KEY_LEFT:
		keyStatus[SIM_KEY_STATUS_ROLL_LEFT] = true; break; // longitudinal

	case GLUT_KEY_RIGHT:
		keyStatus[SIM_KEY_STATUS_ROLL_RIGHT] = true; break; // longitudinal

	case GLUT_KEY_SHIFT_L:
		keyStatus[SIM_KEY_STATUS_UP] = true; break;
	}
}

void keyboardSpecialUp(int key, int x, int y)
{
	switch (key)
	{

	case GLUT_KEY_UP:
		keyStatus[SIM_KEY_STATUS_PITCH_LEFT] = false; break; // lateral

	case GLUT_KEY_DOWN:
		keyStatus[SIM_KEY_STATUS_PITCH_RIGHT] = false; break; // lateral

	case GLUT_KEY_LEFT:
		keyStatus[SIM_KEY_STATUS_ROLL_LEFT] = false; break; // longitudinal

	case GLUT_KEY_RIGHT:
		keyStatus[SIM_KEY_STATUS_ROLL_RIGHT] = false; break; // longitudinal

	case GLUT_KEY_SHIFT_L:
		keyStatus[SIM_KEY_STATUS_UP] = false; break;
	}
}

bool spaceLimits(glm::vec3 position)
{
	if (position.x < 50 && position.x > -50 && position.y > -7.5 && position.y < 9 && position.z < 50 && position.z > -50)
		return true;
	else
		return false;
}

void keyboardUpdateCamera(glm::vec3 *cameraPos, glm::vec3 *cameraDir, glm::vec3 *cameraSide, float *cameraAnglePitch, float *cameraAngleYaw, float *cameraAngleRoll)
{
	float moveSpeed = 0.02f;
	float angleSpeed = 0.005f;

	// position

	if (keyStatus[SIM_KEY_STATUS_FORWARD] == true)
	{
		glm::vec3 newPos = (*cameraPos) + (*cameraDir) * moveSpeed;

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	if (keyStatus[SIM_KEY_STATUS_BACKWARD])
	{
		glm::vec3 newPos = (*cameraPos) - (*cameraDir) * moveSpeed;

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	if (keyStatus[SIM_KEY_STATUS_LEFT])
	{
		glm::vec3 newPos = (*cameraPos) - (*cameraSide) * moveSpeed;

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	if (keyStatus[SIM_KEY_STATUS_RIGHT])
	{
		glm::vec3 newPos = (*cameraPos) + (*cameraSide) * moveSpeed;

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	if (keyStatus[SIM_KEY_STATUS_UP])
	{
		glm::vec3 newPos = (*cameraPos) + glm::vec3(0, moveSpeed, 0);

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	if (keyStatus[SIM_KEY_STATUS_DOWN])
	{
		glm::vec3 newPos = (*cameraPos) - glm::vec3(0, moveSpeed, 0);

		if (spaceLimits(newPos))
			*cameraPos = newPos;
	}

	// rotation

	if (keyStatus[SIM_KEY_STATUS_PITCH_LEFT])
		*cameraAnglePitch += angleSpeed;

	if (keyStatus[SIM_KEY_STATUS_PITCH_RIGHT])
		*cameraAnglePitch -= angleSpeed;

	if (keyStatus[SIM_KEY_STATUS_YAW_LEFT])
		*cameraAngleYaw -= angleSpeed;

	if (keyStatus[SIM_KEY_STATUS_YAW_RIGHT])
		*cameraAngleYaw += angleSpeed;

	if (keyStatus[SIM_KEY_STATUS_ROLL_LEFT])
		*cameraAngleRoll -= angleSpeed;

	if (keyStatus[SIM_KEY_STATUS_ROLL_RIGHT])
		*cameraAngleRoll += angleSpeed;
}

bool keyboardGetMode()
{
	return mode;
}