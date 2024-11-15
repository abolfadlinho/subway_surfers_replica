#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#define _USE_MATH_DEFINES
#include <cstdlib> 
#include <glut.h>
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
#define DEG2RAD(a) (a * 0.0174532925)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//game variables
bool  gameRunning   = true;
float timeRemaining = 600.0f;
int   score         = 0;
int   level         = 1;

std::string gameMessage = "";

float playerX   = 0.0f;
float playerZ   = 0.75f;
float playerY   = 0.0f;
bool  isJumping = false;
float maxY      = 0.7f;
float maxX      = 1.5f;
float minX      = -1.5f;
float speed     = 0.1f;

float leftCoinZ     = -3.0f;
float centreCoinZ   = -3.5;
float rightCoinZ    = -4.0;
float leftBlockZ    = -10.0f;
float centreBlockZ  = -5.0;
float rightBlockZ   = -15.0;
float leftHurdleZ   = -7.0f;
float centreHurdleZ = -12.0f;
float rightHurdleZ  = -17.0;

float blockLength = 4.0f;

bool firstPerson = false;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 0.0f, float eyeY = 2.15f, float eyeZ = 1.5f, float centerX = 0.0f, float centerY = 0.5f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}

	void setView(char viewType) {
		switch (viewType) {
			case 'F': // Front View
				eye = Vector3f(0.0f, 1.0f, 3.0f);  // Camera positioned along the Z-axis
				center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
				up = Vector3f(0.0f, 1.0f, 0.0f); // Y-axis up
				break;
			case 'T': // Top View
				eye = Vector3f(0.0f, 2.0f, 0.0f);  // Camera positioned above the scene
				center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
				up = Vector3f(0.0f, 0.0f, -1.0f); // Z-axis up (camera looking down)
				break;
			case 'R': // Side View
				eye = Vector3f(2.0f, 1.0f, 0.0f);  // Camera positioned on the X-axis
				center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
				up = Vector3f(0.0f, 1.0f, 0.0f); // Y-axis up
				break;
			case 'Z': // first person
				eye = Vector3f(playerX, playerY + 0.6f, 0.15f);
				center = Vector3f(playerX, 1.0f, -1.0f);
				firstPerson = true;
				break;
			case 'X': // third person
				eye = Vector3f(0.0f, 2.15f, 1.15f);
				center = Vector3f(0.0f, 0.5f, 0.0f);
				firstPerson = false;
				break;
		}
	}
};

Camera camera;

void drawBlock(float x, float y, float z) {
	glColor3f(0.0f, 0.0f, 0.0f); // Set the color to black

	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(1.0f, 2.5f, blockLength);
	glutSolidCube(1.0f);

	glPopMatrix();
}

void drawHurdle(float x, float y, float z) {
	glColor3f(0.0f, 7.0f, 0.0f);

	glPushMatrix();
	glTranslatef(x, y, z);

	glScalef(1.0f, 0.6f, 0.02f);

	glutSolidCube(1.0f);

	glPopMatrix();
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

void drawCoin(float x, float z) {
	glColor3f(7.0f, 7.0f, 0.0f); // Set the color to yellow
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glScalef(1.0f, 1.0f, 0.1f);
	glTranslatef(0.0f, 0.4f, 0.0f);
	glutSolidTorus(0.15, 0.15, 10, 10);
	glPopMatrix();

}

void drawLeftBlock(float z) {
	drawBlock(-1.5f, 0.0f, z);
}

void drawCentreBlock(float z) {
	drawBlock(0.0f, 0.0f, z);
}

void drawRightBlock(float z) {
	drawBlock(1.5f, 0.0f, z);
}

void drawLeftWall(double thickness) {

	glColor3f(0.85f, 0.85f, 0.85f);
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.5, 3.25, -5.0);
	glScaled(1.0, thickness, 16.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(0.85f, 0.85f, 0.85f);
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(1.5, 3.25, -5.0);
	glScaled(1.0, thickness, 16.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}

void drawTunnel() {

	glColor3f(0.85f, 0.85f, 0.85f);
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0.0, 13.5, 1.0);
	glScaled(6.5, 1.0f, 2.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	//glTranslated(0.0, 12.5 , 1.0);
	glColor3f(0.0f, 0.0f, 0.0f); // Set the color to yellow
	glPushMatrix();
	glScalef(1.1f, 1.2f, 0.1f);
	glTranslatef(0.0f, -1.5f, -130.0f);
	glutSolidTorus(1.5, 1.5, 20, 20);
	glPopMatrix();
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}

void drawRightWall(double thickness) {
	glColor3f(0.85f, 0.85f, 0.85f);
	glPushMatrix();
	glRotated(90, 0.0, 0.0, 1.0);
	glTranslated(0.0, -1.0, -5.0);
	glTranslated(0.5, -2.25, 0.0);
	glScaled(1.0, thickness, 16.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(0.85f, 0.85f, 0.85f);
	glPushMatrix();
	glRotated(90, 0.0, 0.0, 1.0);
	glTranslated(0.0, -1.0, -5.0);
	glTranslated(1.5, -2.25, 0.0);
	glScaled(1.0, thickness, 16.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}

void drawFloor(double thickness) {
	glColor3f(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0f);
	glScaled(6.5, thickness, 16.5);
	glRotatef(270.0, 0.0f, 0.0f, 1.0f);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the solids
	double xPositions[] = { -1.5, 0.0, 1.5 }; // Z-positions for the three solids
	double solidWidth = 1.0; // Width of each solid
	double solidHeight = 0.05; // Height of each solid
	double solidDepth = 16.5; // Depth of each solid

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslated(xPositions[i], solidHeight / 2.0, -5.0); // Position each solid
		glScaled(solidWidth, solidHeight, solidDepth);       // Scale the solid
		glutSolidCube(1);                                   // Draw the solid
		glPopMatrix();
	}

	// Reset color to white
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawHuman() {

	// Head
	glPushMatrix();
	glTranslatef(0.0f, 1.35f, 0.0f);
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidSphere(0.1, 15, 15); // Head as a sphere
	glPopMatrix();

	// Body
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f);
	glScalef(0.2f, 0.5f, 0.1f); // Body as a cuboid
	glColor3f(7.0f, 0.0f, 0.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Left Arm
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0, -2.2f, 0.0);
	glPushMatrix();
	glTranslatef(-0.15f, 1.1f, 0.0f);
	glRotatef(20.0, 0, 0, 1);
	glScalef(0.05f, 0.3f, 0.05f); // Arm as a cylinder
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Arm
	glPushMatrix();
	glTranslatef(0.15f, 1.1f, 0.0f);
	glRotatef(-20.0, 0, 0, 1);
	glScalef(0.05f, 0.3f, 0.05f); // Arm as a cylinder
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glRotatef(90.0, 1.0, 1.0, 0.0);
	camera.look();
}

void setupScene() {
	glColor3f(1.0f, 1.0f, 1.0f);
	drawLeftWall(0.02);
	drawRightWall(0.02);
	drawTunnel();
	drawFloor(0.02);
}

void drawPlayer() {
	glPushMatrix();
	glTranslatef(playerX, playerY-0.6, playerZ);
	drawHuman();
	glPopMatrix();
}

void gameOver() {
	gameMessage = "GAME LOSE! Score: " + std::to_string(score); // Convert score to string
}

void gameWon() {
	gameMessage = "GAME WON! Score:" + std::to_string(score); // Convert score to string
}

void respawnObjects() {
	//spawn coins
	if (leftCoinZ > 3.0f) {
		//leftCoinZ = -3.0f;
		leftCoinZ = ((centreCoinZ > rightCoinZ) ? rightCoinZ : centreCoinZ) - 8.0f;
	}
	if (centreCoinZ > 3.0f) {
		//centreCoinZ = -3.5f;
		centreCoinZ = ((leftCoinZ > rightCoinZ) ? rightCoinZ : leftCoinZ) - 8.0f;
	}
	if (rightCoinZ > 3.0f) {
		//rightCoinZ = -4.0f;
		rightCoinZ = ((leftCoinZ > centreCoinZ) ? centreCoinZ : leftCoinZ) - 8.0f;
	}
	//spawn blocks
	if (centreBlockZ > 4.0f) {
		centreBlockZ = ((leftBlockZ > rightBlockZ) ? rightBlockZ : leftBlockZ) - 8.0f;
	}
	if (leftBlockZ > 4.0f) {
		leftBlockZ = ((centreBlockZ > rightBlockZ) ? rightBlockZ : centreBlockZ) - 8.0f;
	}
	if (rightBlockZ > 4.0f) {
		rightBlockZ = ((leftBlockZ > centreBlockZ) ? centreBlockZ : leftBlockZ) - 8.0f;
	}
	//spawn hurdles
	if (centreHurdleZ > 3.0f) {
		centreHurdleZ = ((leftHurdleZ > rightHurdleZ) ? rightHurdleZ : leftHurdleZ) - 8.0f;
	}
	if (leftHurdleZ > 3.0f) {
		leftHurdleZ = ((centreHurdleZ > rightHurdleZ) ? rightHurdleZ : centreHurdleZ) - 8.0f;
	}
	if (rightHurdleZ > 3.0f) {
		rightHurdleZ = ((leftHurdleZ > centreHurdleZ) ? centreHurdleZ : leftHurdleZ) - 8.0f;
	}
}

void handleJumping() {
	// jumping
	if (isJumping && playerY < maxY) {
		playerY += 0.06f;
		if (firstPerson) camera.setView('Z');
	}
	else {
		if (playerY > 0.0f && !isJumping)
			playerY -= 0.06f;
		if (firstPerson) camera.setView('Z');
	}
	if (playerY >= maxY) {
		isJumping = false;
	}
}

void checkCollisions() {
	//check collisions with coins
	if (playerX == 0.0f && (std::abs(playerZ - centreCoinZ) < 0.1)) {
		score++;
		centreCoinZ = 2.9f;
		std::cout << "colided with coin" << std::endl;
		if (score == 20) {
			level = 2;
			speed = 0.2;
		}
	}
	if (playerX == -1.5f && (std::abs(playerZ - leftCoinZ) < 0.1)) {
		score++;
		leftCoinZ = 2.9f;
		std::cout << "colided with coin" << std::endl;
		if (score == 20) {
			level = 2;
			speed = 0.2;
		}
	}
	if (playerX == 1.5f && (std::abs(playerZ - rightCoinZ) < 0.1)) {
		score++;
		rightCoinZ = 2.9f;
		std::cout << "colided with coin" << std::endl;
		if (score == 20) {
			level = 2;
			speed = 0.2;
		}
	}
	//check collisions with blocks
	if (playerX == 0.0f && playerZ <= centreBlockZ + (0.5 * blockLength) && playerZ >= centreBlockZ - (0.5 * blockLength)) {
		std::cout << "colided with block" << std::endl;
		centreBlockZ = ((leftBlockZ > rightBlockZ) ? rightBlockZ : leftBlockZ) - 8.0f;
	}
	if (playerX == -1.5f && playerZ <= leftBlockZ + (0.5 * blockLength) && playerZ >= leftBlockZ - (0.5 * blockLength)) {
		std::cout << "colided with block" << std::endl;
		leftBlockZ = ((centreBlockZ > rightBlockZ) ? rightBlockZ : centreBlockZ) - 8.0f;
	}
	if (playerX == 1.5f && playerZ <= rightBlockZ + (0.5 * blockLength) && playerZ >= rightBlockZ - (0.5 * blockLength)) {
		std::cout << "colided with block" << std::endl;
		rightBlockZ = ((leftBlockZ > centreBlockZ) ? centreBlockZ : leftBlockZ) - 8.0f;
	}
	//check collisions with hurdles
	if (playerX == 0.0f && playerZ <= centreHurdleZ + 0.1f && playerZ >= centreHurdleZ - 0.1f && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		centreHurdleZ = ((leftHurdleZ > rightHurdleZ) ? rightHurdleZ : leftHurdleZ) - 8.0f;
	}
	if (playerX == -1.5f && playerZ <= leftHurdleZ + 0.1f && playerZ >= leftHurdleZ - 0.1f && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		leftHurdleZ = ((centreHurdleZ > rightHurdleZ) ? rightHurdleZ : centreHurdleZ) - 8.0f;
	}
	if (playerX == 1.5f && playerZ <= rightHurdleZ + 0.1f && playerZ >= rightHurdleZ - 0.1 && playerY < 0.5) {
		std::cout << "colided with hurdle" << std::endl;
		rightHurdleZ = ((leftHurdleZ > centreHurdleZ) ? centreHurdleZ : leftHurdleZ) - 8.0f;
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

void timer(int) {
	if (gameRunning) {
		timeRemaining -= 0.02f;

		moveObjects();
		glutPostRedisplay();
		glutTimerFunc(20, timer, 0);

		respawnObjects();
		
		handleJumping();

		checkCollisions();
	}
	else {
		std::cout << "Game ended" << std::endl;
	}

}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setupCamera();
	setupLights();
	setupScene();
	drawPlayer();
	drawLeftBlock(leftBlockZ);
	drawCentreBlock(centreBlockZ);
	drawRightBlock(rightBlockZ);
	drawCoin(0.0, centreCoinZ);
	drawCoin(-1.5, leftCoinZ);
	drawCoin(1.5, rightCoinZ);
	drawLeftHurdle(leftHurdleZ);
	drawCentreHurdle(centreHurdleZ);
	drawRightHurdle(rightHurdleZ);


	if (!gameMessage.empty()) {
		if (gameMessage.find("GAME WON!") != std::string::npos) {
			glClearColor(7.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0f, 7.0f, 0.0f); // Set text color to green for "Game Won!"
		}
		else {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(7.0f, 0.0f, 0.0f); // Set text color to red for other messages
		}

		glRasterPos2f(-0.2f, 0.0f); // Position the text in the center

		for (const char& c : gameMessage) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Draw each character
		}
	}

	glFlush();
}


// hamada
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	case 'f':  // Front view
		camera.setView('F');
		break;
	case 't':  // Top view
		camera.setView('T');
		break;
	case 'r':  // Side view
		camera.setView('R');
		break;
	case 'z':  // Side view
		firstPerson = true;
		camera.setView('Z');
		break;
	case 'x':  // Side view
		firstPerson = false;
		camera.setView('X');
		break;
	case '=': 
		if (playerX < 1.5) {
			playerX += 1.5f;
		}
		if (firstPerson) camera.setView('Z');
		break;
	case '-':
		if (playerX > -1.5) {
			playerX -= 1.5f;
		}
		if (firstPerson) camera.setView('Z');
		break;
	case ' ':
		if (!isJumping) {
			isJumping = true;
		}
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
		break;
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.68f, 0.85f, 0.90f, 1.0f);  // Set background color

}



void main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitWindowSize(480, 740);
	glutInitWindowPosition(50, 50);

	init();

	glutCreateWindow("Urban Runner");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}
