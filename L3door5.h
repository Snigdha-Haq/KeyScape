#ifndef L3DOOR5_H_INCLUDED
#define L3DOOR5_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <windows.h>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int level3_energy;
extern bool level3_gameOver;
extern bool level3_keyFound;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

#define L3C_ENEMY_MAX_ENERGY      150
#define L3C_KNIFE_DAMAGE           5
#define L3C_SHOOT_DAMAGE           5
#define L3C_ENEMY_ATTACK_DAMAGE    15
#define L3C_KNIFE_COOLDOWN_FRAMES  20
#define L3C_SHOOT_COOLDOWN_FRAMES  12
#define L3C_ATTACK_FRAME_DELAY     3
#define L3C_DODGE_DURATION         18

#define L3C_PLAYER_NORMAL_W  90
#define L3C_PLAYER_NORMAL_H 130
#define L3C_PLAYER_SLIDE_W  120
#define L3C_PLAYER_SLIDE_H   60
#define L3C_PLAYER_MOVE_SPEED  5
#define L3C_PLAYER_MIN_X       40

#define L3C_APPROACH_DURATION  70
#define L3C_TELEGRAPH_DURATION 25
#define L3C_LUNGE_DURATION     20
#define L3C_RETREAT_DURATION   45

#define L3C_ENEMY_FAR_X    (SCREEN_WIDTH - 330)
#define L3C_ENEMY_NEAR_X   (SCREEN_WIDTH - 480)
#define L3C_ENEMY_LUNGE_X  (L3C_ENEMY_NEAR_X - 150)

#define L3C_MELEE_RANGE     220
#define L3C_DANGER_MARGIN   70

enum L3CEnemyPhase { L3C_PHASE_APPROACH, L3C_PHASE_TELEGRAPH, L3C_PHASE_LUNGE, L3C_PHASE_RETREAT };

static bool level3combat_active = false;
static int level3combat_enemyEnergy = 0;
static int level3combat_attackCooldown = 0;

static int level3combat_enemyPhase = L3C_PHASE_APPROACH;
static int level3combat_phaseTimer = 0;
static float level3combat_enemyX = (float)L3C_ENEMY_FAR_X;
static bool level3combat_damageAppliedThisCycle = false;

static float level3combat_playerX = 150.0f;

static bool level3combat_playerAttacking = false;
static int level3combat_attackType = 0;
static int level3combat_attackAnimFrame = 0;
static int level3combat_attackAnimTimer = 0;

static bool level3combat_dodging = false;
static int level3combat_dodgeType = 0;
static int level3combat_dodgeTimer = 0;
static int level3combat_dodgeFrame = 0;

static char level3combat_hint[64] = "";
static int level3combat_hintTimer = 0;

extern int level3_finishStage;

inline void startLevel3Combat()
{
	level3combat_active = true;
	level3combat_enemyEnergy = L3C_ENEMY_MAX_ENERGY;
	level3combat_attackCooldown = 0;
	level3combat_enemyPhase = L3C_PHASE_APPROACH;
	level3combat_phaseTimer = 0;
	level3combat_enemyX = (float)L3C_ENEMY_FAR_X;
	level3combat_damageAppliedThisCycle = false;
	level3combat_playerX = 150.0f;
	level3combat_playerAttacking = false;
	level3combat_attackAnimFrame = 0;
	level3combat_attackAnimTimer = 0;
	level3combat_dodging = false;
	level3combat_dodgeTimer = 0;
	level3combat_dodgeFrame = 0;
	level3combat_hintTimer = 0;
}

inline void renderLevel3Combat()
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

	int playerImg = idleImg;
	int drawW = L3C_PLAYER_NORMAL_W, drawH = L3C_PLAYER_NORMAL_H;

	if (level3combat_playerAttacking) {
		playerImg = (level3combat_attackType == 0) ? knifeFrames[level3combat_attackAnimFrame] : shootFrames[level3combat_attackAnimFrame];
	}
	else if (level3combat_dodging) {
		if (level3combat_dodgeType == 0) {
			playerImg = jumpFrames[level3combat_dodgeFrame];
		}
		else {
			playerImg = slideImg;
			drawW = L3C_PLAYER_SLIDE_W;
			drawH = L3C_PLAYER_SLIDE_H;
		}
	}
	iShowImage((int)level3combat_playerX, 80, drawW, drawH, playerImg);

	int enemyImg = enemyIdleImg;
	int enemyW = 200, enemyH = 220;

	if (level3combat_enemyPhase == L3C_PHASE_TELEGRAPH) {
		enemyImg = enemyWindupImg;
		enemyW = 220; enemyH = 240;
	}
	else if (level3combat_enemyPhase == L3C_PHASE_LUNGE) {
		enemyImg = enemyStrikeImg;
		enemyW = 230; enemyH = 250;

		iSetColor(255, 120, 0);
		for (int t = 1; t <= 3; t++) {
			int trailX = (int)level3combat_enemyX + t * 25;
			iLine(trailX, 150, trailX + 15, 150);
			iLine(trailX, 200, trailX + 15, 200);
		}
	}
	iShowImage((int)level3combat_enemyX, 80, enemyW, enemyH, enemyImg);

	// Character Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level3_energy > 30 ? 0 : 220, level3_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level3_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	// Enemy Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 80, 200, 20);
	iSetColor(200, 0, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 80, (int)(200.0f * level3combat_enemyEnergy / L3C_ENEMY_MAX_ENERGY), 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 80, 200, 20);
	iText(20, SCREEN_HEIGHT - 95, "Enemy");

	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH / 2 - 190, 30, "LEFT/RIGHT = move   SPACE = knife   F = shoot");

	if (level3combat_enemyPhase == L3C_PHASE_TELEGRAPH || level3combat_enemyPhase == L3C_PHASE_LUNGE) {
		iSetColor(255, 230, 0);
		iText(SCREEN_WIDTH / 2 - 150, 60, "INCOMING! Retreat (LEFT) or dodge (UP/DOWN)!");
	}

	if (level3combat_hintTimer > 0) {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 60, 100, level3combat_hint);
	}
}

inline void level3combat_fixedUpdate()
{
	if (level3_gameOver) {
		level3combat_active = false;
		return;
	}

	if (level3combat_attackCooldown > 0) level3combat_attackCooldown--;
	if (level3combat_hintTimer > 0) level3combat_hintTimer--;

	if (level3combat_playerAttacking) {
		level3combat_attackAnimTimer++;
		if (level3combat_attackAnimTimer >= L3C_ATTACK_FRAME_DELAY) {
			level3combat_attackAnimTimer = 0;
			level3combat_attackAnimFrame++;
			int maxFrames = (level3combat_attackType == 0) ? 7 : 3;
			if (level3combat_attackAnimFrame >= maxFrames) {
				level3combat_playerAttacking = false;
				level3combat_attackAnimFrame = 0;
			}
		}
	}

	if (level3combat_dodging) {
		level3combat_dodgeTimer--;
		if (level3combat_dodgeType == 0) {
			level3combat_dodgeFrame = (level3combat_dodgeTimer / 6) % 3;
		}
		if (level3combat_dodgeTimer <= 0) {
			level3combat_dodging = false;
		}
	}

	if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level3combat_playerX += L3C_PLAYER_MOVE_SPEED;
		float maxX = level3combat_enemyX - 80.0f;
		if (level3combat_playerX > maxX) level3combat_playerX = maxX;
	}
	else if (isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		level3combat_playerX -= L3C_PLAYER_MOVE_SPEED;
		if (level3combat_playerX < L3C_PLAYER_MIN_X) level3combat_playerX = L3C_PLAYER_MIN_X;
	}

	if (!level3combat_dodging) {
		if (isSpecialKeyPressed(GLUT_KEY_UP)) {
			level3combat_dodging = true;
			level3combat_dodgeType = 0;
			level3combat_dodgeTimer = L3C_DODGE_DURATION;
		}
	}
	if (!level3combat_dodging && isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		level3combat_dodging = true;
		level3combat_dodgeType = 1;
		level3combat_dodgeTimer = L3C_DODGE_DURATION;
	}

	if (isKeyPressed(' ') && level3combat_attackCooldown <= 0 && !level3combat_playerAttacking) {
		level3combat_attackCooldown = L3C_KNIFE_COOLDOWN_FRAMES;
		level3combat_playerAttacking = true;
		level3combat_attackType = 0;
		level3combat_attackAnimFrame = 0;
		level3combat_attackAnimTimer = 0;

		float distance = level3combat_enemyX - (level3combat_playerX + L3C_PLAYER_NORMAL_W);
		if (distance <= L3C_MELEE_RANGE) {
			level3combat_enemyEnergy -= L3C_KNIFE_DAMAGE;
			if (level3combat_enemyEnergy < 0) level3combat_enemyEnergy = 0;
			level3_updateScore(25);
			level3_playPlusPointSound();
		}
		else {
			strcpy_s(level3combat_hint, sizeof(level3combat_hint), "Too far! Move closer.");
			level3combat_hintTimer = 40;
		}
	}
	else if ((isKeyPressed('f') || isKeyPressed('F')) && level3combat_attackCooldown <= 0 && !level3combat_playerAttacking) {
		level3combat_enemyEnergy -= L3C_SHOOT_DAMAGE;
		if (level3combat_enemyEnergy < 0) level3combat_enemyEnergy = 0;
		level3combat_attackCooldown = L3C_SHOOT_COOLDOWN_FRAMES;
		level3combat_playerAttacking = true;
		level3combat_attackType = 1;
		level3combat_attackAnimFrame = 0;
		level3combat_attackAnimTimer = 0;
		level3_updateScore(25);
		level3_playPlusPointSound();
	}

	level3combat_phaseTimer++;

	if (level3combat_enemyPhase == L3C_PHASE_APPROACH) {
		float t = (float)level3combat_phaseTimer / L3C_APPROACH_DURATION;
		if (t > 1.0f) t = 1.0f;
		level3combat_enemyX = (float)L3C_ENEMY_FAR_X + ((float)L3C_ENEMY_NEAR_X - (float)L3C_ENEMY_FAR_X) * t;

		if (level3combat_phaseTimer >= L3C_APPROACH_DURATION) {
			level3combat_enemyX = (float)L3C_ENEMY_NEAR_X;
			level3combat_enemyPhase = L3C_PHASE_TELEGRAPH;
			level3combat_phaseTimer = 0;
		}
	}
	else if (level3combat_enemyPhase == L3C_PHASE_TELEGRAPH) {
		level3combat_enemyX = (float)L3C_ENEMY_NEAR_X;
		if (level3combat_phaseTimer >= L3C_TELEGRAPH_DURATION) {
			level3combat_enemyPhase = L3C_PHASE_LUNGE;
			level3combat_phaseTimer = 0;
		}
	}
	else if (level3combat_enemyPhase == L3C_PHASE_LUNGE) {
		float t = (float)level3combat_phaseTimer / L3C_LUNGE_DURATION;
		if (t > 1.0f) t = 1.0f;
		level3combat_enemyX = (float)L3C_ENEMY_NEAR_X + ((float)L3C_ENEMY_LUNGE_X - (float)L3C_ENEMY_NEAR_X) * t;

		if (level3combat_phaseTimer >= L3C_LUNGE_DURATION) {
			level3combat_enemyX = (float)L3C_ENEMY_LUNGE_X;

			if (!level3combat_damageAppliedThisCycle) {
				float playerRightEdge = level3combat_playerX + L3C_PLAYER_NORMAL_W;
				float dangerDistance = level3combat_enemyX - playerRightEdge;
				bool playerInDanger = (dangerDistance <= L3C_DANGER_MARGIN);

				if (playerInDanger && !level3combat_dodging) {
					level3_energy -= L3C_ENEMY_ATTACK_DAMAGE;
					level3_updateScore(-150);
					level3_playNegPointSound();
					if (level3_energy < 0) level3_energy = 0;
				}
				level3combat_damageAppliedThisCycle = true;
			}

			level3combat_enemyPhase = L3C_PHASE_RETREAT;
			level3combat_phaseTimer = 0;
		}
	}
	else if (level3combat_enemyPhase == L3C_PHASE_RETREAT) {
		float t = (float)level3combat_phaseTimer / L3C_RETREAT_DURATION;
		if (t > 1.0f) t = 1.0f;
		level3combat_enemyX = (float)L3C_ENEMY_LUNGE_X + ((float)L3C_ENEMY_FAR_X - (float)L3C_ENEMY_LUNGE_X) * t;

		if (level3combat_phaseTimer >= L3C_RETREAT_DURATION) {
			level3combat_enemyX = (float)L3C_ENEMY_FAR_X;
			level3combat_enemyPhase = L3C_PHASE_APPROACH;
			level3combat_phaseTimer = 0;
			level3combat_damageAppliedThisCycle = false;
		}
	}

	if (level3combat_enemyEnergy <= 0) {
		level3combat_active = false;
		level3_finishStage = 1;
		level3_updateScore(500);
		level3_playPlusPointSound();
	}
	else if (level3_energy <= 0) {
		level3_energy = 0;
		level3_gameOver = true;
		level3combat_active = false;
	}
}

inline void updateLevel3CombatIfActive()
{
	if (level3combat_active) {
		level3combat_fixedUpdate();
	}
}

#endif