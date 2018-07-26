#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <math.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <iomanip>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "Keyboard.h"

GLuint programColor;
GLuint programTexture;

Core::Shader_Loader shaderLoader;

obj::Model fishModel;
std::vector<obj::Model> vecFish;
obj::Model sphereModel;
obj::Model surfaceModel;
obj::Model groundModel;
obj::Model grassModel;
obj::Model rockModel;
obj::Model flowerModel;
obj::Model booble;

const int VALUE_OF_FISH = 80;
float angle[VALUE_OF_FISH];
glm::vec3 fishPosition[VALUE_OF_FISH];
glm::vec3 fishColor[VALUE_OF_FISH];

const int VALUE_OF_BOOBLE = 100;
glm::vec3 booblePosition[VALUE_OF_BOOBLE];
float fishFlowingPosition = 0;

const int VALUE_OF_ROCK = 80;
glm::vec3 rockPosition[VALUE_OF_ROCK];

const int VALUE_OF_GRASS = 80;
glm::vec3 grassPosition[VALUE_OF_ROCK];

const int VALUE_OF_FLOWER = 20;
glm::vec3 flowerPosition[VALUE_OF_FLOWER];

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));


glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery

float cameraAngle = 0;
float cameraAnglePitch = 0;
float cameraAngleRoll = 0;
float cameraAngleYaw = 0;
float lastx, lasty;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::quat cameraRotation = glm::quat(1, 0, 0, 0);
glm::quat rotation = glm::quat(1, 0, 0, 0);

float boobleHeight = 0;

GLuint textureAsteroid;
GLuint textureGrass;
GLuint textureGround;
GLuint textureBuble;
GLuint textureRock;

int i = 0;

void mouse(int x, int y)
{
	float diffx = x - lastx;
	float diffy = y - lasty;
	lastx = x;
	lasty = y;
	cameraAngleYaw += diffx * 0.015f;
	cameraAnglePitch += diffy * 0.015f;
}

glm::mat4 createCameraMatrix()
{
	glm::quat rotX = glm::angleAxis(cameraAnglePitch, glm::vec3(1, 0, 0));
	glm::quat rotY = glm::angleAxis(cameraAngleYaw, glm::vec3(0, 1, 0));
	glm::quat rotZ = glm::angleAxis(cameraAngleRoll, glm::vec3(0, 0, 1));

	glm::quat cameraRotationChange = rotX * rotY * rotZ;

	cameraRotation = cameraRotationChange * cameraRotation;

	cameraAnglePitch = 0;
	cameraAngleYaw = 0;
	cameraAngleRoll = 0;

	cameraDir = glm::inverse(cameraRotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(cameraRotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, cameraRotation);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color, float alpha)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(program, "objectAlpha"), alpha);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void renderScene()
{

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float sin = sinf(time*0.4) / 2;

	if (keyboardGetMode())
	{
		keyboardUpdateCamera(&cameraPos, &cameraDir, &cameraSide, &cameraAnglePitch, &cameraAngleYaw, &cameraAngleRoll);
	}
	else
	{
		cameraRotation = glm::angleAxis(0.05f * time, glm::vec3(0, 1, 0));

		cameraPos.x = 16 * sinf(-time * 0.05f);
		cameraPos.y = 0;
		cameraPos.z = 16 * cosf(-time * 0.05f);
	}

	// Aktualizacja macierzy widoku i rzutowania
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 2, 0)) * glm::scale(glm::vec3(0.15f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(cameraRotation))* shipInitialTransformation;

	/*
	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(110.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(cameraRotation)) * shipInitialTransformation;
	drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.6f)); */
	
//	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -1, -2)) * glm::rotate(-cameraAngle + 2 * glm::radians(90.0f), glm::vec3(0, 1, 0))* glm::scale(glm::vec3(0.5f));

	if (i == 121)
	{
		i = 0;
	}
	if (i == i)
	{
		//glowna rybka
		drawObjectColor(&vecFish[i], shipModelMatrix, glm::vec3(0.2,1,0.5),0);


		if (fishFlowingPosition > 60)
			fishFlowingPosition = 0;
		else
			fishFlowingPosition += 0.02f;

		//pozostale rybki
		for (int j = 0; j < VALUE_OF_FISH; j++)
		{
			glm::mat4 modelMatrix = glm::rotate(glm::radians(angle[j]), glm::vec3(0, 1, 0));

			if (angle[j] > 0 && angle[j] <= 90)
			{
				glm::vec3 newAngle = glm::vec3(fishFlowingPosition, 0, 0);
				drawObjectColor(&vecFish[i], glm::translate(fishPosition[j] + newAngle) * 1 * modelMatrix, fishColor[j],0);
			}else
			if (angle[j] > 90 && angle[j] <= 180)
			{
				glm::vec3 newAngle = glm::vec3(0, 0, -fishFlowingPosition);
				drawObjectColor(&vecFish[i], glm::translate(fishPosition[j] + newAngle) * 1 * modelMatrix, fishColor[j],0);
			}else
			if (angle[j] > 180 && angle[j] <= 270)
			{
				glm::vec3 newAngle = glm::vec3(-fishFlowingPosition, 0,0  );
				drawObjectColor(&vecFish[i], glm::translate(fishPosition[j] + newAngle) * 1 * modelMatrix, fishColor[j],0);
			}else
			if ((angle[j] > 270 && angle[j] <= 360) || angle[j]==0)
			{
				glm::vec3 newAngle = glm::vec3(0, 0, fishFlowingPosition);
				drawObjectColor(&vecFish[i], glm::translate(fishPosition[j] + newAngle) * 1 * modelMatrix, fishColor[j],0);
			}
		}

		//Sleep(4);
	}
	i++;

	for (int j = 0; j < VALUE_OF_BOOBLE; j++)
	{
		if (boobleHeight > 40)
			boobleHeight = 0;
		else 
			boobleHeight += 0.001f;

		glm::vec3 newHigh = glm::vec3(0, boobleHeight, 0);
		drawObjectTexture(&booble, glm::translate(booblePosition[j] + newHigh) * glm::scale(glm::vec3(0.3f)), textureBuble);
	}

	//drawObjectColor(&surfaceModel, glm::translate(glm::vec3(0, 11, 0)) * glm::scale(glm::vec3(4.0f)), glm::vec3(0,0,1), 0.10f);
	drawObjectTexture(&groundModel, glm::translate(glm::vec3(0, -2, 0)) * glm::scale(glm::vec3(1.2f)), textureGround);

	for (int j = 0; j < VALUE_OF_ROCK; j++)
	{
		drawObjectTexture(&rockModel, glm::translate(rockPosition[j]) * glm::scale(glm::vec3(0.5f)), textureRock);
		drawObjectTexture(&grassModel, glm::translate(grassPosition[j]) * glm::scale(glm::vec3(0.1f)*40), textureGrass);
		if (j < VALUE_OF_FLOWER)
		{
			drawObjectTexture(&flowerModel, glm::translate(flowerPosition[j]) * glm::scale(glm::vec3(20.0f)), textureGrass);
		}
	}

	glutSwapBuffers();
}

void readFish()
{
	std::string a = "models/fish/untitled_000";
	std::string b = ".obj";
	std::string path = "";

	for (int j = 0; j < 121; j++)
	{
		std::stringstream ss;
		ss << j;
		std::string str = ss.str();
		if (j<10) { path = a + "00" + str + b; }
		else
			if (j<100) { path = a + "0" + str + b; }
			else
			{
				path = a + str + b;
			}
		std::cout << path << std::endl;

		fishModel = obj::loadModelFromFile(path);
		vecFish.push_back(fishModel);
	}
}

void init()
{
	srand(time(NULL));
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	readFish();
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	groundModel = obj::loadModelFromFile("models/ground2.obj");
	booble = obj::loadModelFromFile("models/booblees.obj");
	surfaceModel = obj::loadModelFromFile("models/ground2.obj");
	rockModel = obj::loadModelFromFile("models/rock.obj");
	grassModel = obj::loadModelFromFile("models/grass3.obj");
	flowerModel = obj::loadModelFromFile("models/grass.obj");

	textureBuble = Core::LoadTexture("textures/boble.png");
	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	textureGround = Core::LoadTexture("textures/Sand.png");
	textureRock = Core::LoadTexture("textures/Rock.png");
	textureGrass = Core::LoadTexture("textures/grass1.png");

	float tab[4] = {0,90,180,270};

	for (int j = 3; j < VALUE_OF_BOOBLE; j++)
	{
		if (j < VALUE_OF_FISH)
		{
			int colX = (rand() % 9);
			int colY = (rand() % 9);
			int colZ = (rand() % 9);

			float fColX = (float)colX / 10;
			float fColY = (float)colY / 10;
			float fColZ = (float)colZ / 10;

			fishColor[j - 1] = glm::vec3(fColX, fColY, fColZ);
			fishColor[j - 3] = glm::vec3(fColX, fColY, fColZ);
			fishColor[j - 2] = glm::vec3(fColX, fColY, fColZ);
			fishColor[j] = glm::vec3(fColX, fColY, fColZ);

			angle[j - 3] = tab[rand() % 4];
			angle[j - 2] = tab[rand() % 4];
			angle[j - 1] = tab[rand() % 4];
			angle[j] = tab[rand() % 4];
		}
		if (j <= VALUE_OF_ROCK)
		{
			rockPosition[j - 1] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 50);
			rockPosition[j - 2] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 1);
			rockPosition[j - 3] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 1);
			rockPosition[j] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 50);
		}

		if (j <= VALUE_OF_GRASS)
		{
			grassPosition[j - 1] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 50);
			grassPosition[j - 2] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 1);
			grassPosition[j - 3] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 1);
			grassPosition[j] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 50);
		}

		if (j < VALUE_OF_FLOWER)
		{
			flowerPosition[j - 1] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 50);
			flowerPosition[j - 2] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 1);
			flowerPosition[j - 3] = glm::vec3((rand() % 46) - 50, -2, (rand() % 46) - 1);
			flowerPosition[j] = glm::vec3((rand() % 46) - 3, -2, (rand() % 46) - 50);
		}

		if (j < VALUE_OF_FISH)
		{

			fishPosition[j - 1] = glm::vec3((rand() % 46) - 50, (rand() % 10), (rand() % 46) - 50);
			fishPosition[j - 2] = glm::vec3((rand() % 46) - 3, (rand() % 10), (rand() % 46) - 1);
			fishPosition[j - 3] = glm::vec3((rand() % 46) - 50, (rand() % 10), (rand() % 46) - 1);
			fishPosition[j] = glm::vec3((rand() % 46) - 3, (rand() % 10), (rand() % 46) - 50);
		}

		if (j < VALUE_OF_BOOBLE)
		{
			booblePosition[j - 1] = glm::vec3((rand() % 46) - 50, (rand() % 8) - 10, (rand() % 46) - 50);
			booblePosition[j - 2] = glm::vec3((rand() % 46) - 3, (rand() % 8) - 10, (rand() % 46) - 1);
			booblePosition[j - 3] = glm::vec3((rand() % 46) - 50, (rand() % 8) - 10, (rand() % 46) - 1);
			booblePosition[j] = glm::vec3((rand() % 46) - 3, (rand() % 8) - 10, (rand() % 46) - 50);
		}
		j += 3;
	}
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Podwodny Program");
	glewInit();
	init();
	glutKeyboardFunc(keyboardPressed);
	glutKeyboardUpFunc(keyboardUp);

	glutSpecialFunc(keyboardSpecialPressed);
	glutSpecialUpFunc(keyboardSpecialUp);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
