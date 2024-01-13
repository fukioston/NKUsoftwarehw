#include<Windows.h>
#include<gl/glut.h> 
#include<math.h>
#include<iostream>
#include<vector>
using namespace std;

int signal;

const int screenWidth = 1200;
const int screenHeight = 800;
const int windowPositionX = 100;
const int windowPositionY = 100;

class Point {
public:
	double x;
	double y;
	Point(double x, double y) {
		this->x = x;
		this->y = y;
	}
	Point() {
	}
};

class Node {
public:
	Point p;
	Node* next;
	Node* last;
	Node(Point t) :p(t.x, t.y) {
		next = this;
		last = this;
	}
};
Node* operateNode;

class List {
public:
	Node* head;
	int length;
	List() {
		length = 0;
		head = NULL;
	}
	void push(Point newp) {
		Node* cur = new Node(newp);
		cur->p = newp;
		if (this->length == 0) {
			cur->next = cur;
			cur->last = cur;
			this->head = cur;
		}
		else {
			cur->last = this->head;
			cur->next = this->head->next;
			this->head->next->last = cur;
			this->head->next = cur;
			this->head = cur;
		}
		this->length++;
	}
	Node* search(Point p) {
		Node* cur = this->head;
		while (1) {
			if (cur->p.x == p.x && cur->p.y == p.y) {
				break;
			}
			else {
				cur = cur->next;
			}
		}
		return cur;
	}
	void deleteNode(Node* targetNode) {
		if (targetNode->next == targetNode->last) {	/* two Nodes or one Node*/
			if (targetNode->next == targetNode) {	/* only one Node*/
				delete targetNode;
				this->head = NULL;
			}
			else {	/* two Nodes */
				targetNode->next->last = targetNode->next;
				targetNode->next->next = targetNode->next;
				this->head = targetNode->next;
				delete targetNode;
			}
		}
		else {	/* more nodes */
			targetNode->last->next = targetNode->next;
			targetNode->next->last = targetNode->last;
			if (targetNode == this->head) {
				this->head = targetNode->last;
			}
			delete targetNode;
		}
		this->length--;
	}

};

vector<List> Allvector = vector<List>();
vector<List> BaseVector = vector<List>();

void baseInit() {
	List house;
	List lwindow;
	List rwindow;
	List chimney;
	//HouseOutlinePoints
	Point house1(-200, 100);
	Point house2(0, 200);
	Point house3(200, 100);
	Point house4(200, -100);
	Point house5(-200, -100);

	//leftWindowPoints
	Point lwindow1(-130, 80);
	Point lwindow2(-50, 80);
	Point lwindow3(-50, 0);
	Point lwindow4(-130, 0);

	//rightWindowPoints
	Point rwindow1(130, 80);
	Point rwindow2(50, 80);
	Point rwindow3(50, 0);
	Point rwindow4(130, 0);

	//chimneyPoints
	Point chimney1(120, 220);
	Point chimney2(160, 220);
	Point chimney3(160, 120);
	Point chimney4(120, 140);

	house.push(house1);
	house.push(house2);
	house.push(house3);
	house.push(house4);
	house.push(house5);

	lwindow.push(lwindow1);
	lwindow.push(lwindow2);
	lwindow.push(lwindow3);
	lwindow.push(lwindow4);

	rwindow.push(rwindow1);
	rwindow.push(rwindow2);
	rwindow.push(rwindow3);
	rwindow.push(rwindow4);

	chimney.push(chimney1);
	chimney.push(chimney2);
	chimney.push(chimney3);
	chimney.push(chimney4);
	Allvector.clear();
	Allvector.push_back(house);
	Allvector.push_back(lwindow);
	Allvector.push_back(rwindow);
	Allvector.push_back(chimney);
	BaseVector = Allvector;
}


void drawBase() {
	Allvector.clear();
	Allvector = BaseVector;
	int i = 0;
	while (i < 4) {
		glBegin(GL_LINE_LOOP);
		List* curList = &Allvector[i];
		Node* cur = curList->head;
		int j;
		for (j = 0; j < curList->length; j++) {
			glVertex2d(cur->p.x, cur->p.y);
			cur = cur->next;
		}
		glEnd();
		i++;
	}
}

void drawAll() {
	int i = 0;
	while (i < Allvector.size()) {
		glBegin(GL_LINE_LOOP);
		int j;
		List* curList = &Allvector[i];
		Node* cur = curList->head;
		for (j = 0; j < curList->length; j++) {
			glVertex2d(cur->p.x, cur->p.y);
			cur = cur->next;
		}
		glEnd();
		i++;
	}
}

void transformation(int& x, int& y) {	//transform the coordinates.
	x = x - screenWidth / 2;
	y = screenHeight / 2 - y;
}

double distanceS(Point p, int x, int y) {
	return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
}

Node* catchNode(int x, int y) {
	int i = 0;
	while (i < Allvector.size()) {
		List curList = Allvector[i];
		Node* cur = curList.head;
		int j = 0;
		while (j < curList.length) {
			if (distanceS(cur->p, x, y) <= 25) {
				return cur;
			}
			else {
				cur = cur->next;
				j++;
			}
		}
		i++;
	}
	return NULL;
}

List* catchList(Node* targetNode) {
	int i = 0;
	while (i < Allvector.size()) {
		List* curList = &Allvector[i];
		Node* cur = curList->head;
		int j = 0;
		while (j < curList->length) {
			if (cur->p.x == targetNode->p.x && cur->p.y == targetNode->p.y) {
				return curList;
			}
			else {
				cur = cur->next;
				j++;
			}
		}
		i++;
	}
}

void mydisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);  //clear the screen
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(3.0);
	drawAll();
	glFlush();
}

void myMouse(int button, int state, int x, int y) {
	transformation(x, y);
	Point newP = Point(x, y);
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && signal == 0) {
		if (Allvector.back().length == 0) {
			glBegin(GL_POINTS);
			glVertex2d(newP.x, newP.y);
			glEnd();
			glFlush();
		}
		else {
			Point lastP = Allvector.back().head->p;
			glBegin(GL_LINES);
			glVertex2d(lastP.x, lastP.y);
			glVertex2d(newP.x, newP.y);
			glEnd();
			glFlush();
		}
		Allvector.back().push(newP);
	}
	else if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && signal == 1) {
		if (catchNode(x, y) != NULL) {
			operateNode = catchNode(x, y);
		}
	}
	else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON && signal == 1) {
		operateNode = NULL;
	}
	else if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && signal == 2) {
		if (catchNode(x, y) != NULL) {
			operateNode = catchNode(x, y);
			List* curList = catchList(operateNode);
			curList->deleteNode(operateNode);
			operateNode = NULL;
			glutPostRedisplay();
		}
	}
}

void myMovedMouse(int x, int y) {
	transformation(x, y);
	if (signal == 1) {
		if (operateNode != NULL) {
			operateNode->p.x = x;
			operateNode->p.y = y;
			glutPostRedisplay();
		}
		
	}
}

void newList() {
	List newList;
	Allvector.push_back(newList);
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'b':
		signal = 0;
		newList();
		break;
	case 'm':
		signal = 1;
		break;
	case 'd':
		signal = 2;
		break;
	case 'r':
		// something.
		glClear(GL_COLOR_BUFFER_BIT);
		baseInit();
		drawBase();
		glFlush();
		break;
	case 'q':
		exit(0);
		break;
	}
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);   
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(screenWidth, screenHeight);  
	glutInitWindowPosition(windowPositionX, windowPositionX); 
	glutCreateWindow("My assignment 2nd."); 

	glutDisplayFunc(&mydisplay); 
	glutMouseFunc(&myMouse);
	glutKeyboardFunc(&myKeyboard);
	glutMotionFunc(&myMovedMouse);
	glClearColor(1.0, 1.0, 1.0, 0.0);   
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(3.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-screenWidth / 2, screenWidth / 2, -screenHeight / 2, screenHeight / 2);
	baseInit();
	drawBase();
	glutMainLoop();  
}

