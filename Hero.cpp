#include "Hero.h"

Hero::Hero() {

}

void Hero::loadImage() {
	char name[60];
	IMAGE img;
	//跑步
	for (int i = 0; i < this->imageNum; i++) {
		sprintf(name, "res/%s%d.png", this->cname, i + 1);
		loadimage(&img, name, 0, 0, true);
		this->image.push_back(img);
	}

	//下滑
	for (int i = 0; i < this->imageSlipNum; i++) {
		sprintf(name, "res/%sSlip%d.png", this->cname, i + 1);
		loadimage(&img, name, 0, 0, true);
		this->imageSlip.push_back(img);
	}
}

void Hero::setInfo(char cname[], string name, int imageNum, int imageSlipNum) {
	sprintf(this->cname, cname);
	this->imageNum = imageNum;
	this->imageSlipNum = imageSlipNum;
	this->name = name;

	loadImage();

	this->x = WIN_WIDTH * 0.5 - image[0].getwidth() * 0.5 - 200;
	this->y = 345 - image[0].getheight();
	this->blood = 100;
	this->speed = 0;
	this->imageIndex = 0;
	this->score = 0;
	this->isJump = false;
	this->isSlip = false;
	this->jumpDown = false;
}

void Hero::update() {
	if (isJump && !firstSlipThenJump) {
		putimagePNG2(this->x, this->y, &this->image[this->imageIndex]);

	}
	else if (isSlip || firstSlipThenJump) {
		if (name == "goldHandsome") {
			isSlip = false;
			firstSlipThenJump = false;
			return;
		}

		int delay[] = { 8, 39 };
		static int countTimes = 0;

		if (firstSlipThenJump) isSlip = false;

		if (countTimes < delay[0] && isSlip) {
			putimagePNG2(this->x, this->y, &imageSlip[0]);
			countTimes++;
		}
		else if (countTimes < delay[1] && isSlip) {
			putimagePNG2(this->x, this->y, &imageSlip[1]);
			countTimes++;
		}
		else {// 下滑结束
			isSlip = false;
			normalizeY();
			firstSlipThenJump = false;
			countTimes = 0;
		}
	}
	else {
		putimagePNG2(this->x, this->y, &this->image[this->imageIndex]);
	}
}

void Hero::setSkill(string skill) {
	this->skillText = skill;
}

void Hero::setRGB(COLORREF rgb) {
	this->rgb = rgb;
}

void Hero::update(int x, int y) {
	if (isJump) {
		putimagePNG2(x, y, &this->image[this->imageIndex]);

	}
	else if (isSlip || firstSlipThenJump) {
		
		int delay[] = { 8, 39 };
		static int countTimes = 0;

		if (firstSlipThenJump) isSlip = false;
		
		if (countTimes < delay[0] && isSlip) {
			putimagePNG2(x, y, &imageSlip[0]);
			countTimes++;
		}
		else if (countTimes < delay[1] && isSlip) {
			putimagePNG2(x, y, &imageSlip[1]);
			countTimes++;
		}
		else {// 下滑结束
			isSlip = false;
			normalizeY();
			countTimes = 0;
			firstSlipThenJump = false;
		}
	}
	else {
		putimagePNG2(x, y, &this->image[this->imageIndex]);
	}
}


void Hero::run() {
	this->imageIndex = (this->imageIndex + 1) % this->imageNum;// 切换下一帧
}

void Hero::jump() {
	int riseY = 9;
	int landY = 7;
	int highMax = 345 - this->image[this->imageIndex].getheight() * 2;
	if (name == "jingxiaoshuai")	highMax -= 60;

	this->isJump = true;

	if (isSlip) {
		jumpDown = true;
		riseY += 6;
		landY = riseY;
	}

	if (this->y >= highMax && !this->jumpDown) {
		this->y -= riseY;
	}
	else {
		jumpDown = true;
		this->y += landY;

		if (this->y >= 345 - this->image[this->imageIndex].getheight()) {// 跳跃结束
			this->isJump = false;
			this->jumpDown = false;
			this->isSlip = false;
			normalizeY();// 调正y的坐标
		}
	}
}

void Hero::slip() {
	this->isSlip = true;
	this->y = 345 - this->imageSlip[1].getheight();

	if (isJump) {
		isJump = false;
		this->normalizeY();
	}
}

void Hero::move() {

	if (isJump && !firstSlipThenJump)	jump();
	else if (isSlip) slip();
	else run();
}

void Hero::normalizeY() {
	this->y = 345 - this->image[0].getheight();
}