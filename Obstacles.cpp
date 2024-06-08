#include "Obstacles.h"
#include "Basic.h"

Obstacles::Obstacles() {

}

void Obstacles::setInfo(char type[], string name, int imgRunNum, int imgTypeNum, int speed, int power, int score, int typeNum, int imgWidth, int imgHeight) {
	sprintf(this->type, type);
	this->typeNum = typeNum;
	this->imgRunNum = imgRunNum;
	this->imgTypeNum = imgTypeNum;
	this->name = name;
	this->speed = speed;
	this->power = power;
	this->score = score;
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	loadImage();

	this->x = WIN_WIDTH;
	this->y = 345 + 5 - this->img[0][0].getheight();

	this->exist = true;
	this->hited = false;
	this->imgIndex = 0;
	this->pass = false;
}

void Obstacles::loadImage() {
	IMAGE img;
	char name[60];

	for (int i = 0; i < this->imgTypeNum; i++) {
		vector<IMAGE> imgArray;
		for (int j = 0; j < this->imgRunNum; j++) {
			if (this->imgTypeNum == 1)	sprintf(name, "res/%s%d.png", this->type, j + 1);
			else sprintf(name, "res/%s%d.png", this->type, i + 1);
			loadimage(&img, name, this->imgWidth, this->imgHeight, true);
			imgArray.push_back(img);
		}
		this->img.push_back(imgArray);
	}
}


