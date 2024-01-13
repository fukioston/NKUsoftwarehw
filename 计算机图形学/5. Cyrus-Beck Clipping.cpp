#include <gl/glut.h>
#include <iostream>
#include<stdlib.h>
#include <vector>
#include <cmath>
using namespace std;

struct Point2D
{
	float x, y;
	Point2D()
	{
		x = 0.0f;
		y = 0.0f;
	}
	Point2D(float xx, float yy)
	{
		x = xx;
		y = yy;
	}
	Point2D& operator+(const Point2D& p)
	{
		Point2D temp;
		temp.x = x + p.x;
		temp.y = y + p.y;
		return temp;
	}
	Point2D& operator-(const Point2D& p)
	{
		Point2D temp(x - p.x, y - p.y);
		return temp;
	}
	float operator*(const Point2D& p)
	{
		return x * p.x + y * p.y;
	}

	Point2D operator*(const float k)
	{
		return Point2D(x * k, y * k);
	}

	float length()
	{
		return sqrtf(x * x + y * y);
	}

	void InverseDir()
	{
		x = -x;
		y = -y;
	}
};

struct Line2D
{
	Point2D start;
	Point2D end;
	float _length;

	Line2D() : start(), end()
	{
		_length = 0.0f;
	}
	Line2D(const Point2D& start, const Point2D& end) : start(start), end(end)
	{
	}
	Line2D(const Line2D& line) : start(line.start), end(line.end)
	{}

	float length()
	{
		_length = (end - start).length();
	}

	Line2D& operator = (const Line2D& line)
	{
		start = line.start;
		end = line.end;
		return *this;
	}

	Point2D GetVector()
	{
		return end - start;
	}
};
struct Polygon1
{
	int _num;//Num of lines, not points
	Point2D* points;
	Point2D* norms;

	Polygon1()
	{
		_num = 0;
	}
	Polygon1(vector<Point2D> p)
	{
		Set(p);
	}
	~Polygon1()
	{
		delete[] points;
	}

	void Clear()
	{
		delete[] points;
	}

	void Set(vector<Point2D> p)
	{
		_num = p.size();
		points = new Point2D[_num];
		for (int i = 0; i < _num; ++i)
			points[i] = p[i];

		norms = new Point2D[_num];
		ComputeNormals();
	}

	Line2D GetLine(int index)
	{
		Line2D temp;
		if (index >= 0 && index < _num - 1)
		{
			temp.start = points[index];
			temp.end = points[index + 1];
		}
		else if (index == _num - 1)
		{
			temp.start = points[index];
			temp.end = points[0];
		}
		return temp;
	}

	Point2D GetNormal(int index)
	{
		Point2D temp;
		if (index >= 0 && index < _num)
		{
			temp = norms[index];
		}
		return temp;
	}

	void ComputeNormals()
	{
		for (int i = 0; i < _num; ++i)
		{
			Line2D now = GetLine(i);
			Line2D next;
			if (i == _num - 1)
				next = GetLine(0);
			else
				next = GetLine(i + 1);

			Point2D v = now.GetVector();
			Point2D vn = next.GetVector();
			Point2D norm;
			if (v.x != 0)
			{
				norm.y = 1;
				norm.x = (-v.y) / v.x;
			}
			else//x and y couldn't be zero at same time
			{
				norm.x = 1;
				norm.y = (-v.x) / v.y;
			}

			if (norm * vn > 0)
				norm.InverseDir();
			norms[i] = norm;
		}
	}

	const Point2D& GetPoint(int index)
	{
		if (index >= 0 && index <= _num)
			return points[index];
		return Point2D();
	}
};

/*
Global Varibles
*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
Point2D gstart;
Point2D gend;
Line2D src;
Line2D dest;
Line2D src1[100];
Line2D dest1[100];
bool acc[100];
bool buildpoly = true;
Polygon1 g_Poly;
int g_Count;
std::vector<Point2D> g_V;

int Cyrus_Beck(Line2D& src, Line2D& dest, Polygon1& poly, int k)
{
	float tin = 0.0f, tout = 1.0f;
	Point2D&& vec = src.GetVector();

	for (int i = 0; i < poly._num; ++i)
	{
		Line2D&& line = poly.GetLine(i);
		Point2D&& norm = poly.GetNormal(i);
		float nc = vec * norm;
		if (nc == 0)
			continue;
		else
		{
			float hit = (line.start - src.start) * norm / nc;
			if (nc > 0)//out
				tout = min(tout, hit);
			else
				tin = max(tin, hit);
		}
	}

	if (tin <= tout)
	{
		dest.start = src.start + vec * tin;
		dest.end = src.start + vec * tout;
		dest1[k] = dest;
	}
	return tin > tout;
}

void myInit()
{
	/*
	Output Info
	*/
	std::vector<Point2D> v;
	v.emplace_back();
	g_Count = 0;
	for (int i = 0;i < 100;i++)
		acc[i] = false;


	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);//Map Color Black
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)SCREEN_WIDTH, (GLdouble)SCREEN_HEIGHT, 0.0);
	glViewport(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);
}

void myMouse(int button, int state, int x, int y)
{
	if (buildpoly)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			g_V.emplace_back(x, y);
			glutPostRedisplay();
			glFlush();
		}
		return;
	}
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
		return;
	}


	glFlush();
}
void processKey(unsigned char key, int x, int y) {
	if (key == ' ') {
		if (buildpoly) {
			//build over
			g_Poly.Set(g_V);
			g_V.clear();
			buildpoly = false;
			glutPostRedisplay();
			glFlush();
		}
		switch (g_Count)
		{
		case 0:
		{
			for (int i = 0; i < 100; i++) {
				src.start.x = rand() % 800;
				src.start.y = rand() % 600;
				src.end.x = rand() % 800;
				src.end.y = rand() % 600;
				src1[i] = src;
				acc[i] = !Cyrus_Beck(src1[i], dest1[i], g_Poly, i);

				glutPostRedisplay();
				glFlush();
			}
			g_Count = 2;
			glutPostRedisplay();
			glFlush();
			break;
		}
		}
	}

}
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	Point2D temp;
	if (buildpoly)
	{
		glColor3f(0.0f, 0.0f, 0.0f);//Poly
		glPointSize(3.0);

		glBegin(GL_POLYGON);

		for (int i = 0; i < g_V.size(); ++i)
			glVertex2d(g_V[i].x, g_V[i].y);

		glEnd();
		glFlush();
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);//Poly
		glPointSize(3.0);
		glBegin(GL_POLYGON);

		for (int i = 0; i < g_Poly._num; ++i)
		{
			temp = g_Poly.GetPoint(i);
			glVertex2d(temp.x, temp.y);
		}


		glEnd();
		glFlush();
		if (g_Count == 2)
		{

			for (int i = 0;i < 10;i++) {
				//Draw Line
				glColor3f(1.0f, 0.0f, 0.0f);//Normal Line, Red
				glPointSize(2.0);
				glBegin(GL_LINES);
				glVertex2d(src1[i].start.x, src1[i].start.y);
				glVertex2d(src1[i].end.x, src1[i].end.y);
				if (acc[i])
				{
					//Draw Cutted Line
					glColor3f(0.0f, 0.0f, 1.0f);//Normal Line, Green
					glPointSize(3.0);
					glVertex2d(dest1[i].start.x, dest1[i].start.y);
					glVertex2d(dest1[i].end.x, dest1[i].end.y);
				}
				glEnd();
				glFlush();
			}
		}
	}

}


int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(400, 50);
	glutCreateWindow("Cyrus_Beck");
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(processKey);
	myInit();
	glutMainLoop();

	return 0;
}