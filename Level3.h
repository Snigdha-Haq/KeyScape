#ifndef LEVEL3_H_INCLUDED
#define LEVEL3_H_INCLUDED

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

extern int gameState;
extern bool isSoundMuted;

// SOUND EFFECT HELPERS
inline void level3_playPlusPointSound() {
	if (isSoundMuted) return;
	mciSendString("close sfx_plus3", NULL, 0, NULL);
	mciSendString("open \"Audios/plusPoint.MP3\" type mpegvideo alias sfx_plus3", NULL, 0, NULL);
	mciSendString("play sfx_plus3 from 0", NULL, 0, NULL);
}

inline void level3_playNegPointSound() {
	if (isSoundMuted) return;
	mciSendString("close sfx_neg3", NULL, 0, NULL);
	mciSendString("open \"Audios/negPoint.MP3\" type mpegvideo alias sfx_neg3", NULL, 0, NULL);
	mciSendString("play sfx_neg3 from 0", NULL, 0, NULL);
}

inline void level3_playWinSound() {
	if (isSoundMuted) return;
	mciSendString("close sfx_win3", NULL, 0, NULL);
	mciSendString("open \"Audios/win_and_lose_melodies_-_arranged_win.MP3\" type mpegvideo alias sfx_win3", NULL, 0, NULL);
	mciSendString("play sfx_win3 from 0", NULL, 0, NULL);
}

inline void level3_playLoseSound() {
	if (isSoundMuted) return;
	mciSendString("close sfx_lose3", NULL, 0, NULL);
	mciSendString("open \"Audios/win_and_lose_melodies_-_arranged_lose.MP3\" type mpegvideo alias sfx_lose3", NULL, 0, NULL);
	mciSendString("play sfx_lose3 from 0", NULL, 0, NULL);
}

// Player physics
static float level3_playerX = 100.0f;
static float level3_playerY = 80.0f;

#define LEVEL3_PLAYER_NORMAL_W 90
#define LEVEL3_PLAYER_NORMAL_H 130
#define LEVEL3_PLAYER_SLIDE_W  120
#define LEVEL3_PLAYER_SLIDE_H  60

static int level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
static int level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
static int level3_playerSpeed = 5;
static const float level3_groundY = 80.0f;

// Slide mechanism
static bool level3_isSliding = false;
static int level3_slideTimer = 0;
#define LEVEL3_SLIDE_DURATION 28

// Enemy speed
static float level3_enemyAutoSpeed = 3.8f;
static bool level3_facingRight = true;

static int level3_bgX = 0;
static int level3_distanceCovered = 0;
#define LEVEL3_TARGET_DISTANCE 7200

// Animation
static bool level3_isMoving = false;
static int level3_animFrame = 0;
static int level3_animTimer = 0;
#define LEVEL3_ANIM_FRAME_DELAY 6

// Jump physics
static bool level3_isJumping = false;
static float level3_jumpVelocity = 0.0f;
#define LEVEL3_JUMP_STRENGTH 16.0f
#define LEVEL3_GRAVITY 0.8f
static int level3_jumpFrameIndex = 0;

// Energy & State
int level3_energy = 100;
bool level3_gameOver = false;
bool level3_keyFound = false;
#define level3_gameWon level3_keyFound
static bool level3_isPaused = false;
static bool level3_hasPlayedEndAudio = false;

// Settings UI constants
static bool level3_showSettingsMenu = false;
#define LEVEL3_SETTING_BTN_X 945
#define LEVEL3_SETTING_BTN_Y 45
#define LEVEL3_SETTING_BTN_R 22

#define LEVEL3_SUB_BTN_R     20
#define LEVEL3_SUB_R_Y       100
#define LEVEL3_SUB_P_Y       150
#define LEVEL3_SUB_M_Y       200
#define LEVEL3_SUB_S_Y       250

// Score
int level3_score = 0;
int level3_highScore = 0;
static bool level3_highScoreLoaded = false;
static int level3_scoreMultiplier = 1;
static int level3_multiplierTimer = 0;

// Power-up
struct Level3PowerUp {
	float x, y;
	int size;
	bool active;
	float speed;
};
static Level3PowerUp level3_power2x;
static int level3_powerSpawnCounter = 0;

inline void level3_loadHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level3_highscore.txt", "r");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &level3_highScore);
		fclose(fp);
	}
	level3_highScoreLoaded = true;
}

inline void level3_saveHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level3_highscore.txt", "w");
	if (fp != NULL) {
		fprintf(fp, "%d", level3_highScore);
		fclose(fp);
	}
}

inline void level3_updateScore(int addPoints)
{
	if (addPoints > 0) {
		level3_score += (addPoints * level3_scoreMultiplier);
	}
	else {
		level3_score += addPoints;
		if (level3_score < 0) level3_score = 0;
	}

	if (level3_score > level3_highScore) {
		level3_highScore = level3_score;
		level3_saveHighScore();
	}
}

// 5 KEYS CONFIGURATION
#define LEVEL3_NUM_KEYS 5
#define LEVEL3_KEY_SIZE 42

struct Level3Key {
	float y;
	int size;
	int id;
	int trackPos;
	bool isAir;
	bool collected;
};

static Level3Key level3_keys[LEVEL3_NUM_KEYS];
static int level3_keySpawnDistance[LEVEL3_NUM_KEYS] = { 800, 2000, 3200, 4400, 5600 };

int level3_keyColor[LEVEL3_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // 0: Gold
	{ 65, 145, 220 },  // 1: Blue
	{ 60, 200, 90 },   // 2: Green
	{ 225, 60, 60 },   // 3: Red
	{ 180, 80, 220 }   // 4: Purple
};

bool level3_keyCollected[LEVEL3_NUM_KEYS] = { false, false, false, false, false };

// 12 OBSTACLES
#define LEVEL3_NUM_OBSTACLES 12

struct Level3Obstacle {
	float x, y, baseY;
	int width, height, type;
	bool isAir;
	float waveAngle;
	bool spawned, active, hit;
};

static Level3Obstacle level3_obstacles[LEVEL3_NUM_OBSTACLES];
static int level3_obstacleSpawnDistance[LEVEL3_NUM_OBSTACLES] = {
	450, 1100, 1600, 2300, 2800, 3500, 4000, 4700, 5200, 5900, 6300, 6700
};

char level3_message[120] = "";
int level3_messageTimer = 0;
int level3_finishStage = 0;

// 5 CAVE ENTRANCES / DOORS CONFIGURATION
#define LEVEL3_NUM_DOORS 5
#define LEVEL3_OPT_W   150
#define LEVEL3_OPT_H   70
#define LEVEL3_OPT_GAP 30
#define LEVEL3_OPT_Y   330

struct Level3Door {
	int x, y, width, height;
	bool visited;
	int assignedKeyColorId;
	int handlerIndex; // 0..4 (Sequential: Cave 0->Door1, ..., Cave 4->Door5/Boss)
};

static Level3Door level3_doors[LEVEL3_NUM_DOORS];
static bool level3_doorsVisible = false;
static bool level3_insideTask = false;
static int level3_currentTaskDoor = -1;
static float level3_shineTimer = 0.0f;
static float level3_clamOpenAngle = 0.0f;

// 5 SEPARATE DOOR OPERATOR HEADERS
#include "L3door1.h"
#include "L3door2.h"
#include "L3door3.h"
#include "L3door4.h"
#include "L3door5.h"

inline void level3_drawBoldText(int x, int y, const char* str, void* font)
{
	iText(x, y, (char*)str, font);
	iText(x + 1, y, (char*)str, font);
	iText(x, y + 1, (char*)str, font);
	iText(x + 1, y + 1, (char*)str, font);
}

inline bool level3_rectOverlap(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline void level3_drawKey(float x, float y, int size, int r, int g, int b)
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

inline void level3_drawShinyRealKey(int cx, int cy)
{
	float pulse = (sin(level3_shineTimer * 0.1f) + 1.0f) * 0.5f;
	int glowRadius = (int)(70 + pulse * 25);

	iSetColor(255, 235, 120);
	iFilledCircle(cx, cy, glowRadius);
	iSetColor(255, 255, 200);
	iFilledCircle(cx, cy, (int)(glowRadius * 0.7f));

	iSetColor(255, 255, 255);
	for (int a = 0; a < 8; a++) {
		float angle = a * 3.14159f / 4.0f + level3_shineTimer * 0.05f;
		int x1 = cx + (int)(cos(angle) * (glowRadius + 15));
		int y1 = cy + (int)(sin(angle) * (glowRadius + 15));
		iLine(cx, cy, x1, y1);
	}

	float kx = cx - 50.0f;
	float ky = cy - 25.0f;
	level3_drawKey(kx, ky, 100, 255, 215, 0);
}

inline void level3_drawClamAndPearl(int cx, int cy)
{
	iSetColor(190, 160, 130);
	iFilledCircle(cx, cy - 20, 112);
	iSetColor(140, 110, 85);
	iCircle(cx, cy - 20, 112);
	iSetColor(245, 238, 225);
	iFilledCircle(cx, cy - 16, 102);
	iSetColor(255, 248, 238);
	iFilledCircle(cx, cy - 14, 90);

	float pearlGlow = (sin(level3_shineTimer * 0.15f) + 1.0f) * 0.5f;
	iSetColor(230, 245, 255);
	iFilledCircle(cx, cy + 10, (int)(42 + pearlGlow * 8));
	iSetColor(255, 255, 255);
	iFilledCircle(cx, cy + 10, 36);

	int topY = cy - 20 + (int)level3_clamOpenAngle;
	iSetColor(210, 180, 145);
	iFilledCircle(cx, topY + 80, 108);
	iSetColor(150, 120, 90);
	iCircle(cx, topY + 80, 108);
}

inline void level3_draw2XOrb(float x, float y)
{
	iSetColor(255, 215, 0);
	iFilledCircle(x + 20, y + 20, 24);
	iSetColor(255, 140, 0);
	iCircle(x + 20, y + 20, 24);
	iSetColor(255, 255, 230);
	iFilledCircle(x + 20, y + 20, 18);
	iSetColor(180, 20, 10);
	level3_drawBoldText((int)x + 10, (int)y + 12, "2X", GLUT_BITMAP_TIMES_ROMAN_24);
}

inline void level3_drawSettingsUI()
{
	iSetColor(30, 45, 65);
	iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SETTING_BTN_Y, LEVEL3_SETTING_BTN_R);
	iSetColor(255, 255, 255);
	iCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SETTING_BTN_Y, LEVEL3_SETTING_BTN_R);

	for (int i = 0; i < 8; i++) {
		float angle = (float)i * 3.14159f / 4.0f;
		int x1 = LEVEL3_SETTING_BTN_X + (int)(cosf(angle) * 12.0f);
		int y1 = LEVEL3_SETTING_BTN_Y + (int)(sinf(angle) * 12.0f);
		int x2 = LEVEL3_SETTING_BTN_X + (int)(cosf(angle) * 20.0f);
		int y2 = LEVEL3_SETTING_BTN_Y + (int)(sinf(angle) * 20.0f);
		iLine(x1, y1, x2, y2);
	}
	iSetColor(230, 240, 255);
	iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SETTING_BTN_Y, 8);

	if (level3_showSettingsMenu) {
		// Restart 'R'
		iSetColor(220, 50, 50);
		iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_R_Y, LEVEL3_SUB_BTN_R);
		level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_R_Y - 7, "R", GLUT_BITMAP_TIMES_ROMAN_24);

		// Pause 'P'
		iSetColor(50, 130, 220);
		iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_P_Y, LEVEL3_SUB_BTN_R);
		level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_P_Y - 7, "P", GLUT_BITMAP_TIMES_ROMAN_24);

		// Menu 'M'
		iSetColor(45, 175, 75);
		iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_M_Y, LEVEL3_SUB_BTN_R);
		level3_drawBoldText(LEVEL3_SETTING_BTN_X - 8, LEVEL3_SUB_M_Y - 7, "M", GLUT_BITMAP_TIMES_ROMAN_24);

		// Sound 'S'
		if (isSoundMuted) iSetColor(120, 120, 120);
		else iSetColor(230, 140, 20);
		iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_S_Y, LEVEL3_SUB_BTN_R);
		level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_S_Y - 7, "S", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	if (level3_isPaused && !level3_gameOver && !level3_keyFound) {
		iSetColor(0, 0, 0);
		iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 30, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2, "Click 'P' or press 'P' to Resume", GLUT_BITMAP_HELVETICA_18);
	}
}

// SETUP & RESET
inline void setupLevel3()
{
	srand((unsigned int)time(0));

	if (!level3_highScoreLoaded) level3_loadHighScore();

	level3_playerX = 100.0f;
	level3_playerY = level3_groundY;
	level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
	level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
	level3_isSliding = false;
	level3_slideTimer = 0;
	level3_bgX = 0;
	level3_distanceCovered = 0;
	level3_facingRight = true;
	level3_score = 0;
	level3_scoreMultiplier = 1;
	level3_multiplierTimer = 0;

	level3_power2x.active = false;
	level3_power2x.size = 40;
	level3_power2x.speed = 4.2f;
	level3_powerSpawnCounter = 0;

	level3_energy = 100;
	level3_gameOver = false;
	level3_keyFound = false;
	level3_isPaused = false;
	level3_showSettingsMenu = false;
	level3_hasPlayedEndAudio = false;

	level3_isMoving = false;
	level3_animFrame = 0;
	level3_animTimer = 0;
	level3_isJumping = false;
	level3_jumpVelocity = 0.0f;
	level3_jumpFrameIndex = 0;
	level3_message[0] = '\0';
	level3_messageTimer = 0;

	// Setup 5 Keys
	bool keyAirList[LEVEL3_NUM_KEYS] = { false, true, false, true, false };
	for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
		level3_keys[i].id = i;
		level3_keys[i].isAir = keyAirList[i];
		level3_keys[i].trackPos = level3_keySpawnDistance[i];
		level3_keys[i].collected = false;
		level3_keys[i].size = LEVEL3_KEY_SIZE;
		level3_keys[i].y = level3_keys[i].isAir ? 210.0f : (level3_groundY + 25.0f);
		level3_keyCollected[i] = false;
	}

	// Setup Obstacles
	int types[LEVEL3_NUM_OBSTACLES] = { 0, 2, 1, 0, 2, 0, 1, 2, 0, 1, 2, 0 };
	bool airMode[LEVEL3_NUM_OBSTACLES] = { false, true, true, false, true, false, false, true, false, true, true, false };
	for (int i = 0; i < LEVEL3_NUM_OBSTACLES; i++) {
		level3_obstacles[i].spawned = false;
		level3_obstacles[i].active = false;
		level3_obstacles[i].hit = false;
		level3_obstacles[i].type = types[i];
		level3_obstacles[i].isAir = airMode[i];
		level3_obstacles[i].waveAngle = 0.0f;
		level3_obstacles[i].x = (float)SCREEN_WIDTH + 500.0f;
		level3_obstacles[i].width = (types[i] == 0) ? 65 : ((types[i] == 1) ? 70 : 50);
		level3_obstacles[i].height = (types[i] == 0) ? 60 : ((types[i] == 1) ? 70 : 75);
		level3_obstacles[i].baseY = (types[i] == 0) ? level3_groundY : (airMode[i] ? 150.0f : (level3_groundY + 15.0f));
		level3_obstacles[i].y = level3_obstacles[i].baseY;
	}

	// Setup 5 Cave Entrances based on bgCave.png
	int caveX[5] = { 215, 325, 445, 565, 680 };
	int caveY = 160;
	int caveW = 90;
	int caveH = 150;

	// Sequential 1-by-1 setup: Cave 0 -> Door1, ..., Cave 4 -> Door5 (Final Key/Combat)
	for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
		level3_doors[i].x = caveX[i];
		level3_doors[i].y = caveY;
		level3_doors[i].width = caveW;
		level3_doors[i].height = caveH;
		level3_doors[i].visited = false;
		level3_doors[i].assignedKeyColorId = i; // Key 0 to Cave 0, Key 1 to Cave 1, etc.
		level3_doors[i].handlerIndex = i;       // Handler 0..4 sequentially
	}

	level3_doorsVisible = false;
	level3_finishStage = 0;
	level3_shineTimer = 0.0f;
	level3_clamOpenAngle = 0.0f;
	level3combat_active = false;
	level3_insideTask = false;
	level3_currentTaskDoor = -1;
}

// RENDER FUNCTION
inline void renderLevel3()
{
	static int jungleBg = -1, caveBg = -1, insideCaveBg = -1, idleImg = -1, slideImg = -1;
	static int runFrames[8], jumpFrames[3], obstacleImgs[3] = { -1, -1, -1 };
	static int bgSeaScoreImg = -1, bgSeaOutImg = -1;

	if (jungleBg == -1) {
		jungleBg = iLoadImage("Image/bgJungle.png");
		caveBg = iLoadImage("Image/bgCave.png");
		insideCaveBg = iLoadImage("Image/bgInsideCave.png");
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

		bgSeaScoreImg = iLoadImage("Image/bgSeaScore.png");
		bgSeaOutImg = iLoadImage("Image/bgSeaOut.png");

		obstacleImgs[0] = iLoadImage("Image/crab.png");
		obstacleImgs[1] = iLoadImage("Image/octopass.png");
		obstacleImgs[2] = iLoadImage("Image/seahorse.png");
	}

	// 1. GAME OVER
	if (level3_gameOver) {
		if (!level3_hasPlayedEndAudio) {
			level3_playLoseSound();
			level3_hasPlayedEndAudio = true;
		}
		if (bgSeaOutImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaOutImg);
		else { iSetColor(225, 230, 235); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

		iSetColor(180, 25, 25);
		level3_drawBoldText(440, 365, "YOU ARE OUT!", GLUT_BITMAP_TIMES_ROMAN_24);
		char endScoreBuf[100];
		sprintf_s(endScoreBuf, sizeof(endScoreBuf), "Score: %d     |     High Score: %d", level3_score, level3_highScore);
		iSetColor(15, 35, 75);
		level3_drawBoldText(390, 315, endScoreBuf, GLUT_BITMAP_HELVETICA_18);
		level3_drawSettingsUI();
		return;
	}

	// 2. VICTORY
	if (level3_keyFound && level3_finishStage == 3) {
		if (!level3_hasPlayedEndAudio) {
			level3_playWinSound();
			level3_hasPlayedEndAudio = true;
		}
		if (bgSeaScoreImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaScoreImg);
		else { iSetColor(15, 35, 55); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

		char scoreBuf[64];
		sprintf_s(scoreBuf, sizeof(scoreBuf), "Your Score: %d", level3_score);
		iSetColor(255, 255, 255);
		level3_drawBoldText(395, 185, scoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);
		level3_drawSettingsUI();
		return;
	}

	// 3. BACKGROUND (SMOOTH SCROLL TRANSITION FROM JUNGLE TO CAVE)
	int caveScreenX = LEVEL3_TARGET_DISTANCE - level3_distanceCovered;

	if (level3_insideTask) {
		if (insideCaveBg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, insideCaveBg);
		else if (caveBg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, caveBg);
	}
	else if (level3_doorsVisible) {
		if (caveBg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, caveBg);
	}
	else {
		if (caveScreenX < SCREEN_WIDTH) {
			int jungleX = caveScreenX - SCREEN_WIDTH;
			if (jungleBg >= 0) iShowImage(jungleX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, jungleBg);
			if (caveBg >= 0) iShowImage(caveScreenX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, caveBg);
		}
		else {
			if (jungleBg >= 0) {
				iShowImage(level3_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, jungleBg);
				iShowImage(level3_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, jungleBg);
			}
		}
	}

	// 4. OBSTACLES, KEYS & POWERUPS (RUNNING PHASE)
	if (!level3_doorsVisible) {
		for (int i = 0; i < LEVEL3_NUM_OBSTACLES; i++) {
			if (level3_obstacles[i].active && obstacleImgs[level3_obstacles[i].type] >= 0) {
				iShowImage((int)level3_obstacles[i].x, (int)level3_obstacles[i].y,
					level3_obstacles[i].width, level3_obstacles[i].height, obstacleImgs[level3_obstacles[i].type]);
			}
		}

		for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
			if (!level3_keys[i].collected) {
				float screenKeyX = (float)(level3_keys[i].trackPos - level3_distanceCovered + 100);
				if (screenKeyX >= -80 && screenKeyX <= SCREEN_WIDTH + 80) {
					level3_drawKey(screenKeyX, level3_keys[i].y, level3_keys[i].size,
						level3_keyColor[i][0], level3_keyColor[i][1], level3_keyColor[i][2]);
				}
			}
		}

		if (level3_power2x.active) level3_draw2XOrb(level3_power2x.x, level3_power2x.y);

		// Player
		int playerImg = level3_isSliding ? slideImg : (level3_isJumping ? jumpFrames[level3_jumpFrameIndex] : (level3_isMoving ? runFrames[level3_animFrame] : idleImg));
		iShowImage((int)level3_playerX, (int)level3_playerY, level3_playerWidth, level3_playerHeight, playerImg);
	}

	// 5. CAVE DOORS & 5 KEYS FLOATING ON ENTRANCES
	if (level3_doorsVisible) {
		if (level3combat_active) {
			renderLevel3Combat();
		}
		else if (!level3_insideTask) {
			if (level3_finishStage == 0) {
				float floatOffset = sinf(level3_shineTimer * 0.1f) * 6.0f;

				// Find which cave is currently active (next sequentially)
				int currentTargetCave = 0;
				while (currentTargetCave < LEVEL3_NUM_DOORS && level3_doors[currentTargetCave].visited) {
					currentTargetCave++;
				}

				for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
					int colId = level3_doors[i].assignedKeyColorId;
					float keyCenterX = (float)(level3_doors[i].x + level3_doors[i].width / 2 - 25);
					float keyCenterY = (float)(level3_doors[i].y + 35) + floatOffset;

					if (level3_doors[i].visited) {
						// Completed Cave: Golden Ring & Status
						iSetColor(80, 200, 100);
						iCircle(level3_doors[i].x + level3_doors[i].width / 2, (int)keyCenterY + 20, 28);
						level3_drawKey(keyCenterX, keyCenterY, 50,
							level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);
						iSetColor(255, 215, 0);
						level3_drawBoldText(level3_doors[i].x + 10, level3_doors[i].y - 30, "CLEARED!", GLUT_BITMAP_HELVETICA_18);
					}
					else if (i == currentTargetCave) {
						// Active Cave: Pulsing Ring
						float pulse = (sinf(level3_shineTimer * 0.2f) + 1.0f) * 4.0f;
						iSetColor(255, 255, 255);
						iCircle(level3_doors[i].x + level3_doors[i].width / 2, (int)keyCenterY + 20, (int)(32 + pulse));
						iSetColor(level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);
						iCircle(level3_doors[i].x + level3_doors[i].width / 2, (int)keyCenterY + 20, 30);

						level3_drawKey(keyCenterX, keyCenterY, 50,
							level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);

						iSetColor(255, 255, 255);
						level3_drawBoldText(level3_doors[i].x + 22, level3_doors[i].y - 30, "ENTER", GLUT_BITMAP_HELVETICA_18);
					}
					else {
						// Locked Future Cave: Dimmed Key
						iSetColor(80, 80, 80);
						iCircle(level3_doors[i].x + level3_doors[i].width / 2, (int)keyCenterY + 20, 28);
						level3_drawKey(keyCenterX, keyCenterY, 50, 120, 120, 120);

						iSetColor(180, 180, 180);
						level3_drawBoldText(level3_doors[i].x + 18, level3_doors[i].y - 30, "LOCKED", GLUT_BITMAP_HELVETICA_18);
					}
				}
			}
			else if (level3_finishStage == 1) {
				level3_drawShinyRealKey(SCREEN_WIDTH / 2, 330);
				iSetColor(255, 255, 255);
				iFilledRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
				iSetColor(0, 0, 0);
				iRectangle(SCREEN_WIDTH / 2 - 170, 190, 340, 45);
				iText(SCREEN_WIDTH / 2 - 145, 208, "FINAL KEY FOUND! Click to Open Clam!");
			}
			else if (level3_finishStage >= 2) {
				level3_drawClamAndPearl(SCREEN_WIDTH / 2, 300);
				if (level3_finishStage == 2) {
					iSetColor(255, 255, 255);
					iFilledRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
					iSetColor(0, 0, 0);
					iRectangle(SCREEN_WIDTH / 2 - 150, 130, 300, 40);
					iText(SCREEN_WIDTH / 2 - 110, 145, "Click the Pearl to Complete Level!");
				}
			}
		}
		else {
			iSetColor(10, 10, 20);
			iFilledRectangle(SCREEN_WIDTH / 2 - 380, 190, 760, 240);
			iSetColor(255, 215, 0);
			iRectangle(SCREEN_WIDTH / 2 - 380, 190, 760, 240);

			int h = level3_doors[level3_currentTaskDoor].handlerIndex;
			if (h == 0) L3Door1_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 1) L3Door2_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 2) L3Door3_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 3) L3Door4_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);

			iSetColor(255, 255, 255);
			iText(SCREEN_WIDTH / 2 - 180, LEVEL3_OPT_Y + LEVEL3_OPT_H + 30, "Choose correctly to unlock, wrong answer deducts energy!");
		}
	}

	// 6. HUD (5 KEYS)
	for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
		int hx = 25 + i * 32, hy = 25;
		if (level3_keyCollected[i]) iSetColor(level3_keyColor[i][0], level3_keyColor[i][1], level3_keyColor[i][2]);
		else iSetColor(160, 160, 160);
		iFilledCircle(hx, hy, 11);
		iSetColor(0, 0, 0);
		iCircle(hx, hy, 11);
	}
	iSetColor(255, 255, 255);
	iText(20, 48, "Keys Collected (5)");

	// Energy
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level3_energy > 30 ? 0 : 220, level3_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level3_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(255, 255, 255);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	// Score
	char scBuf[64];
	sprintf_s(scBuf, sizeof(scBuf), "Score: %d", level3_score);
	level3_drawBoldText(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 40, scBuf, GLUT_BITMAP_HELVETICA_18);

	if (level3_doorsVisible && !level3_insideTask && level3_finishStage == 0 && !level3combat_active) {
		iSetColor(255, 255, 255);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 230, SCREEN_HEIGHT - 45, "Complete caves 1 to 5 in order! Final Key in Cave 5.", GLUT_BITMAP_HELVETICA_18);
	}

	if (level3_messageTimer > 0) {
		iSetColor(255, 40, 40);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 75, level3_message, GLUT_BITMAP_HELVETICA_18);
	}

	level3_drawSettingsUI();
}

// LOGIC UPDATE
inline void level3_fixedUpdate()
{
	if (level3_isPaused || level3_gameOver || level3_keyFound) return;

	if (level3combat_active) {
		updateLevel3CombatIfActive();
		return;
	}

	level3_shineTimer += 1.0f;
	if (level3_finishStage >= 2 && level3_clamOpenAngle < 80.0f) level3_clamOpenAngle += 2.5f;
	if (level3_messageTimer > 0) level3_messageTimer--;

	if (level3_multiplierTimer > 0) {
		level3_multiplierTimer--;
		if (level3_multiplierTimer <= 0) level3_scoreMultiplier = 1;
	}

	if (!level3_doorsVisible && !level3_power2x.active) {
		level3_powerSpawnCounter++;
		if (level3_powerSpawnCounter >= 350) {
			if (rand() % 100 < 30) {
				level3_power2x.active = true;
				level3_power2x.x = (float)SCREEN_WIDTH + 50.0f;
				level3_power2x.y = (rand() % 2 == 0) ? 215.0f : 100.0f;
				level3_powerSpawnCounter = 0;
			}
		}
	}

	level3_isMoving = false;

	if (!level3_doorsVisible && !level3_isJumping && isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		level3_isSliding = true;
		level3_slideTimer = LEVEL3_SLIDE_DURATION;
		level3_playerWidth = LEVEL3_PLAYER_SLIDE_W;
		level3_playerHeight = LEVEL3_PLAYER_SLIDE_H;
	}

	if (level3_isSliding) {
		level3_slideTimer--;
		if (level3_slideTimer <= 0) {
			level3_isSliding = false;
			level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
			level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
		}
	}

	if (!level3_doorsVisible && !level3_isSliding && isSpecialKeyPressed(GLUT_KEY_UP)) {
		if (!level3_isJumping) {
			level3_isJumping = true;
			level3_jumpVelocity = LEVEL3_JUMP_STRENGTH;
			level3_playerY += 2.0f;
		}
	}

	float extraMoveEnemies = 0.0f;

	if (!level3_doorsVisible && (isSpecialKeyPressed(GLUT_KEY_RIGHT) || level3_isSliding)) {
		level3_isMoving = true;
		level3_facingRight = true;
		level3_bgX -= level3_playerSpeed;
		if (level3_bgX <= -SCREEN_WIDTH) level3_bgX = 0;
		level3_distanceCovered += level3_playerSpeed;
		level3_updateScore(1);
		extraMoveEnemies = (float)level3_playerSpeed;

		if (level3_distanceCovered >= LEVEL3_TARGET_DISTANCE) {
			level3_doorsVisible = true;
			level3_playerX = 100.0f;
			level3_isSliding = false;
			level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
			level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
		}
	}
	else if (!level3_doorsVisible && !level3_isSliding && isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		if (level3_distanceCovered > 0) {
			level3_isMoving = true;
			level3_facingRight = false;
			level3_bgX += level3_playerSpeed;
			if (level3_bgX >= 0) level3_bgX = -SCREEN_WIDTH;
			level3_distanceCovered -= level3_playerSpeed;
			extraMoveEnemies = -(float)level3_playerSpeed;
		}
	}

	if (level3_power2x.active) {
		level3_power2x.x -= (level3_power2x.speed + extraMoveEnemies);
		if (level3_power2x.x < -60) level3_power2x.active = false;
	}

	if (!level3_doorsVisible) {
		for (int i = 0; i < LEVEL3_NUM_OBSTACLES; i++) {
			if (!level3_obstacles[i].spawned && level3_distanceCovered >= level3_obstacleSpawnDistance[i]) {
				level3_obstacles[i].spawned = true;
				level3_obstacles[i].active = true;
				level3_obstacles[i].x = (float)SCREEN_WIDTH + 20;
				level3_obstacles[i].y = level3_obstacles[i].baseY;
			}
			else if (level3_obstacles[i].active) {
				level3_obstacles[i].x -= (level3_enemyAutoSpeed + extraMoveEnemies);
				if (level3_obstacles[i].isAir) {
					level3_obstacles[i].waveAngle += 0.08f;
					level3_obstacles[i].y = level3_obstacles[i].baseY + sinf(level3_obstacles[i].waveAngle) * 12.0f;
				}
				if (level3_obstacles[i].x + level3_obstacles[i].width < -60) level3_obstacles[i].active = false;
			}
		}

		// Collisions with key pickups
		for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
			if (level3_keys[i].collected) continue;
			float screenKeyX = (float)(level3_keys[i].trackPos - level3_distanceCovered + 100);
			if (level3_rectOverlap(screenKeyX - 5.0f, level3_keys[i].y - 5.0f, level3_keys[i].size + 10, level3_keys[i].size + 10,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_keys[i].collected = true;
				level3_keyCollected[level3_keys[i].id] = true;
				level3_updateScore(150);
				level3_playPlusPointSound();
			}
		}

		if (level3_power2x.active) {
			if (level3_rectOverlap(level3_power2x.x, level3_power2x.y, level3_power2x.size, level3_power2x.size,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_power2x.active = false;
				level3_scoreMultiplier = 2;
				level3_multiplierTimer = 500;
				strcpy_s(level3_message, sizeof(level3_message), "2X SCORE BOOST ACTIVATED FOR 10 SECONDS!");
				level3_messageTimer = 70;
				level3_playPlusPointSound();
			}
		}

		for (int i = 0; i < LEVEL3_NUM_OBSTACLES; i++) {
			if (!level3_obstacles[i].active || level3_obstacles[i].hit) continue;
			if (level3_rectOverlap(level3_obstacles[i].x, level3_obstacles[i].y, level3_obstacles[i].width, level3_obstacles[i].height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_obstacles[i].hit = true;
				int damageEnergy = (level3_obstacles[i].type == 0) ? 15 : ((level3_obstacles[i].type == 1) ? 22 : 18);
				int deductScore = (level3_obstacles[i].type == 0) ? 100 : ((level3_obstacles[i].type == 1) ? 200 : 150);

				level3_messageTimer = 60;
				level3_energy -= damageEnergy;
				level3_updateScore(-deductScore);
				level3_playNegPointSound();

				if (level3_energy <= 0) {
					level3_energy = 0;
					level3_gameOver = true;
				}
			}
		}
	}

	if (level3_isJumping) {
		level3_playerY += level3_jumpVelocity;
		level3_jumpVelocity -= LEVEL3_GRAVITY;
		if (level3_jumpVelocity > 3.0f) level3_jumpFrameIndex = 0;
		else if (level3_jumpVelocity >= -3.0f) level3_jumpFrameIndex = 1;
		else level3_jumpFrameIndex = 2;

		if (level3_playerY <= level3_groundY) {
			level3_playerY = level3_groundY;
			level3_isJumping = false;
			level3_jumpVelocity = 0.0f;
			level3_jumpFrameIndex = 0;
		}
	}

	if (level3_isMoving && !level3_isSliding) {
		level3_animTimer++;
		if (level3_animTimer >= LEVEL3_ANIM_FRAME_DELAY) {
			level3_animTimer = 0;
			level3_animFrame = (level3_animFrame + 1) % 8;
		}
	}
	else {
		level3_animFrame = 0;
		level3_animTimer = 0;
	}
}

// MOUSE CLICKS
inline void handleLevel3Clicks(int mx, int my)
{
	float distSettings = sqrtf((float)((mx - LEVEL3_SETTING_BTN_X) * (mx - LEVEL3_SETTING_BTN_X) +
		(my - LEVEL3_SETTING_BTN_Y) * (my - LEVEL3_SETTING_BTN_Y)));
	if (distSettings <= LEVEL3_SETTING_BTN_R) {
		level3_showSettingsMenu = !level3_showSettingsMenu;
		return;
	}

	if (level3_showSettingsMenu) {
		float distR = sqrtf((float)((mx - LEVEL3_SETTING_BTN_X) * (mx - LEVEL3_SETTING_BTN_X) + (my - LEVEL3_SUB_R_Y) * (my - LEVEL3_SUB_R_Y)));
		if (distR <= LEVEL3_SUB_BTN_R) { setupLevel3(); return; }

		float distP = sqrtf((float)((mx - LEVEL3_SETTING_BTN_X) * (mx - LEVEL3_SETTING_BTN_X) + (my - LEVEL3_SUB_P_Y) * (my - LEVEL3_SUB_P_Y)));
		if (distP <= LEVEL3_SUB_BTN_R) { level3_isPaused = !level3_isPaused; return; }

		float distM = sqrtf((float)((mx - LEVEL3_SETTING_BTN_X) * (mx - LEVEL3_SETTING_BTN_X) + (my - LEVEL3_SUB_M_Y) * (my - LEVEL3_SUB_M_Y)));
		if (distM <= LEVEL3_SUB_BTN_R) { gameState = 5; return; }

		float distS = sqrtf((float)((mx - LEVEL3_SETTING_BTN_X) * (mx - LEVEL3_SETTING_BTN_X) + (my - LEVEL3_SUB_S_Y) * (my - LEVEL3_SUB_S_Y)));
		if (distS <= LEVEL3_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) mciSendString("stop bgMusic", NULL, 0, NULL);
			return;
		}
	}

	if (level3_gameOver || level3_isPaused || !level3_doorsVisible || level3combat_active || level3_keyFound) return;

	if (level3_finishStage == 1) {
		if (mx >= SCREEN_WIDTH / 2 - 120 && mx <= SCREEN_WIDTH / 2 + 120 && my >= 200 && my <= 420) {
			level3_finishStage = 2;
		}
		return;
	}

	if (level3_finishStage == 2) {
		float dist = sqrtf((float)((mx - SCREEN_WIDTH / 2) * (mx - SCREEN_WIDTH / 2) + (my - 310) * (my - 310)));
		if (dist <= 50.0f) {
			level3_finishStage = 3;
			level3_keyFound = true;
			level3_updateScore(1000);
			level3_playPlusPointSound();
		}
		return;
	}

	if (level3_insideTask) {
		int h = level3_doors[level3_currentTaskDoor].handlerIndex;
		bool done = false;
		if (h == 0) done = L3Door1_HandleClick(mx, my, SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y, &level3_doors[level3_currentTaskDoor].visited, &level3_insideTask);
		else if (h == 1) done = L3Door2_HandleClick(mx, my, SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y, &level3_doors[level3_currentTaskDoor].visited, &level3_insideTask);
		else if (h == 2) done = L3Door3_HandleClick(mx, my, SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y, &level3_doors[level3_currentTaskDoor].visited, &level3_insideTask);
		else if (h == 3) done = L3Door4_HandleClick(mx, my, SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y, &level3_doors[level3_currentTaskDoor].visited, &level3_insideTask);
		if (done && !level3_insideTask) level3_currentTaskDoor = -1;
		return;
	}

	// Sequential progression check: which cave is currently unlocked?
	int currentTargetCave = 0;
	while (currentTargetCave < LEVEL3_NUM_DOORS && level3_doors[currentTargetCave].visited) {
		currentTargetCave++;
	}

	// Click detection on Cave Entrances
	for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
		if (mx >= level3_doors[i].x && mx <= level3_doors[i].x + level3_doors[i].width &&
			my >= level3_doors[i].y && my <= level3_doors[i].y + level3_doors[i].height) {

			if (level3_doors[i].visited) {
				strcpy_s(level3_message, sizeof(level3_message), "Cave already cleared! Move to the next cave.");
				level3_messageTimer = 70;
				return;
			}

			// Must complete in strict sequence (Cave 0 -> Cave 1 -> ... -> Cave 4)
			if (i != currentTargetCave) {
				char warnBuf[100];
				sprintf_s(warnBuf, sizeof(warnBuf), "Locked! You must complete Cave %d first.", currentTargetCave + 1);
				strcpy_s(level3_message, sizeof(level3_message), warnBuf);
				level3_messageTimer = 80;
				playNegPointSound();
				return;
			}

			int requiredKey = level3_doors[i].assignedKeyColorId;
			if (!level3_keyCollected[requiredKey]) {
				level3_playNegPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "Key missing! You didn't collect this cave's key.");
				level3_messageTimer = 90;
				return;
			}

			// Final Cave (Cave 4 / Handler 4): Combat fight to win the Final Real Key
			if (level3_doors[i].handlerIndex == 4) {
				level3_doors[i].visited = true;
				startLevel3Combat();
			}
			else {
				// Regular Task Caves (1 to 4)
				level3_currentTaskDoor = i;
				level3_insideTask = true;
				int h = level3_doors[i].handlerIndex;
				if (h == 0) L3Door1_GenerateTask();
				else if (h == 1) L3Door2_GenerateTask();
				else if (h == 2) L3Door3_GenerateTask();
				else if (h == 3) L3Door4_GenerateTask();
			}
			return;
		}
	}
}

inline void handleLevel3SpecialKeyboard(unsigned char key)
{
	if (level3_isPaused || level3_gameOver || level3_keyFound || level3_doorsVisible) return;
	if (key == GLUT_KEY_UP && !level3_isJumping && !level3_isSliding) {
		level3_isJumping = true;
		level3_jumpVelocity = LEVEL3_JUMP_STRENGTH;
		level3_playerY += 2.0f;
	}
	else if (key == GLUT_KEY_DOWN && !level3_isJumping && !level3_isSliding) {
		level3_isSliding = true;
		level3_slideTimer = LEVEL3_SLIDE_DURATION;
		level3_playerWidth = LEVEL3_PLAYER_SLIDE_W;
		level3_playerHeight = LEVEL3_PLAYER_SLIDE_H;
	}
}

inline void handleLevel3Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R') setupLevel3();
	else if (key == 'p' || key == 'P') level3_isPaused = !level3_isPaused;
	else if (key == 'm' || key == 'M') gameState = 5;
	else if (key == 's' || key == 'S') {
		isSoundMuted = !isSoundMuted;
		if (isSoundMuted) mciSendString("stop bgMusic", NULL, 0, NULL);
	}
}

#endif