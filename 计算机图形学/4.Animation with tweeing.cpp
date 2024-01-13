#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <Windows.h>
#include <ctime>
using namespace std;

#define NUM 11
bool isPaused = false;
vector<vector<float>> startPoints(NUM, vector<float>(2));
vector<vector<float>> endPoints(NUM, vector<float>(2));
vector<vector<float>> currentPoints(NUM, vector<float>(2));
vector<vector<float>> initialCoords = { {46,6},{56,6},{56,31},{76,31},{61,63.5},{71,63.5},{51,96},{31,63.5},{41,63.5},{26,31},{46,31} };
vector<vector<float>> finalCoords = { {41,6},{56,6},{56,42},{71,42},{71,60},{56,60},{56,78},{86,78},{86,96},{66,96},{41,96} };
float interpolationParam = 0;
int frameCount = 0;
int direction = 1;
float delta = 0.1;

void draw(float param) {
    for (int i = 0; i < NUM; i++) {
        currentPoints[i][0] = startPoints[i][0] * (1 - param) + endPoints[i][0] * param;
        currentPoints[i][1] = startPoints[i][1] * (1 - param) + endPoints[i][1] * param;
    }
}

void onMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isPaused = !isPaused;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(3);
    glColor3f(0.0, 0.0, 0.0);

    glBegin(GL_LINE_LOOP);
    for (auto& point : currentPoints) {
        glVertex2fv(point.data());
    }
    glEnd();

    if (!isPaused) {
        frameCount += direction;
        interpolationParam += delta;
        if (frameCount == 10 || frameCount == 0) {
            delta = -delta;
            direction = -direction;
            interpolationParam = frameCount == 10 ? 1 : 0;
        }
        draw(interpolationParam);
    }

    Sleep(400);
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    for (int i = 0; i < NUM; i++) {
        startPoints[i] = initialCoords[i];
        endPoints[i] = finalCoords[i];
        currentPoints[i] = initialCoords[i];
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(500, 150);
    glutInitWindowSize(500, 500);
    glutCreateWindow("4");

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);

    glutDisplayFunc(display);
    glutMouseFunc(onMouseClick);
    glutMainLoop();

    return 0;
}
