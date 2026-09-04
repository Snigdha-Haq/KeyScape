#ifndef LEVEL2_H_INCLUDED
#define LEVEL2_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

// ---------------- BASIC LEVEL STATE ----------------
static float level2_playerX = 100.0f;
static float level2_playerY = 80.0f;
static int level2_playerWidth = 90, level2_playerHeight = 130;
static int level2_playerSpeed = 5;
static const float level2_groundY = 80.0f;

// Autonomous movement speeds towards player
static float level2_enemyAutoSpeed = 3.5f;
static float level2_keyAutoSpeed = 2.5f;

// Direction facing (true = right, false = left)
static bool level2_facingRight = true;

// Background sliding position
static int level2_bgX = 0;
static int level2_distanceCovered = 0;
#define LEVEL2_TARGET_DISTANCE 4500

// Animation
static bool level2_isMoving = false;
static int level2_animFrame = 0;
static int level2_animTimer = 0;
#define LEVEL2_ANIM_FRAME_DELAY 6

// Jump physics
static bool level2_isJumping = false;
static float level2_jumpVelocity = 0.0f;
#define LEVEL2_JUMP_STRENGTH 16.0f
#define LEVEL2_GRAVITY 0.8f
static int level2_jumpFrameIndex = 0;

// Energy & Game State
static int level2_energy = 100;
static bool level2_gameOver = false;
static bool level2_keyFound = false;

// ---------------- KEYS (RUNNING PHASE) ----------------
#define LEVEL2_NUM_KEYS 4
#define LEVEL2_KEY_SIZE 40

struct Level2Key {
	float x;
	int y, size;
	int id;
	bool spawned;
	bool active;
	bool collected;
};

static Level2Key level2_keys[LEVEL2_NUM_KEYS];
static int level2_keySpawnDistance[LEVEL2_NUM_KEYS] = { 400, 1300, 2200, 3100 };

static int level2_keyColor[LEVEL2_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // Key 0 - Gold
	{ 65, 145, 220 },  // Key 1 - Blue
	{ 60, 200, 90 },   // Key 2 - Green
	{ 225, 60, 60 }    // Key 3 - Red
};

static bool level2_keyCollected[LEVEL2_NUM_KEYS] = { false, false, false, false };

// ---------------- OBSTACLES (CRAB, OCTOPASS, SEAHORSE) ----------------
#define LEVEL2_NUM_OBSTACLES 5

struct Level2Obstacle {
	float x;
	int y, width, height;
	int type; // 0: Crab, 1: Octopass, 2: Seahorse
	bool spawned;
	bool active;
	bool hit;
};

static Level2Obstacle level2_obstacles[LEVEL2_NUM_OBSTACLES];
static int level2_obstacleSpawnDistance[LEVEL2_NUM_OBSTACLES] = { 850, 1750, 2650, 3550, 4000 };
#define LEVEL2_OBSTACLE_DAMAGE 15

// ---------------- DOORS ----------------
struct Level2Door {
	int x, y, width, height;
	bool visited;
};

#define LEVEL2_DOOR_WIDTH  120
#define LEVEL2_DOOR_HEIGHT 180
#define LEVEL2_DOOR_GAP    60
#define LEVEL2_DOORS_START_X ((SCREEN_WIDTH - (4*LEVEL2_DOOR_WIDTH + 3*LEVEL2_DOOR_GAP)) / 2)
#define LEVEL2_DOOR_Y 100

static Level2Door level2_doors[4];
static bool level2_doorsVisible = false;

static int level2_realKeyDoor = -1;

enum Level2TaskType { LEVEL2_TASK_NONE = 0, LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
static int level2_doorTaskType[4];

static bool level2_insideTask = false;
static int level2_currentTaskDoor = -1;

// ---------------- SHINY KEY & CLAM / PEARL CUTSCENE ----------------
static int level2_finishStage = 0;
static float level2_shineTimer = 0.0f;
static float level2_clamOpenAngle = 0.0f;

// --- Task states ---
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
#define LEVEL2_WRONG_TASK_PENALTY 8

static char level2_message[100] = "";
static int level2_messageTimer = 0;

// ---------------- TASK GENERATORS ----------------
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
	if (t == LEVEL2_TASK_MATH)   level2_generateMathTask();
	else if (t == LEVEL2_TASK_PUZZLE) level2_generatePuzzleTask();
	else if (t == LEVEL2_TASK_COLOR)  level2_generateColorTask();
}

inline void setupLevel2()
{
	srand((unsigned int)time(0));

	level2_doors[0] = { LEVEL2_DOORS_START_X, LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[1] = { LEVEL2_DOORS_START_X + (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[2] = { LEVEL2_DOORS_START_X + 2 * (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[3] = { LEVEL2_DOORS_START_X + 3 * (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };

	level2_playerX = 100.0f;
	level2_playerY = level2_groundY;
	level2_bgX = 0;
	level2_distanceCovered = 0;
	level2_doorsVisible = false;
	level2_facingRight = true;

	level2_energy = 100;
	level2_keyFound = false;
	level2_gameOver = false;
	level2_isMoving = false;
	level2_animFrame = 0;
	level2_animTimer = 0;

	level2_isJumping = false;
	level2_jumpVelocity = 0.0f;
	level2_jumpFrameIndex = 0;

	level2_message[0] = '\0';
	level2_messageTimer = 0;
	level2_finishStage = 0;
	level2_shineTimer = 0.0f;
	level2_clamOpenAngle = 0.0f;

	for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
		level2_keys[i].id = i;
		level2_keys[i].spawned = false;
		level2_keys[i].active = false;
		level2_keys[i].collected = false;
		level2_keys[i].size = LEVEL2_KEY_SIZE;
		level2_keyCollected[i] = false;
	}

	int obstacleTypes[LEVEL2_NUM_OBSTACLES] = { 0, 1, 2, 0, 1 };
	for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
		level2_obstacles[i].spawned = false;
		level2_obstacles[i].active = false;
		level2_obstacles[i].hit = false;
		level2_obstacles[i].type = obstacleTypes[i];

		if (level2_obstacles[i].type == 0) { // Crab
			level2_obstacles[i].width = 65;
			level2_obstacles[i].height = 60;
		}
		else if (level2_obstacles[i].type == 1) { // Octopass
			level2_obstacles[i].width = 70;
			level2_obstacles[i].height = 70;
		}
		else { // Seahorse
			level2_obstacles[i].width = 50;
			level2_obstacles[i].height = 80;
		}
	}

	level2_realKeyDoor = rand() % 4;
	{
		int types[3] = { LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
		for (int i = 2; i > 0; i--) {
			int j = rand() % (i + 1);
			int tmp = types[i]; types[i] = types[j]; types[j] = tmp;
		}
		int ti = 0;
		for (int i = 0; i < 4; i++) {
			if (i == level2_realKeyDoor) level2_doorTaskType[i] = LEVEL2_TASK_NONE;
			else level2_doorTaskType[i] = types[ti++];
		}
	}
	level2_insideTask = false;
	level2_currentTaskDoor = -1;
}

inline bool level2_rectOverlap(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline bool level2_isInside(int px, int py, Level2Door d)
{
	return (px >= d.x && px <= d.x + d.width && py >= d.y && py <= d.y + d.height);
}

inline bool level2_isInsideBox(int px, int py, int bx, int by, int bw, int bh)
{
	return (px >= bx && px <= bx + bw && py >= by && py <= by + bh);
}

// ---------------- DRAWING HELPERS ----------------
inline void level2_drawKey(float x, int y, int size, int r, int g, int b)
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
	level2_drawKey(kx, (int)ky, 100, 255, 215, 0);

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

inline void level2_optionBoxPos(int idx, int* bx, int* by)
{
	int totalW = 4 * LEVEL2_OPT_W + 3 * LEVEL2_OPT_GAP;
	int startX = (SCREEN_WIDTH - totalW) / 2;
	*bx = startX + idx * (LEVEL2_OPT_W + LEVEL2_OPT_GAP);
	*by = LEVEL2_OPT_Y;
}

inline void renderLevel2()
{
	static int UnderSeaBg = -1, doorClosedImg = -1, doorOpenImg = -1, idleImg = -1;
	static int runFrames[8];
	static int jumpFrames[3];
	static int obstacleImgs[3] = { -1, -1, -1 };

	static int bgMathImg = -1, bgPuzzleImg = -1, bgColorImg = -1, bgPearlImg = -1;
	static bool jumpImagesOK = true;

	if (UnderSeaBg == -1) {
		UnderSeaBg = iLoadImage("Image/UnderSea.png");
		doorClosedImg = iLoadImage("Image/doorclosed.png");
		doorOpenImg = iLoadImage("Image/dooropened.png");
		idleImg = iLoadImage("Image/idle_1.png");
		runFrames[0] = iLoadImage("Image/run_1.png");
		runFrames[1] = iLoadImage("Image/run_2.png");
		runFrames[2] = iLoadImage("Image/run_3.png");
		runFrames[3] = iLoadImage("Image/run_4.png");
		runFrames[4] = iLoadImage("Image/run_5.png");
		runFrames[5] = iLoadImage("Image/run_6.png");
		runFrames[6] = iLoadImage("Image/run_7.png");
		runFrames[7] = iLoadImage("Image/run_8.png");

		jumpFrames[0] = iLoadImage("Image/jump_1.png");
		jumpFrames[1] = iLoadImage("Image/jump_2.png");
		jumpFrames[2] = iLoadImage("Image/jump_3.png");

		jumpImagesOK = (jumpFrames[0] >= 0 && jumpFrames[1] >= 0 && jumpFrames[2] >= 0);

		bgMathImg = iLoadImage("Image/bgMath.png");
		bgPuzzleImg = iLoadImage("Image/bgPuzzle.png");
		bgColorImg = iLoadImage("Image/bgColor.png");
		bgPearlImg = iLoadImage("Image/bgPearl.png");

		obstacleImgs[0] = iLoadImage("Image/crab.png");
		obstacleImgs[1] = iLoadImage("Image/octopass.png");
		obstacleImgs[2] = iLoadImage("Image/seahorse.png");
	}

	// 1. Background Selection
	if (level2_doorsVisible && level2_finishStage >= 1) {
		if (bgPearlImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgPearlImg);
		else iShowImage(level2_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
	}
	else if (level2_doorsVisible && level2_insideTask && level2_currentTaskDoor >= 0) {
		int t = level2_doorTaskType[level2_currentTaskDoor];
		int selectedBg = -1;
		if (t == LEVEL2_TASK_MATH) selectedBg = bgMathImg;
		else if (t == LEVEL2_TASK_PUZZLE) selectedBg = bgPuzzleImg;
		else if (t == LEVEL2_TASK_COLOR) selectedBg = bgColorImg;

		if (selectedBg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, selectedBg);
		else iShowImage(level2_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
	}
	else {
		iShowImage(level2_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
		iShowImage(level2_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
	}

	// 2. Obstacles and Keys
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (level2_obstacles[i].active) {
				int img = obstacleImgs[level2_obstacles[i].type];
				if (img >= 0) {
					iShowImage((int)level2_obstacles[i].x, level2_obstacles[i].y,
						level2_obstacles[i].width, level2_obstacles[i].height, img);
				}
			}
		}

		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (level2_keys[i].active && !level2_keys[i].collected) {
				level2_drawKey(level2_keys[i].x, level2_keys[i].y, level2_keys[i].size,
					level2_keyColor[i][0], level2_keyColor[i][1], level2_keyColor[i][2]);
			}
		}
	}

	// 3. Player
	if (!level2_doorsVisible) {
		int playerImg;
		if (level2_isJumping) playerImg = jumpImagesOK ? jumpFrames[level2_jumpFrameIndex] : idleImg;
		else if (level2_isMoving) playerImg = runFrames[level2_animFrame];
		else playerImg = idleImg;

		iShowImage((int)level2_playerX, (int)level2_playerY, level2_playerWidth, level2_playerHeight, playerImg);
	}

	// 4. Doors & End Sequence
	if (level2_doorsVisible) {
		if (!level2_insideTask) {
			if (level2_finishStage == 0) {
				for (int i = 0; i < 4; i++) {
					int imgToUse = level2_doors[i].visited ? doorOpenImg : doorClosedImg;
					iShowImage(level2_doors[i].x, level2_doors[i].y, level2_doors[i].width, level2_doors[i].height, imgToUse);

					iSetColor(80, 80, 80);
					iRectangle(level2_doors[i].x - 3, level2_doors[i].y - 3, level2_doors[i].width + 6, level2_doors[i].height + 6);

					if (level2_doors[i].visited) {
						iSetColor(255, 215, 0);
						iText(level2_doors[i].x + 30, level2_doors[i].y + level2_doors[i].height + 10, "OPEN!");
					}
				}
			}

			if (level2_finishStage == 1) {
				level2_drawShinyRealKey(SCREEN_WIDTH / 2, 330);

				iSetColor(255, 255, 255);
				iFilledRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
				iSetColor(0, 0, 0);
				iRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
				iText(SCREEN_WIDTH / 2 - 145, 208, "ASHOL CHABI! Jhinuk khulte Click Koro!");
			}
			else if (level2_finishStage >= 2) {
				level2_drawClamAndPearl(SCREEN_WIDTH / 2, 300);

				if (level2_finishStage == 2) {
					iSetColor(255, 255, 255);
					iFilledRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
					iSetColor(0, 0, 0);
					iRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
					iText(SCREEN_WIDTH / 2 - 120, 145, "Mukta te Click kore Level Complete koro!");
				}
			}
		}
		else {
			int t = level2_doorTaskType[level2_currentTaskDoor];
			char buf[64];

			if (t == LEVEL2_TASK_MATH) {
				iSetColor(255, 255, 255);
				sprintf_s(buf, sizeof(buf), "%d + %d = ?", level2_mathA, level2_mathB);
				iText(SCREEN_WIDTH / 2 - 50, 250, buf);
				for (int i = 0; i < 4; i++) {
					level2_drawOptionBox(i, 0);
					int bx, by; level2_optionBoxPos(i, &bx, &by);
					sprintf_s(buf, sizeof(buf), "%d", level2_mathOptions[i]);
					iSetColor(0, 0, 0);
					iText(bx + LEVEL2_OPT_W / 2 - 8, by + LEVEL2_OPT_H / 2, buf);
				}
			}
			else if (t == LEVEL2_TASK_PUZZLE) {
				iSetColor(255, 255, 255);
				iText(SCREEN_WIDTH / 2 - 150, 230, "Missing shonkha ta khuje bar koro:");
				int seqX = SCREEN_WIDTH / 2 - 140;
				for (int i = 0; i < 4; i++) {
					if (i == level2_puzzleBlankIndex) sprintf_s(buf, sizeof(buf), "_");
					else sprintf_s(buf, sizeof(buf), "%d", level2_puzzleSeq[i]);
					iText(seqX + i * 70, 260, buf);
				}
				for (int i = 0; i < 4; i++) {
					level2_drawOptionBox(i, 0);
					int bx, by; level2_optionBoxPos(i, &bx, &by);
					sprintf_s(buf, sizeof(buf), "%d", level2_puzzleOptions[i]);
					iSetColor(0, 0, 0);
					iText(bx + LEVEL2_OPT_W / 2 - 8, by + LEVEL2_OPT_H / 2, buf);
				}
			}
			else if (t == LEVEL2_TASK_COLOR) {
				iSetColor(255, 255, 255);
				iText(SCREEN_WIDTH / 2 - 120, 220, "Eki color ta niche theke select koro:");
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
			iText(SCREEN_WIDTH / 2 - 160, LEVEL2_OPT_Y + LEVEL2_OPT_H + 30, "Shothik uttor dile dorja khulbe, bhul hole abar cheshta korte hobe.");
		}
	}

	// 5. HUD Display
	for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
		int hx = 25 + i * 35, hy = 25;
		if (level2_keyCollected[i])
			iSetColor(level2_keyColor[i][0], level2_keyColor[i][1], level2_keyColor[i][2]);
		else
			iSetColor(160, 160, 160);
		iFilledCircle(hx, hy, 12);
		iSetColor(0, 0, 0);
		iCircle(hx, hy, 12);
	}
	iSetColor(0, 0, 0);
	iText(20, 50, "Keys Collected");

	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(0, 200, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level2_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	iSetColor(0, 0, 0);
	if (!level2_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT - 40, "LEFT/RIGHT to Move, UP ARROW to jump!");
	}
	else if (!level2_insideTask && !level2_gameOver && !level2_keyFound && level2_finishStage == 0) {
		iText(SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT - 40, "Ekta dorjaay ASHOL chabi ache - beche nao!");
	}

	if (level2_messageTimer > 0) {
		iSetColor(220, 20, 20);
		iText(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT - 70, level2_message);
	}

	if (level2_gameOver)
		iText(SCREEN_WIDTH / 2 - 130, 480, "GAME OVER - Press R to restart");
	else if (level2_keyFound && level2_finishStage == 3)
		iText(SCREEN_WIDTH / 2 - 170, 500, "MUKTA PAWA GECHE! LEVEL 2 COMPLETE!");
}

inline void level2_fixedUpdate()
{
	if (level2_gameOver || level2_keyFound) return;

	level2_shineTimer += 1.0f;

	if (level2_finishStage >= 2 && level2_clamOpenAngle < 80.0f) {
		level2_clamOpenAngle += 2.5f;
	}

	if (level2_messageTimer > 0) level2_messageTimer--;

	level2_isMoving = false;

	// Jump
	if (!level2_doorsVisible && isSpecialKeyPressed(GLUT_KEY_UP)) {
		if (!level2_isJumping) {
			level2_isJumping = true;
			level2_jumpVelocity = LEVEL2_JUMP_STRENGTH;
			level2_playerY += 2.0f;
		}
	}

	// ---------------- PLAYER HORIZONTAL MOVEMENT ----------------
	float extraMoveKeys = 0.0f;
	float extraMoveEnemies = 0.0f;

	if (!level2_doorsVisible && isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level2_isMoving = true;
		level2_facingRight = true;

		level2_bgX -= level2_playerSpeed;
		if (level2_bgX <= -SCREEN_WIDTH) {
			level2_bgX = 0;
		}

		level2_distanceCovered += level2_playerSpeed;

		// Extra shift because player is running forward
		extraMoveKeys = (float)level2_playerSpeed;
		extraMoveEnemies = (float)level2_playerSpeed;

		if (level2_distanceCovered >= LEVEL2_TARGET_DISTANCE) {
			level2_doorsVisible = true;
		}
	}
	else if (!level2_doorsVisible && isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		if (level2_distanceCovered > 0) {
			level2_isMoving = true;
			level2_facingRight = false;

			level2_bgX += level2_playerSpeed;
			if (level2_bgX >= 0) {
				level2_bgX = -SCREEN_WIDTH;
			}

			level2_distanceCovered -= level2_playerSpeed;

			// Subtracted shift because player is backing away
			extraMoveKeys = -(float)level2_playerSpeed;
			extraMoveEnemies = -(float)level2_playerSpeed;
		}
	}

	// ---------------- AUTONOMOUS SPAWN & MOVEMENT TOWARDS PLAYER ----------------
	if (!level2_doorsVisible) {
		// Keys move towards player
		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (!level2_keys[i].spawned && level2_distanceCovered >= level2_keySpawnDistance[i]) {
				level2_keys[i].spawned = true;
				level2_keys[i].active = true;
				level2_keys[i].x = (float)SCREEN_WIDTH + 20;
				level2_keys[i].y = (int)level2_groundY + 45;
			}
			else if (level2_keys[i].active) {
				// Autonomous forward travel + relative shift from player movement
				level2_keys[i].x -= (level2_keyAutoSpeed + extraMoveKeys);
				if (level2_keys[i].x + level2_keys[i].size < -60) {
					level2_keys[i].active = false;
				}
			}
		}

		// Enemies move towards player
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].spawned && level2_distanceCovered >= level2_obstacleSpawnDistance[i]) {
				level2_obstacles[i].spawned = true;
				level2_obstacles[i].active = true;
				level2_obstacles[i].x = (float)SCREEN_WIDTH + 20;

				if (level2_obstacles[i].type == 0)      level2_obstacles[i].y = (int)level2_groundY;
				else if (level2_obstacles[i].type == 1) level2_obstacles[i].y = (int)level2_groundY + 10;
				else                                    level2_obstacles[i].y = (int)level2_groundY + 30;
			}
			else if (level2_obstacles[i].active) {
				// Autonomous forward crawl/swim + relative shift from player movement
				level2_obstacles[i].x -= (level2_enemyAutoSpeed + extraMoveEnemies);
				if (level2_obstacles[i].x + level2_obstacles[i].width < -60) {
					level2_obstacles[i].active = false;
				}
			}
		}
	}

	// Jump Physics
	if (level2_isJumping) {
		level2_playerY += level2_jumpVelocity;
		level2_jumpVelocity -= LEVEL2_GRAVITY;

		if (level2_jumpVelocity > 3.0f) level2_jumpFrameIndex = 0;
		else if (level2_jumpVelocity >= -3.0f) level2_jumpFrameIndex = 1;
		else level2_jumpFrameIndex = 2;

		if (level2_playerY <= level2_groundY) {
			level2_playerY = level2_groundY;
			level2_isJumping = false;
			level2_jumpVelocity = 0.0f;
			level2_jumpFrameIndex = 0;
		}
	}

	// Collisions & Pickups
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (!level2_keys[i].active || level2_keys[i].collected) continue;

			if (level2_rectOverlap(level2_keys[i].x, (float)level2_keys[i].y, level2_keys[i].size, level2_keys[i].size,
				level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight)) {
				level2_keys[i].collected = true;
				level2_keys[i].active = false;
				level2_keyCollected[level2_keys[i].id] = true;
			}
		}

		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].active || level2_obstacles[i].hit) continue;

			if (level2_rectOverlap(level2_obstacles[i].x, (float)level2_obstacles[i].y, level2_obstacles[i].width, level2_obstacles[i].height,
				level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight)) {
				level2_obstacles[i].hit = true;
				level2_energy -= LEVEL2_OBSTACLE_DAMAGE;
				if (level2_energy <= 0) {
					level2_energy = 0;
					level2_gameOver = true;
				}
			}
		}
	}

	if (level2_isMoving) {
		level2_animTimer++;
		if (level2_animTimer >= LEVEL2_ANIM_FRAME_DELAY) {
			level2_animTimer = 0;
			level2_animFrame = (level2_animFrame + 1) % 8;
		}
	}
	else {
		level2_animFrame = 0;
		level2_animTimer = 0;
	}
}

inline void handleLevel2DoorClicks(int mx, int my)
{
	if (level2_gameOver || level2_keyFound) return;

	if (level2_finishStage == 1) {
		if (mx >= SCREEN_WIDTH / 2 - 120 && mx <= SCREEN_WIDTH / 2 + 120 &&
			my >= 200 && my <= 420) {
			level2_finishStage = 2;
		}
		return;
	}

	if (level2_finishStage == 2) {
		float dist = sqrtf((float)((mx - SCREEN_WIDTH / 2)*(mx - SCREEN_WIDTH / 2) + (my - 310)*(my - 310)));
		if (dist <= 50.0f) {
			level2_finishStage = 3;
			level2_keyFound = true;
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
					strcpy_s(level2_message, sizeof(level2_message), "Shothik uttor! Dorja khule geche - onno dorja bechey nao.");
					level2_messageTimer = 100;
				}
				else {
					level2_energy -= LEVEL2_WRONG_TASK_PENALTY;
					strcpy_s(level2_message, sizeof(level2_message), "Bhul uttor! Abar cheshta koro.");
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
				strcpy_s(level2_message, sizeof(level2_message), "Ei dorjay ashol chabi nei, onno dorja bechey nao.");
				level2_messageTimer = 70;
			}
			return;
		}
	}
}

inline void handleLevel2SpecialKeyboard(unsigned char key)
{
	if (key == GLUT_KEY_UP) {
		if (level2_gameOver || level2_keyFound) return;
		if (level2_doorsVisible) return;
		if (!level2_isJumping) {
			level2_isJumping = true;
			level2_jumpVelocity = LEVEL2_JUMP_STRENGTH;
			level2_playerY += 2.0f;
		}
	}
}

inline void handleLevel2Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R') {
		setupLevel2();
		return;
	}
}

#endif