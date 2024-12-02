#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cstdlib> 
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <string>
#include <Windows.h>
#include <mmsystem.h>
#include <thread>
#include <vector>
#include <algorithm>

#define GLUT_KEY_ESCAPE 27

//pi used for angles
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//game variables
bool  gameRunning = false;
int   score = 0;
int   level = 1;
float timeX  = 0.0; //increases while game running. used for synchronous annimations

std::string gameMessage = ""; //will be displayed when game over or game win

float playerX   = 0.0f;
float playerZ   = 0.75f;
float playerY   = 0.0f;
bool  isJumping = false;
//bool isMovingLeft = false;
//bool isMovingRight = false;
float maxY      = 1.7f;
//float maxX      = 1.5f;
//float minX      = -1.5f;
float speed     = 0.3f; //speed of movement of objects, will be changed to 0.6 in level 2

// same pattern: hurdle coin block for every lane. spaced with same pattern
float leftCoinZ     = -8.0f;
float centreCoinZ   = -28.0;
float rightCoinZ    = -48.0;
float leftBlockZ    = -18.0f;
float centreBlockZ  = -38.0;
float rightBlockZ   = -58.0;
float leftHurdleZ   = -3.0f;
float centreHurdleZ = -23.0f;
float rightHurdleZ  = -43.0;

float spacing = 20.0f;
float respawnPoint = 18.0f;


bool firstPerson = false; //camera depends on this
bool unlucky = true; //chooses wether player is ball or runner


//initializing variables for 3d models to draw
Model_3DS ball;
Model_3DS ball2;
Model_3DS train;
Model_3DS rail;
Model_3DS hurdle;
Model_3DS coin;
Model_3DS building;
Model_3DS tunnel;
Model_3DS tunnel2;
Model_3DS trash;
Model_3DS diamond;
Model_3DS finish_line;
Model_3DS lamp;
Model_3DS person;


GLTexture tex_ground; //this will store grass texture on floor

//camera variables
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)480 / (GLdouble)740;
GLdouble zNear = 0.1;
GLdouble zFar = 120;

//light variables
GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 50.0f, 100.0f, 0.0f, 0.0f };
GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };

GLuint tex; //this will store sky texture in background


class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(0, 8, 25); //initially above and behind player
Vector At(0, 0, 0); //initially looking towards origin
Vector Up(0, 1, 0);

int cameraZoom = 0; //used to zoom in and out

//Threads for sound
void playBackground() {
	PlaySound(TEXT("Sounds/background.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}
void playCrash() {
	PlaySound(TEXT("Sounds/crash.wav"), NULL, SND_ASYNC | SND_FILENAME);
}
void playPoint() {
	PlaySound(TEXT("Sounds/point.wav"), NULL, SND_ASYNC | SND_FILENAME);
}

void InitLightSource()
{
	glEnable(GL_LIGHTING); //lighting of whole program

	glEnable(GL_LIGHT0);

	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); //uses universal variable because it changes periodically to simulate sun and clouds movement
}

void drawBlock(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(0.015f, 0.015, 0.015); //logic for scaling
	//glRotatef(90.0, 0.0, 1.0, 0.0); //logic for rotating
	train.Draw();
	glPopMatrix();
}

void drawHurdle(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	if (x == 1.5f) glTranslatef(1.3f, 0.0f, 0.0f);
	if (x == -1.5f) glTranslatef(-1.3f, 0.0f, 0.0f);
	glScalef(0.005f, 0.01f, 0.01); //logic for scaling
	//glRotatef(90.0, 0.0, 1.0, 0.0); //logic for rotating
	if (level == 2) {
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glScalef(0.6f, 0.3f, 1.0);
		trash.Draw();
	}
	else {
		hurdle.Draw();
	}
	glPopMatrix();
}

void drawCoin(float x, float z) {
	glPushMatrix();
	glTranslatef(x+0.15f , 1.5f, z);
	if (x == 1.5f) glTranslatef(1.3f, 0.0f, 0.0f);
	if (x == -1.5f) glTranslatef(-1.3f, 0.0f, 0.0f);
	glScalef(0.08f, 0.08f, 0.08f); //logic for scaling
	if (level == 2) {
		glRotatef(-timeX * 100, 0, 1, 0);
		glRotatef(90.0, 1.0, 0.0, 0.0); //logic for rotating
		coin.Draw();
	}
	else {
		glScalef(0.15f, 0.15f, 0.15f); //logic for scaling
		//glRotatef(90.0, 1.0, 0.0, 0.0);
		glRotatef(-timeX * 100, 0, 1, 0);
		diamond.Draw();
	}
	glPopMatrix();
	if ((score == 19 && level ==1) || score == 39) {
		glPushMatrix();
		glTranslatef(x + 0.7f, 0.0, z + 0.5f);
		if (x == 1.5f) glTranslatef(1.3f, 0.0f, 0.0f);
		if (x == -1.5f) glTranslatef(-1.3f, 0.0f, 0.0f);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glScalef(0.05f, 0.15f, 0.15f);
		finish_line.Draw();
		glPopMatrix();
	}

}

void drawLeftHurdle(float z) {
	drawHurdle(-1.5f, 0.0f, z);
}
void drawCentreHurdle(float z) {
	drawHurdle(0.0f, 0.0f, z);
}
void drawRightHurdle(float z) {
	drawHurdle(1.5f, 0.0f, z);
}
void drawLeftBlock(float z) {
	drawBlock(-2.8f, 0.0f, z);
}
void drawCentreBlock(float z) {
	drawBlock(0.0f, 0.0f, z);
}
void drawRightBlock(float z) {
	drawBlock(2.8f, 0.0f, z);
}

void drawLeftWall() {
	glPushMatrix();
	glRotated(90, 0, 1.0, 0);
	glTranslated(0.5, 7, 8.0);
	glScaled(0.05, 0.05, 0.05);
	building.Draw();
	glPopMatrix();
}
void drawTunnel() {
	//TODO
	glPushMatrix();
	glRotated(-90, 0.0, 1.0, 0.0);
	glTranslated(-35.0, 0.0, 1.5);
	glScaled(0.1, 0.07, 0.025);
	tunnel.Draw();
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 0.0, 1.0, 0.0);
	glTranslated(-35.0, 0.0, -1.5);
	glScaled(0.1, 0.07, 0.025);
	tunnel2.Draw();
	glPopMatrix();
}
void drawRightWall() {
	glPushMatrix();
	glRotated(270, 0, 1.0, 0);
	glTranslated(0.5, 7, 8.0);
	glScaled(0.05, 0.05, 0.05);
	building.Draw();
	glPopMatrix();
}
void drawFloor() {
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -40);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -40);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.

	glPushMatrix();
	glScalef(0.08, 0.05, 0.57);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(0.0, -2.5, -35.0f);
	rail.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, -2.5, 0.0f);
	rail.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, -2.5, 35.0f);
	rail.Draw();
	glPopMatrix();
	glPopMatrix();
}
void drawLamp() {
	glPushMatrix();
	glTranslatef(-1.8, 0.0, 12.0);
	glScalef(0.3, 0.2, 0.5);
	glRotatef(90.0, 00, 1.0, 0.0);
	lamp.Draw();
	glPopMatrix();
}

void setupScene() {
	drawLeftWall();
	drawRightWall();
	drawTunnel();
	drawFloor();
	drawLamp();
}

void drawPlayer() {


	//spotlight
	glEnable(GL_LIGHT2);

	// Spotlight properties
	float flickerIntensity = 0.3f + 0.6f * sin(timeX * 10.0f); // Vary intensity between 0.3 and 0.9
	GLfloat spotlightColor[] = { 100*flickerIntensity, 0.0f, 0.0f, 1.0f }; // Red/Cyan color
	GLfloat xPos = (playerX == 0.0) ? playerX : ((playerX == 1.5f) ? playerX + 1.3f : playerX - 1.3f);
	GLfloat spotlightPosition[] = { xPos, playerY + 4.0f, playerZ + 12.0f, 1.0f }; // Slightly above the player
	GLfloat spotlightDirection[] = { 0.0f, -1.0f, 0.0f }; // Pointing downwards
	GLfloat spotlightCutoff = 25.0f; // Cone angle in degrees

	glLightfv(GL_LIGHT2, GL_DIFFUSE, spotlightColor);    
	glLightfv(GL_LIGHT2, GL_SPECULAR, spotlightColor);   
	glLightfv(GL_LIGHT2, GL_POSITION, spotlightPosition); 
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotlightCutoff);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotlightDirection); 
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 50.0f);

	if (unlucky) {
		glPushMatrix();
		glTranslatef(playerX, playerY + 0.5, playerZ + 12.0);
		if (playerX == 1.5f) glTranslatef(1.3f, 0.0, 0.0);
		if (playerX == -1.5f) glTranslatef(-1.3f, 0.0, 0.0);
		glRotatef(180, 0, 1, 0);
		//glTranslatef(-0.1f, 0.0, 0.0);
		if (static_cast<int>(timeX) % 2 == 0) {
			glScalef(1.0f, 1.0f, -1.0f);
			glRotatef(180, 0, 1, 0);
		}
		glTranslatef(-0.15f, 0.0, 0.0);
		person.Draw();
		glPopMatrix();
	}
	else {
		glPushMatrix();
		glTranslatef(playerX, playerY + 0.5, playerZ + 12.0);
		if (playerX == 1.5f) glTranslatef(1.3f, 0.0, 0.0);
		if (playerX == -1.5f) glTranslatef(-1.3f, 0.0, 0.0);
		//glRotatef(180, 0, 1, 0);
		glRotatef(-timeX * 100, 1, 0, 0);
		if (level == 2) {
			glScaled(0.005, 0.005, 0.005);
			ball.Draw();
		}
		else {
			glScaled(0.5, 0.5, 0.5);
			ball2.Draw();
		}
		glPopMatrix();
	}

	//glDisable(GL_LIGHT2);
}

void resetGameVariables() {
	playerX = 0.0f;
	playerZ = 0.75f;
	playerY = 0.0f;
	isJumping = false;
	speed = 0.3f;

	leftCoinZ = -3.0f;
	centreCoinZ = -23.0;
	rightCoinZ = -43.0;
	leftBlockZ = -55.0f;
	centreBlockZ = -35.0;
	rightBlockZ = -75.0;
	leftHurdleZ = -21.0f;
	centreHurdleZ = -41.0f;
	rightHurdleZ = -61.0;

	firstPerson = false;

	level = 1;
	timeX = 0.0;

	lightColor[0] = 1.0f; lightColor[1] = 1.0f; lightColor[2] = 1.0f; lightColor[3] = 1.0f;
	lightPosition[0] = 50.0f; lightPosition[1] = 100.0f; lightPosition[2] = 0.0f; lightPosition[3] = 1.0f;
	lightIntensity[0] = 0.7f; lightIntensity[1] = 0.7f; lightIntensity[2] = 0.7f; lightIntensity[3] = 1.0f;
	light_position[0] = 0.0f; light_position[1] = 10.0f; light_position[2] = 0.0f; light_position[3] = 1.0f;

}
void gameOver() {
	gameRunning = false;
	glutPostRedisplay(); //change scene
	PlaySound(TEXT("Sounds/lose.wav"), NULL, SND_ASYNC | SND_FILENAME);
	gameMessage = "GAME OVER! Score: " + std::to_string(score) + ". Press SPACE to try again.";
	resetGameVariables();
}
void gameWon() {
	gameRunning = false;
	glutPostRedisplay(); //change scene
	PlaySound(TEXT("Sounds/win.wav"), NULL, SND_ASYNC | SND_FILENAME);
	gameMessage = "GAME WON! Score:" + std::to_string(score) + ". Press SPACE to play again."; // Convert score to string
	resetGameVariables();
}

void respawnObjects() {

	//if item moves beyond respawnPoint it respawns behind the furthest item of it's kind by a value "spacing"

	//spawn coins
	if (leftCoinZ > respawnPoint) {//18.0f) {
		leftCoinZ = ((centreCoinZ > rightCoinZ) ? rightCoinZ : centreCoinZ) - spacing;//20.0f;
	}
	if (centreCoinZ > respawnPoint) {//18.0f) {
		centreCoinZ = ((leftCoinZ > rightCoinZ) ? rightCoinZ : leftCoinZ) - spacing;//20.0f;
	}
	if (rightCoinZ > respawnPoint) {//18.0f) {
		rightCoinZ = ((leftCoinZ > centreCoinZ) ? centreCoinZ : leftCoinZ) - spacing;//20.0f;
	}

	//spawn blocks
	if (centreBlockZ > respawnPoint) {//18.0f) {
		centreBlockZ = ((leftBlockZ > rightBlockZ) ? rightBlockZ : leftBlockZ) - spacing;//20.0f;
	}
	if (leftBlockZ > respawnPoint) {//18.0f) {
		leftBlockZ = ((centreBlockZ > rightBlockZ) ? rightBlockZ : centreBlockZ) - spacing;//20.0f;
	}
	if (rightBlockZ > respawnPoint) {//18.0f) {
		rightBlockZ = ((leftBlockZ > centreBlockZ) ? centreBlockZ : leftBlockZ) - spacing;//20.0f;
	}

	//spawn hurdles
	if (centreHurdleZ > respawnPoint) {//18.0f) {
		centreHurdleZ = ((leftHurdleZ > rightHurdleZ) ? rightHurdleZ : leftHurdleZ) - spacing;//20.0f;
	}
	if (leftHurdleZ > respawnPoint) {//18.0f) {
		leftHurdleZ = ((centreHurdleZ > rightHurdleZ) ? rightHurdleZ : centreHurdleZ) - spacing;//20.0f;
	}
	if (rightHurdleZ > respawnPoint) {//18.0f) {
		rightHurdleZ = ((leftHurdleZ > centreHurdleZ) ? centreHurdleZ : leftHurdleZ) - spacing;//20.0f;
	}
}
void handleJumping() {
	if (isJumping && playerY < maxY) {
		playerY += 0.1f;
	}
	else {
		if (playerY > 0.0f && !isJumping) {
			playerY -= 0.1f;
		}
	}
	if (playerY >= maxY) {
		isJumping = false;
	}
}
void handleMovement() {
	//if (isMovingLeft) {
	//	if (playerX == 0.0f || playerX <= -2.8f) {
	//		isMovingLeft = false;
	//	}
	//	else {
	//		playerX -= 0.1f;
	//	}
	//}
	//if (isMovingRight) {
	//	if (playerX == 0.0f || playerX >= 2.8f) {
	//		isMovingRight = false;
	//	}
	//	else {
	//		playerX += 0.1f;
	//	}
	//}
}
void checkCollisions() {

	//all values here are trial and error based on the model sizes and positions

	//check collisions with coins
	if (playerX == 0.0f && (std::abs(playerZ + 12.0 - centreCoinZ) < speed)) {
		score++;
		centreCoinZ = 23.9f;
		std::cout << "colided with coin" << std::endl;
		std::thread t3(playPoint);
		t3.detach();
		if (score == 20 && level == 1) {
			level = 2;
			speed *= 2;
			gameRunning = false;
			firstPerson = false;
			lightColor[0] = 1.0f; // Red
			lightColor[1] = 191.0f / 255.0f; // Green
			lightColor[2] = 55.0f / 255.0f; // Blue

			lightIntensity[0] = 0.1f; // Red
			lightIntensity[1] = 0.1f; // Green
			lightIntensity[2] = 0.1f; // Blue

			// Request a scene update
			glutPostRedisplay();
		}
		if (score == 40) {
			gameWon();
		}
	}
	if (playerX == -1.5f && (std::abs(playerZ + 12.0 - leftCoinZ) < speed)) {
		score++;
		leftCoinZ = 23.9f;
		std::cout << "colided with coin" << std::endl;
		std::thread t3(playPoint);
		t3.detach();
		if (score == 20 && level == 1) {
			level = 2;
			speed *= 2;
			gameRunning = false;
			firstPerson = false;
			lightColor[0] = 1.0f; // Red
			lightColor[1] = 191.0f / 255.0f; // Green
			lightColor[2] = 55.0f / 255.0f; // Blue

			lightIntensity[0] = 0.1f; // Red
			lightIntensity[1] = 0.1f; // Green
			lightIntensity[2] = 0.1f; // Blue

			// Request a scene update
			glutPostRedisplay();
		}
		if (score == 40) {
			gameWon();
		}
	}
	if (playerX == 1.5f && (std::abs(playerZ + 12.0 - rightCoinZ) < speed)) {
		score++;
		rightCoinZ = 23.9f;
		std::cout << "colided with coin" << std::endl;
		std::thread t3(playPoint);
		t3.detach();
		if (score == 20 && level == 1) {
			level = 2;
			speed *= 2;
			gameRunning = false;
			firstPerson = false;
			lightColor[0] = 1.0f; // Red
			lightColor[1] = 191.0f / 255.0f; // Green
			lightColor[2] = 55.0f / 255.0f; // Blue

			lightIntensity[0] = 0.1f; // Red
			lightIntensity[1] = 0.1f; // Green
			lightIntensity[2] = 0.1f; // Blue

			// Request a scene update
			glutPostRedisplay();
		}
		if (score == 40) {
			gameWon();
		}
	}
	//check collisions with blocks
	if (playerX == 0.0f && playerZ <= centreBlockZ - 7.0 && playerZ >= centreBlockZ - 15.0) {
		std::cout << "colided with block" << std::endl;
		centreBlockZ = ((leftBlockZ > rightBlockZ) ? rightBlockZ : leftBlockZ) - 20.0f;
		gameOver();
	}
	if (playerX == -1.5f && playerZ <= leftBlockZ - 7.0 && playerZ >= leftBlockZ - 15.0) {
		std::cout << "colided with block" << std::endl;
		leftBlockZ = ((centreBlockZ > rightBlockZ) ? rightBlockZ : centreBlockZ) - 20.0f;
		gameOver();
	}
	if (playerX == 1.5f && playerZ <= rightBlockZ - 7.0 && playerZ >= rightBlockZ - 15.0) {
		std::cout << "colided with block" << std::endl;
		rightBlockZ = ((leftBlockZ > centreBlockZ) ? centreBlockZ : leftBlockZ) - 20.0f;
		gameOver();
	}
	//check collisions with hurdles
	if (playerX == 0.0f && playerZ <= centreHurdleZ - 11.0 && playerZ >= centreHurdleZ - (11.0 + level*2.0) && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		centreHurdleZ = ((leftHurdleZ > rightHurdleZ) ? rightHurdleZ : leftHurdleZ) - 20.0f;
		score--;
		std::thread t2(playCrash);
		t2.detach();
	}
	if (playerX == -1.5f && playerZ <= leftHurdleZ - 11.0 && playerZ >= leftHurdleZ - (11.0 + level * 2.0) && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		leftHurdleZ = ((centreHurdleZ > rightHurdleZ) ? rightHurdleZ : centreHurdleZ) - 20.0f;
		score--;
		std::thread t2(playCrash);
		t2.detach();
	}
	if (playerX == 1.5f && playerZ <= rightHurdleZ - 11.0 && playerZ >= rightHurdleZ - (11.0 + level * 2.0) && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		rightHurdleZ = ((leftHurdleZ > centreHurdleZ) ? centreHurdleZ : leftHurdleZ) - 20.0f;
		score--;
		std::thread t2(playCrash);
		t2.detach();
	}
}
void moveObjects() {
	leftCoinZ += speed;
	centreCoinZ += speed;
	rightCoinZ += speed;
	leftBlockZ += speed;
	centreBlockZ += speed;
	rightBlockZ += speed;
	leftHurdleZ += speed;
	centreHurdleZ += speed;
	rightHurdleZ += speed;
}
void renderText(const char* text, float x, float y) {
	// Save the current projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set orthographic projection for text rendering

	// Save the model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Render the text
	glColor3f(0.0f, 0.0f, 0.0f); // White color for text
	glRasterPos2f(x, y);         // Set text position
	for (const char* c = text; *c != '\0'; ++c) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
	char* text2 = "Press SPACE to start the game";
	if (!gameRunning && score != 20) {
		glRasterPos2f(x-0.3, y-0.05f);         // Set text position
		for (const char* c2 = text2; *c2 != '\0'; ++c2) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c2);
		}
	}
	char* text4 = "Press SPACE to start Level 2.";
	if (!gameRunning && score == 20) {
		glRasterPos2f(x - 0.3, y - 0.05f);         // Set text position
		for (const char* c4 = text4; *c4 != '\0'; ++c4) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c4);
		}
	}
	std::string scoreString = "Score: " + std::to_string(score); // Construct the score string
	const char* score = scoreString.c_str(); // Convert to const char* if required
	if (gameRunning) {
		glRasterPos2f(x + 0.05, y - 0.05f); // Set text position
		for (const char* c3 = score; *c3 != '\0'; ++c3) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c3); // Render each character
		}
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	// Restore the model-view matrix
	glPopMatrix();

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Return to model-view matrix mode
	glMatrixMode(GL_MODELVIEW);
}
void updateLightPosition() {
	// Use timeX to calculate the light's position in circular motion
	lightPosition[0] = 50.0f * cos(timeX); // X-coordinate
	lightPosition[2] = 50.0f * sin(timeX); // Z-coordinate

	// Apply the updated light position
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void timer(int) {
	if (gameRunning) {

		if (firstPerson) {
			Eye.y = playerY + 1.5f;
			Eye.x = (playerX == 0.0)?0.0:((playerX == -1.5)?-2.8:2.8);
			Eye.z = 12;
			At.x = (playerX == 0.0) ? 0.0 : ((playerX == -1.5) ? -2.8 : 2.8);
			glLoadIdentity(); //Clear Model_View Matrix

			gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

			glLightfv(GL_LIGHT0, GL_POSITION, light_position);

			glutPostRedisplay();
		}
		else {
			Eye.y = 8;
			Eye.z = 25;
			Eye.x = 0;
			At.x = 0;
			glLoadIdentity(); //Clear Model_View Matrix

			gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

			glLightfv(GL_LIGHT0, GL_POSITION, light_position);

			glutPostRedisplay();
		}


		timeX += 0.1f;

		if (timeX >= 2 * M_PI) timeX -= 2 * M_PI; //time is always between 0 and 2pi because it is used in angles

		updateLightPosition();
		moveObjects();
		respawnObjects();

		handleJumping();
		handleMovement();

		checkCollisions();
	}
	else {
		//if game lose or win return to original position of camera to view text
		Eye.y = 8;
		Eye.z = 25;
		Eye.x = 0;
		At.x = 0;
		glLoadIdentity(); //Clear Model_View Matrix

		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glutPostRedisplay();
	}

	glutPostRedisplay();
	glutTimerFunc(20, timer, 0);
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);


	setupScene();

	drawPlayer();
	drawCentreBlock(centreBlockZ);
	drawLeftBlock(leftBlockZ);
	drawRightBlock(rightBlockZ);
	drawCoin(1.5, rightCoinZ);
	drawCoin(-1.5, leftCoinZ);
	drawCoin(0.0, centreCoinZ);
	drawLeftHurdle(leftHurdleZ);
	drawCentreHurdle(centreHurdleZ);
	drawRightHurdle(rightHurdleZ);

	//this block displays sky in background as sphere and sene is inside sphere
	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	renderText("Urban Runner", -0.22f, 0.9f);

	if (!gameMessage.empty()) {
		if (gameMessage.find("GAME WON!") != std::string::npos) {
			glClearColor(0.0f, 7.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0f, 0.0f, 0.0f); // Set text color to green for "Game Won!"
		}
		else {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(7.0f, 0.0f, 0.0f); // Set text color to red for other messages
		}

		glRasterPos2f(-6.0f, 0.0f); // Position the text in the center

		for (const char& c : gameMessage) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Draw each character
		}
	}
	glutSwapBuffers();
}

// hamada
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	switch (key) {
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'z':
		if (gameRunning) {
			firstPerson = true;
		}
		break;
	case 'x':
		if (gameRunning) {
			firstPerson = false;
		}
		break;
	case 'p':  //pause
		if (speed == 0.0) {
			speed = level*0.3;
		}
		else {
			speed = 0.0;
		}
		break;
	case '=': //right
		if (gameRunning && playerX < 1.5) {
			playerX += 1.5f;
		}
		break;
	case '-': //left
		if (gameRunning && playerX > -1.5) {
			playerX -= 1.5f;
		}
		break;
	case ' ': //start game and jump
		if (!gameRunning) {
			gameRunning = true;
			if (!gameMessage.empty()) { //if not between level one and two then restart (game won or lost)
				gameMessage = "";
				score = 0;
			}
		}
		else {
			if (!isJumping) {
				isJumping = true;
			}
		}
		break;
	case 'l': //toggle lights
		lightIntensity[0] = (lightIntensity[0] == 0.7f) ? 0.1f : 0.7f;
		lightIntensity[1] = (lightIntensity[1] == 0.7f) ? 0.1f : 0.7f;
		lightIntensity[2] = (lightIntensity[2] == 0.7f) ? 0.1f : 0.7f;
		break;
	case 'u': //toggle player (runner/ball)
		unlucky = !unlucky;
		break;
	case GLUT_KEY_ESCAPE: //close game
		exit(EXIT_SUCCESS);
		break;
	}

	glutPostRedisplay();
}

void Motion(int x, int y)
{
	y = 740 - y;
	if (cameraZoom - y > 0) { //zoom in
		Eye.z += -0.1;
	} else { //zoom out
		if(Eye.z < 30) Eye.z += 0.1;
	}
	cameraZoom = y;
	glLoadIdentity();	//Clear Model_View Matrix
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glutPostRedisplay();	//Re-draw scene 
}
void Mouse(int button, int state, int x, int y)
{
	y = 740 - y;

	if (state == GLUT_DOWN)
	{
		//cameraZoom = y;
		if (button == GLUT_LEFT_BUTTON) //jump
		{
			if (gameRunning) {
				if (!isJumping) {
					isJumping = true;
				}
			}
		}
		else if (button == GLUT_RIGHT_BUTTON) //toggle camera view
		{
			if (gameRunning) {
				firstPerson = !firstPerson;
			}
		}
	}
}
void Reshape(int w, int h) {
	if (h == 0) {
		h = 1;
	}

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)w / (GLdouble)h, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

}
void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT: // Move player to the left
		if (gameRunning && playerX > -1.5) {
			playerX -= 1.5f;
			/*playerX -= 0.1f;
			isMovingLeft = true;*/
		}
		break;

	case GLUT_KEY_RIGHT: // Move player to the right
		if (gameRunning && playerX < 1.5) {
			playerX += 1.5f;
			/*
			playerX += 0.1f;
			isMovingRight = true;*/
		}
		break;

	default:
		break;
	}
	glutPostRedisplay(); // Trigger a redisplay to update the scene
}

void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}
void LoadAssets()
{
	//train
	train.Load("Models/imported/train2/Locomotive TGM3 N120711.3DS");
	//fence
	hurdle.Load("Models/imported/fence/Fence N130912.3DS");
	//coin
	coin.Load("Models/imported/coin/Coin 1.3DS");
	//diamond
	diamond.Load("Models/imported/diamond/Mirror Visionnaire Forma Mentis N251119.3ds");
	//lamp
	lamp.Load("Models/imported/lamp/Lamppost N131219.3ds");
	//building
	building.Load("Models/imported/building/Building 9-storey N121121.3ds");
	//ball
	ball.Load("Models/imported/ball/Basketball Molten N221222.3ds");
	ball2.Load("Models/imported/Pallone/Ball 3DS.3ds");
	//person
	person.Load("Models/imported/person/Man running N090413.3DS");
	//trash
	trash.Load("Models/imported/trash/Trash box N240315.3DS");
	//tunnel
	tunnel.Load("Models/imported/bridge/Bridge Ponte De Tijolos N250515.3DS");
	tunnel2.Load("Models/imported/bridge/Bridge Ponte De Tijolos N250515.3DS");
	//railway track
	rail.Load("Models/imported/grill/Convector grill floor radiator N081220.3ds");
	//finish line
	finish_line.Load("Models/imported/finishline/120512_fence_museum_barrier_rope.3DS");


	// Loading texture files for sky and ground
	tex_ground.Load("Textures/grass.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	InitLightSource();
	InitMaterial();
	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

void runPythonScript() {
	system("\python script.py");
}

void main(int argc, char** argv)
{
	std::thread pythonThread(runPythonScript);
	pythonThread.detach();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(480, 740);

	glutInitWindowPosition(500, 50);

	glutCreateWindow("Urban Runner");

	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutSpecialFunc(SpecialKey);

	init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}