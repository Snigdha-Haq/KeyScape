#ifndef LEVEL1_H_INCLUDED
#define LEVEL1_H_INCLUDED

#include <cstdlib>
#include <ctime>
#include <cstdio>


#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

// ---------------- BASIC LEVEL STATE ----------------
static int level1_playerX = 100, level1_playerY = 80;
static int level1_playerWidth = 90, level1_playerHeight = 130;
static int level1_playerSpeed = 5;

// Background sliding position
static int level1_bgX = 0;
static int level1_distanceCovered = 0;
#define TARGET_DISTANCE 1500 // Run this far in total to make doors appear
#define COMBAT_TRIGGER_DISTANCE (TARGET_DISTANCE / 2) // Enemy appears halfway there

// Animation
static bool level1_isMoving = false;
static int level1_animFrame = 0;
static int level1_animTimer = 0;
#define ANIM_FRAME_DELAY 6

// Energy & Game State
static int level1_energy = 100;
static bool level1_gameOver = false;
static bool level1_keyFound = false;

// Result page - shown right after clicking a door
static bool level1_resultPage = false;

// ---------------- COMBAT ----------------
#define ENEMY_MAX_ENERGY      100
#define PLAYER_ATTACK_DAMAGE  15   // how much you deal per SPACE press
#define ENEMY_ATTACK_DAMAGE   10   // how much the enemy deals per hit
#define ATTACK_COOLDOWN_FRAMES 15  // prevents holding SPACE from spamming
#define ENEMY_ATTACK_INTERVAL  45  // enemy attacks roughly every 45 fixedUpdate ticks

static bool level1_combatActive = false; // true while the fight screen is showing
static bool level1_combatDone = false;   // true once this enemy has been beaten (so it can't retrigger)
static int level1_enemyEnergy = ENEMY_MAX_ENERGY;
static int level1_attackCooldown = 0;
static int level1_enemyAttackTimer = 0;

// Doors
struct Level1Door {
	int x, y, width, height;
	bool visited;
};

#define DOOR_WIDTH  120
#define DOOR_HEIGHT 180
#define DOOR_GAP    60
#define DOORS_START_X ((SCREEN_WIDTH - (3*DOOR_WIDTH + 2*DOOR_GAP)) / 2)
#define DOOR_Y 100

static Level1Door level1_doors[3];
static int level1_correctPath;
static int level1_chosenPath = -1;
static bool level1_doorsVisible = false;

inline void setupLevel1()
{
	srand((unsigned int)time(0));
	level1_correctPath = rand() % 3;

	level1_doors[0] = { DOORS_START_X, DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };
	level1_doors[1] = { DOORS_START_X + (DOOR_WIDTH + DOOR_GAP), DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };
	level1_doors[2] = { DOORS_START_X + 2 * (DOOR_WIDTH + DOOR_GAP), DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };

	level1_playerX = 100;
	level1_playerY = 80;
	level1_bgX = 0;
	level1_distanceCovered = 0;
	level1_doorsVisible = false;
	level1_resultPage = false;

	level1_energy = 100;
	level1_keyFound = false;
	level1_gameOver = false;
	level1_chosenPath = -1;
	level1_isMoving = false;
	level1_animFrame = 0;
	level1_animTimer = 0;

	level1_combatActive = false;
	level1_combatDone = false;
	level1_enemyEnergy = ENEMY_MAX_ENERGY;
	level1_attackCooldown = 0;
	level1_enemyAttackTimer = 0;
}

// The energy bar is drawn the same way on every page, so it lives
// in its own function instead of being copy-pasted everywhere.
inline void level1_drawEnergyBar()
{
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(0, 200, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level1_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");
}

inline void renderLevel1()
{
	static int desertBg = -1, doorClosedImg = -1, doorOpenImg = -1, idleImg = -1;
	static int runFrames[8];

	if (desertBg == -1) {
		desertBg = iLoadImage("Image/desert1.png");
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
	}

	// ---------------- RESULT PAGE (after a door click) ----------------
	if (level1_resultPage) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);

		int bigW = 300, bigH = 450;
		iShowImage(SCREEN_WIDTH / 2 - bigW / 2, 80, bigW, bigH, doorOpenImg);

		iSetColor(0, 0, 0);
		if (level1_keyFound) {
			iText(SCREEN_WIDTH / 2 - 110, 550, "KEY FOUND! Level complete!");
		}
		else if (level1_gameOver) {
			iText(SCREEN_WIDTH / 2 - 130, 550, "GAME OVER - Press R to restart");
		}
		else {
			iText(SCREEN_WIDTH / 2 - 160, 550, "Nothing here... an enemy attacked! -40 Energy");
			iText(SCREEN_WIDTH / 2 - 110, 40, "Click anywhere to go back");
		}

		level1_drawEnergyBar();
		return;
	}

	// ---------------- COMBAT SCREEN ----------------
	if (level1_combatActive) {
		// Static background, no scrolling during the fight
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);

		// Player on the left
		iShowImage(150, 80, level1_playerWidth, level1_playerHeight, idleImg);

		// Enemy on the right - TODO: replace this placeholder box with a
		// real enemy sprite the same way you did for the player/doors:
		//   static int enemyImg = -1;
		//   if (enemyImg == -1) enemyImg = iLoadImage("Image/enemy.png");
		//   iShowImage(SCREEN_WIDTH - 300, 80, 150, 180, enemyImg);
		iSetColor(150, 30, 30);
		iFilledRectangle(SCREEN_WIDTH - 300, 80, 150, 180);
		iSetColor(0, 0, 0);
		iRectangle(SCREEN_WIDTH - 300, 80, 150, 180);
		iText(SCREEN_WIDTH - 270, 250, "ENEMY");

		// Player energy bar (bottom-left, same as always)
		level1_drawEnergyBar();

		// Enemy energy bar (top-right, mirrored style)
		iSetColor(200, 200, 200);
		iFilledRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, 200, 20);
		iSetColor(200, 0, 0);
		iFilledRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, 2 * level1_enemyEnergy, 20);
		iSetColor(0, 0, 0);
		iRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, 200, 20);
		iText(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 55, "Enemy");

		iSetColor(0, 0, 0);
		iText(SCREEN_WIDTH / 2 - 100, 30, "Press SPACE to attack!");
		return;
	}

	// ---------------- NORMAL SCREEN (running or door screen) ----------------
	iShowImage(level1_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);
	iShowImage(level1_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);
	iShowImage(level1_bgX - SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);

	int playerImg = level1_isMoving ? runFrames[level1_animFrame] : idleImg;
	iShowImage(level1_playerX, level1_playerY, level1_playerWidth, level1_playerHeight, playerImg);

	if (level1_doorsVisible) {
		for (int i = 0; i < 3; i++) {
			int imgToUse = level1_doors[i].visited ? doorOpenImg : doorClosedImg;
			iShowImage(level1_doors[i].x, level1_doors[i].y, level1_doors[i].width, level1_doors[i].height, imgToUse);
		}
	}

	level1_drawEnergyBar();

	iSetColor(0, 0, 0);
	if (!level1_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT - 40, "Hold RIGHT to run, LEFT to go back");
	}
	else if (!level1_gameOver && !level1_keyFound) {
		iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT - 40, "Click a door to open it");
	}
}

inline void level1_fixedUpdate()
{
	if (level1_gameOver || level1_keyFound || level1_resultPage) return;

	// ---------------- COMBAT LOGIC ----------------
	if (level1_combatActive) {
		if (level1_attackCooldown > 0) level1_attackCooldown--;

		// Player attacks on SPACE, limited by cooldown
		if (isKeyPressed(' ') && level1_attackCooldown <= 0) {
			level1_enemyEnergy -= PLAYER_ATTACK_DAMAGE;
			if (level1_enemyEnergy < 0) level1_enemyEnergy = 0;
			level1_attackCooldown = ATTACK_COOLDOWN_FRAMES;
		}

		// Enemy attacks automatically on its own timer
		level1_enemyAttackTimer++;
		if (level1_enemyAttackTimer >= ENEMY_ATTACK_INTERVAL) {
			level1_enemyAttackTimer = 0;
			level1_energy -= ENEMY_ATTACK_DAMAGE;
			if (level1_energy < 0) level1_energy = 0;
		}

		// Check outcome
		if (level1_enemyEnergy <= 0) {
			level1_combatActive = false;
			level1_combatDone = true; // won't fight again this level
		}
		else if (level1_energy <= 0) {
			level1_gameOver = true;
		}

		return; // skip running logic while fighting
	}

	// ---------------- RUNNING LOGIC ----------------
	level1_isMoving = false;

	if (!level1_doorsVisible) {
		if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
			level1_isMoving = true;

			level1_bgX -= level1_playerSpeed;
			if (level1_bgX <= -SCREEN_WIDTH) {
				level1_bgX = 0;
			}

			level1_distanceCovered += level1_playerSpeed;

			// Trigger the fight once, on the way to the doors
			if (!level1_combatDone && level1_distanceCovered >= COMBAT_TRIGGER_DISTANCE) {
				level1_combatActive = true;
				level1_enemyEnergy = ENEMY_MAX_ENERGY;
				level1_attackCooldown = 0;
				level1_enemyAttackTimer = 0;
			}

			if (level1_distanceCovered >= TARGET_DISTANCE) {
				level1_doorsVisible = true;
			}
		}
		else if (isSpecialKeyPressed(GLUT_KEY_LEFT) && level1_distanceCovered > 0) {
			level1_isMoving = true;

			level1_bgX += level1_playerSpeed;
			if (level1_bgX >= SCREEN_WIDTH) {
				level1_bgX = 0;
			}

			level1_distanceCovered -= level1_playerSpeed;
			if (level1_distanceCovered < 0) {
				level1_distanceCovered = 0;
			}
		}
	}

	if (level1_isMoving) {
		level1_animTimer++;
		if (level1_animTimer >= ANIM_FRAME_DELAY) {
			level1_animTimer = 0;
			level1_animFrame = (level1_animFrame + 1) % 8;
		}
	}
	else {
		level1_animFrame = 0;
		level1_animTimer = 0;
	}
}

inline bool level1_isInside(int px, int py, Level1Door d)
{
	return (px >= d.x && px <= d.x + d.width && py >= d.y && py <= d.y + d.height);
}

inline void handleLevel1DoorClicks(int mx, int my)
{
	if (level1_gameOver || level1_keyFound) return;

	if (level1_resultPage) {
		level1_resultPage = false;
		return;
	}

	if (!level1_doorsVisible) return;

	for (int i = 0; i < 3; i++) {
		if (!level1_doors[i].visited && level1_isInside(mx, my, level1_doors[i])) {
			level1_doors[i].visited = true;
			level1_chosenPath = i;
			level1_resultPage = true;

			if (i == level1_correctPath) {
				level1_keyFound = true;
			}
			else {
				level1_energy -= 40;
				if (level1_energy <= 0) {
					level1_energy = 0;
					level1_gameOver = true;
				}
			}
		}
	}
}

inline void handleLevel1Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R')
		setupLevel1();
}

#endif