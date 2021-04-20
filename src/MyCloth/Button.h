#pragma once
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <GL/glut.h>
#include "Vector3.h"

#pragma comment(lib,"glut32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")



struct Button
{
	Vector3 left_down_pos;
	Vector3 hdir;
	Vector3 vdir;
	GLfloat Width;		//��Ļ���ص�λ
	GLfloat Height;
	bool Pressed;

	Button(GLfloat w,GLfloat h):Width(w),Height(h),Pressed(false){}
	void SetButton(Vector3 lu_pos, Vector3 hdir, Vector3 vdir)
	{

		Pressed = false;
	}
	void Render()
	{
		Vector3 left_up_pos = left_down_pos + vdir * Height;
		Vector3 right_down_pos = left_down_pos + hdir * Width;
		Vector3 right_up_pos = left_down_pos + vdir * Height + hdir * Width;

		glBegin(GL_QUADS);
		glVertex3f(left_up_pos.x, left_up_pos.y, left_up_pos.z);
		glVertex3f(right_up_pos.x, right_up_pos.y, right_up_pos.z);
		glVertex3f(right_down_pos.x, right_down_pos.y, right_down_pos.z);
		glVertex3f(right_up_pos.x, right_up_pos.y, right_up_pos.z);
		glEnd();
	}
	bool OnMouseDown(int mousex, int mousey)
	{
		//����λ�ã�mousex��mousey����ϵ��ԭ��λ�����Ͻ�
		//���뽫mousey�任��ԭ��λ�����½ǵ�����ϵ��
		/*
		mousey = g_fHeight - mousey;
		if (mousex > m_fPosX && mousex < m_fPosX + m_fWidth &&
			mousey > m_fPosY && mousey < m_fPosY + m_fHeight)
		{
			printf("button is pressed .... \n");
			m_bPressed = true;

			return true;
		}
		return false;
		*/
		return false;
	}
	void OnMouseUp()
	{
		Pressed = false;
	}
};





