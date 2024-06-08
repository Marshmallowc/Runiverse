#include "Basic.h"

int randInt() {
	srand(time(NULL) + GetTickCount());
	int ran = rand() % 1000;
	return ran;
}