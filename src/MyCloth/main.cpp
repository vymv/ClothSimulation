#define NDEBUG
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include "Spring.h"
#include "constant.h"
#include "Button.h"
using namespace std;



//弹簧和布料
Point cloth[PRESITION + 1][PRESITION + 1];
int springIndex = 0;
Spring springs[PRESITION*PRESITION * 2 + PRESITION * (PRESITION + 1) * 2 + (PRESITION - 2)*(PRESITION + 1) * 2];


//相机参数
static float c = PI / 180.0f; //弧度和角度转换参数
static int rotateByY = 90, oldy = -1, oldx = -1; //du是视点绕y轴的角度,opengl里默认y轴是上方向
static float r = 50.0f, h = 0.0f;//r是视点绕y轴的半径,h是视点高度即在y轴上的坐标
Vector3 scale;
Vector3 eyepos;


//碰撞体参数
GLfloat sphereRadius = 3.0f;
Vector3 spherePos = Vector3(0.0f, 0.0f, 0.0f);
Vector3 cubePos = Vector3(0.0f, 0.0f, 0.0f);
GLfloat cubeEdge = 5.0f;
bool isSphere = 1;

//UI
bool isframe = false;
int mode = 1;

//风力
Vector3 wind = Vector3(0, 0, 0);

void initCloth()
{
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			cloth[i][j].isFixed = 0;
			//设置固定点
			if (mode == 1)
			{
				if (i == 0)
					cloth[i][j].isFixed = 1;
			}

			

				//初始点位置
			cloth[i][j].pos = Vector3((i - PRESITION / 2) * (W*1.0f / PRESITION), 5, ((j - PRESITION / 2) * (L*1.0f / PRESITION))+ 5);
			cloth[i][j].prePos = cloth[i][j].pos;
			cloth[i][j].velocity = Vector3(0, 0, 0);
			cloth[i][j].force = Vector3(0, 0, 0);


		}

	springIndex = 0;
	//设置各类弹簧
	for (int i = 0; i < PRESITION + 1; i++)
	{
		for (int j = 0; j < PRESITION + 1; j++)
		{
			//struct
			if (i < PRESITION)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i + 1][j];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
			if (j < PRESITION)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i][j + 1];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
			//shear
			if (i < PRESITION&&j < PRESITION)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i + 1][j + 1];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
			if (i < PRESITION&&j >= 1)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i + 1][j - 1];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
			//bend
			if (i + 2 < PRESITION)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i + 2][j];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
			if (j + 2 < PRESITION)
			{
				springs[springIndex].p1 = &cloth[i][j];
				springs[springIndex].p2 = &cloth[i][j + 2];
				springs[springIndex].originLen = (springs[springIndex].p1->pos - springs[springIndex].p2->pos).Length();
				springIndex++;
			}
		}
	}

}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	initCloth();
	
}

void CalculateNormal(int i, int j)
{

	Vector3 result;

	
	//左上角
	if (i == 0 && j == 0)
	{
		Vector3 v[3];

		v[0] = cloth[0][1].pos - cloth[0][0].pos;
		v[1] = cloth[1][1].pos - cloth[0][0].pos;
		v[2] = cloth[1][0].pos - cloth[0][0].pos;
		
		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2])).Normalize();
	}
	//右上角
	else if (i == 0 && j == PRESITION)
	{
		Vector3 v[2];

		v[0] = cloth[i + 1][j].pos - cloth[i][j].pos;
		v[1] = cloth[i][j - 1].pos - cloth[i][j].pos;
		
		result = v[0].CrossProduct(v[1]).Normalize();
	}
	//左下角
	else if (i == PRESITION && j == 0)
	{
		Vector3 v[2];
		
		v[0] = cloth[i - 1][j].pos - cloth[i][j].pos;
		v[1] = cloth[i][j + 1].pos - cloth[i][j].pos;

		result = v[0].CrossProduct(v[1]).Normalize();
	}
	//右下角
	else if (i == PRESITION && j == PRESITION)
	{
		Vector3 v[3];

		v[0] = cloth[i][j - 1].pos - cloth[i][j].pos;
		v[1] = cloth[i - 1][j - 1].pos - cloth[i][j].pos;
		v[2] = cloth[i - 1][j].pos - cloth[i][j].pos;
		
		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2])).Normalize();
	}
	//上边
	else if (i == 0)
	{
		Vector3 v[4];

		v[0] = cloth[i][j + 1].pos - cloth[i][j].pos;
		v[1] = cloth[i + 1][j + 1].pos - cloth[i][j].pos;
		v[2] = cloth[i + 1][j].pos - cloth[i][j].pos;
		v[3] = cloth[i][j - 1].pos - cloth[i][j].pos;
		
		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2]) + v[2].CrossProduct(v[3])).Normalize();
	}
	//下边
	else if (i == PRESITION)
	{
		Vector3 v[4];

		v[0] = cloth[i][j - 1].pos - cloth[i][j].pos;
		v[1] = cloth[i - 1][j - 1].pos - cloth[i][j].pos;
		v[2] = cloth[i - 1][j].pos - cloth[i][j].pos;
		v[3] = cloth[i][j + 1].pos - cloth[i][j].pos;

		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2]) + v[2].CrossProduct(v[3])).Normalize();
	}
	//左边
	else if (j == 0)
	{
		Vector3 v[4];

		v[0] = cloth[i - 1][j].pos - cloth[i][j].pos;
		v[1] = cloth[i][j + 1].pos - cloth[i][j].pos;
		v[2] = cloth[i + 1][j + 1].pos - cloth[i][j].pos;
		v[3] = cloth[i + 1][j].pos - cloth[i][j].pos;

		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2]) + v[2].CrossProduct(v[3])).Normalize();
	}
	else if (j == PRESITION)
	{
		Vector3 v[4];
		v[0] = cloth[i + 1][j].pos - cloth[i][j].pos;
		v[1] = cloth[i][j - 1].pos - cloth[i][j].pos;
		v[2] = cloth[i - 1][j - 1].pos - cloth[i][j].pos;
		v[3] = cloth[i - 1][j].pos - cloth[i][j].pos;
		result = (v[0].CrossProduct(v[1]) + v[1].CrossProduct(v[2]) + v[2].CrossProduct(v[3])).Normalize();
	}
	else
	{
		Vector3 v[6];
		v[5] = cloth[i - 1][j].pos - cloth[i][j].pos;
		v[4] = cloth[i - 1][j - 1].pos - cloth[i][j].pos;
		v[3] = cloth[i][j - 1].pos - cloth[i][j].pos;
		v[2] = cloth[i + 1][j].pos - cloth[i][j].pos;
		v[1] = cloth[i + 1][j + 1].pos - cloth[i][j].pos;
		v[0] = cloth[i][j + 1].pos - cloth[i][j].pos;
		for (int i = 0; i < 5; i++)
			result = result + v[i].CrossProduct(v[i + 1]);
		result.Normalize();
	}

	result.Normalize();
	cloth[i][j].normal = result;


}

void Mouse(int button, int state, int x, int y) //处理鼠标点击
{

	if (button == GLUT_LEFT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:
			//左键按下：
			oldx = x, oldy = y;
			break;

		case GLUT_UP:
			break;
		}


}

void onMouseMove(int x, int y) //处理鼠标拖动
{
	rotateByY += 0.3f*(x - oldx); //鼠标在窗口x轴方向上的增量加到视点绕y轴的角度上，这样就左右转了
	h += (y - oldy); //鼠标在窗口y轴方向上的改变加到视点的y坐标上，就上下转了

	if (h > 30.0f) h = 30.0f; //视点y坐标作一些限制，不会使视点太奇怪
	else if (h < -30.0f) h = -30.0f;
	oldx = x, oldy = y; //把此时的鼠标坐标作为旧值，为下一次计算增量做准备
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//设置相机角度
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	eyepos = Vector3(r*cos(c*rotateByY) + scale.x, h + scale.y, r * sin(c*rotateByY) + scale.z);
	gluLookAt(eyepos.x, eyepos.y, eyepos.z, 0, 0, 0, 0, 1, 0); 

	
	//画面
	glColor3f(249 / 256.0, 230 / 256.0, 167 / 256.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < PRESITION; i++)
		for (int j = 0; j < PRESITION; j++)
		{

				//第一个三角形
				glNormal3f(cloth[i][j].normal.x, cloth[i][j].normal.y, cloth[i][j].normal.z);
				glVertex3f(cloth[i][j].pos.x, cloth[i][j].pos.y, cloth[i][j].pos.z);

				glNormal3f(cloth[i + 1][j].normal.x, cloth[i + 1][j].normal.y, cloth[i + 1][j].normal.z);
				glVertex3f(cloth[i + 1][j].pos.x, cloth[i + 1][j].pos.y, cloth[i + 1][j].pos.z);

				glNormal3f(cloth[i + 1][j + 1].normal.x, cloth[i + 1][j + 1].normal.y, cloth[i + 1][j + 1].normal.z);
				glVertex3f(cloth[i + 1][j + 1].pos.x, cloth[i + 1][j + 1].pos.y, cloth[i + 1][j + 1].pos.z);

				//第二个三角形
				glNormal3f(cloth[i][j].normal.x, cloth[i][j].normal.y, cloth[i][j].normal.z);
				glVertex3f(cloth[i][j].pos.x, cloth[i][j].pos.y, cloth[i][j].pos.z);

				glNormal3f(cloth[i][j + 1].normal.x, cloth[i][j + 1].normal.y, cloth[i][j + 1].normal.z);
				glVertex3f(cloth[i][j + 1].pos.x, cloth[i][j + 1].pos.y, cloth[i][j + 1].pos.z);

				glNormal3f(cloth[i + 1][j + 1].normal.x, cloth[i + 1][j + 1].normal.y, cloth[i + 1][j + 1].normal.z);
				glVertex3f(cloth[i + 1][j + 1].pos.x, cloth[i + 1][j + 1].pos.y, cloth[i + 1][j + 1].pos.z);
			
			

		}
	glEnd();


	//画点

	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			CalculateNormal(i, j);
			if (isframe)
			{
				glNormal3f(cloth[i][j].normal.x, cloth[i][j].normal.y, cloth[i][j].normal.z);
				glVertex3f(cloth[i][j].pos.x, cloth[i][j].pos.y, cloth[i][j].pos.z);
			}
		}

	glEnd();

	
	//画弹簧
	if (isframe)
	{
		glColor3f(0, 1, 0);
		for (int i = 0; i < springIndex; i++)
		{
			glBegin(GL_LINES);
			glVertex3f(springs[i].p1->pos.x, springs[i].p1->pos.y, springs[i].p1->pos.z);
			glVertex3f(springs[i].p2->pos.x, springs[i].p2->pos.y, springs[i].p2->pos.z);
			glEnd();
		}
	}

	

	//画球
	if (isSphere)
	{
		glPushMatrix();
		glColor3f(0, 1, 0);
		glTranslatef(spherePos.x, spherePos.y, spherePos.z);
		glutSolidSphere(sphereRadius-0.1, 20, 20);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(cubePos.x, cubePos.y, cubePos.z);
		glutSolidCube(cubeEdge-1);
		glPopMatrix();
	}

	//UI
	glColor3f(179.0 / 256.0, 177.0 / 256.0, 179.0 / 256.0);
	Vector3 vdir = Vector3(0, 1, 0);
	Vector3 hdir = vdir.CrossProduct(eyepos).Normalize();
	Vector3 bpos = Vector3(eyepos.x/2.0, eyepos.y/2.0, eyepos.z/2.0);


	glutSwapBuffers();
	glutPostRedisplay();

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLdouble)w / (GLdouble)h, 10.0, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

	float lightPosition1[4] = { 0, 300, 100,0.0 };
	float lightPosition2[4] = { 100, 100, 100,0.0 };
	float lightAmbient[3] = { 1.0, 1.0, 1.0 };
	float lightDiffuse[3] = { 0.8f, 0.8f, 0.8f };
	float lightSpecular[3] = { 0.0f, 0.8f, 0.0f };
	float global_ambient[4] = { 0, 0, 1, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);




}

void ballCollision()
{
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			Vector3 dist = cloth[i][j].pos - spherePos;
			Vector3 dir = dist;
			dir.Normalize();
			if ((dist.Length() < sphereRadius))
			{
				cloth[i][j].pos = cloth[i][j].pos + dir * (sphereRadius - dist.Length());
			}
		}
}

void cubeCollision()
{
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			Vector3 mindir = Vector3(0, 0, 0);
			GLfloat mindis = 99999;
			if ((cloth[i][j].pos.x > cubePos.x - cubeEdge / 2.0 &&cloth[i][j].pos.x < cubePos.x + cubeEdge / 2.0) &&
				(cloth[i][j].pos.y > cubePos.y - cubeEdge / 2.0 &&cloth[i][j].pos.y < cubePos.y + cubeEdge / 2.0) &&
				(cloth[i][j].pos.z > cubePos.z - cubeEdge / 2.0 &&cloth[i][j].pos.z < cubePos.z + cubeEdge / 2.0))
			{
				if (cloth[i][j].pos.x > (cubePos.x - cubeEdge / 2.0) && cloth[i][j].pos.x < cubePos.x)
				{
					if ((cloth[i][j].pos.x - (cubePos.x - cubeEdge / 2.0)) < mindis)
					{
						mindir = Vector3(-1, 0, 0);
						mindis = cloth[i][j].pos.x - (cubePos.x - cubeEdge / 2.0);
					}
				}
				else if (cloth[i][j].pos.x > cubePos.x && cloth[i][j].pos.x < cubePos.x + cubeEdge / 2.0)
				{
					if ((cubePos.x + cubeEdge / 2.0) - cubePos.x < mindis)
					{
						mindir = Vector3(1, 0, 0);
						mindis = (cubePos.x + cubeEdge / 2.0) - cloth[i][j].pos.x;
					}
				}


				if (cloth[i][j].pos.y > (cubePos.y - cubeEdge / 2.0) && cloth[i][j].pos.y < cubePos.y)
				{
					if (cloth[i][j].pos.y - (cubePos.y - cubeEdge / 2.0) < mindis)
					{
						mindir = Vector3(0, -1, 0);
						mindis = cloth[i][j].pos.y - (cubePos.y - cubeEdge / 2.0);
					}
				}
				else if (cloth[i][j].pos.y > cubePos.y && cloth[i][j].pos.y < cubePos.y + cubeEdge / 2.0)
				{
					if ((cubePos.y + cubeEdge / 2.0) - cloth[i][j].pos.y < mindis)
					{
						mindir = Vector3(0, 1, 0);
						mindis = (cubePos.y + cubeEdge / 2.0) - cloth[i][j].pos.y;
					}
				}

				if (cloth[i][j].pos.z > (cubePos.z - cubeEdge / 2.0) && cloth[i][j].pos.z < cubePos.z)
				{
					if (cloth[i][j].pos.z - (cubePos.z - cubeEdge / 2.0) < mindis)
					{
						mindir = Vector3(0, 0, -1);
						mindis = cloth[i][j].pos.z - (cubePos.z - cubeEdge / 2.0);
					}

				}
				else if (cloth[i][j].pos.z > cubePos.z && cloth[i][j].pos.z < cubePos.z + cubeEdge / 2.0)
				{
					if ((cubePos.z + cubeEdge / 2.0) - cloth[i][j].pos.z < mindis)
					{
						mindir = Vector3(0, 0, 1);
						mindis = (cubePos.z + cubeEdge / 2.0) - cloth[i][j].pos.z;
					}
				}
				//将点的位置挪到立方体外
				cloth[i][j].pos = cloth[i][j].pos + mindir * (mindis);
			}

			
		}
}
void timerFunc(int x)
{
	//给每个点施加外力
	GLfloat gravityForce = G * MASS;
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			cloth[i][j].clearForces();
			//重力
			cloth[i][j].AddForce(Vector3(0, -gravityForce, 0));
			//风力
			cloth[i][j].AddForce(wind);
		}
	//弹簧对点施加力
	for (int i = 0; i < springIndex; i++)
		springs[i].ApplyForce();

	//根据力更新点的 速度，位置
	for (int i = 0; i <= PRESITION; i++)
		for (int j = 0; j <= PRESITION; j++)
		{
			cloth[i][j].Update(isSphere);
		}
	//位置修正
	for(int i=0;i<springIndex;i++)
		springs[i].Correct();

	//碰撞检测
	if (isSphere)
		ballCollision();
	else
		cubeCollision();
	

	glutPostRedisplay();
	glutTimerFunc(DELTA_TIME, timerFunc, 1);
}



void keyboard(unsigned char k, int x, int y)
{
	if (k == 27)
		exit(0);
	else if (k == 'x' || k == 'X')
	{
		init();
		isSphere = !isSphere;
	}
	else if (k == 'f' || k == 'F')
	{
		isframe = !isframe;
	}
	else if (k == 'w' || k == 'W')
	{
		wind.z += 0.1f;
		printf("当前风力：（%f,%f,%f）\n", wind.x, wind.y, wind.z);
	}
	else if (k == 's' || k == 'S')
	{
		wind.z -= 0.1f;
		printf("当前风力：（%f,%f,%f）\n", wind.x, wind.y, wind.z);
	}
	else if (k == '1')
	{
		mode = 1;
		init();
	}
	else if (k == '2')
	{
		mode = 2;
		init();
	}


}

void SpecialKey(GLint key, GLint x, GLint y)
{
	GLfloat step = 0.1f;
	if (key == GLUT_KEY_UP)
	{
		wind.y += step;
	}
	if (key == GLUT_KEY_LEFT)
	{
		wind.x -= step;
	}
	if (key == GLUT_KEY_DOWN)
	{
		wind.y -= step;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		wind.x += step;
	}
	printf("当前风力：（%f,%f,%f）\n", wind.x, wind.y, wind.z);

}
void mouseWheel(int button, int state, int x, int y)
{
	Vector3 dir = eyepos.Normalize();
	if (state == 1)//向前
	{
		scale = scale + dir * (-5.0);
		glutPostRedisplay();
	}
	else if (state == -1)//向后
	{
		scale = scale + dir * 5.0;
		glutPostRedisplay();
	}
	//printf("(%f,%f,%f)\n", r*cos(c*rotateByY), h, z + r * sin(c*rotateByY));

}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glEnable(GL_DEPTH_TEST);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	glutMouseWheelFunc(mouseWheel);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKey);
	glutTimerFunc(DELTA_TIME, timerFunc, 1);
	glutMainLoop();
	return 0;
}