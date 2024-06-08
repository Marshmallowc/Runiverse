#pragma once
#include<iostream>
#include<vector>
#include<graphics.h>
#include "tools.h"
#include "Basic.h"
using namespace std;

class Hero {
public:

	vector<IMAGE> image;
	vector<IMAGE> imageSlip;
	char cname[60];
	string name;
	int x, y;// 坐标
	int speed;// 速度
	int imageIndex;// 当前是哪张照片
	int imageNum;// 角色跑步照片数
	int imageSlipNum;// 角色下滑照片数
	int blood;// 血量
	int score;
	bool firstSlipThenJump;
	bool isJump;
	bool isSlip;
	bool jumpDown;
	COLORREF rgb;
	string skillText;

	Hero();
	void setInfo(char cname[], string name, int imageNum, int SlipNum);
	void setSkill(string skill);
	void setRGB(COLORREF rgb);
	void loadImage();
	void move();
	void mixMove();// 混合动作
	void run();// 一帧一帧跑步
	void jump();// 一帧一帧跳跃
	void slip();// 一帧一帧下滑
	void normalizeY();// 标准化角色Y坐标
	void update();// 更新角色图片
	void update(int x, int y);
};