#include <Gl/glut.h>
#include <iostream>
using namespace std;
// Cohen-Sutherland 算法的裁剪区域代码
const int INSIDE = 0;  // 0000
const int LEFT = 1;    // 0001
const int RIGHT = 2;   // 0010
const int BOTTOM = 4;  // 0100
const int TOP = 8;     // 1000

// 裁剪窗口的边界
const int xMin = 100, xMax = 300, yMin = 100, yMax = 300;

// 计算裁剪区域代码
int computeCode(double x, double y) {
    int code = INSIDE;
    if (x < xMin) code |= LEFT;
    else if (x > xMax) code |= RIGHT;
    if (y < yMin) code |= BOTTOM;
    else if (y > yMax) code |= TOP;
    return code;
}

// Cohen-Sutherland 线段裁剪算法实现
void cohenSutherlandClip(double x1, double y1, double x2, double y2) {
    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);
    bool isClipped = false;

    double originalX1 = x1, originalY1 = y1, originalX2 = x2, originalY2 = y2;

    while (true) {
        if (!(code1 | code2)) {
            // 如果两个端点都在裁剪区域内（平凡接受）
            isClipped = true;
            break;
        }
        else if (code1 & code2) {
            // 如果两个端点都在裁剪区域外（平凡拒绝）
            return;
        }
        else {
            // 线段至少有一部分在裁剪区域内
            double x, y;
            int outcodeOut = code1 ? code1 : code2;

            if (outcodeOut & TOP) {
                x = x1 + (x2 - x1) * (yMax - y1) / (y2 - y1);
                y = yMax;
            }
            else if (outcodeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (yMin - y1) / (y2 - y1);
                y = yMin;
            }
            else if (outcodeOut & RIGHT) {
                y = y1 + (y2 - y1) * (xMax - x1) / (x2 - x1);
                x = xMax;
            }
            else if (outcodeOut & LEFT) {
                y = y1 + (y2 - y1) * (xMin - x1) / (x2 - x1);
                x = xMin;
            }

            // 替换裁剪区域外的点，并重复
            if (outcodeOut == code1) {
                x1 = x;
                y1 = y;
                code1 = computeCode(x1, y1);
            }
            else {
                x2 = x;
                y2 = y;
                code2 = computeCode(x2, y2);
            }

            isClipped = true;
        }
    }


    // 如果线段被裁剪，绘制裁剪后的部分
    if (isClipped) {
        glColor3f(1.0, 0.0, 0.0);  // 红色
        glBegin(GL_LINES);
        glVertex2d(x1, y1);
        glVertex2d(x2, y2);
        glEnd();
    }
}

double xx1[10], yy1[10], xx2[10], yy2[10];
void generateRandomLines(int numberOfLines) {
    srand(time(0));  // 初始化随机数种子

    for (int i = 0; i < numberOfLines; i++) {
         xx1[i] = rand() % 400;
         yy1[i] = rand() % 400;
         xx2[i] = rand() % 400;
         yy2[i] = rand() % 400;
        // 绘制原始线段
        glColor3f(0.0, 0.0, 1.0);  // 蓝色
        glBegin(GL_LINES);
        glVertex2d(xx1[i], yy1[i]);
        glVertex2d(xx2[i], yy2[i]);
        glEnd();
        cohenSutherlandClip(xx1[i], yy1[i], xx2[i], yy2[i]);
    }
}
void keyboard(unsigned char key, int x, int y) {
    if (key == 'r') {  // If the Enter key is pressed
        cout << "r" << endl;
        for (int i = 0; i < 5; i++) {
            cohenSutherlandClip(xx1[i], yy1[i], xx2[i], yy2[i]);
        }
       
    }
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制裁剪窗口（黑色）
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(xMin, yMin);
    glVertex2i(xMax, yMin);
    glVertex2i(xMax, yMax);
    glVertex2i(xMin, yMax);
    glEnd();

    // 绘制随机线段
    generateRandomLines(5);

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 400, 0, 400);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}