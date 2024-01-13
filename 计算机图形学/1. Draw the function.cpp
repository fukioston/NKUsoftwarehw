#include <GL/glut.h>
#include <math.h>


const float A = 1.0f;
const float B = 0.0f;
const float C = 1.0f;
const float D = 0.0f;

float sinc(float x) {
    if (x == 0.0f) {
        return 1.0f;
    }
    return sin(x) / x;
}
void display() {
    float x, y;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);

    for (x = -20.0f; x < 20.0f; x += 0.01f) {
        y = sinc(x);
        glVertex2f(x * A + B, y * C + D);
    }

    glEnd();
    glFlush();
}
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-20.0 * A + B, 20.0 * A + B, -1.0 * C + D, 1.0 * C + D);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("1");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}



