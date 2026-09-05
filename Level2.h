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

#define LEVEL2_PLAYER_NORMAL_W 90
#define LEVEL2_PLAYER_NORMAL_H 130
#define LEVEL2_PLAYER_SLIDE_W  120
#define LEVEL2_PLAYER_SLIDE_H  60

static int level2_playerWidth = LEVEL2_PLAYER_NORMAL_W;
static int level2_playerHeight = LEVEL2_PLAYER_NORMAL_H;
static int level2_playerSpeed = 5;
static const float level2_groundY = 80.0f;

// Slide mechanism
static bool level2_isSliding = false;
static int level2_slideTimer = 0;
#define LEVEL2_SLIDE_DURATION 28

// Autonomous speeds towards player
static float level2_enemyAutoSpeed = 3.8f;

static bool level2_facingRight = true;

static int level2_bgX = 0;
static int level2_distanceCovered = 0;
#define LEVEL2_TARGET_DISTANCE 6800

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

// ---------------- SCORE & PERSISTENT HIGH SCORE ----------------
static int level2_score = 0;
static int level2_highScore = 0;
static bool level2_highScoreLoaded = false;

inline void level2_loadHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level2_highscore.txt", "r");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &level2_highScore);
		fclose(fp);
	}
	level2_highScoreLoaded = true;
}

inline void level2_saveHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level2_highscore.txt", "w");
	if (fp != NULL) {
		fprintf(fp, "%d", level2_highScore);
		fclose(fp);
	}
}

inline void level2_updateScore(int addPoints)
{
	level2_score += addPoints;
	if (level2_score > level2_highScore) {
		level2_highScore = level2_score;
		level2_saveHighScore();
	}
}

// ---------------- KEYS (TRACK-ANCHORED: RETRIEVABLE ON BACKTRACK) ----------------
#define LEVEL2_NUM_KEYS 4
#define LEVEL2_KEY_SIZE 40

struct Level2Key {
	float y;
	int size;
	int id;
	int trackPos; // Absolute position on track
	bool isAir;
	bool collected;
};

static Level2Key level2_keys[LEVEL2_NUM_KEYS];
static int level2_keySpawnDistance[LEVEL2_NUM_KEYS] = { 700, 2200, 3900, 5300 };

static int level2_keyColor[LEVEL2_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // Key 0 - Gold
	{ 65, 145, 220 },  // Key 1 - Blue
	{ 60, 200, 90 },   // Key 2 - Green
	{ 225, 60, 60 }    // Key 3 - Red
};

static bool level2_keyCollected[LEVEL2_NUM_KEYS] = { false, false, false, false };

// ---------------- OBSTACLES (PRECISE HEIGHT FOR SLIDING) ----------------
#define LEVEL2_NUM_OBSTACLES 12

struct Level2Obstacle {
	float x;
	float y;
	float baseY;
	int width, height;
	int type;       // 0: Crab, 1: Octopass, 2: Seahorse
	bool isAir;
	float waveAngle;
	bool spawned;
	bool active;
	bool hit;
};

static Level2Obstacle level2_obstacles[LEVEL2_NUM_OBSTACLES];
static int level2_obstacleSpawnDistance[LEVEL2_NUM_OBSTACLES] = {
	450, 950, 1500, 1950, 2600, 3100, 3600, 4200, 4700, 5300, 5800, 6300
};
#define LEVEL2_OBSTACLE_DAMAGE 22

// ---------------- DOORS ----------------
struct Level2Door {
	int x, y, width, height;
	bool visited;
	int assignedKeyColorId;
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

static char level2_message[120] = "";
static int level2_messageTimer = 0;

// ---------------- TEXT HELPERS ----------------
inline void level2_drawBoldText(int x, int y, const char* str, void* font)
{
	iText(x, y, (char*)str, font);
	iText(x + 1, y, (char*)str, font);
	iText(x, y + 1, (char*)str, font);
	iText(x + 1, y + 1, (char*)str, font);
}

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

// ---------------- SETUP & RESET ----------------
inline void setupLevel2()
{
	srand((unsigned int)time(0));

	if (!level2_highScoreLoaded) {
		level2_loadHighScore();
	}

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

	level2_playerX = 100.0f;
	level2_playerY = level2_groundY;
	level2_playerWidth = LEVEL2_PLAYER_NORMAL_W;
	level2_playerHeight = LEVEL2_PLAYER_NORMAL_H;

	level2_isSliding = false;
	level2_slideTimer = 0;

	level2_bgX = 0;
	level2_distanceCovered = 0;
	level2_doorsVisible = false;
	level2_facingRight = true;

	level2_score = 0;
	level2_energy = 100;
	level2_gameOver = false;
	level2_keyFound = false;
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

	// Anchored Keys (Track relative)
	bool keyAirList[LEVEL2_NUM_KEYS] = { false, true, false, true };
	for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
		level2_keys[i].id = i;
		level2_keys[i].isAir = keyAirList[i];
		level2_keys[i].trackPos = level2_keySpawnDistance[i];
		level2_keys[i].collected = false;
		level2_keys[i].size = LEVEL2_KEY_SIZE;
		level2_keys[i].y = level2_keys[i].isAir ? 210.0f : (level2_groundY + 25.0f);
		level2_keyCollected[i] = false;
	}

	// 12 Obstacles setup with accurate touchable height for air enemies
	int types[LEVEL2_NUM_OBSTACLES] = { 0, 2, 1, 0, 2, 0, 1, 2, 0, 1, 2, 0 };
	bool airMode[LEVEL2_NUM_OBSTACLES] = { false, true, true, false, true, false, false, true, false, true, true, false };

	for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
		level2_obstacles[i].spawned = false;
		level2_obstacles[i].active = false;
		level2_obstacles[i].hit = false;
		level2_obstacles[i].type = types[i];
		level2_obstacles[i].isAir = airMode[i];
		level2_obstacles[i].waveAngle = 0.0f;
		level2_obstacles[i].x = (float)SCREEN_WIDTH + 500.0f;

		if (level2_obstacles[i].type == 0) { // Ground Crab
			level2_obstacles[i].width = 65;
			level2_obstacles[i].height = 60;
			level2_obstacles[i].baseY = (float)level2_groundY;
		}
		else if (level2_obstacles[i].type == 1) { // Octopass
			level2_obstacles[i].width = 70;
			level2_obstacles[i].height = 70;
			// Hits normal player (head at 210), clears sliding player (height 60, top at 140)
			level2_obstacles[i].baseY = level2_obstacles[i].isAir ? 150.0f : ((float)level2_groundY + 10.0f);
		}
		else { // Seahorse
			level2_obstacles[i].width = 50;
			level2_obstacles[i].height = 75;
			level2_obstacles[i].baseY = level2_obstacles[i].isAir ? 155.0f : ((float)level2_groundY + 20.0f);
		}
		level2_obstacles[i].y = level2_obstacles[i].baseY;
	}

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
inline void level2_drawKey(float x, float y, int size, int r, int g, int b)
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
	level2_drawKey(kx, ky, 100, 255, 215, 0);

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
	static int UnderSeaBg = -1, doorClosedImg = -1, doorOpenImg = -1, idleImg = -1, slideImg = -1;
	static int runFrames[8];
	static int jumpFrames[3];
	static int obstacleImgs[3] = { -1, -1, -1 };

	static int bgMathImg = -1, bgPuzzleImg = -1, bgColorImg = -1, bgPearlImg = -1;
	static int bgSeaScoreImg = -1, bgSeaOutImg = -1;
	static bool jumpImagesOK = true;

	if (UnderSeaBg == -1) {
		UnderSeaBg = iLoadImage("Image/UnderSea.png");
		doorClosedImg = iLoadImage("Image/doorclosed.png");
		doorOpenImg = iLoadImage("Image/dooropened.png");
		idleImg = iLoadImage("Image/idle_1.png");
		slideImg = iLoadImage("Image/slide.png");

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

		bgSeaScoreImg = iLoadImage("Image/bgSeaScore.png");
		bgSeaOutImg = iLoadImage("Image/bgSeaOut.png");

		obstacleImgs[0] = iLoadImage("Image/crab.png");
		obstacleImgs[1] = iLoadImage("Image/octopass.png");
		obstacleImgs[2] = iLoadImage("Image/seahorse.png");
	}

	// ---------------- 1. GAME OVER SCREEN ----------------
	if (level2_gameOver) {
		if (bgSeaOutImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaOutImg);
		else {
			iSetColor(225, 230, 235);
			iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		iSetColor(180, 25, 25);
		level2_drawBoldText(440, 365, "YOU ARE OUT!", GLUT_BITMAP_TIMES_ROMAN_24);

		char endScoreBuf[100];
		sprintf_s(endScoreBuf, sizeof(endScoreBuf), "Score: %d     |     High Score: %d", level2_score, level2_highScore);
		iSetColor(15, 35, 75);
		level2_drawBoldText(390, 315, endScoreBuf, GLUT_BITMAP_HELVETICA_18);

		iSetColor(215, 65, 45);
		iFilledRectangle(380, 245, 290, 38);
		iSetColor(255, 255, 255);
		iRectangle(380, 245, 290, 38);
		level2_drawBoldText(402, 257, "Click or Press 'R' to Restart", GLUT_BITMAP_HELVETICA_18);
		return;
	}

	// ---------------- 2. VICTORY SCREEN ----------------
	if (level2_keyFound && level2_finishStage == 3) {
		if (bgSeaScoreImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaScoreImg);
		else {
			iSetColor(15, 35, 55);
			iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		char yourScoreBuf[64];
		sprintf_s(yourScoreBuf, sizeof(yourScoreBuf), "Your Score: %d", level2_score);
		iSetColor(255, 255, 255);
		level2_drawBoldText(395, 185, yourScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

		char highScoreBuf[64];
		sprintf_s(highScoreBuf, sizeof(highScoreBuf), "HighScore: %d", level2_highScore);
		iSetColor(255, 240, 180);
		level2_drawBoldText(395, 125, highScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);
		return;
	}

	// ---------------- 3. BACKGROUND (GAMEPLAY) ----------------
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

	// 4. Obstacles and Keys
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (level2_obstacles[i].active) {
				int img = obstacleImgs[level2_obstacles[i].type];
				if (img >= 0) {
					iShowImage((int)level2_obstacles[i].x, (int)level2_obstacles[i].y,
						level2_obstacles[i].width, level2_obstacles[i].height, img);
				}
			}
		}

		// Render uncollected keys when within current viewport
		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (!level2_keys[i].collected) {
				float screenKeyX = (float)(level2_keys[i].trackPos - level2_distanceCovered + 100);
				if (screenKeyX >= -50 && screenKeyX <= SCREEN_WIDTH + 50) {
					level2_drawKey(screenKeyX, level2_keys[i].y, level2_keys[i].size,
						level2_keyColor[i][0], level2_keyColor[i][1], level2_keyColor[i][2]);
				}
			}
		}
	}

	// 5. Player
	if (!level2_doorsVisible) {
		int playerImg;
		if (level2_isSliding) {
			playerImg = (slideImg >= 0) ? slideImg : idleImg;
		}
		else if (level2_isJumping) {
			playerImg = jumpImagesOK ? jumpFrames[level2_jumpFrameIndex] : idleImg;
		}
		else if (level2_isMoving) {
			playerImg = runFrames[level2_animFrame];
		}
		else {
			playerImg = idleImg;
		}

		iShowImage((int)level2_playerX, (int)level2_playerY, level2_playerWidth, level2_playerHeight, playerImg);
	}

	// 6. Doors & Tasks
	if (level2_doorsVisible) {
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
				level2_drawBoldText(SCREEN_WIDTH / 2 - 50, 250, buf, GLUT_BITMAP_TIMES_ROMAN_24);
				for (int i = 0; i < 4; i++) {
					level2_drawOptionBox(i, 0);
					int bx, by; level2_optionBoxPos(i, &bx, &by);
					sprintf_s(buf, sizeof(buf), "%d", level2_mathOptions[i]);
					iSetColor(0, 0, 0);
					level2_drawBoldText(bx + LEVEL2_OPT_W / 2 - 12, by + LEVEL2_OPT_H / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
				}
			}
			else if (t == LEVEL2_TASK_PUZZLE) {
				iSetColor(255, 255, 255);
				level2_drawBoldText(SCREEN_WIDTH / 2 - 160, 230, "Find the missing number in sequence:", GLUT_BITMAP_HELVETICA_18);
				int seqX = SCREEN_WIDTH / 2 - 140;
				for (int i = 0; i < 4; i++) {
					if (i == level2_puzzleBlankIndex) sprintf_s(buf, sizeof(buf), "_");
					else sprintf_s(buf, sizeof(buf), "%d", level2_puzzleSeq[i]);
					level2_drawBoldText(seqX + i * 70, 260, buf, GLUT_BITMAP_TIMES_ROMAN_24);
				}
				for (int i = 0; i < 4; i++) {
					level2_drawOptionBox(i, 0);
					int bx, by; level2_optionBoxPos(i, &bx, &by);
					sprintf_s(buf, sizeof(buf), "%d", level2_puzzleOptions[i]);
					iSetColor(0, 0, 0);
					level2_drawBoldText(bx + LEVEL2_OPT_W / 2 - 12, by + LEVEL2_OPT_H / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
				}
			}
			else if (t == LEVEL2_TASK_COLOR) {
				iSetColor(255, 255, 255);
				level2_drawBoldText(SCREEN_WIDTH / 2 - 150, 220, "Select the matching color from below:", GLUT_BITMAP_HELVETICA_18);
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

	// 7. HUD Display
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

	// Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level2_energy > 30 ? 0 : 220, level2_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level2_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	// Live Score
	char scoreBuf[64];
	iSetColor(0, 0, 0);
	sprintf_s(scoreBuf, sizeof(scoreBuf), "Score: %d", level2_score);
	level2_drawBoldText(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 40, scoreBuf, GLUT_BITMAP_HELVETICA_18);

	if (!level2_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 40, "UP to Jump (Ground), DOWN to Slide (Air)!");
	}
	else if (!level2_insideTask && level2_finishStage == 0) {
		iText(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT - 40, "Match the door's color circle with your collected keys!");
	}

	if (level2_messageTimer > 0) {
		iSetColor(220, 20, 20);
		level2_drawBoldText(SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT - 70, level2_message, GLUT_BITMAP_HELVETICA_18);
	}
}

inline void level2_fixedUpdate()
{
	if (level2_gameOver || level2_keyFound) {
		if (isKeyPressed('r') || isKeyPressed('R')) {
			setupLevel2();
			return;
		}
		return;
	}

	level2_shineTimer += 1.0f;

	if (level2_finishStage >= 2 && level2_clamOpenAngle < 80.0f) {
		level2_clamOpenAngle += 2.5f;
	}

	if (level2_messageTimer > 0) level2_messageTimer--;

	level2_isMoving = false;

	// Slide Down Check
	if (!level2_doorsVisible && !level2_isJumping && isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		level2_isSliding = true;
		level2_slideTimer = LEVEL2_SLIDE_DURATION;
		level2_playerWidth = LEVEL2_PLAYER_SLIDE_W;
		level2_playerHeight = LEVEL2_PLAYER_SLIDE_H;
	}

	if (level2_isSliding) {
		level2_slideTimer--;
		if (level2_slideTimer <= 0) {
			level2_isSliding = false;
			level2_playerWidth = LEVEL2_PLAYER_NORMAL_W;
			level2_playerHeight = LEVEL2_PLAYER_NORMAL_H;
		}
	}

	// Jump Up Check
	if (!level2_doorsVisible && !level2_isSliding && isSpecialKeyPressed(GLUT_KEY_UP)) {
		if (!level2_isJumping) {
			level2_isJumping = true;
			level2_jumpVelocity = LEVEL2_JUMP_STRENGTH;
			level2_playerY += 2.0f;
		}
	}

	float extraMoveEnemies = 0.0f;

	// Forward Movement
	if (!level2_doorsVisible && (isSpecialKeyPressed(GLUT_KEY_RIGHT) || level2_isSliding)) {
		level2_isMoving = true;
		level2_facingRight = true;

		level2_bgX -= level2_playerSpeed;
		if (level2_bgX <= -SCREEN_WIDTH) {
			level2_bgX = 0;
		}

		level2_distanceCovered += level2_playerSpeed;
		level2_updateScore(1);

		extraMoveEnemies = (float)level2_playerSpeed;

		if (level2_distanceCovered >= LEVEL2_TARGET_DISTANCE) {
			level2_doorsVisible = true;
		}
	}
	// Backward Movement (Enables returning to uncollected keys)
	else if (!level2_doorsVisible && !level2_isSliding && isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		if (level2_distanceCovered > 0) {
			level2_isMoving = true;
			level2_facingRight = false;

			level2_bgX += level2_playerSpeed;
			if (level2_bgX >= 0) {
				level2_bgX = -SCREEN_WIDTH;
			}

			level2_distanceCovered -= level2_playerSpeed;
			extraMoveEnemies = -(float)level2_playerSpeed;
		}
	}

	// Autonomous Enemies Movement
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].spawned && level2_distanceCovered >= level2_obstacleSpawnDistance[i]) {
				level2_obstacles[i].spawned = true;
				level2_obstacles[i].active = true;
				level2_obstacles[i].x = (float)SCREEN_WIDTH + 20;
				level2_obstacles[i].y = level2_obstacles[i].baseY;
			}
			else if (level2_obstacles[i].active) {
				level2_obstacles[i].x -= (level2_enemyAutoSpeed + extraMoveEnemies);

				// Floating enemies oscillate slightly
				if (level2_obstacles[i].isAir) {
					level2_obstacles[i].waveAngle += 0.08f;
					level2_obstacles[i].y = level2_obstacles[i].baseY + sinf(level2_obstacles[i].waveAngle) * 12.0f;
				}

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
		// Key pickups (Active relative to player in both directions)
		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (level2_keys[i].collected) continue;

			float screenKeyX = (float)(level2_keys[i].trackPos - level2_distanceCovered + 100);

			if (level2_rectOverlap(screenKeyX, level2_keys[i].y, level2_keys[i].size, level2_keys[i].size,
				level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight)) {
				level2_keys[i].collected = true;
				level2_keyCollected[level2_keys[i].id] = true;
				level2_updateScore(150);
			}
		}

		// Obstacle Collisions: Sliders dodge under air obstacles
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].active || level2_obstacles[i].hit) continue;

			if (level2_rectOverlap(level2_obstacles[i].x, level2_obstacles[i].y, level2_obstacles[i].width, level2_obstacles[i].height,
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

	if (level2_isMoving && !level2_isSliding) {
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
	// Restart click when Game Over
	if (level2_gameOver) {
		if (mx >= 380 && mx <= 670 && my >= 245 && my <= 285) {
			setupLevel2();
		}
		return;
	}

	if (level2_keyFound) return;

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
			level2_updateScore(1000);
		}
		return;
	}

	if (level2_finishStage >= 1) return;
	if (!level2_doorsVisible) return;

	// Inside Mini-game tasks
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
					strcpy_s(level2_message, sizeof(level2_message), "Correct! Door unlocked - Select another door.");
					level2_messageTimer = 100;
				}
				else {
					level2_energy -= LEVEL2_WRONG_TASK_PENALTY;
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

	// ---------------- DOOR CLICK WITH STRICT KEY CHECK ----------------
	for (int i = 0; i < 4; i++) {
		if (level2_isInside(mx, my, level2_doors[i])) {
			int requiredKey = level2_doors[i].assignedKeyColorId;

			// Check if the matching key is collected
			if (!level2_keyCollected[requiredKey]) {
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

inline void handleLevel2SpecialKeyboard(unsigned char key)
{
	if (level2_gameOver || level2_keyFound) return;
	if (level2_doorsVisible) return;

	if (key == GLUT_KEY_UP) {
		if (!level2_isJumping && !level2_isSliding) {
			level2_isJumping = true;
			level2_jumpVelocity = LEVEL2_JUMP_STRENGTH;
			level2_playerY += 2.0f;
		}
	}
	else if (key == GLUT_KEY_DOWN) {
		if (!level2_isJumping && !level2_isSliding) {
			level2_isSliding = true;
			level2_slideTimer = LEVEL2_SLIDE_DURATION;
			level2_playerWidth = LEVEL2_PLAYER_SLIDE_W;
			level2_playerHeight = LEVEL2_PLAYER_SLIDE_H;
		}
	}
}

inline void handleLevel2Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R') {
		setupLevel2();
	}
}

#endif