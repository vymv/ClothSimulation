#pragma once
#include <GL/glut.h>
#include <cmath>
class Vector3
{
public:
	GLfloat x;
	GLfloat y;
	GLfloat z;
	Vector3(GLfloat a, GLfloat b, GLfloat c) :x(a), y(b), z(c) {}
	Vector3() :x(0), y(0), z(0) {}
	GLfloat operator*(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3 operator*(GLfloat a)
	{
		return Vector3(x*a, y*a, z*a);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator/ (GLfloat a)
	{
		return Vector3(x / a, y / a, z / a);
	}
	Vector3 operator-()
	{
		return Vector3(-x, -y, -z);
	}
	GLfloat Length()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
	Vector3 CrossProduct(Vector3 a)
	{
		GLfloat rx = y * a.z - z * a.y;
		GLfloat ry = z * a.x - x * a.z;
		GLfloat rz = x * a.y - y * a.x;
		return Vector3(rx, ry, rz);
	}
	Vector3 Normalize()
	{
		GLfloat len = Length();
		x = x / len;
		y = y / len;
		z = z / len;
		return Vector3(x, y, z);
	}
};