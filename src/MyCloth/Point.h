#pragma once

#include <GL/glut.h>
#include <vector>
#include <iostream>

#include "Vector3.h"
#include "Spring.h"
using namespace std;
class Spring;
class Point
{
public:
	Vector3 pos;
	Vector3 prePos;
	Vector3 velocity;
	Vector3 force;
	Vector3 normal;
	int isFixed;


	Point()
	{
		pos.x = 0; pos.y = 0; pos.z = 0;
		velocity.x = 0; velocity.y = 0; velocity.z = 0;
		force.x = 0; force.y = 0; force.z = 0;
		isFixed = 0;
	};
	void AddForce(Vector3 f)
	{
		force = force + f;
	};
	void Update(bool isSphere)
	{
		GLfloat h = DELTA_TIME / 100.0;

		Vector3 temp = pos;

		//如果点不固定，则更新位置
		if (!isFixed)
		{
			velocity = force * 0.1;
			//velocity = velocity + force / MASS * h;
			Vector3 nextPos = CalculatePos();
			pos = nextPos;
		}

		prePos = temp;
		/*
		//Runge-Kutta
		Vector3 xt = pos;
		Vector3 vt = velocity;

		Vector3 v1, v2, v3, v4;
		Vector3 a1, a2, a3, a4;

		a1 = SpringEquation(xt,vt);
		v1 = vt;
		a2 = SpringEquation(xt + v1 * h / 2, vt + a1 * h / 2);
		v2 = vt + a1 * h / 2;
		a3 = SpringEquation(xt + v2 * h / 2, vt + a2 * h / 2);
		v3 = vt + a2 * h / 2;
		a4 = SpringEquation(xt + v2 * h, vt + a3 * h);
		v4 = vt + a3 * h;
		
		prePos = pos;
		pos = xt + (v1 + v1 * 2.0 + v3 * 2.0 + v4)*h / 6.0;
		velocity = vt + (a1 + a2 * 2.0 + a3 * 2.0 + a4)*h / 6.0;
		*/

	}



	Vector3 CalculatePos()
	{
		Vector3 nextPos = pos * 2 - prePos + force * 0.1f * 0.1f;
		//Vector3 nextPos = pos + velocity * DELTA_TIME / 100.0;
		clearForces();
		return nextPos;
	}

	void clearForces()
	{
		force.x = 0;
		force.y = 0;
		force.z = 0;
	}
};