#pragma once

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"


void keyboardPressed(unsigned char key, int x, int y);

void keyboardUp(unsigned char key, int x, int y);

void keyboardSpecialPressed(int key, int x, int y);

void keyboardSpecialUp(int key, int x, int y);

bool spaceLimits(glm::vec3 position);

void keyboardUpdateCamera(glm::vec3 *cameraPos, glm::vec3 *cameraDir, glm::vec3 *cameraSide, float *cameraAnglePitch, float *cameraAngleYaw, float *cameraAngleRoll);

bool keyboardGetMode();