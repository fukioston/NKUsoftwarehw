#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

const double M_PI = 3.14159265358979323846;
float cameraPosX = 0.0f, cameraPosY = 2.0f, cameraPosZ = 10.0f; // 摄像机的位置
float cameraLookAtX = 0.0f, cameraLookAtY = 1.0f, cameraLookAtZ = -100.0f; // 摄像机正在看向的点
float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f; // 摄像机的上向量
float cameraRotX = 0.0f, cameraRotY = 0.0f, cameraRotZ = 0.0f;
class Object {
public:
	GLfloat posx, posy, posz;
	GLfloat centerx, centery, centerz;
	GLfloat upx, upy, upz;
	GLfloat rightx, righty, rightz;
	GLfloat rotx, roty,rotz;
public:
	Object(float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i)
	{
		posx = a; posy = b; posz = c;
		rotx = 0.0f;
		roty = 0.0f;
		rotz = 0.0f;

	}
	void display() {
		glColor3f(1.0f, 0.5f, 0.5f); // 设置茶壶的颜色
		glPushMatrix(); // 保存当前的变换矩阵
		glTranslatef(posx, posy, posz); // 移动到指定位置
		glRotatef(rotx, 1.0f, 0.0f, 0.0f);
		glRotatef(roty, 0.0f, 1.0f, 0.0f);
		glRotatef(rotz, 0.0f, 0.0f, 1.0f);
		glutWireTeapot(1.0f); // 绘制大小为 1.0 的茶壶
		glPopMatrix(); // 恢复之前的变换矩阵
	}
};

static Object teapot1(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
static Object teapot2(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
static Object* teapot = &teapot1;

void drawPlane() {
	glBegin(GL_LINES);
	for (int i = -100.0f; i < 100.f; i += 1) {
		if (i == 0) {
			glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			glColor3f(0.0f, 1.0f, 0.0f);
		}
		glVertex3f(i, 0.0f, 100.0f);
		glVertex3f(i, 0.0f, -100.0f);
		glVertex3f(100.0f, 0.0f, i);
		glVertex3f(-100.0f, 0.0f, i);
	}
	glEnd();
}

void display(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)800 / (GLfloat)600, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 应用相机旋转
	glRotatef(cameraRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(cameraRotY, 0.0f, 1.0f, 0.0f);
	glRotatef(cameraRotZ, 0.0f, 0.0f, 1.0f);
	// 应用相机位置
	gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
		cameraPosX, cameraPosY, 0.0f,
		0.0f, 1.0f, 0.0f);

	drawPlane();
	teapot1.display();
	teapot2.display();

	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.0f, 20.0f);
}

void onKeyPress(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		teapot = &teapot1;
		break;
	case '2':
		teapot = &teapot2;
		break;
	default:
		break;
	}
	const float moveSpeed = 0.1f;
	const float rotSpeed = 1.0f;
	switch (key) {
	case 'a': case 'A':
		cameraPosX -= moveSpeed;
		break;
	case 'd': case 'D':
		cameraPosX += moveSpeed;
		break;
	case 'w': case 'W':
		cameraPosZ -= moveSpeed;
		break;
	case 's': case 'S':
		cameraPosZ += moveSpeed;
		break;
	case 'q': case 'Q':
		cameraPosY += moveSpeed;
		break;
	case 'e': case 'E':
		cameraPosY -= moveSpeed;
		break;
	case 'j': case 'J':
		cameraRotY -= rotSpeed;
		break;
	case 'l': case 'L':
		cameraRotY += rotSpeed;
		break;
	case 'i': case 'I':
		cameraRotX -= rotSpeed;
		break;
	case 'k': case 'K':
		cameraRotX += rotSpeed;
		break;
	case 'o': case 'O':
		cameraRotZ += rotSpeed;
		break;
	case 'P': case 'p':
		cameraRotZ -= rotSpeed;
		break;
	case 'b': case 'B':
		teapot->posx -= moveSpeed;
		break;
	case 'm': case 'M':
		teapot->posx += moveSpeed;
		break;
	case 'h': case 'H':
		teapot->posz -= moveSpeed;
		break;
	case 'n': case 'N':
		teapot->posz += moveSpeed;
		break;
	case ',':
		teapot->posy -= moveSpeed;
		break;
	case '.':
		teapot->posy += moveSpeed;
		break;
	case '3':
		teapot->rotx -= rotSpeed;
		break;
	case '4':
		teapot->rotx += rotSpeed;
		break;
	case '5':
		teapot->roty += rotSpeed;
		break;
	case '6':
		teapot->roty -= rotSpeed;
		break;
	case '7':
		teapot->rotz += moveSpeed;
		break;
	case '8':
		teapot->rotz -= moveSpeed;
		break;
	}

}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Lab 10 & 11");

	glutIdleFunc(&display);
	glutDisplayFunc(&display);
	glutReshapeFunc(&reshape);
	glutKeyboardFunc(&onKeyPress);
	glutMainLoop();
	return 0;
}