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
extern bool isSoundMuted;

void level2_updateScore(int addPoints);

// Sound functions declared in level2.h
void playPlusPointSound();
void playNegPointSound();

// =================================================================
//  LEVEL 2 COMBAT - GUARDIAN FIGHT DATA & LOGIC
// =================================================================
#define L2C_ENEMY_MAX_ENERGY      150
#define L2C_KNIFE_DAMAGE           12
#define L2C_SHOOT_DAMAGE           12
#define L2C_ENEMY_ATTACK_DAMAGE    15
#define L2C_KNIFE_COOLDOWN_FRAMES  20
#define L2C_SHOOT_COOLDOWN_FRAMES  12
#define L2C_ATTACK_FRAME_DELAY      3
#define L2C_DODGE_DURATION         18

#define L2C_PLAYER_NORMAL_W  90
#define L2C_PLAYER_NORMAL_H 130
#define L2C_PLAYER_SLIDE_W  120
#define L2C_PLAYER_SLIDE_H   60
#define L2C_PLAYER_MOVE_SPEED  5
#define L2C_PLAYER_MIN_X       40

#define L2C_APPROACH_DURATION  70
#define L2C_TELEGRAPH_DURATION 25
#define L2C_LUNGE_DURATION     20
#define L2C_RETREAT_DURATION   45

#define L2C_ENEMY_FAR_X    (SCREEN_WIDTH - 330)
#define L2C_ENEMY_NEAR_X   (SCREEN_WIDTH - 480)
#define L2C_ENEMY_LUNGE_X  (L2C_ENEMY_NEAR_X - 150)

#define L2C_MELEE_RANGE     220
#define L2C_DANGER_MARGIN    70

enum L2CEnemyPhase { L2C_PHASE_APPROACH, L2C_PHASE_TELEGRAPH, L2C_PHASE_LUNGE, L2C_PHASE_RETREAT };

static bool level2combat_active = false;
static int level2combat_enemyEnergy = 0;
static int level2combat_attackCooldown = 0;

static int level2combat_enemyPhase = L2C_PHASE_APPROACH;
static int level2combat_phaseTimer = 0;
static float level2combat_enemyX = (float)L2C_ENEMY_FAR_X;
static bool level2combat_damageAppliedThisCycle = false;

static float level2combat_playerX = 150.0f;

static bool level2combat_playerAttacking = false;
static int level2combat_attackType = 0;   // 0 = knife, 1 = shoot
static int level2combat_attackAnimFrame = 0;
static int level2combat_attackAnimTimer = 0;

static bool level2combat_dodging = false;
static int level2combat_dodgeType = 0;    // 0 = jump, 1 = slide
static int level2combat_dodgeTimer = 0;
static int level2combat_dodgeFrame = 0;

static char level2combat_hint[64] = "";
static int level2combat_hintTimer = 0;

// Cutscene Stage forward decl
static int level2_finishStage = 0;

inline void startLevel2Combat()
{
	level2combat_active = true;
	level2combat_enemyEnergy = L2C_ENEMY_MAX_ENERGY;
	level2combat_attackCooldown = 0;

	level2combat_enemyPhase = L2C_PHASE_APPROACH;
	level2combat_phaseTimer = 0;
	level2combat_enemyX = (float)L2C_ENEMY_FAR_X;
	level2combat_damageAppliedThisCycle = false;

	level2combat_playerX = 150.0f;

	level2combat_playerAttacking = false;
	level2combat_attackAnimFrame = 0;
	level2combat_attackAnimTimer = 0;

	level2combat_dodging = false;
	level2combat_dodgeTimer = 0;
	level2combat_dodgeFrame = 0;

	level2combat_hintTimer = 0;
}

inline void renderLevel2Combat()
{
	static int bgImg = -1, idleImg = -1;
	static int enemyIdleImg = -1, enemyWindupImg = -1, enemyStrikeImg = -1;
	static int knifeFrames[7];
	static int shootFrames[3];
	static int jumpFrames[3];
	static int slideImg = -1;

	if (bgImg == -1) {
		bgImg = iLoadImage("Image/UnderSeacombat.png");
		idleImg = iLoadImage("Image/idle_1.png");

		enemyIdleImg = iLoadImage("Image/Enemy_idle.png");
		if (enemyIdleImg < 0) enemyIdleImg = iLoadImage("Image/Enemy.png");

		enemyWindupImg = iLoadImage("Image/Enemy_aim.png");
		if (enemyWindupImg < 0) enemyWindupImg = enemyIdleImg;

		enemyStrikeImg = iLoadImage("Image/Enemy_charge.png");
		if (enemyStrikeImg < 0) enemyStrikeImg = enemyIdleImg;

		knifeFrames[0] = iLoadImage("Image/knife_1.png");
		knifeFrames[1] = iLoadImage("Image/knife_2.png");
		knifeFrames[2] = iLoadImage("Image/knife_3.png");
		knifeFrames[3] = iLoadImage("Image/knife_4.png");
		knifeFrames[4] = iLoadImage("Image/knife_5.png");
		knifeFrames[5] = iLoadImage("Image/knife_6.png");
		knifeFrames[6] = iLoadImage("Image/knife_7.png");

		shootFrames[0] = iLoadImage("Image/shoot_1.png");
		shootFrames[1] = iLoadImage("Image/shoot_2.png");
		shootFrames[2] = iLoadImage("Image/shoot_3.png");

		jumpFrames[0] = iLoadImage("Image/jump_1.png");
		jumpFrames[1] = iLoadImage("Image/jump_2.png");
		jumpFrames[2] = iLoadImage("Image/jump_3.png");

		slideImg = iLoadImage("Image/slide.png");
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImg);

	// Player
	int playerImg = idleImg;
	int drawW = L2C_PLAYER_NORMAL_W, drawH = L2C_PLAYER_NORMAL_H;

	if (level2combat_playerAttacking) {
		playerImg = (level2combat_attackType == 0)
			? knifeFrames[level2combat_attackAnimFrame]
			: shootFrames[level2combat_attackAnimFrame];
	}
	else if (level2combat_dodging) {
		if (level2combat_dodgeType == 0) {
			playerImg = jumpFrames[level2combat_dodgeFrame];
		}
		else {
			playerImg = slideImg;
			drawW = L2C_PLAYER_SLIDE_W;
			drawH = L2C_PLAYER_SLIDE_H;
		}
	}
	iShowImage((int)level2combat_playerX, 80, drawW, drawH, playerImg);

	// Enemy
	int enemyImg = enemyIdleImg;
	int enemyW = 200, enemyH = 220;

	if (level2combat_enemyPhase == L2C_PHASE_TELEGRAPH) {
		enemyImg = enemyWindupImg;
		enemyW = 220; enemyH = 240;
	}
	else if (level2combat_enemyPhase == L2C_PHASE_LUNGE) {
		enemyImg = enemyStrikeImg;
		enemyW = 230; enemyH = 250;

		iSetColor(255, 120, 0);
		for (int t = 1; t <= 3; t++) {
			int trailX = (int)level2combat_enemyX + t * 25;
			iLine(trailX, 150, trailX + 15, 150);
			iLine(trailX, 200, trailX + 15, 200);
		}
	}
	iShowImage((int)level2combat_enemyX, 80, enemyW, enemyH, enemyImg);

	// ---- Energy Bars ----
	// 1. Character Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level2_energy > 30 ? 0 : 220, level2_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level2_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	// 2. Enemy Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 80, 200, 20);
	iSetColor(200, 0, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 80,
		(int)(200.0f * level2combat_enemyEnergy / L2C_ENEMY_MAX_ENERGY), 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 80, 200, 20);
	iText(20, SCREEN_HEIGHT - 95, "Enemy");

	// Instructions
	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH / 2 - 190, 30, "LEFT/RIGHT = move   SPACE = knife   F = shoot");

	if (level2combat_enemyPhase == L2C_PHASE_TELEGRAPH || level2combat_enemyPhase == L2C_PHASE_LUNGE) {
		iSetColor(255, 230, 0);
		iText(SCREEN_WIDTH / 2 - 150, 60, "INCOMING! Retreat (LEFT) or dodge (UP/DOWN)!");
	}

	if (level2combat_hintTimer > 0) {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 60, 100, level2combat_hint);
	}
}

inline void level2combat_fixedUpdate()
{
	if (level2_gameOver) {
		level2combat_active = false;
		return;
	}

	if (level2combat_attackCooldown > 0) level2combat_attackCooldown--;
	if (level2combat_hintTimer > 0) level2combat_hintTimer--;

	if (level2combat_playerAttacking) {
		level2combat_attackAnimTimer++;
		if (level2combat_attackAnimTimer >= L2C_ATTACK_FRAME_DELAY) {
			level2combat_attackAnimTimer = 0;
			level2combat_attackAnimFrame++;
			int maxFrames = (level2combat_attackType == 0) ? 7 : 3;
			if (level2combat_attackAnimFrame >= maxFrames) {
				level2combat_playerAttacking = false;
				level2combat_attackAnimFrame = 0;
			}
		}
	}

	if (level2combat_dodging) {
		level2combat_dodgeTimer--;
		if (level2combat_dodgeType == 0) {
			level2combat_dodgeFrame = (level2combat_dodgeTimer / 6) % 3;
		}
		if (level2combat_dodgeTimer <= 0) {
			level2combat_dodging = false;
		}
	}

	if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level2combat_playerX += L2C_PLAYER_MOVE_SPEED;
		float maxX = level2combat_enemyX - 80.0f;
		if (level2combat_playerX > maxX) level2combat_playerX = maxX;
	}
	else if (isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		level2combat_playerX -= L2C_PLAYER_MOVE_SPEED;
		if (level2combat_playerX < L2C_PLAYER_MIN_X) level2combat_playerX = L2C_PLAYER_MIN_X;
	}

	if (!level2combat_dodging) {
		if (isSpecialKeyPressed(GLUT_KEY_UP)) {
			level2combat_dodging = true;
			level2combat_dodgeType = 0;
			level2combat_dodgeTimer = L2C_DODGE_DURATION;
		}
	}
	if (!level2combat_dodging && isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		level2combat_dodging = true;
		level2combat_dodgeType = 1;
		level2combat_dodgeTimer = L2C_DODGE_DURATION;
	}

	// Knife Attack
	if (isKeyPressed(' ') && level2combat_attackCooldown <= 0 && !level2combat_playerAttacking) {
		level2combat_attackCooldown = L2C_KNIFE_COOLDOWN_FRAMES;
		level2combat_playerAttacking = true;
		level2combat_attackType = 0;
		level2combat_attackAnimFrame = 0;
		level2combat_attackAnimTimer = 0;

		float distance = level2combat_enemyX - (level2combat_playerX + L2C_PLAYER_NORMAL_W);
		if (distance <= L2C_MELEE_RANGE) {
			level2combat_enemyEnergy -= L2C_KNIFE_DAMAGE;
			if (level2combat_enemyEnergy < 0) level2combat_enemyEnergy = 0;
			level2_updateScore(25);
			playPlusPointSound();
		}
		else {
			strcpy_s(level2combat_hint, sizeof(level2combat_hint), "Too far! Move closer.");
			level2combat_hintTimer = 40;
		}
	}
	// Shoot Attack
	else if ((isKeyPressed('f') || isKeyPressed('F')) && level2combat_attackCooldown <= 0 && !level2combat_playerAttacking) {
		level2combat_enemyEnergy -= L2C_SHOOT_DAMAGE;
		if (level2combat_enemyEnergy < 0) level2combat_enemyEnergy = 0;
		level2combat_attackCooldown = L2C_SHOOT_COOLDOWN_FRAMES;
		level2combat_playerAttacking = true;
		level2combat_attackType = 1;
		level2combat_attackAnimFrame = 0;
		level2combat_attackAnimTimer = 0;
		level2_updateScore(25);
		playPlusPointSound();
	}

	level2combat_phaseTimer++;

	if (level2combat_enemyPhase == L2C_PHASE_APPROACH) {
		float t = (float)level2combat_phaseTimer / L2C_APPROACH_DURATION;
		if (t > 1.0f) t = 1.0f;
		level2combat_enemyX = (float)L2C_ENEMY_FAR_X + ((float)L2C_ENEMY_NEAR_X - (float)L2C_ENEMY_FAR_X) * t;

		if (level2combat_phaseTimer >= L2C_APPROACH_DURATION) {
			level2combat_enemyX = (float)L2C_ENEMY_NEAR_X;
			level2combat_enemyPhase = L2C_PHASE_TELEGRAPH;
			level2combat_phaseTimer = 0;
		}
	}
	else if (level2combat_enemyPhase == L2C_PHASE_TELEGRAPH) {
		level2combat_enemyX = (float)L2C_ENEMY_NEAR_X;

		if (level2combat_phaseTimer >= L2C_TELEGRAPH_DURATION) {
			level2combat_enemyPhase = L2C_PHASE_LUNGE;
			level2combat_phaseTimer = 0;
		}
	}
	else if (level2combat_enemyPhase == L2C_PHASE_LUNGE) {
		float t = (float)level2combat_phaseTimer / L2C_LUNGE_DURATION;
		if (t > 1.0f) t = 1.0f;
		level2combat_enemyX = (float)L2C_ENEMY_NEAR_X + ((float)L2C_ENEMY_LUNGE_X - (float)L2C_ENEMY_NEAR_X) * t;

		if (level2combat_phaseTimer >= L2C_LUNGE_DURATION) {
			level2combat_enemyX = (float)L2C_ENEMY_LUNGE_X;

			if (!level2combat_damageAppliedThisCycle) {
				float playerRightEdge = level2combat_playerX + L2C_PLAYER_NORMAL_W;
				float dangerDistance = level2combat_enemyX - playerRightEdge;
				bool playerInDanger = (dangerDistance <= L2C_DANGER_MARGIN);

				if (playerInDanger && !level2combat_dodging) {
					level2_energy -= L2C_ENEMY_ATTACK_DAMAGE;
					level2_updateScore(-150);
					playNegPointSound();
					if (level2_energy < 0) level2_energy = 0;
				}
				level2combat_damageAppliedThisCycle = true;
			}

			level2combat_enemyPhase = L2C_PHASE_RETREAT;
			level2combat_phaseTimer = 0;
		}
	}
	else if (level2combat_enemyPhase == L2C_PHASE_RETREAT) {
		float t = (float)level2combat_phaseTimer / L2C_RETREAT_DURATION;
		if (t > 1.0f) t = 1.0f;
		level2combat_enemyX = (float)L2C_ENEMY_LUNGE_X + ((float)L2C_ENEMY_FAR_X - (float)L2C_ENEMY_LUNGE_X) * t;

		if (level2combat_phaseTimer >= L2C_RETREAT_DURATION) {
			level2combat_enemyX = (float)L2C_ENEMY_FAR_X;
			level2combat_enemyPhase = L2C_PHASE_APPROACH;
			level2combat_phaseTimer = 0;
			level2combat_damageAppliedThisCycle = false;
		}
	}

	if (level2combat_enemyEnergy <= 0) {
		level2combat_active = false;
		level2_finishStage = 1;
		level2_updateScore(500);
		playPlusPointSound();
	}
	else if (level2_energy <= 0) {
		level2_energy = 0;
		level2_gameOver = true;
		level2combat_active = false;
	}
}

// ---------------- DOORS DATA & SETUP ----------------
struct Level2Door {
	int x, y, width, height;
	bool visited;
	int assignedKeyColorId;
};

#define LEVEL2_DOOR_WIDTH   120
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
	level2combat_active = false;

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

	if (level2combat_active) {
		renderLevel2Combat();
		return;
	}

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
	if (level2combat_active) return;
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
			playPlusPointSound();
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
					playPlusPointSound();
					strcpy_s(level2_message, sizeof(level2_message), "Correct! Door unlocked - Select another door.");
					level2_messageTimer = 100;
				}
				else {
					level2_energy -= LEVEL2_WRONG_TASK_PENALTY;
					level2_updateScore(-100);
					playNegPointSound();
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
				playNegPointSound();
				strcpy_s(level2_message, sizeof(level2_message), "Key missing! Go back and collect the matching key.");
				level2_messageTimer = 90;
				return;
			}

			if (i == level2_realKeyDoor) {
				level2_doors[i].visited = true;
				startLevel2Combat();
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

inline void updateLevel2CombatIfActive()
{
	if (level2combat_active) {
		level2combat_fixedUpdate();
	}
}

#endif