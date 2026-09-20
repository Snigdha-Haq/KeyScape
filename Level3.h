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
static const float level3_groundY = 165.0f;
static float level3_playerX = 100.0f;
static float level3_playerY = 165.0f;

#define LEVEL3_PLAYER_NORMAL_W 90
#define LEVEL3_PLAYER_NORMAL_H 130
#define LEVEL3_PLAYER_SLIDE_W  120
#define LEVEL3_PLAYER_SLIDE_H  60

static int level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
static int level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
static const int LEVEL3_BASE_SPEED = 5;
static int level3_playerSpeed = LEVEL3_BASE_SPEED;

// Speed Multipliers
static float level3_speedMultiplier = 1.0f;
static int level3_speedMultiplierTimer = 0;

// Slide mechanism
static bool level3_isSliding = false;
static int level3_slideTimer = 0;
#define LEVEL3_SLIDE_DURATION 28

// Auto speed for floating treats
static float level3_treatAutoSpeed = 3.8f;
static bool level3_facingRight = true;

static int level3_bgX = 0;
static int level3_distanceCovered = 0;
#define LEVEL3_TARGET_DISTANCE 57600

// EXCLAVE / ELEVATED PLATFORM CONFIGURATION (6 EXCLAVES)
#define LEVEL3_NUM_EXCLAVES      6
#define LEVEL3_EXCLAVE_WIDTH     2200
#define LEVEL3_EXCLAVE_HEIGHT    185
#define LEVEL3_EXCLAVE_DRAW_Y    110.0f
#define LEVEL3_EXCLAVE_SURFACE_Y 260.0f

static int level3_exclaveStarts[LEVEL3_NUM_EXCLAVES] = { 6000, 15000, 24000, 33000, 42000, 50000 };

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
static float level3_heatEnergyDrain = 0.0f;
static int level3_consecutiveMissedTreats = 0;
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
static float level3_scoreMultiplier = 1.0f;
static int level3_multiplierTimer = 0;

// Power-ups Dimension (150x30)
#define LEVEL3_POWERUP_W 150
#define LEVEL3_POWERUP_H 30

struct Level3PowerUp {
	float x, y;
	int width, height;
	bool active;
	float speed;
};
static Level3PowerUp level3_power2x;
static Level3PowerUp level3_powerHalf;
static Level3PowerUp level3_powerSpeed2x;
static Level3PowerUp level3_powerSpeedHalf;
static int level3_powerSpawnCounter = 0;

inline bool level3_isWorldXOnExclave(int worldX)
{
	for (int e = 0; e < LEVEL3_NUM_EXCLAVES; e++) {
		if (worldX >= level3_exclaveStarts[e] &&
			worldX <= level3_exclaveStarts[e] + LEVEL3_EXCLAVE_WIDTH) {
			return true;
		}
	}
	return false;
}

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
		level3_score += (int)(addPoints * level3_scoreMultiplier);
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
static int level3_keySpawnDistance[LEVEL3_NUM_KEYS] = { 7000, 16500, 27000, 37500, 48500 };

int level3_keyColor[LEVEL3_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // 0: Gold
	{ 65, 145, 220 },  // 1: Blue
	{ 60, 200, 90 },   // 2: Green
	{ 225, 60, 60 },   // 3: Red
	{ 180, 80, 220 }   // 4: Purple
};

bool level3_keyCollected[LEVEL3_NUM_KEYS] = { false, false, false, false, false };

// 24 ICE CREAM TREATS
#define LEVEL3_NUM_ICECREAMS 24

struct Level3IceCream {
	float x, y, baseY;
	int width, height, type;
	int trackPos;
	bool isAir;
	float waveAngle;
	bool spawned, active, collected;
	int energyGain;
	int scoreGain;
};

static Level3IceCream level3_icecreams[LEVEL3_NUM_ICECREAMS];
static int level3_icecreamSpawnDistance[LEVEL3_NUM_ICECREAMS] = {
	2200, 4400, 6800, 9200, 11600, 14000,
	16400, 18800, 21200, 23600, 26000, 28400,
	30800, 33200, 35600, 38000, 40400, 42800,
	45200, 47600, 50000, 52400, 54600, 56400
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
	int centerX, centerY;
	int width, height;
	bool visited;
	int assignedKeyColorId;
	int handlerIndex;
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

// Ice cream Fallback renderer
inline void level3_drawIceCreamFallback(float x, float y, int type)
{
	iSetColor(210, 140, 70);
	double vx[3] = { x + 10, x + 40, x + 25 };
	double vy[3] = { y + 25, y + 25, y };
	iFilledPolygon(vx, vy, 3);

	if (type == 0) iSetColor(255, 160, 180);      // Strawberry
	else if (type == 1) iSetColor(255, 230, 140); // Vanilla / Mango
	else if (type == 2) iSetColor(140, 230, 180); // Mint
	else iSetColor(180, 120, 230);                // Blueberry

	iFilledCircle(x + 25, y + 35, 18);
	iSetColor(255, 255, 255);
	iFilledCircle(x + 22, y + 40, 5);
}

inline void level3_drawSettingsUI()
{
	static int btnSettings = -1, btnRestart = -1, btnPauPlay = -1, btnPause = -1, btnMenu = -1, btnSoundOn = -1, btnSoundOff = -1;

	if (btnSettings == -1) {
		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");
		btnRestart = iLoadImage("Image/restart.png");
		if (btnRestart < 0) btnRestart = iLoadImage("restart.png");
		btnPauPlay = iLoadImage("Image/PauPlay.png");
		if (btnPauPlay < 0) btnPauPlay = iLoadImage("PauPlay.png");
		btnPause = iLoadImage("Image/pause.png");
		if (btnPause < 0) btnPause = iLoadImage("pause.png");
		btnMenu = iLoadImage("Image/menu.png");
		if (btnMenu < 0) btnMenu = iLoadImage("menu.png");
		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");
		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}

	if (btnSettings >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 22, LEVEL3_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	else { iSetColor(30, 45, 65); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SETTING_BTN_Y, LEVEL3_SETTING_BTN_R); }

	if (level3_showSettingsMenu) {
		if (btnRestart >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_R_Y - 20, 40, 40, btnRestart);
		else { iSetColor(220, 50, 50); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_R_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_R_Y - 7, "R", GLUT_BITMAP_TIMES_ROMAN_24); }

		if (level3_isPaused) {
			if (btnPause >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_P_Y - 20, 40, 40, btnPause);
			else { iSetColor(50, 130, 220); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_P_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_P_Y - 7, "P", GLUT_BITMAP_TIMES_ROMAN_24); }
		}
		else {
			if (btnPauPlay >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_P_Y - 20, 40, 40, btnPauPlay);
			else { iSetColor(50, 130, 220); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_P_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_P_Y - 7, "P", GLUT_BITMAP_TIMES_ROMAN_24); }
		}

		if (btnMenu >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_M_Y - 20, 40, 40, btnMenu);
		else { iSetColor(45, 175, 75); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_M_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 8, LEVEL3_SUB_M_Y - 7, "M", GLUT_BITMAP_TIMES_ROMAN_24); }

		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else { iSetColor(120, 120, 120); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_S_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_S_Y - 7, "S", GLUT_BITMAP_TIMES_ROMAN_24); }
		}
		else {
			if (btnSoundOn >= 0) iShowImage(LEVEL3_SETTING_BTN_X - 20, LEVEL3_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else { iSetColor(230, 140, 20); iFilledCircle(LEVEL3_SETTING_BTN_X, LEVEL3_SUB_S_Y, LEVEL3_SUB_BTN_R); level3_drawBoldText(LEVEL3_SETTING_BTN_X - 6, LEVEL3_SUB_S_Y - 7, "S", GLUT_BITMAP_TIMES_ROMAN_24); }
		}
	}

	if (level3_isPaused && !level3_gameOver && !level3_keyFound) {
		iSetColor(0, 0, 0);
		iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 30, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
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
	level3_playerSpeed = LEVEL3_BASE_SPEED;
	level3_speedMultiplier = 1.0f;
	level3_speedMultiplierTimer = 0;
	level3_score = 0;
	level3_scoreMultiplier = 1.0f;
	level3_multiplierTimer = 0;

	level3_power2x.active = false;
	level3_power2x.width = LEVEL3_POWERUP_W;
	level3_power2x.height = LEVEL3_POWERUP_H;
	level3_power2x.speed = 4.2f;

	level3_powerHalf.active = false;
	level3_powerHalf.width = LEVEL3_POWERUP_W;
	level3_powerHalf.height = LEVEL3_POWERUP_H;
	level3_powerHalf.speed = 4.2f;

	level3_powerSpeed2x.active = false;
	level3_powerSpeed2x.width = LEVEL3_POWERUP_W;
	level3_powerSpeed2x.height = LEVEL3_POWERUP_H;
	level3_powerSpeed2x.speed = 4.2f;

	level3_powerSpeedHalf.active = false;
	level3_powerSpeedHalf.width = LEVEL3_POWERUP_W;
	level3_powerSpeedHalf.height = LEVEL3_POWERUP_H;
	level3_powerSpeedHalf.speed = 4.2f;

	level3_powerSpawnCounter = 0;

	level3_energy = 100;
	level3_heatEnergyDrain = 0.0f;
	level3_consecutiveMissedTreats = 0;
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

	// Setup 5 Keys (Air keys are placed higher: 350.0f)
	bool keyAirList[LEVEL3_NUM_KEYS] = { false, false, false, true, false };
	for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
		level3_keys[i].id = i;
		level3_keys[i].isAir = keyAirList[i];
		level3_keys[i].trackPos = level3_keySpawnDistance[i];
		level3_keys[i].collected = false;
		level3_keys[i].size = LEVEL3_KEY_SIZE;

		if (level3_isWorldXOnExclave(level3_keys[i].trackPos)) {
			level3_keys[i].y = level3_keys[i].isAir ? (LEVEL3_EXCLAVE_SURFACE_Y + 150.0f) : (LEVEL3_EXCLAVE_SURFACE_Y + 25.0f);
		}
		else {
			level3_keys[i].y = level3_keys[i].isAir ? 350.0f : (level3_groundY + 25.0f);
		}
		level3_keyCollected[i] = false;
	}

	// 24 Ice Cream Setup
	int types[LEVEL3_NUM_ICECREAMS] = {
		0, 2, 1, 3, 0, 2, 3, 1, 0, 2, 1, 3,
		2, 0, 3, 1, 2, 0, 1, 3, 2, 0, 3, 1
	};
	bool airMode[LEVEL3_NUM_ICECREAMS] = {
		false, true, false, true, false, true,
		false, false, true, false, true, false,
		true, false, false, true, false, true,
		false, true, true, false, false, true
	};

	for (int i = 0; i < LEVEL3_NUM_ICECREAMS; i++) {
		level3_icecreams[i].spawned = false;
		level3_icecreams[i].active = false;
		level3_icecreams[i].collected = false;
		level3_icecreams[i].type = types[i];
		level3_icecreams[i].isAir = airMode[i];
		level3_icecreams[i].trackPos = level3_icecreamSpawnDistance[i];
		level3_icecreams[i].waveAngle = 0.0f;
		level3_icecreams[i].x = (float)SCREEN_WIDTH + 500.0f;
		level3_icecreams[i].width = 50;
		level3_icecreams[i].height = 55;

		if (level3_isWorldXOnExclave(level3_icecreams[i].trackPos)) {
			level3_icecreams[i].baseY = airMode[i] ? (LEVEL3_EXCLAVE_SURFACE_Y + 150.0f) : (LEVEL3_EXCLAVE_SURFACE_Y + 15.0f);
		}
		else {
			level3_icecreams[i].baseY = (types[i] == 0) ? level3_groundY + 10.0f : (airMode[i] ? 350.0f : (level3_groundY + 25.0f));
		}
		level3_icecreams[i].y = level3_icecreams[i].baseY;

		if (types[i] == 0) { level3_icecreams[i].energyGain = 20; level3_icecreams[i].scoreGain = 60; }
		else if (types[i] == 1) { level3_icecreams[i].energyGain = 30; level3_icecreams[i].scoreGain = 100; }
		else if (types[i] == 2) { level3_icecreams[i].energyGain = 38; level3_icecreams[i].scoreGain = 150; }
		else { level3_icecreams[i].energyGain = 45; level3_icecreams[i].scoreGain = 200; }
	}

	// Cave Coordinates
	int caveCentersX[5] = { 145, 325, 500, 680, 860 };
	int caveCenterY = 290;

	for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
		level3_doors[i].centerX = caveCentersX[i];
		level3_doors[i].centerY = caveCenterY;
		level3_doors[i].width = 110;
		level3_doors[i].height = 150;
		level3_doors[i].visited = false;
		level3_doors[i].assignedKeyColorId = i;
		level3_doors[i].handlerIndex = i;
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
	static int runFrames[8], jumpFrames[3], icecreamImgs[4] = { -1, -1, -1, -1 };
	static int bgSeaScoreImg = -1, bgSeaOutImg = -1, exclaveImg = -1;

	// Power-up image handles
	static int imgPowerScore2x = -1, imgPowerScoreHalf = -1, imgPowerSpeed2x = -1, imgPowerSpeedHalf = -1;

	if (jungleBg == -1) {
		jungleBg = iLoadImage("Image/bgJungle.png");
		if (jungleBg < 0) jungleBg = iLoadImage("Image/bgJungle.jpg");

		caveBg = iLoadImage("Image/bgCave.png");
		insideCaveBg = iLoadImage("Image/bgInsideCave.png");
		idleImg = iLoadImage("Image/idle_1.png");
		slideImg = iLoadImage("Image/slide.png");

		exclaveImg = iLoadImage("Image/exclave.png");

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

		icecreamImgs[0] = iLoadImage("Image/icecream1.png");
		if (icecreamImgs[0] < 0) icecreamImgs[0] = iLoadImage("Image/icecream.png");
		icecreamImgs[1] = iLoadImage("Image/icecream2.png");
		icecreamImgs[2] = iLoadImage("Image/icecream3.png");
		icecreamImgs[3] = iLoadImage("Image/icecream4.png");

		// Load Power-Up PNG Images
		imgPowerScore2x = iLoadImage("Image/SCORE-2x.png");
		if (imgPowerScore2x < 0) imgPowerScore2x = iLoadImage("SCORE-2x.png");

		imgPowerScoreHalf = iLoadImage("Image/SCORE-0.5x.png");
		if (imgPowerScoreHalf < 0) imgPowerScoreHalf = iLoadImage("SCORE-0.5x.png");
		if (imgPowerScoreHalf < 0) imgPowerScoreHalf = iLoadImage("Image/SCORE: 0.5x.png");
		if (imgPowerScoreHalf < 0) imgPowerScoreHalf = iLoadImage("SCORE: 0.5x.png");

		imgPowerSpeed2x = iLoadImage("Image/SPEED-2x.png");
		if (imgPowerSpeed2x < 0) imgPowerSpeed2x = iLoadImage("SPEED-2x.png");

		imgPowerSpeedHalf = iLoadImage("Image/SPEED-0.5x.png");
		if (imgPowerSpeedHalf < 0) imgPowerSpeedHalf = iLoadImage("SPEED-0.5x.png");
	}

	// 1. GAME OVER
	if (level3_gameOver) {
		if (!level3_hasPlayedEndAudio) {
			level3_playLoseSound();
			level3_hasPlayedEndAudio = true;
		}
		if (bgSeaOutImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaOutImg);
		else { iSetColor(225, 230, 235); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

		iSetColor(160, 50, 15);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 70, 345, "Level : HARD", GLUT_BITMAP_TIMES_ROMAN_24);

		char endScoreBuf[64];
		sprintf_s(endScoreBuf, sizeof(endScoreBuf), "Your Score : %d", level3_score);
		int scoreOffset = (level3_score >= 1000) ? 90 : 80;
		iSetColor(0, 0, 0);
		level3_drawBoldText(SCREEN_WIDTH / 2 - scoreOffset, 280, endScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

		char endHighScoreBuf[64];
		sprintf_s(endHighScoreBuf, sizeof(endHighScoreBuf), "High-Score : %d", level3_highScore);
		int highOffset = (level3_highScore >= 1000) ? 95 : 85;
		level3_drawBoldText(SCREEN_WIDTH / 2 - highOffset, 215, endHighScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

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

		iSetColor(160, 50, 15);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 70, 345, "Level : HARD", GLUT_BITMAP_TIMES_ROMAN_24);

		char yourScoreBuf[64];
		sprintf_s(yourScoreBuf, sizeof(yourScoreBuf), "Your Score : %d", level3_score);
		int scoreOffset = (level3_score >= 1000) ? 90 : 80;
		iSetColor(0, 0, 0);
		level3_drawBoldText(SCREEN_WIDTH / 2 - scoreOffset, 280, yourScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

		char highScoreBuf[64];
		sprintf_s(highScoreBuf, sizeof(highScoreBuf), "High-Score : %d", level3_highScore);
		int highOffset = (level3_highScore >= 1000) ? 95 : 85;
		level3_drawBoldText(SCREEN_WIDTH / 2 - highOffset, 215, highScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

		level3_drawSettingsUI();
		return;
	}

	// 3. BACKGROUND
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

	// 4. RUNNING PHASE
	if (!level3_doorsVisible) {
		// Draw all 6 Exclave Platforms
		for (int e = 0; e < LEVEL3_NUM_EXCLAVES; e++) {
			float screenExclaveX = (float)(level3_exclaveStarts[e] - level3_distanceCovered + 100);
			if (screenExclaveX + LEVEL3_EXCLAVE_WIDTH > -200 && screenExclaveX < SCREEN_WIDTH + 200) {
				if (exclaveImg >= 0) {
					iShowImage((int)screenExclaveX, (int)LEVEL3_EXCLAVE_DRAW_Y, LEVEL3_EXCLAVE_WIDTH, LEVEL3_EXCLAVE_HEIGHT, exclaveImg);
				}
				else {
					iSetColor(140, 100, 70);
					iFilledRectangle((int)screenExclaveX, (int)LEVEL3_EXCLAVE_DRAW_Y, LEVEL3_EXCLAVE_WIDTH, LEVEL3_EXCLAVE_HEIGHT);
				}
			}
		}

		// Draw Ice Creams
		for (int i = 0; i < LEVEL3_NUM_ICECREAMS; i++) {
			if (level3_icecreams[i].active) {
				if (icecreamImgs[level3_icecreams[i].type] >= 0) {
					iShowImage((int)level3_icecreams[i].x, (int)level3_icecreams[i].y,
						level3_icecreams[i].width, level3_icecreams[i].height, icecreamImgs[level3_icecreams[i].type]);
				}
				else {
					level3_drawIceCreamFallback(level3_icecreams[i].x, level3_icecreams[i].y, level3_icecreams[i].type);
				}
			}
		}

		// Draw Keys
		for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
			if (!level3_keys[i].collected) {
				float screenKeyX = (float)(level3_keys[i].trackPos - level3_distanceCovered + 100);
				if (screenKeyX >= -80 && screenKeyX <= SCREEN_WIDTH + 80) {
					level3_drawKey(screenKeyX, level3_keys[i].y, level3_keys[i].size,
						level3_keyColor[i][0], level3_keyColor[i][1], level3_keyColor[i][2]);
				}
			}
		}

		// Draw Power-ups using Images (150x30)
		if (level3_power2x.active) {
			if (imgPowerScore2x >= 0) iShowImage((int)level3_power2x.x, (int)level3_power2x.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H, imgPowerScore2x);
			else { iSetColor(50, 140, 245); iFilledRectangle((int)level3_power2x.x, (int)level3_power2x.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H); }
		}
		if (level3_powerHalf.active) {
			if (imgPowerScoreHalf >= 0) iShowImage((int)level3_powerHalf.x, (int)level3_powerHalf.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H, imgPowerScoreHalf);
			else { iSetColor(255, 205, 10); iFilledRectangle((int)level3_powerHalf.x, (int)level3_powerHalf.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H); }
		}
		if (level3_powerSpeed2x.active) {
			if (imgPowerSpeed2x >= 0) iShowImage((int)level3_powerSpeed2x.x, (int)level3_powerSpeed2x.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H, imgPowerSpeed2x);
			else { iSetColor(50, 220, 90); iFilledRectangle((int)level3_powerSpeed2x.x, (int)level3_powerSpeed2x.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H); }
		}
		if (level3_powerSpeedHalf.active) {
			if (imgPowerSpeedHalf >= 0) iShowImage((int)level3_powerSpeedHalf.x, (int)level3_powerSpeedHalf.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H, imgPowerSpeedHalf);
			else { iSetColor(240, 110, 70); iFilledRectangle((int)level3_powerSpeedHalf.x, (int)level3_powerSpeedHalf.y, LEVEL3_POWERUP_W, LEVEL3_POWERUP_H); }
		}

		// Draw Player
		int playerImg = level3_isSliding ? slideImg : (level3_isJumping ? jumpFrames[level3_jumpFrameIndex] : (level3_isMoving ? runFrames[level3_animFrame] : idleImg));
		iShowImage((int)level3_playerX, (int)level3_playerY, level3_playerWidth, level3_playerHeight, playerImg);
	}

	// 5. CAVE DOORS & PUZZLES
	if (level3_doorsVisible) {
		if (level3combat_active) {
			renderLevel3Combat();
		}
		else if (!level3_insideTask) {
			if (level3_finishStage == 0) {
				float floatOffset = sinf(level3_shineTimer * 0.1f) * 6.0f;

				int currentTargetCave = 0;
				while (currentTargetCave < LEVEL3_NUM_DOORS && level3_doors[currentTargetCave].visited) {
					currentTargetCave++;
				}

				for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
					int colId = level3_doors[i].assignedKeyColorId;
					int cx = level3_doors[i].centerX;
					int cy = (int)((float)level3_doors[i].centerY + floatOffset);

					float keyDrawX = (float)(cx - 24);
					float keyDrawY = (float)(cy - 12);

					if (level3_doors[i].visited) {
						iSetColor(80, 220, 100);
						iCircle(cx, cy, 32);
						iCircle(cx, cy, 33);
						level3_drawKey(keyDrawX, keyDrawY, 48,
							level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);

						iSetColor(255, 215, 0);
						level3_drawBoldText(cx - 36, cy - 65, "CLEARED!", GLUT_BITMAP_HELVETICA_18);
					}
					else if (i == currentTargetCave) {
						float pulse = (sinf(level3_shineTimer * 0.2f) + 1.0f) * 4.0f;
						iSetColor(255, 255, 255);
						iCircle(cx, cy, (int)(34 + pulse));
						iCircle(cx, cy, (int)(35 + pulse));
						iSetColor(level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);
						iCircle(cx, cy, 33);

						level3_drawKey(keyDrawX, keyDrawY, 48,
							level3_keyColor[colId][0], level3_keyColor[colId][1], level3_keyColor[colId][2]);

						iSetColor(255, 255, 255);
						level3_drawBoldText(cx - 28, cy - 65, "ENTER", GLUT_BITMAP_TIMES_ROMAN_24);
					}
					else {
						iSetColor(70, 70, 70);
						iCircle(cx, cy, 30);
						level3_drawKey(keyDrawX, keyDrawY, 48, 120, 120, 120);

						iSetColor(180, 180, 180);
						level3_drawBoldText(cx - 32, cy - 65, "LOCKED", GLUT_BITMAP_HELVETICA_18);
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
			int h = level3_doors[level3_currentTaskDoor].handlerIndex;
			if (h == 0) L3Door1_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 1) L3Door2_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 2) L3Door3_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
			else if (h == 3) L3Door4_RenderTask(SCREEN_WIDTH, LEVEL3_OPT_W, LEVEL3_OPT_H, LEVEL3_OPT_GAP, LEVEL3_OPT_Y);
		}
	}

	// 6. HUD
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

	// Energy Bar & Warning
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level3_energy > 30 ? 0 : 220, level3_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level3_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(255, 255, 255);

	char energyNotice[64];
	sprintf_s(energyNotice, sizeof(energyNotice), "Energy (Consecutive Misses: %d/3)", level3_consecutiveMissedTreats);
	iText(20, SCREEN_HEIGHT - 55, energyNotice);

	// Score & Speed Readout
	char scBuf[80];
	if (level3_scoreMultiplier == 2.0f) {
		sprintf_s(scBuf, sizeof(scBuf), "Score: %d (Score:2x)", level3_score);
	}
	else if (level3_scoreMultiplier == 0.5f) {
		sprintf_s(scBuf, sizeof(scBuf), "Score: %d (Score:0.5x)", level3_score);
	}
	else {
		sprintf_s(scBuf, sizeof(scBuf), "Score: %d", level3_score);
	}
	level3_drawBoldText(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, scBuf, GLUT_BITMAP_HELVETICA_18);

	if (level3_speedMultiplier != 1.0f) {
		char spdBuf[64];
		if (level3_speedMultiplier == 2.0f) {
			sprintf_s(spdBuf, sizeof(spdBuf), "Speed:2x");
		}
		else {
			sprintf_s(spdBuf, sizeof(spdBuf), "Speed:0.5x");
		}
		iSetColor(level3_speedMultiplier > 1.0f ? 50 : 240, level3_speedMultiplier > 1.0f ? 220 : 100, 70);
		level3_drawBoldText(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 65, spdBuf, GLUT_BITMAP_HELVETICA_18);
	}

	if (level3_doorsVisible && !level3_insideTask && level3_finishStage == 0 && !level3combat_active) {
		iSetColor(255, 255, 255);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 230, SCREEN_HEIGHT - 45, "Complete caves 1 to 5 in order! Final Key in Cave 5.", GLUT_BITMAP_HELVETICA_18);
	}

	if (level3_messageTimer > 0) {
		iSetColor(255, 220, 40);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT - 75, level3_message, GLUT_BITMAP_HELVETICA_18);
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

	// Score Multiplier Timer
	if (level3_multiplierTimer > 0) {
		level3_multiplierTimer--;
		if (level3_multiplierTimer <= 0) level3_scoreMultiplier = 1.0f;
	}

	// Speed Multiplier Timer
	if (level3_speedMultiplierTimer > 0) {
		level3_speedMultiplierTimer--;
		if (level3_speedMultiplierTimer <= 0) {
			level3_speedMultiplier = 1.0f;
			level3_playerSpeed = LEVEL3_BASE_SPEED;
		}
	}

	// Surface Detection for Player (Checks all 6 exclaves)
	float currentGroundY = level3_groundY;
	int playerWorldX = level3_distanceCovered;
	bool onExclave = level3_isWorldXOnExclave(playerWorldX);

	if (onExclave) {
		currentGroundY = LEVEL3_EXCLAVE_SURFACE_Y;
	}

	// Power-up Spawn Handler (Air spawn height increased to +160.0f so jump is mandatory)
	if (!level3_doorsVisible) {
		level3_powerSpawnCounter++;
		if (level3_powerSpawnCounter >= 250) {
			int pick = rand() % 100;
			int futureWorldX = level3_distanceCovered + SCREEN_WIDTH + 50;
			float spawnGround = level3_isWorldXOnExclave(futureWorldX) ? LEVEL3_EXCLAVE_SURFACE_Y : level3_groundY;
			float targetY = (rand() % 2 == 0) ? (spawnGround + 160.0f) : (spawnGround + 20.0f);

			// Score:2x (37% chance)
			if (pick < 37 && !level3_power2x.active) {
				level3_power2x.active = true;
				level3_power2x.x = (float)SCREEN_WIDTH + 50.0f;
				level3_power2x.y = targetY;
				level3_powerSpawnCounter = 0;
			}
			// Score:0.5x (38% chance)
			else if (pick >= 37 && pick < 75 && !level3_powerHalf.active) {
				level3_powerHalf.active = true;
				level3_powerHalf.x = (float)SCREEN_WIDTH + 50.0f;
				level3_powerHalf.y = targetY;
				level3_powerSpawnCounter = 0;
			}
			// Speed:2x (12% chance)
			else if (pick >= 75 && pick < 87 && !level3_powerSpeed2x.active) {
				level3_powerSpeed2x.active = true;
				level3_powerSpeed2x.x = (float)SCREEN_WIDTH + 50.0f;
				level3_powerSpeed2x.y = targetY;
				level3_powerSpawnCounter = 0;
			}
			// Speed:0.5x (13% chance)
			else if (pick >= 87 && !level3_powerSpeedHalf.active) {
				level3_powerSpeedHalf.active = true;
				level3_powerSpeedHalf.x = (float)SCREEN_WIDTH + 50.0f;
				level3_powerSpeedHalf.y = targetY;
				level3_powerSpawnCounter = 0;
			}
		}
	}

	level3_isMoving = false;

	// Slide Input
	if (!level3_doorsVisible && !level3_isJumping && (GetAsyncKeyState(VK_DOWN) & 0x8000)) {
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

	// Jump Input
	if (!level3_doorsVisible && !level3_isSliding && (GetAsyncKeyState(VK_UP) & 0x8000)) {
		if (!level3_isJumping) {
			level3_isJumping = true;
			level3_jumpVelocity = LEVEL3_JUMP_STRENGTH;
			level3_playerY += 2.0f;
		}
	}

	float extraMoveTreats = 0.0f;

	// Forward Movement & Wall-Collision Check
	if (!level3_doorsVisible && ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || level3_isSliding)) {
		int nextWorldX = level3_distanceCovered + level3_playerSpeed;

		for (int e = 0; e < LEVEL3_NUM_EXCLAVES; e++) {
			if (nextWorldX >= level3_exclaveStarts[e] && level3_distanceCovered < level3_exclaveStarts[e]) {
				if (level3_playerY < LEVEL3_EXCLAVE_SURFACE_Y - 10.0f) {
					level3_gameOver = true;
					level3_energy = 0;
					level3_playLoseSound();
					return;
				}
			}
		}

		level3_isMoving = true;
		level3_facingRight = true;
		level3_bgX -= level3_playerSpeed;
		if (level3_bgX <= -SCREEN_WIDTH) level3_bgX = 0;
		level3_distanceCovered += level3_playerSpeed;
		level3_updateScore(1);
		extraMoveTreats = (float)level3_playerSpeed;

		level3_heatEnergyDrain += 0.04f;
		if (level3_heatEnergyDrain >= 1.0f) {
			level3_energy -= (int)level3_heatEnergyDrain;
			level3_heatEnergyDrain = 0.0f;
			if (level3_energy <= 0) {
				level3_energy = 0;
				level3_gameOver = true;
				level3_playLoseSound();
				return;
			}
		}

		if (level3_distanceCovered >= LEVEL3_TARGET_DISTANCE) {
			level3_doorsVisible = true;
			level3_playerX = 100.0f;
			level3_playerY = level3_groundY;
			level3_isSliding = false;
			level3_playerWidth = LEVEL3_PLAYER_NORMAL_W;
			level3_playerHeight = LEVEL3_PLAYER_NORMAL_H;
		}
	}
	else if (!level3_doorsVisible && !level3_isSliding && (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
		if (level3_distanceCovered > 0) {
			level3_isMoving = true;
			level3_facingRight = false;
			level3_bgX += level3_playerSpeed;
			if (level3_bgX >= 0) level3_bgX = -SCREEN_WIDTH;
			level3_distanceCovered -= level3_playerSpeed;
			extraMoveTreats = -(float)level3_playerSpeed;
		}
	}

	// Physics: Jump & Gravity
	if (level3_isJumping || level3_playerY > currentGroundY) {
		level3_playerY += level3_jumpVelocity;
		level3_jumpVelocity -= LEVEL3_GRAVITY;

		if (level3_jumpVelocity > 3.0f) level3_jumpFrameIndex = 0;
		else if (level3_jumpVelocity >= -3.0f) level3_jumpFrameIndex = 1;
		else level3_jumpFrameIndex = 2;

		if (level3_playerY <= currentGroundY) {
			level3_playerY = currentGroundY;
			level3_isJumping = false;
			level3_jumpVelocity = 0.0f;
			level3_jumpFrameIndex = 0;
		}
	}
	else if (level3_playerY < currentGroundY) {
		level3_playerY = currentGroundY;
	}

	// Power-ups Movement
	if (level3_power2x.active) {
		level3_power2x.x -= (level3_power2x.speed + extraMoveTreats);
		if (level3_power2x.x < -160) level3_power2x.active = false;
	}

	if (level3_powerHalf.active) {
		level3_powerHalf.x -= (level3_powerHalf.speed + extraMoveTreats);
		if (level3_powerHalf.x < -160) level3_powerHalf.active = false;
	}

	if (level3_powerSpeed2x.active) {
		level3_powerSpeed2x.x -= (level3_powerSpeed2x.speed + extraMoveTreats);
		if (level3_powerSpeed2x.x < -160) level3_powerSpeed2x.active = false;
	}

	if (level3_powerSpeedHalf.active) {
		level3_powerSpeedHalf.x -= (level3_powerSpeedHalf.speed + extraMoveTreats);
		if (level3_powerSpeedHalf.x < -160) level3_powerSpeedHalf.active = false;
	}

	// Ice Creams, Keys & Power-ups Updates
	if (!level3_doorsVisible) {
		for (int i = 0; i < LEVEL3_NUM_ICECREAMS; i++) {
			if (!level3_icecreams[i].spawned && level3_distanceCovered >= level3_icecreams[i].trackPos) {
				level3_icecreams[i].spawned = true;
				level3_icecreams[i].active = true;
				level3_icecreams[i].x = (float)SCREEN_WIDTH + 20;
				level3_icecreams[i].y = level3_icecreams[i].baseY;
			}
			else if (level3_icecreams[i].active) {
				level3_icecreams[i].x -= (level3_treatAutoSpeed + extraMoveTreats);

				int curItemWorldX = (int)(level3_distanceCovered + level3_icecreams[i].x - 100);
				float targetBaseY = level3_isWorldXOnExclave(curItemWorldX) ?
					(level3_icecreams[i].isAir ? (LEVEL3_EXCLAVE_SURFACE_Y + 150.0f) : (LEVEL3_EXCLAVE_SURFACE_Y + 15.0f)) :
					(level3_icecreams[i].isAir ? 350.0f : (level3_groundY + 15.0f));
				level3_icecreams[i].baseY = targetBaseY;

				if (level3_icecreams[i].isAir) {
					level3_icecreams[i].waveAngle += 0.08f;
					level3_icecreams[i].y = level3_icecreams[i].baseY + sinf(level3_icecreams[i].waveAngle) * 12.0f;
				}
				else {
					level3_icecreams[i].y = level3_icecreams[i].baseY;
				}

				if (level3_icecreams[i].x + level3_icecreams[i].width < -60) {
					level3_icecreams[i].active = false;

					level3_consecutiveMissedTreats++;
					level3_energy -= 20;
					level3_playNegPointSound();

					if (level3_consecutiveMissedTreats >= 3 || level3_energy <= 0) {
						level3_energy = 0;
						level3_gameOver = true;
						level3_playLoseSound();
						return;
					}
					else {
						sprintf_s(level3_message, sizeof(level3_message), "MISSED ICE CREAM! (%d/3 MISSED IN A ROW!)", level3_consecutiveMissedTreats);
						level3_messageTimer = 70;
					}
				}
			}
		}

		// Key Updates (Air keys are high up at 350.0f)
		for (int i = 0; i < LEVEL3_NUM_KEYS; i++) {
			if (level3_keys[i].collected) continue;

			if (level3_isWorldXOnExclave(level3_keys[i].trackPos)) {
				level3_keys[i].y = level3_keys[i].isAir ? (LEVEL3_EXCLAVE_SURFACE_Y + 150.0f) : (LEVEL3_EXCLAVE_SURFACE_Y + 25.0f);
			}
			else {
				level3_keys[i].y = level3_keys[i].isAir ? 350.0f : (level3_groundY + 25.0f);
			}

			float screenKeyX = (float)(level3_keys[i].trackPos - level3_distanceCovered + 100);
			if (level3_rectOverlap(screenKeyX - 5.0f, level3_keys[i].y - 5.0f, level3_keys[i].size + 10, level3_keys[i].size + 10,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_keys[i].collected = true;
				level3_keyCollected[level3_keys[i].id] = true;
				level3_updateScore(150);
				level3_playPlusPointSound();
			}
		}

		// Score:2x Collision
		if (level3_power2x.active) {
			if (level3_rectOverlap(level3_power2x.x, level3_power2x.y, level3_power2x.width, level3_power2x.height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_power2x.active = false;
				level3_scoreMultiplier = 2.0f;
				level3_multiplierTimer = 500;
				strcpy_s(level3_message, sizeof(level3_message), "SCORE:2X BOOST ACTIVATED FOR 10 SECONDS!");
				level3_messageTimer = 70;
				level3_playPlusPointSound();
			}
		}

		// Score:0.5x Collision
		if (level3_powerHalf.active) {
			if (level3_rectOverlap(level3_powerHalf.x, level3_powerHalf.y, level3_powerHalf.width, level3_powerHalf.height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_powerHalf.active = false;
				level3_scoreMultiplier = 0.5f;
				level3_multiplierTimer = 500;
				strcpy_s(level3_message, sizeof(level3_message), "SCORE:0.5X ACTIVATED! HALF SCORE FOR 10 SECONDS!");
				level3_messageTimer = 70;
				level3_playNegPointSound();
			}
		}

		// Speed:2x Collision
		if (level3_powerSpeed2x.active) {
			if (level3_rectOverlap(level3_powerSpeed2x.x, level3_powerSpeed2x.y, level3_powerSpeed2x.width, level3_powerSpeed2x.height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_powerSpeed2x.active = false;
				level3_speedMultiplier = 2.0f;
				level3_playerSpeed = (int)(LEVEL3_BASE_SPEED * 2.0f);
				level3_speedMultiplierTimer = 500;
				strcpy_s(level3_message, sizeof(level3_message), "SPEED:2X ACTIVATED! DOUBLE RUNNING SPEED FOR 10 SECONDS!");
				level3_messageTimer = 70;
				level3_playPlusPointSound();
			}
		}

		// Speed:0.5x Collision
		if (level3_powerSpeedHalf.active) {
			if (level3_rectOverlap(level3_powerSpeedHalf.x, level3_powerSpeedHalf.y, level3_powerSpeedHalf.width, level3_powerSpeedHalf.height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_powerSpeedHalf.active = false;
				level3_speedMultiplier = 0.5f;
				level3_playerSpeed = (int)(LEVEL3_BASE_SPEED * 0.5f);
				if (level3_playerSpeed < 2) level3_playerSpeed = 2;
				level3_speedMultiplierTimer = 500;
				strcpy_s(level3_message, sizeof(level3_message), "SPEED:0.5X ACTIVATED! HALF RUNNING SPEED FOR 10 SECONDS!");
				level3_messageTimer = 70;
				level3_playNegPointSound();
			}
		}

		// Ice Cream Collision
		for (int i = 0; i < LEVEL3_NUM_ICECREAMS; i++) {
			if (!level3_icecreams[i].active || level3_icecreams[i].collected) continue;
			if (level3_rectOverlap(level3_icecreams[i].x, level3_icecreams[i].y, level3_icecreams[i].width, level3_icecreams[i].height,
				level3_playerX, level3_playerY, level3_playerWidth, level3_playerHeight)) {
				level3_icecreams[i].collected = true;
				level3_icecreams[i].active = false;

				level3_consecutiveMissedTreats = 0;

				level3_energy += level3_icecreams[i].energyGain;
				if (level3_energy > 100) level3_energy = 100;

				level3_updateScore(level3_icecreams[i].scoreGain);
				level3_playPlusPointSound();

				sprintf_s(level3_message, sizeof(level3_message), "YUMMY! +%d ENERGY RECOVERED!", level3_icecreams[i].energyGain);
				level3_messageTimer = 60;
			}
		}
	}

	// Running Animation Delay
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

	int currentTargetCave = 0;
	while (currentTargetCave < LEVEL3_NUM_DOORS && level3_doors[currentTargetCave].visited) {
		currentTargetCave++;
	}

	for (int i = 0; i < LEVEL3_NUM_DOORS; i++) {
		int minX = level3_doors[i].centerX - level3_doors[i].width / 2;
		int maxX = level3_doors[i].centerX + level3_doors[i].width / 2;
		int minY = level3_doors[i].centerY - level3_doors[i].height / 2;
		int maxY = level3_doors[i].centerY + level3_doors[i].height / 2;

		if (mx >= minX && mx <= maxX && my >= minY && my <= maxY) {
			if (level3_doors[i].visited) {
				strcpy_s(level3_message, sizeof(level3_message), "Cave already cleared! Move to the next cave.");
				level3_messageTimer = 70;
				return;
			}

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

			if (level3_doors[i].handlerIndex == 4) {
				level3_doors[i].visited = true;
				startLevel3Combat();
			}
			else {
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