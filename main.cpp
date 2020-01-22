#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <GL/glut.h>

#define HEI 47
#define WID 47
#define INFINITY -1.0

using namespace std;

char maze[HEI][WID];
int source[2];
int target[2];

class Cell {
	public:
		int x;
		int y;
		float d;
		float h;
		float f;
		shared_ptr<Cell> prev;

		Cell(int x,int y,shared_ptr<Cell> prev) {
			this->x = x;
			this->y = y;
			this->d = INFINITY;
			this->h = INFINITY;
			this->f = INFINITY;
			this->prev = prev;
		}

		bool operator>(const Cell& a) {
			return f > a.f;
		}
};

class Node {
	public:
		shared_ptr<Cell> cell;

		Node(shared_ptr<Cell> cell) {
			this->cell = cell;
		}

		bool operator<(const Node& n) {
			return cell->f > n.cell->f;
		}
};

vector<Node> pq;

float squaredEuclideanDistance(int x1,int y1,int x2,int y2) {
	float x = x1 - x2;
	float y = y1 - y2;
	return x*x + y*y;
}

void initialize() {
	shared_ptr<Cell> cell = make_shared<Cell>(source[0],source[1],nullptr);
	cell->d = 0.0;
	cell->h = squaredEuclideanDistance(source[0],source[1],target[0],target[1]);
	cell->f = cell->d + cell->h;
	pq.push_back(Node(cell));
}

bool processCell(int x,int y,shared_ptr<Cell> prev) {
	if(x>=0 && x<HEI && y>=0 && y<WID)
		if(maze[x][y]==' ') {
			shared_ptr<Cell> cell = make_shared<Cell>(x,y,prev);
			cell->d = squaredEuclideanDistance(source[0],source[1],cell->x,cell->y);
			cell->h = squaredEuclideanDistance(cell->x,cell->y,target[0],target[1]);
			cell->f = cell->d + cell->h;
			pq.push_back(Node(cell));
			push_heap(pq.begin(),pq.end());
			maze[cell->x][cell->y] = 'q';
		} else if(maze[x][y]=='T') {
			for(shared_ptr<Cell> c=prev;c!=nullptr&&c->d>0.0;c=c->prev)
				maze[c->x][c->y] = 'p';
			return true;
		}
	return false;
}

void aStarAlgorithm() {
	initialize();
	while(pq.size()>0) {
		Node n = *pq.begin();
		pop_heap(pq.begin(),pq.end());
		pq.pop_back();
		if(processCell(n.cell->x,n.cell->y-1,n.cell)) break;
		if(processCell(n.cell->x,n.cell->y+1,n.cell)) break;
		if(processCell(n.cell->x-1,n.cell->y,n.cell)) break;
		if(processCell(n.cell->x+1,n.cell->y,n.cell)) break;
	}
}

void displayMe() {
	glClear(GL_COLOR_BUFFER_BIT);
	for(int i=0;i<HEI;++i) {
		for(int j=0;j<WID;++j) {
			if(maze[i][j]=='*')
				glColor3f(0.0,1.0,0.0);
			else if(maze[i][j]==' ')
				glColor3f(1.0,1.0,1.0);
			else if(maze[i][j]=='S')
				glColor3f(0.0,0.0,0.0);
			else if(maze[i][j]=='T')
				glColor3f(1.0,1.0,0.0);
			else if(maze[i][j]=='q')
				glColor3f(0.9,0.9,0.9);
			else if(maze[i][j]=='p')
				glColor3f(1.0,0.0,0.0);

			glBegin(GL_QUADS);
				glVertex3f(	2.0*(((float)j)/((float)WID))			-1.0,
							2.0*(((float)(HEI-i-1))/((float)HEI))	-1.0,0.0);
				glVertex3f(	2.0*(((float)(j+1))/((float)WID))		-1.0,
							2.0*(((float)(HEI-i-1))/((float)HEI))	-1.0,0.0);
				glVertex3f(	2.0*(((float)(j+1))/((float)WID))		-1.0,
							2.0*(((float)(HEI-i))/((float)HEI))		-1.0,0.0);
				glVertex3f(	2.0*(((float)j)/((float)WID))			-1.0,
							2.0*(((float)(HEI-i))/((float)HEI))		-1.0,0.0);
			glEnd();
		}
	}
	glFlush();
}

void loadMaze() {
	ifstream file("maze.txt");
	if(file.is_open()) {
		for(int i=0;i<HEI;++i) {
			for(int j=0;j<WID;++j) {
				file.get(maze[i][j]);
				if(maze[i][j]=='S') {
					source[0] = i;
					source[1] = j;
				} else if(maze[i][j]=='T') {
					target[0] = i;
					target[1] = j;
				}
			}
			file.get();
		}
		file.close();
	}
	else
		cout << "Unable to open file" << endl;
}

int main(int argc,char** argv) {
	loadMaze();
	aStarAlgorithm();
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Maze");
	glClearColor(0.0,0.0,0.0,0.0);
	glutDisplayFunc(displayMe);
	glutMainLoop();
	return 0;
}