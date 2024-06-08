/*
	天天酷跑开发日志
	1.	创建项目
	2.	导入素材
	3.	创建游戏界面
		实际的开发流程
		对于初学者，最好的方式，建议从用户见面入手
		
		选择图形库或者其他引擎
		天天酷跑，是基于“easyx”图形库的
		1）创建游戏窗口
		2）设计游戏背景
			a.3重背景不同的速度同时移动（因为离你近的东西移动得快嘛）
			b.循环滚动背景的实现
		3）实现游戏背景
			a.加载背景资源
			b.渲染（背景知识：坐标）
			遇到问题：背景透明的png格式图片会出现黑色
	4.实现玩家的奔跑
	5.实现玩家的跳跃
	6.实现玩家的滑铲
	7.优化帧Gettickcount代替Sleep;
	8.随机小乌龟的出现
	9.封装障碍物
	10.实现障碍物
	11.实现碰撞效果、血条
	12.实现开局界面、死亡界面、胜利界面
	13.优化开局界面（让用户有多种选择：开始游戏、更换角色、历史记录、退出游戏）
		1）P图
		2）录入键盘指令对应多种选择
		3）实现各种选择的功能
	14.障碍物结构体优化成类
	15.游戏背景优化为类
	16.优化游戏界面，不要死的界面，要动态变化
	17.增加游戏难度、角色距离、当前速度
	18.在角色跳起来的时候，允许角色下蹲（相当于快速降低降落的速度&&假设角色刚开始向上跳时）
	19.将玩家数据记录至文本文件


*/

#include "Background.h"
#include "Hero.h"
#include "Obstacles.h"
#include "Basic.h"

void initGameInterface();
string toString(int num);
bool lowerFrameRate(int speed);

typedef enum {
	TURTLE, //乌龟 0
	LION, // 1
	PILLAR1,
	PILLAR2,
	PILLAR3,
	PILLAR4,
	OBSTACLE_TYPE_COUNT // 6 
}obstacle_type;

LOGFONT f;//字体变量

//英雄
Hero hero;// 游戏角色
Hero screenHero;// 游戏界面角色
Hero magicGirl;
Hero goldHandsome;

//数字图片
IMAGE imgNumbers[10];

//背景
Background bg;

//障碍物
Obstacles obstacle[OBSTACLE_COUNT];
Obstacles turtle;
Obstacles lion;
Obstacles pillar;

//int update;// 表示是否需要马上刷新画面
int lastObstacleIndex;
bool startGame, GameMenu, thirdInterface, chooseHero;
int interfaceIndex;
int dis;
int maxScore;
int maxDistance;

//初始化
void init() {

	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//加载背景资源
	char name[60];
	sprintf(name, "sunny");
	bg.setInfo(name, "sunny", 3);
	bg.addYWithSpeed(0, 2);
	bg.addYWithSpeed(119, 4);
	bg.addYWithSpeed(330, 7);

	//初始化英雄基本信息
	sprintf(name, "magicGirl");
	magicGirl.setInfo(name, "magicGirl", 12, 2);
	magicGirl.setSkill("我是魔法少女，我既能跳得又高又远，也能下滑躲过敌人！");
	magicGirl.setRGB(RGB(255, 183, 12));

	sprintf(name, "jingxiaoshuai");
	goldHandsome.setInfo(name, "jingxiaoshuai", 12, 0);
	goldHandsome.setSkill("我是金小帅，之所以“小”，是因为我不用下滑也可以躲过敌人，我才不需要这种烦人的技能...");
	goldHandsome.setRGB(RGB(153, 255, 30));

	hero = magicGirl;

	//加载英雄图片
	magicGirl.loadImage();
	goldHandsome.loadImage();

	//加载障碍物
	sprintf(name, "turtle");
	turtle.setInfo(name, "turtle",1, 1, 0, 9, 1, 0, 0, 0);
	sprintf(name, "lion");
	lion.setInfo(name, "lion", 6, 1, 3, 30, 4, 1, 0, 0);
	sprintf(name, "pillar");
	pillar.setInfo(name, "pillar", 1, 5, 0, 40, 4, 1, 63, 260);

	turtle.loadImage();
	lion.loadImage();
	pillar.loadImage();

	//预加载音效
	preLoadSound("res/hit.mp3");

	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	
	lastObstacleIndex = -1;
	bg.difficulty = 0;

	//加载数字图片
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgNumbers[i], name);
	}

	//读取文本文件中的数据
	ifstream ifs("text.txt");
	ifs >> maxScore >> maxDistance;
	ifs.close();

}

void createObstacle() {
	int i;
	for (i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacle[i].exist == false) {
			break;
		}
	}
	
	if (i >= OBSTACLE_COUNT)
		return;

	int type = (randInt() % OBSTACLE_TYPE_COUNT);
	obstacle[i].bonus = 0;
	int fre[] = { 2, 4, 6 };
	if (type < fre[0]) {
		obstacle[i] = turtle;
	}
	else if (type < fre[1]) {
		obstacle[i] = lion;
	}
	else if (type < fre[2]) {
		obstacle[i] = pillar;
		obstacle[i].typeNum = randInt() % obstacle[i].imgTypeNum;
		obstacle[i].y = 0;
	}

	string finalPillar = "pillar";
	finalPillar += obstacle[i].imgTypeNum + '0';
	if (lastObstacleIndex >= 0 &&
			obstacle[lastObstacleIndex].name >= "pillar1" &&
			obstacle[lastObstacleIndex].name <= finalPillar &&
			obstacle[i].name == "lion" &&
			obstacle[i].x > WIN_WIDTH - 500) {
		obstacle[i] = turtle;
	}


	lastObstacleIndex = i;
}

void checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacle[i].exist && !obstacle[i].hited) {

			int a1x, a1y, a2x, a2y;
			int off = 30;// 偏量

			if (!hero.isSlip) {// 非下蹲情况：奔跑、跳跃
				a1x = hero.x + off;
				a1y = hero.y + off;
				a2x = hero.x + hero.image[hero.imageIndex].getwidth() - off;
				a2y = hero.y + hero.image[hero.imageIndex].getheight();
			}
			else {// 下蹲状况
				a1x = hero.x + off;
				a1y = 345 - hero.imageSlip[1].getheight();
				a2x = hero.x + hero.imageSlip[1].getwidth() - off;
				a2y = 345;
			}

			int b1x = obstacle[i].x + off;
			int b1y = obstacle[i].y + off;
			int b2x = obstacle[i].x + obstacle[i].img[obstacle[i].typeNum][obstacle[i].imgIndex].getwidth() - off;
			int b2y = obstacle[i].y + obstacle[i].img[obstacle[i].typeNum][obstacle[i].imgIndex].getheight() - 10;

			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) {// 碰撞了
				hero.blood -= obstacle[i].power;
				cout << "血量剩余" << hero.blood << endl;
				playSound("res/hit.mp3");
				obstacle[i].hited = true;
			}
		}
	}
}

//修改移动数据
void updateData() {
	//背景移动
	bg.move();

	//英雄移动
	hero.move();
	
	//创建障碍物
	static int frameCount = 0;
	static int enermyFre = 100;
	frameCount++;
	if (frameCount > enermyFre) {
		frameCount = 0;
		enermyFre = 100 + (randInt() % 17) * (randInt() % 21);
		createObstacle();
	}
	
	//更新障碍物的坐标
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacle[i].exist) {
			obstacle[i].x -= obstacle[i].speed + bg.speed[2];
			if (obstacle[i].x < -obstacle[i].img[obstacle[i].typeNum][0].getwidth() * 2) {
				obstacle[i].exist = false; 
				
			}
			
			int len = obstacle[i].imgRunNum;
			obstacle[i].imgIndex = (obstacle[i].imgIndex + 1) % len;
		}
	}

	//碰撞检测
	checkHit();


}

//处理用户按键的输入
void keyEvent() {

	if (kbhit()) { //如果有按键按下
		char ch;
		ch = getch();// 不需要按下回车就可以直接读取
	
		if (ch == ' ' && startGame) {	
			if (hero.isSlip && hero.name != "goldHandsome")	hero.firstSlipThenJump = true;
			hero.isJump = true;
		}
		else if (ch == 's' && startGame) {
			if (!hero.isJump && hero.name == "jingxiaoshuai")	return;
			if (hero.isSlip)	hero.firstSlipThenJump = false;
			hero.isSlip = true;
		}
		else if (ch == '1' && !startGame && interfaceIndex == 1) {// 开始游戏
 			startGame = true;
			GameMenu = false;
		}
		else if (ch == '2' && !startGame && interfaceIndex == 1) {// 选择英雄
			GameMenu = false;
			chooseHero = true;
			interfaceIndex = 2;

			Hero hero1 = magicGirl;
			Hero hero2 = goldHandsome;

			while (interfaceIndex == 2) {
				keyEvent();

				char heroname1[] = { "a) 魔法少女" };
				char heroname2[] = { "b) 金小帅" };

				//LOGFONT f1;
				f.lfHeight = 40;
				f.lfWidth = 19;
				f.lfWeight = 800;
				////setbkmode(TRANSPARENT);
				settextcolor(RGB(26, 20, 31));
				settextstyle(&f);


				bool update = lowerFrameRate(80);

				if (update) {
					BeginBatchDraw();
					loadimage(0, "res/GameMenu.png");
					outtextxy(300 - 40, 230 + 10 + hero1.image[0].getheight() - 90, heroname1);
					outtextxy(600 - 30, 230 + 10 + hero1.image[0].getheight() - 90, heroname2);

					hero1.run();
					hero2.run();
					hero1.update(300, 230 - 100);
					hero2.update(600, 230 - 60);
					EndBatchDraw();
				}
			}
		}
		else if (ch == '3' && !startGame && interfaceIndex == 1) {
			GameMenu = false;
			interfaceIndex = 2;

			while (interfaceIndex == 2) {
				keyEvent();

				char str[] = { "s键表示下滑、空格键表示跳跃，成功躲避一个障碍物将获得相应分数，游戏难度随时间增加，集齐15分有惊喜" };
				char str1[] = { "s键表示下滑、空格键表示跳跃、o键开关音乐" };
				char str2[] = { "成功躲避一个障碍物将获得相应分数" };
				char str3[] = { "游戏难度随时间增加，集齐15分有惊喜" };


				int maxWidth = 600; // 绘制区域的宽度
				gettextstyle(&f);
				f.lfHeight = 35;//字体高度(像素为单位)
				f.lfWidth = 17;//字体宽度
				f.lfWeight = 750;
				settextcolor(RGB(177, 232, 5));
				strcpy(f.lfFaceName, "微软雅黑");
				f.lfQuality = ANTIALIASED_QUALITY;//打磨字体 使字体变得更加光滑
				setbkmode(TRANSPARENT);
				settextstyle(&f);
				string line;
				int x = 80, y = 50;

				loadimage(0, "res/GameMenu.png");
				outtextxy(x, y, str1);
				outtextxy(x, y + 60, str2);
				outtextxy(x, y + 120, str3);
				settextcolor(RGB(26, 20, 31));
				outtextxy(800, 333, "d)  返 回");

				bool update = lowerFrameRate(60);
				if (update) {
					hero.run();
					hero.update(80, 250);
				}

				Sleep(80);
			}
		}
		else if (ch == '4' && !startGame) {
			exit(0);
		}
		else if (ch == 'a' && !startGame && chooseHero) {// 选择魔法少女
			hero = magicGirl;
			chooseHero = false;
			interfaceIndex = 1;
		}
		else if (ch == 'b' && !startGame && chooseHero) {// 选择金小帅
			hero = goldHandsome;
			chooseHero = false;
			interfaceIndex = 1;
		}
		else if (ch == 'd' && !startGame) {
			interfaceIndex = 1;
			chooseHero = false;
		}
		else if (ch == 'o') {
			static int count = 0;
			count++;
			if(count % 2 == 0)	mciSendString("stop res/bg.mp3", 0, 0, 0);
			else mciSendString("play res/bg.mp3 repeat", 0, 0, 0);

}
	}
}

void updateEnermy() {
	//渲染障碍物
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacle[i].exist) {
			putimagePNG2(obstacle[i].x, obstacle[i].y, WIN_WIDTH, &obstacle[i].img[obstacle[i].typeNum][obstacle[i].imgIndex]);
		}
	}

}

void updateBloodBar() {
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, hero.blood / 100.0);
}

void checkGameOver() {
	if (hero.blood <= 0) {
		if (maxScore < hero.score)	maxScore = hero.score;
		if (maxDistance < dis)	maxDistance = dis;

		ofstream ofs("text.txt");
		ofs << maxScore << ' ' << maxDistance;

		ofs.close();

		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		initGameInterface();


		system("pause");

		//暂停之后，充钱复活or直接开始下一局
		hero.blood = 100;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
		hero.score = 0;
		bg.difficulty = 0;
		bg.speed[0] = 2;
		bg.speed[1] = 4;
		bg.speed[2] = 7;
		turtle.bonus = 0;
		lion.bonus = 0;
		pillar.bonus = 0;
		dis = 0;
	}
}

void checkScore() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacle[i].exist &&
				obstacle[i].pass == false &&
				!obstacle[i].hited &&
				obstacle[i].x + obstacle[i].img[obstacle[i].imgTypeNum][obstacle[i].imgIndex].getwidth() < hero.x) {
			hero.score += obstacle[i].score + obstacle[i].bonus;
			obstacle[i].pass = true;
			cout << "score:" << hero.score << endl;
		}
	}
}

void updateScore() {
	char str[8];
	sprintf(str, "%d", hero.score);

	int x = 20;
	int y = 25;

	for (int i = 0; i < str[i]; i++) {
		int num = str[i] - '0';
		putimagePNG(x, y, &imgNumbers[num]);
		x += imgNumbers[num].getwidth() + 5; 
	}

}

void checkVectory() {
	if (hero.score >= WIN_SCORE) {
		startGame = false;

		if (maxScore < hero.score)	maxScore = hero.score;
		if (maxDistance < dis)	maxDistance = dis;

		ofstream ofs("text.txt");
		ofs << maxScore << ' ' << maxDistance;

		ofs.close();
		
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(1000);
		loadimage(0, "res/win.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");

		hero.blood = 100;
		hero.score = 0;
		bg.difficulty = 0;
		bg.speed[0] = 2;
		bg.speed[1] = 4;
		bg.speed[2] = 7;
		turtle.bonus = 0;
		lion.bonus = 0;
		pillar.bonus = 0;
		dis = 0;
		mciSendString("play res/bg.mp3", 0, 0, 0);
	}
}

void setFont()
{
	static int height = 49;
	static int width = 26;
	static bool down = false;

	char str1[] = {"1)  开 始 游 戏"};
	char str2[] = {"2)  切 换 角 色"};
	char str3[] = {"3)  使 用 说 明"};
	char str4[] = {"4)  退 出 游 戏"};

	gettextstyle(&f);
	f.lfHeight = height;//字体高度(像素为单位)
	f.lfWidth = width;//字体宽度
	f.lfWeight = 700;
	settextcolor(RGB(29, 20, 31));
	strcpy(f.lfFaceName, "微软雅黑");
	f.lfQuality = ANTIALIASED_QUALITY;//打磨字体 使字体变得更加光滑
	settextstyle(&f);
	setbkmode(TRANSPARENT);

	char skillText[60];
	for (int i = 0; i < hero.skillText.size(); i++) {
		skillText[i] = hero.skillText[i];
	}

	outtextxy(WIN_WIDTH * 2 / 3 - 60, (WIN_HEIGHT / 5) , str1);
	outtextxy(WIN_WIDTH * 2 / 3 - 60, (WIN_HEIGHT / 5) * 2 , str2);
	outtextxy(WIN_WIDTH * 2 / 3 - 60, (WIN_HEIGHT / 5) * 3 , str3);
	outtextxy(WIN_WIDTH * 2 / 3 - 60, (WIN_HEIGHT / 5) * 4 , str4);

	int maxWidth = 200; // 绘制区域的宽度
	f.lfWeight = 350;
	f.lfHeight = 25;
	f.lfWidth = 11;
	settextcolor(hero.rgb);
	settextstyle(&f);
	string line;
	int x = 100 + hero.image[0].getwidth() + 10 - 80, y = 180 - 60;
	for (char c : hero.skillText) {
		line += c;
		if (textwidth(line.c_str()) > maxWidth) {
			outtextxy(x, y, line.c_str());
			line = ""; // 清空当前行文字
			y += textheight("A"); // 下一行起始位置
		}
	}
	outtextxy(x, y, line.c_str()); // 输出最后一行文字


	if (height < 53 && !down)	height++, width++, down = true;
	else if (height > 48) height--, width--;
	else down = false;

}

void initGameInterface() {
	//清空所有障碍物
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacle[i].exist = false;
	}

	//游戏初始界面
	startGame = false;
	loadimage(0, "res/start.png");
	system("pause");

	//游戏菜单
	GameMenu = true;
	interfaceIndex = 1;

	while (!startGame) {
		keyEvent();

		if(interfaceIndex == 1){

			BeginBatchDraw();
			loadimage(0, "res/GameMenu.png");
			hero.run();
			hero.update(20, 180);

			string arr1 = "最远距离" + toString(maxDistance);
			string arr2 = "最高得分" + toString(maxScore);

			char str1[50], str2[50];
			for (int i = 0; i < arr1.size(); i++) {
				str1[i] = arr1[i];
			}
			str1[arr1.size()] = 'm';
			str1[arr1.size() + 1] = '\0';

			for (int i = 0; i < arr2.size(); i++) {
				str2[i] = arr2[i];
			}
			str2[arr2.size()] = '\0';



			f.lfHeight = 30;
			f.lfWidth = 14;
			f.lfWeight = 500;
			setbkmode(TRANSPARENT);
			settextcolor(RGB(234, 35, 124));
			settextstyle(&f);
			outtextxy(20, 10, str1);
			outtextxy(20, 40, str2);
			
			bool update = lowerFrameRate(80);
			if (update)	setFont();

			Sleep(110);

			EndBatchDraw();
			
		}
	}
	
}

bool lowerFrameRate(int speed) {
	static int timer = 0;
	static bool update = false;

	timer += getDelay();
	if (timer >= speed) {
		timer = 0;
		update = true;
		return true;
	}
	else if (timer == 0) {
		update = false;
	}

	return false;
}

string toString(int num) {
	if (num == 0)	return "0";

	string arr = "";
	while (num) {
		arr += (num % 10) + '0';
		num /= 10;
	}

	reverse(arr.begin(), arr.end());
	return arr;
}

void updateDistance() {
	dis += bg.speed[bg.speed.size() - 1];
	string arr = toString(dis);
	char num[10000];
	for (int i = 0; i < arr.size(); i++) {
		num[i] = arr[i];
	}
	num[arr.size()] = 'm';
	num[arr.size() + 1] = '\0';

	gettextstyle(&f);
	f.lfHeight = 31;
	f.lfWidth = 14;
	f.lfWeight = 580;
	settextcolor(RGB(255, 248, 170));
	settextstyle(&f);
	outtextxy(850, 10, num);
}

void checkGameDifficulty() {
	//随着游戏时间的增加，难度升级
	static long long time = 0;

	int add[] = { 1, 1, 2 };
	if (time >= 500) {
		time = 0;
		bg.difficulty++;

		//增加背景的移动速度
		for (int i = 0; i < 3; i++) {
			bg.speed[i] += add[i];
		}

		//增加障碍物的奖励
		turtle.bonus += bg.difficulty * 2;
		lion.bonus += bg.difficulty * 2;
		pillar.bonus += bg.difficulty * 2;
	}

	time++;
}

void updateSpeed() {
	char num[10000];
	string arr = toString(bg.speed[bg.speed.size() - 1]);

	for (int i = 0; i < arr.size(); i++) {
		num[i] = arr[i];
	}

	num[arr.size()] = 'm';
	num[arr.size() + 1] = '\/';
	num[arr.size() + 2] = 's';
	num[arr.size() + 3] = '\0';

	gettextstyle(&f);
	f.lfHeight = 31;
	f.lfWidth = 14;
	f.lfWeight = 580;
	settextcolor(RGB(255, 248, 170));
	settextstyle(&f);
	outtextxy(850, 46, num);
	
}

int main()
{	
	//初始化
	init();// 加载数据

	while (1) {
		//初始化界面
		initGameInterface();

		//开始游戏
		while (startGame) {

			//键盘监听
			keyEvent();

			//减缓每一帧的更新速度
			int rate = 30 - 2 * bg.difficulty;
			if (rate < 20)	rate = 20;
			bool update = lowerFrameRate(rate);

			//更新游戏的每一帧数据
			if (update) {

				//更新每一帧图片
				BeginBatchDraw();
				bg.update();
				hero.update();// 更新英雄图片
				updateEnermy();
				updateBloodBar();// 更新背景中的血量信息
				updateScore();// 更新背景中的分数信息
				updateDistance();// 更新玩家走过的距离
				updateSpeed();// 更新玩家此刻的移动速度
				EndBatchDraw();

				//更新每一帧之后的数据
				updateData();

				//检测玩家游戏情况
				checkVectory();
				checkGameOver();
				checkScore();
				checkGameDifficulty();// 检测是否需要更新游戏难度


			}

				
		}

	}

	
	system("pause");
	return 0;
}