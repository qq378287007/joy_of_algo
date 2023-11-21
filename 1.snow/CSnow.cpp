#include <iostream>
#include <random>
#include <ctime>
using namespace std;

#include "graphics.h"
#include "CSnow.h"

CSnow::CSnow(int x, int y, int speed, int layers)
{
	m_pos = {x, y};
	m_speed = speed;
	m_layers = layers;
}

CSnow::CSnow(const CSnow &snow)
{
	m_pos = snow.m_pos;
	m_speed = snow.m_speed;
	m_layers = snow.m_layers;
}

CSnow::~CSnow()
{
	int sss = 0;
}

void CSnow::UpdatePosition(int dps)
{
	if (dps == 0)
		m_pos.y += m_speed;
	else
		m_pos.y += dps;
}

CSnow MakeSnow(int maxWidth)
{
	static default_random_engine e(time(nullptr));

	uniform_int_distribution<unsigned int> ypos(0, maxWidth - 1);
	uniform_int_distribution<unsigned int> rspeed(1, 5);
	uniform_int_distribution<unsigned int> rlayers(1, 30);

	int x = ypos(e);
	int speed = rspeed(e);
	int layers = rlayers(e);

	return CSnow(x, 0, speed, layers);
}
