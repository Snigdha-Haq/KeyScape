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
int level2_energy = 100;
bool level2_gameOver = false;
bool level2_keyFound = false;

// ---------------- SCORE & PERSISTENT HIGH SCORE ----------------
int level2_score = 0;
int level2_highScore = 0;
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

// ---------------- KEYS ----------------
#define LEVEL2_NUM_KEYS 4
#define LEVEL2_KEY_SIZE 40

struct Level2Key {
	float y;
	int size;
	int id;
	int trackPos;
	bool isAir;
	bool collected;
};

static Level2Key level2_keys[LEVEL2_NUM_KEYS];
static int level2_keySpawnDistance[LEVEL2_NUM_KEYS] = { 700, 2200, 3900, 5300 };

int level2_keyColor[LEVEL2_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // Key 0 - Gold
	{ 65, 145, 220 },  // Key 1 - Blue
	{ 60, 200, 90 },   // Key 2 - Green
	{ 225, 60, 60 }    // Key 3 - Red
};

bool level2_keyCollected[LEVEL2_NUM_KEYS] = { false, false, false, false };

// ---------------- OBSTACLES ----------------
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

char level2_message[120] = "";
int level2_messageTimer = 0;

// Include Doors Header after shared variables definition
#include "L2doors.h"

// ---------------- TEXT HELPERS ----------------
inline void level2_drawBoldText(int x, int y, const char* str, void* font)
{
	iText(x, y, (char*)str, font);
	iText(x + 1, y, (char*)str, font);
	iText(x, y + 1, (char*)str, font);
	iText(x + 1, y + 1, (char*)str, font);
}

inline bool level2_rectOverlap(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline void level2_drawKey(float x, float y, int size, int r, int g, int b)
{
	level2_drawKeyVisual(x, y, size, r, g, b);
}

// ---------------- SETUP & RESET ----------------
inline void setupLevel2()
{
	srand((unsigned int)time(0));

	if (!level2_highScoreLoaded) {
		level2_loadHighScore();
	}

	setupLevel2Doors();

	level2_playerX = 100.0f;
	level2_playerY = level2_groundY;
	level2_playerWidth = LEVEL2_PLAYER_NORMAL_W;
	level2_playerHeight = LEVEL2_PLAYER_NORMAL_H;

	level2_isSliding = false;
	level2_slideTimer = 0;

	level2_bgX = 0;
	level2_distanceCovered = 0;
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

		if (level2_obstacles[i].type == 0) {
			level2_obstacles[i].width = 65;
			level2_obstacles[i].height = 60;
			level2_obstacles[i].baseY = (float)level2_groundY;
		}
		else if (level2_obstacles[i].type == 1) {
			level2_obstacles[i].width = 70;
			level2_obstacles[i].height = 70;
			level2_obstacles[i].baseY = level2_obstacles[i].isAir ? 150.0f : ((float)level2_groundY + 10.0f);
		}
		else {
			level2_obstacles[i].width = 50;
			level2_obstacles[i].height = 75;
			level2_obstacles[i].baseY = level2_obstacles[i].isAir ? 155.0f : ((float)level2_groundY + 20.0f);
		}
		level2_obstacles[i].y = level2_obstacles[i].baseY;
	}
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

	// 1. GAME OVER SCREEN
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

	// 2. VICTORY SCREEN
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

	// 3. BACKGROUND (GAMEPLAY)
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

	// 6. Doors & Tasks (Rendered via L2doors.h)
	renderLevel2DoorsContent(doorClosedImg, doorOpenImg);

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
	if (level2_gameOver) {
		if (mx >= 380 && mx <= 670 && my >= 245 && my <= 285) {
			setupLevel2();
		}
		return;
	}

	handleL2DoorClicks(mx, my);
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