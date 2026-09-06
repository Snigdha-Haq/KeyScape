#ifndef L2DOORS_H_INCLUDED
#define L2DOORS_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

// Shared State Variables from level2
extern int level2_energy;
extern bool level2_gameOver;
extern bool level2_keyFound;
extern bool level2_keyCollected[4];
extern int level2_keyColor[4][3];
extern char level2_message[120];
extern int level2_messageTimer;

void level2_updateScore(int addPoints);

// Sound functions declared in level2.h
void playPlusPointSound();
void playNegPointSound();

// ---------------- DOORS DATA & SETUP ----------------
struct Level2Door {
	int x, y, width, height;
	bool visited;
	int assignedKeyColorId;
};

#define LEVEL2_DOOR_WIDTH  120
#define LEVEL2_DOOR_HEIGHT 180
#define LEVEL2_DOOR_GAP    60
#define LEVEL2_DOORS_START_X ((SCREEN_WIDTH - (4 * LEVEL2_DOOR_WIDTH + 3 * LEVEL2_DOOR_GAP)) / 2)
#define LEVEL2_DOOR_Y 100

static Level2Door level2_doors[4];
static bool level2_doorsVisible = false;
static int level2_realKeyDoor = -1;

enum Level2TaskType { LEVEL2_TASK_NONE = 0, LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
static int level2_doorTaskType[4];

static bool level2_insideTask = false;
static int level2_currentTaskDoor = -1;

// Cutscene
static int level2_finishStage = 0;
static float level2_shineTimer = 0.0f;
static float level2_clamOpenAngle = 0.0f;

// Task states
static int level2_mathA = 0, level2_mathB = 0;
static int level2_mathOptions[4];
static int level2_mathCorrectIndex = 0;

static int level2_puzzleSeq[4];
static int level2_puzzleBlankIndex = 0;
static int level2_puzzleStep = 0;
static int level2_puzzleOptions[4];
static int level2_puzzleCorrectIndex = 0;

static int level2_colorTarget[3];
static int level2_colorOptions[4][3];
static int level2_colorCorrectIndex = 0;

#define LEVEL2_OPT_W   150
#define LEVEL2_OPT_H   70
#define LEVEL2_OPT_GAP 30
#define LEVEL2_OPT_Y   330
#define LEVEL2_WRONG_TASK_PENALTY 15

inline void level2_drawKeyVisual(float x, float y, int size, int r, int g, int b)
{
	iSetColor(r, g, b);
	iFilledCircle(x + size * 0.3f, y + size * 0.3f, size * 0.28f);
	iSetColor(255, 255, 255);
	iFilledCircle(x + size * 0.3f, y + size * 0.3f, size * 0.12f);
	iSetColor(r, g, b);
	iFilledRectangle(x + size * 0.28f, y + size * 0.22f, size * 0.55f, size * 0.16f);
	iFilledRectangle(x + size * 0.68f, y + size * 0.05f, size * 0.1f, size * 0.17f);
	iFilledRectangle(x + size * 0.84f, y + size * 0.05f, size * 0.1f, size * 0.22f);
	iSetColor(0, 0, 0);
	iCircle(x + size * 0.3f, y + size * 0.3f, size * 0.28f);
}

inline void level2_drawShinyRealKey(int cx, int cy)
{
	float pulse = (sin(level2_shineTimer * 0.1f) + 1.0f) * 0.5f;
	int glowRadius = (int)(70 + pulse * 25);

	iSetColor(255, 235, 120);
	iFilledCircle(cx, cy, glowRadius);
	iSetColor(255, 255, 200);
	iFilledCircle(cx, cy, (int)(glowRadius * 0.7f));

	iSetColor(255, 255, 255);
	for (int a = 0; a < 8; a++) {
		float angle = a * 3.14159f / 4.0f + level2_shineTimer * 0.05f;
		int x1 = cx + (int)(cos(angle) * (glowRadius + 15));
		int y1 = cy + (int)(sin(angle) * (glowRadius + 15));
		iLine(cx, cy, x1, y1);
	}

	float kx = cx - 50.0f;
	float ky = cy - 25.0f;
	level2_drawKeyVisual(kx, ky, 100, 255, 215, 0);

	int sX1 = cx + 45 + (int)(sin(level2_shineTimer * 0.2f) * 10);
	int sY1 = cy + 30;
	iSetColor(255, 255, 255);
	iFilledCircle(sX1, sY1, 4);
	iLine(sX1 - 8, sY1, sX1 + 8, sY1);
	iLine(sX1, sY1 - 8, sX1, sY1 + 8);
}

inline void level2_drawClamAndPearl(int cx, int cy)
{
	iSetColor(190, 160, 130);
	iFilledCircle(cx, cy - 20, 112);
	iSetColor(140, 110, 85);
	iCircle(cx, cy - 20, 112);

	iSetColor(245, 238, 225);
	iFilledCircle(cx, cy - 16, 102);

	iSetColor(255, 248, 238);
	iFilledCircle(cx, cy - 14, 90);

	iSetColor(175, 145, 115);
	for (int i = -3; i <= 3; i++) {
		iLine(cx, cy - 80, cx + i * 26, cy - 10);
	}

	float pearlGlow = (sin(level2_shineTimer * 0.15f) + 1.0f) * 0.5f;
	iSetColor(230, 245, 255);
	iFilledCircle(cx, cy + 10, (int)(42 + pearlGlow * 8));

	iSetColor(255, 255, 255);
	iFilledCircle(cx, cy + 10, 36);
	iSetColor(215, 220, 230);
	iCircle(cx, cy + 10, 36);

	iSetColor(255, 255, 255);
	iFilledCircle(cx - 10, cy + 22, 9);
	iFilledCircle(cx + 8, cy + 15, 4);

	int topY = cy - 20 + (int)level2_clamOpenAngle;

	iSetColor(210, 180, 145);
	iFilledCircle(cx, topY + 80, 108);
	iSetColor(150, 120, 90);
	iCircle(cx, topY + 80, 108);

	iSetColor(230, 205, 175);
	iFilledCircle(cx, topY + 77, 98);
	iSetColor(175, 140, 105);
	for (int i = -3; i <= 3; i++) {
		iLine(cx, topY + 140, cx + i * 26, topY + 60);
	}

	iSetColor(255, 250, 210);
	for (int r = 0; r < 6; r++) {
		float ang = 0.6f + r * 0.35f;
		iLine(cx, cy + 10, cx + (int)(cos(ang) * 140), cy + 10 + (int)(sin(ang) * 140));
	}
}

inline void level2_optionBoxPos(int idx, int* bx, int* by)
{
	int totalW = 4 * LEVEL2_OPT_W + 3 * LEVEL2_OPT_GAP;
	int startX = (SCREEN_WIDTH - totalW) / 2;
	*bx = startX + idx * (LEVEL2_OPT_W + LEVEL2_OPT_GAP);
	*by = LEVEL2_OPT_Y;
}

inline void level2_drawOptionBox(int idx, int selectedForNothing)
{
	int totalW = 4 * LEVEL2_OPT_W + 3 * LEVEL2_OPT_GAP;
	int startX = (SCREEN_WIDTH - totalW) / 2;
	int bx = startX + idx * (LEVEL2_OPT_W + LEVEL2_OPT_GAP);
	int by = LEVEL2_OPT_Y;

	iSetColor(255, 255, 255);
	iFilledRectangle(bx, by, LEVEL2_OPT_W, LEVEL2_OPT_H);
	iSetColor(60, 60, 60);
	iRectangle(bx, by, LEVEL2_OPT_W, LEVEL2_OPT_H);
}

inline bool level2_isInside(int px, int py, Level2Door d)
{
	return (px >= d.x && px <= d.x + d.width && py >= d.y && py <= d.y + d.height);
}

inline bool level2_isInsideBox(int px, int py, int bx, int by, int bw, int bh)
{
	return (px >= bx && px <= bx + bw && py >= by && py <= by + bh);
}

// ---------------- TASK GENERATION ----------------
inline void level2_generateMathTask()
{
	level2_mathA = 1 + rand() % 40;
	level2_mathB = 1 + rand() % 40;
	int correct = level2_mathA + level2_mathB;
	level2_mathCorrectIndex = rand() % 4;
	for (int i = 0; i < 4; i++) {
		if (i == level2_mathCorrectIndex) {
			level2_mathOptions[i] = correct;
		}
		else {
			int fake;
			do { fake = correct + (rand() % 21 - 10); } while (fake == correct || fake < 0);
			level2_mathOptions[i] = fake;
		}
	}
}

inline void level2_generatePuzzleTask()
{
	level2_puzzleStep = 2 + rand() % 4;
	int start = 1 + rand() % 10;
	for (int i = 0; i < 4; i++) level2_puzzleSeq[i] = start + i * level2_puzzleStep;
	level2_puzzleBlankIndex = rand() % 4;
	int correct = level2_puzzleSeq[level2_puzzleBlankIndex];
	level2_puzzleCorrectIndex = rand() % 4;
	for (int i = 0; i < 4; i++) {
		if (i == level2_puzzleCorrectIndex) {
			level2_puzzleOptions[i] = correct;
		}
		else {
			int fake;
			do { fake = correct + (rand() % 9 - 4); } while (fake == correct || fake <= 0);
			level2_puzzleOptions[i] = fake;
		}
	}
}

inline void level2_randColor(int c[3])
{
	c[0] = rand() % 256;
	c[1] = rand() % 256;
	c[2] = rand() % 256;
}

inline void level2_generateColorTask()
{
	level2_randColor(level2_colorTarget);
	level2_colorCorrectIndex = rand() % 4;
	for (int i = 0; i < 4; i++) {
		if (i == level2_colorCorrectIndex) {
			level2_colorOptions[i][0] = level2_colorTarget[0];
			level2_colorOptions[i][1] = level2_colorTarget[1];
			level2_colorOptions[i][2] = level2_colorTarget[2];
		}
		else {
			level2_randColor(level2_colorOptions[i]);
		}
	}
}

inline void level2_generateTaskForDoor(int doorIndex)
{
	int t = level2_doorTaskType[doorIndex];
	if (t == LEVEL2_TASK_MATH)        level2_generateMathTask();
	else if (t == LEVEL2_TASK_PUZZLE) level2_generatePuzzleTask();
	else if (t == LEVEL2_TASK_COLOR)  level2_generateColorTask();
}

inline void setupLevel2Doors()
{
	int doorColors[4] = { 0, 1, 2, 3 };
	for (int i = 3; i > 0; i--) {
		int j = rand() % (i + 1);
		int tmp = doorColors[i]; doorColors[i] = doorColors[j]; doorColors[j] = tmp;
	}

	for (int i = 0; i < 4; i++) {
		level2_doors[i].x = LEVEL2_DOORS_START_X + i * (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP);
		level2_doors[i].y = LEVEL2_DOOR_Y;
		level2_doors[i].width = LEVEL2_DOOR_WIDTH;
		level2_doors[i].height = LEVEL2_DOOR_HEIGHT;
		level2_doors[i].visited = false;
		level2_doors[i].assignedKeyColorId = doorColors[i];
	}

	level2_doorsVisible = false;
	level2_finishStage = 0;
	level2_shineTimer = 0.0f;
	level2_clamOpenAngle = 0.0f;

	level2_realKeyDoor = rand() % 4;
	{
		int taskTypes[3] = { LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
		for (int i = 2; i > 0; i--) {
			int j = rand() % (i + 1);
			int tmp = taskTypes[i]; taskTypes[i] = taskTypes[j]; taskTypes[j] = tmp;
		}
		int ti = 0;
		for (int i = 0; i < 4; i++) {
			if (i == level2_realKeyDoor) level2_doorTaskType[i] = LEVEL2_TASK_NONE;
			else level2_doorTaskType[i] = taskTypes[ti++];
		}
	}
	level2_insideTask = false;
	level2_currentTaskDoor = -1;
}

inline void renderLevel2DoorsContent(int doorClosedImg, int doorOpenImg)
{
	if (!level2_doorsVisible) return;

	if (!level2_insideTask) {
		if (level2_finishStage == 0) {
			for (int i = 0; i < 4; i++) {
				int imgToUse = level2_doors[i].visited ? doorOpenImg : doorClosedImg;
				iShowImage(level2_doors[i].x, level2_doors[i].y, level2_doors[i].width, level2_doors[i].height, imgToUse);

				int colId = level2_doors[i].assignedKeyColorId;
				int circleX = level2_doors[i].x + level2_doors[i].width / 2;
				int circleY = level2_doors[i].y + level2_doors[i].height + 25;

				iSetColor(level2_keyColor[colId][0], level2_keyColor[colId][1], level2_keyColor[colId][2]);
				iFilledCircle(circleX, circleY, 15);
				iSetColor(255, 255, 255);
				iCircle(circleX, circleY, 15);
				iSetColor(0, 0, 0);
				iCircle(circleX, circleY, 16);

				if (level2_doors[i].visited) {
					iSetColor(255, 215, 0);
					iText(level2_doors[i].x + 35, level2_doors[i].y + level2_doors[i].height + 48, "OPEN!");
				}
			}
		}

		if (level2_finishStage == 1) {
			level2_drawShinyRealKey(SCREEN_WIDTH / 2, 330);
			iSetColor(255, 255, 255);
			iFilledRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
			iSetColor(0, 0, 0);
			iRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
			iText(SCREEN_WIDTH / 2 - 145, 208, "REAL KEY FOUND! Click to Open Clam!");
		}
		else if (level2_finishStage >= 2) {
			level2_drawClamAndPearl(SCREEN_WIDTH / 2, 300);
			if (level2_finishStage == 2) {
				iSetColor(255, 255, 255);
				iFilledRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
				iSetColor(0, 0, 0);
				iRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
				iText(SCREEN_WIDTH / 2 - 110, 145, "Click the Pearl to Complete Level!");
			}
		}
	}
	else {
		int t = level2_doorTaskType[level2_currentTaskDoor];
		char buf[64];

		if (t == LEVEL2_TASK_MATH) {
			iSetColor(255, 255, 255);
			sprintf_s(buf, sizeof(buf), "%d + %d = ?", level2_mathA, level2_mathB);
			iText(SCREEN_WIDTH / 2 - 50, 250, buf, GLUT_BITMAP_TIMES_ROMAN_24);
			for (int i = 0; i < 4; i++) {
				level2_drawOptionBox(i, 0);
				int bx, by; level2_optionBoxPos(i, &bx, &by);
				sprintf_s(buf, sizeof(buf), "%d", level2_mathOptions[i]);
				iSetColor(0, 0, 0);
				iText(bx + LEVEL2_OPT_W / 2 - 12, by + LEVEL2_OPT_H / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
			}
		}
		else if (t == LEVEL2_TASK_PUZZLE) {
			iSetColor(255, 255, 255);
			iText(SCREEN_WIDTH / 2 - 160, 230, "Find the missing number in sequence:", GLUT_BITMAP_HELVETICA_18);
			int seqX = SCREEN_WIDTH / 2 - 140;
			for (int i = 0; i < 4; i++) {
				if (i == level2_puzzleBlankIndex) sprintf_s(buf, sizeof(buf), "_");
				else sprintf_s(buf, sizeof(buf), "%d", level2_puzzleSeq[i]);
				iText(seqX + i * 70, 260, buf, GLUT_BITMAP_TIMES_ROMAN_24);
			}
			for (int i = 0; i < 4; i++) {
				level2_drawOptionBox(i, 0);
				int bx, by; level2_optionBoxPos(i, &bx, &by);
				sprintf_s(buf, sizeof(buf), "%d", level2_puzzleOptions[i]);
				iSetColor(0, 0, 0);
				iText(bx + LEVEL2_OPT_W / 2 - 12, by + LEVEL2_OPT_H / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
			}
		}
		else if (t == LEVEL2_TASK_COLOR) {
			iSetColor(255, 255, 255);
			iText(SCREEN_WIDTH / 2 - 150, 220, "Select the matching color from below:", GLUT_BITMAP_HELVETICA_18);
			iSetColor(level2_colorTarget[0], level2_colorTarget[1], level2_colorTarget[2]);
			iFilledRectangle(SCREEN_WIDTH / 2 - 35, 240, 70, 60);
			iSetColor(0, 0, 0);
			iRectangle(SCREEN_WIDTH / 2 - 35, 240, 70, 60);

			for (int i = 0; i < 4; i++) {
				int bx, by; level2_optionBoxPos(i, &bx, &by);
				iSetColor(level2_colorOptions[i][0], level2_colorOptions[i][1], level2_colorOptions[i][2]);
				iFilledRectangle(bx, by, LEVEL2_OPT_W, LEVEL2_OPT_H);
				iSetColor(0, 0, 0);
				iRectangle(bx, by, LEVEL2_OPT_W, LEVEL2_OPT_H);
			}
		}

		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 180, LEVEL2_OPT_Y + LEVEL2_OPT_H + 30, "Choose correctly to unlock the door, wrong answer deducts energy!");
	}
}

inline void handleL2DoorClicks(int mx, int my)
{
	if (level2_keyFound) return;

	if (level2_finishStage == 1) {
		if (mx >= SCREEN_WIDTH / 2 - 120 && mx <= SCREEN_WIDTH / 2 + 120 &&
			my >= 200 && my <= 420) {
			level2_finishStage = 2;
		}
		return;
	}

	if (level2_finishStage == 2) {
		float dist = sqrtf((float)((mx - SCREEN_WIDTH / 2) * (mx - SCREEN_WIDTH / 2) + (my - 310) * (my - 310)));
		if (dist <= 50.0f) {
			level2_finishStage = 3;
			level2_keyFound = true;
			level2_updateScore(1000);
		}
		return;
	}

	if (level2_finishStage >= 1) return;
	if (!level2_doorsVisible) return;

	if (level2_insideTask) {
		int t = level2_doorTaskType[level2_currentTaskDoor];

		for (int i = 0; i < 4; i++) {
			int bx, by;
			level2_optionBoxPos(i, &bx, &by);
			if (level2_isInsideBox(mx, my, bx, by, LEVEL2_OPT_W, LEVEL2_OPT_H)) {
				bool correct = false;
				if (t == LEVEL2_TASK_MATH)        correct = (i == level2_mathCorrectIndex);
				else if (t == LEVEL2_TASK_PUZZLE) correct = (i == level2_puzzleCorrectIndex);
				else if (t == LEVEL2_TASK_COLOR)  correct = (i == level2_colorCorrectIndex);

				if (correct) {
					level2_doors[level2_currentTaskDoor].visited = true;
					level2_insideTask = false;
					level2_currentTaskDoor = -1;
					level2_updateScore(200);
					playPlusPointSound(); // সঠিক উত্তরে plusPoint.MP3 বাজবে
					strcpy_s(level2_message, sizeof(level2_message), "Correct! Door unlocked - Select another door.");
					level2_messageTimer = 100;
				}
				else {
					level2_energy -= LEVEL2_WRONG_TASK_PENALTY;
					playNegPointSound(); // ভুল উত্তরে negPoint.MP3 বাজবে
					strcpy_s(level2_message, sizeof(level2_message), "Wrong answer! Try again.");
					level2_messageTimer = 70;
					if (level2_energy <= 0) {
						level2_energy = 0;
						level2_gameOver = true;
					}
					else {
						level2_generateTaskForDoor(level2_currentTaskDoor);
					}
				}
				return;
			}
		}
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (level2_isInside(mx, my, level2_doors[i])) {
			int requiredKey = level2_doors[i].assignedKeyColorId;

			if (!level2_keyCollected[requiredKey]) {
				playNegPointSound(); // চাবি না থাকলে negPoint.MP3 বাজবে
				strcpy_s(level2_message, sizeof(level2_message), "Key missing! Go back and collect the matching key.");
				level2_messageTimer = 90;
				return;
			}

			if (i == level2_realKeyDoor) {
				level2_doors[i].visited = true;
				level2_finishStage = 1;
			}
			else if (!level2_doors[i].visited) {
				level2_currentTaskDoor = i;
				level2_insideTask = true;
				level2_generateTaskForDoor(i);
			}
			else {
				strcpy_s(level2_message, sizeof(level2_message), "Real key is not here! Pick another door.");
				level2_messageTimer = 70;
			}
			return;
		}
	}
}

#endif