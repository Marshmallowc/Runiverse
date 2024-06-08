#pragma once
#include <iostream>
#include <graphics.h>
#include <vector>
#include "Basic.h"
using namespace std;

class Obstacles{
public:
	vector<vector<IMAGE>> img;
	char type[60];// 障碍物的类型
	int typeNum;
	string name;
	int imgIndex;
	int imgRunNum;
	int imgTypeNum;
	int imgWidth;
	int imgHeight;
	int x, y;
	int speed;
	int power;
	int score;
	int bonus;// 随着难度上升有分数奖励
	bool exist;
	bool hited;// 表示是否已经发生碰撞
	bool pass;// 表示是否已经被通过

	Obstacles();
	void setInfo(char type[], string name, int imgNum, int imgTypeNum, int speed, int power, int score, int typeNum, int imgWidth, int imgHeight);
	void loadImage();

};

