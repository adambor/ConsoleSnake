#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <time.h>   
#include <conio.h> 

int windowWidth;
int windowHeight;

char gameEnded = 0;

const char HEAD = 'O';
const char BODY = 'o';
const char MEAL = 'X';
const char WALL = '#';

int SPEED = 300;
const double SPEED_DECREASE = 0.98;

int score = 0;

typedef struct kadksaks {
	int x;
	int y;
	char type;
	char movementDir;
	char isLast;
} BLOCK;

BLOCK map[1000];
int mapLength = 0;

int gameWidth = 16;
int gameHeight = 16;

int main() {

	void getWindowSize(int *width, int *height);
	void tick(long deltaT, char *renderBuffer, char *turnPoints);
	int addToMap(int posX, int posY, char type, char isLast);
	void generateNewMeal(char *renderBuffer);

	long lastTick = clock();
	long currentTick;

	getWindowSize(&windowWidth, &windowHeight);

	windowHeight -= 5;

	srand(time(NULL));

	addToMap(10, 10, HEAD, 1);

	char renderBuffer[windowHeight*windowWidth];

	char turnPoints[(windowWidth+1)*(windowHeight+1)];

	printf("Size of renderBuffer: %d\n",sizeof(renderBuffer));
	printf("Size of turnPoints: %d\n", sizeof(turnPoints));

	memset(turnPoints, 0, sizeof(turnPoints));
	memset(renderBuffer, ' ', windowHeight*windowWidth);

	generateNewMeal(renderBuffer);

	sleep(3);

	while (gameEnded==0) {
		currentTick = clock();
		tick(currentTick - lastTick, renderBuffer, turnPoints);
		lastTick = currentTick;
	}

	printf("Game lost!\n");

	//getch();

	while(1) {}

	return 0;
}

char getDirection(int id) {
	return map[id].movementDir;
}

void setDirection(int id, char direction) {
	map[id].movementDir = direction;
}

void endgame() {
	gameEnded = 1;
}

void move(int id, int movX, int movY) {
	map[id].x+=movX;
	map[id].y+=movY;
	
	if (map[id].x > windowWidth-1) {
		map[id].x = 0;
	}
	if (map[id].x < 0) {
		map[id].x = windowWidth-1;
	}

	if (map[id].y > windowHeight-1) {
		map[id].y = 0;
	}
	if (map[id].y < 0) {
		map[id].y = windowHeight-1;
	}
}

int addToMap(int posX, int posY, char type, char isLast) {
	map[mapLength].x = posX;
	map[mapLength].y = posY;
	map[mapLength].type = type;
	map[mapLength].isLast = isLast;

	mapLength++;
	return mapLength - 1;
}

void getWindowSize(int *width, int *height) {
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	printf("columns: %d\n", columns);
	printf("rows: %d\n", rows);

	*width = columns;
	*height = rows;
}

long lastRenderLength;
char lastDirection;
// 0-stil;
// 1-up;
// 2-down;
// 3-left;
// 4-right;

int growTimer = 0;

void tick(long deltaT, char *renderBuffer, char *turnPoints) {
	void render(char *renderBuffer, char *turnPoints);
	void setDirection(int id, char direction);
	char getDirection(int id);
	void addTurningPoint(int x, int y, char dirAfter);
	void endgame();

	if (_kbhit()) {
		char c = _getch();
		//printf("Character input %d\n", c);

		char dir;

		switch (c) {
		case 72:
			dir = 1;
			break;
		case 80:
			dir = 2;
			break;
		case 75:
			dir = 3;
			break;
		case 77:
			dir = 4;
		}

		if (dir != 0 && getDirection(0) != dir) {
			setDirection(0, dir);
		}
	}

	lastRenderLength += deltaT;
	if (lastRenderLength >= SPEED) {
		if (lastDirection != map[0].movementDir && map[0].isLast == 0) {
			//Add turning point
			turnPoints[(map[0].y*windowWidth) + map[0].x] = map[0].movementDir;
			if (map[0].isLast == 0 &&
				(
					(map[0].movementDir == 1 && lastDirection == 2) ||
					(map[0].movementDir == 2 && lastDirection == 1) ||
					(map[0].movementDir == 3 && lastDirection == 4) ||
					(map[0].movementDir == 4 && lastDirection == 3)
				)) {
				endgame();
				return;
			}
			lastDirection = map[0].movementDir;
		}
		render(renderBuffer, turnPoints);
		lastRenderLength = 0;
	}
}

int meal_x = -1;
int meal_y = -1;

void generateNewMeal(char *renderBuffer) {
	do {
		meal_x = 1 + (rand() % gameWidth);
		meal_y = 1 + (rand() % gameHeight);
	} while (renderBuffer[(meal_y*windowWidth) + meal_x]!=' ');
}

void render(char *renderBuffer, char *turnPoints) {

	char grow = 0;

	void move(int id, int movX, int movY);
	int addToMap(int x, int y, char type, char direction);
	void setDirection(int id, char direction);
	void endgame();
	void generateNewMeal(char *renderBuffer);

	int head_next_x;
	int head_next_y;

	switch (map[0].movementDir) {
	case 1:
		head_next_x = map[0].x;
		head_next_y = map[0].y - 1;
		break;
	case 2:
		head_next_x = map[0].x;
		head_next_y = map[0].y + 1;
		break;
	case 3:
		head_next_x = map[0].x - 1;
		head_next_y = map[0].y;
		break;
	case 4:
		head_next_x = map[0].x + 1;
		head_next_y = map[0].y;
		break;
	}

	if (head_next_x < 0) {
		head_next_x = windowWidth - 1;
	}
	if (head_next_y < 0) {
		head_next_y = windowHeight - 1;
	}
	if (head_next_x > windowWidth - 1) {
		head_next_x = 0;
	}
	if (head_next_y > windowHeight - 1) {
		head_next_y = 0;
	}

	if (head_next_x == meal_x && head_next_y == meal_y) {
		grow = 1;
		SPEED *= SPEED_DECREASE;
		score += 10;
		generateNewMeal(renderBuffer);
	}

	memset(renderBuffer, ' ', windowHeight*windowWidth);

	int mapLengthCurrent = mapLength;

	if (meal_x!=-1 && meal_y!=-1) {
		renderBuffer[(meal_y*windowWidth) + meal_x] = MEAL;
	}

	for (int i = 0;i < gameHeight + 2;i++) {
		renderBuffer[(i*windowWidth)] = WALL;
		renderBuffer[(i*windowWidth)+gameWidth+1] = WALL;
	}
	for (int i = 0;i < gameWidth + 2;i++) {
		renderBuffer[i] = WALL;
		renderBuffer[((gameHeight+1)*windowWidth)+i] = WALL;
	}

	for (int i = mapLengthCurrent-1;i > -1; i--) {
		if (map[i].movementDir!=0) {

			if (i != 0 && turnPoints[(map[i].y*windowWidth) + map[i].x] != 0) {
				map[i].movementDir = turnPoints[(map[i].y*windowWidth) + map[i].x];
				if (map[i].isLast == 1) {
					turnPoints[(map[i].y*windowWidth) + map[i].x] = 0;
				}
			}

			if (grow == 1 && map[i].isLast == 1) {
				int pos = addToMap(map[i].x, map[i].y, BODY, 1);
				map[i].isLast = 0;
				setDirection(pos,map[i].movementDir);
			}

			switch (map[i].movementDir) {
			case 1:
				move(i, 0, -1);
				break;
			case 2:
				move(i, 0, 1);
				break;
			case 3:
				move(i, -1, 0);
				break;
			case 4:
				move(i, 1, 0);
				break;
			}

			if (renderBuffer[(map[i].y*windowWidth) + map[i].x] == BODY ||
				renderBuffer[(map[i].y*windowWidth) + map[i].x] == HEAD ||
				renderBuffer[(map[i].y*windowWidth) + map[i].x] == WALL) {
				endgame();
				return;
			}

		}
		renderBuffer[(map[i].y*windowWidth) + map[i].x] = map[i].type;
	}

	char *str1 = "Speed:";
	char intstr1[12];
	itoa(SPEED, intstr1, 10);

	char *str2 = "Score:";
	char intstr2[12];
	itoa(score, intstr2, 10);

	strcpy(&renderBuffer[(gameHeight+3)*windowWidth], str1);
	strcpy(&renderBuffer[(gameHeight + 3)*windowWidth + 10], intstr1);
	strcpy(&renderBuffer[(gameHeight + 4)*windowWidth], str2);
	strcpy(&renderBuffer[(gameHeight + 4)*windowWidth + 10], intstr2);

	COORD newPosition = { 0, 0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newPosition);
	
	write(1,renderBuffer,windowHeight*windowWidth);
}