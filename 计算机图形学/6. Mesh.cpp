//#include <GL/glut.h>
//#include <cmath>
//#include <fstream>
//#include<vector>
//std::ofstream outfile("ball.txt"); // 创建文件输出流对象
// std::ofstream outfile2("house.txt"); 
//
//struct Vec3 {
//    float x, y, z;
//    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
//    Vec3() : x(0), y(0), z(0) {}
//
//    Vec3 normalize() {
//        float len = sqrt(x * x + y * y + z * z);
//        return Vec3(x / len, y / len, z / len);
//    }
//};
//
//void recordVertex(const Vec3& v) {
//    outfile << v.x << ' ' << v.y << ' ' << v.z << '\n';
//}
// void recordVertex2(const Vec3& v) {
//    outfile2 << v.x << ' ' << v.y << ' ' << v.z << '\n';
//}
//
//void subdivide(Vec3 v1, Vec3 v2, Vec3 v3, int depth) {
//    if (depth == 0) {
//        glVertex3f(v1.x, v1.y, v1.z);
//        recordVertex(v1); // 记录顶点
//        glVertex3f(v2.x, v2.y, v2.z);
//        recordVertex(v2); // 记录顶点
//        glVertex3f(v3.x, v3.y, v3.z);
//        recordVertex(v3); // 记录顶点
//        return;
//    }
//
//    Vec3 v12 = Vec3((v1.x + v2.x) / 2, (v1.y + v2.y) / 2, (v1.z + v2.z) / 2).normalize();
//    Vec3 v23 = Vec3((v2.x + v3.x) / 2, (v2.y + v3.y) / 2, (v2.z + v3.z) / 2).normalize();
//    Vec3 v31 = Vec3((v3.x + v1.x) / 2, (v3.y + v1.y) / 2, (v3.z + v1.z) / 2).normalize();
//
//    subdivide(v1, v12, v31, depth - 1);
//    subdivide(v2, v23, v12, depth - 1);
//    subdivide(v3, v31, v23, depth - 1);
//    subdivide(v12, v23, v31, depth - 1);
//}
//
//// 新增：定义房子的顶点和面
//std::vector<Vec3> houseVertices = {
//    // 立方体的底部
//    Vec3(-0.5, 0.0, -0.5),  // 0
//    Vec3(0.5, 0.0, -0.5),   // 1
//    Vec3(0.5, 0.0, 0.5),    // 2
//    Vec3(-0.5, 0.0, 0.5),   // 3
//    // 立方体的顶部
//    Vec3(-0.5, 1.0, -0.5),  // 4
//    Vec3(0.5, 1.0, -0.5),   // 5
//    Vec3(0.5, 1.0, 0.5),    // 6
//    Vec3(-0.5, 1.0, 0.5),   // 7
//    // 屋顶尖顶
//    Vec3(0.0, 1.5, 0.0)     // 8
//};
//
//std::vector<std::vector<int>> houseFaces = {
//    // 立方体的侧面
//    {0, 1, 5, 4},
//    {1, 2, 6, 5},
//    {2, 3, 7, 6},
//    {3, 0, 4, 7},
//    // 底部和顶部
//    {0, 3, 2, 1},
//    {4, 5, 6, 7},
//    // 屋顶的三角面
//    {4, 5, 8},
//    {5, 6, 8},
//    {6, 7, 8},
//    {7, 4, 8}
//};
//
//
//// 新增：绘制房子的函数
//void drawHouse() {
//    for (const auto& face : houseFaces) {
//        glBegin(face.size() == 4 ? GL_QUADS : GL_POLYGON);
//        for (int vertexIndex : face) {
//            Vec3 v = houseVertices[vertexIndex];
//            glVertex3f(v.x, v.y, v.z);
//              recordVertex2(v);
//        }
//        glEnd();
//    }
//}
//
//
//
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//    gluLookAt(1.0, 2.0, 4.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);
//
//    glColor3f(1.0, 0.0, 0.0);  // 设置房子的颜色为红色
//    glScalef(1.4f, 1.4f, 1.4f);
//    drawHouse();  // 绘制房子
//
//    glColor3f(1.0, 1.0, 1.0);  // 白色，以便在黑色背景下看清楚
//    glScalef(0.3f, 0.3f, 0.3f);
//    glBegin(GL_TRIANGLES);
//
//    // 八面体的六个顶点
//    Vec3 top(0.0, 0.0, 1.0);
//    Vec3 bottom(0.0, 0.0, -1.0);
//    Vec3 front(1.0, 0.0, 0.0);
//    Vec3 back(-1.0, 0.0, 0.0);
//    Vec3 left(0.0, 1.0, 0.0);
//    Vec3 right(0.0, -1.0, 0.0);
//
//    // 八面体的八个面
//    subdivide(top, left, front, 4);
//    subdivide(top, front, right, 4);
//    subdivide(top, right, back, 4);
//    subdivide(top, back, left, 4);
//    subdivide(bottom, front, left, 4);
//    subdivide(bottom, right, front, 4);
//    subdivide(bottom, back, right, 4);
//    subdivide(bottom, left, back, 4);
//
//    glEnd();
//    glBegin(GL_TRIANGLES);
//
//
//    Vec3 x1(1.0, 0.0, 1.0);
//    Vec3 x2(0.0, 0.0, -1.0);
//    Vec3 x3(1.0, 0.0, 0.0);
//    Vec3 x4(-1.0, 0.0, 0.0);
//    Vec3 x5(0.0, 1.0, 0.0);
//    Vec3 x6(0.0, -1.0, 0.0);
//
//    glEnd();
//
//    glutSwapBuffers();
//}
//
//void init() {
//    glClearColor(0.0, 0.0, 0.0, 0.0);
//    glEnable(GL_DEPTH_TEST);
//
//    // 设置正交投影
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
//    // 设置线框模式
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    // 设置模型视图矩阵
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//}
//
//void reshape(int w, int h) {
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
//    glMatrixMode(GL_MODELVIEW);
//}
//
//int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//    glutInitWindowSize(800, 600);
//    glutCreateWindow("Recursive Sphere - Octahedron Base");
//    init();
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutMainLoop();
//    return 0;
//}




//下面是读取的代码
#include <GL/glut.h>
#include <cmath>
#include <fstream>
#include <vector>

struct Vec3 {
    float x, y, z;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

// 读取顶点坐标的函数
std::vector<Vec3> readVertices(const std::string& filename) {
    std::vector<Vec3> vertices;
    std::ifstream infile(filename);
    float x, y, z;

    while (infile >> x >> y >> z) {
        vertices.push_back(Vec3(x, y, z));
    }

    return vertices;
}
std::vector<std::vector<int>> houseFaces = {
    // 立方体的侧面
    {0, 1, 5, 4},
    {1, 2, 6, 5},
    {2, 3, 7, 6},
    {3, 0, 4, 7},
    // 底部和顶部
    {0, 3, 2, 1},
    {4, 5, 6, 7},
    // 屋顶的三角面
    {4, 5, 8},
    {5, 6, 8},
    {6, 7, 8},
    {7, 4, 8}
};
 

// 使用读取的顶点绘制图形的函数
void drawFromVertices(const std::vector<Vec3>& vertices) {
    glBegin(GL_TRIANGLES);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}
void drawHouseVertices(const std::vector<Vec3>& vertices) {
    int vertexIndex = 0;
    for (const auto& face : houseFaces) {
        glBegin(face.size() == 4 ? GL_QUADS : GL_TRIANGLES);
        for (int i = 0; i < face.size(); ++i) {
            Vec3 v = vertices[vertexIndex++];
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
}


// 渲染函数
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(1.0, 2.0, 4.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);
    std::vector<Vec3> vertices2 = readVertices("house.txt");
    glColor3f(1.0, 0.0, 0.0);
    drawHouseVertices(vertices2);
    glScalef(0.3f, 0.3f, 0.3f);
    glColor3f(1.0, 1.0, 1.0); // 白色

    // 读取并绘制顶点
    std::vector<Vec3> vertices = readVertices("ball.txt");
    drawFromVertices(vertices);

    glutSwapBuffers();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
    // 设置线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 设置模型视图矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Recursive Sphere - Octahedron Base");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
