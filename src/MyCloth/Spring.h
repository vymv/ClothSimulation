#pragma once
#include "constant.h"
#include "Point.h"
class Spring
{
public:
	Point * p1;
	Point * p2;
	GLfloat originLen;
	Spring()
	{
		p1 = NULL;
		p2 = NULL;
		originLen = -1;
	}
	
	GLfloat GetCurrLen()
	{
		return (p1->pos - p2->pos).Length();
	}
	void ApplyForce()
	{

		//�������ķ��򣨵��ɷ���p1->p2��
		Vector3 dir = p2->pos - p1->pos;
		dir = dir / dir.Length();

		//�ٶ��ڵ��ɷ����ϵ�ͶӰ
		float v1 = dir * p1->velocity;
		float v2 = dir * p2->velocity;

		//���㵯��
		float fSD = -ks * (originLen - GetCurrLen()) - kd * (v1 - v2);
		//Vector3 correction = (distance*(1 - lengthAtRest / currentLength))*.5;

		//�Ѹ�����ͶӰ������������
		Vector3 force = dir * fSD;

		//�Ե�ʩ����
		p1->AddForce(force);
		p2->AddForce(-force);

	}
	void Correct()
	{
		Vector3 distance = p2->pos - p1->pos;

		Vector3 correction = distance*(1 - originLen / distance.Length())*.5; 
		Vector3 negCorrection = correction * -1; 
		if (p1->isFixed == 0)
		{
			p1->pos = p1->pos + correction;
		}
		if (p2->isFixed == 0)
		{
			p2->pos = p2->pos + negCorrection;
		}
		
	}
};