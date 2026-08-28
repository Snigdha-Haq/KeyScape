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
#define TARGET_DISTANCE 1500 // Run 1500 pixels to make doors appear

// Animation
static bool level1_isMoving = false;
static int level1_animFrame = 0;
static int level1_animTimer = 0;
#define ANIM_FRAME_DELAY 6

// Energy & Game State
static int level1_energy = 100;
static bool level1_gameOver = false;
static bool level1_keyFound = false;

// ---------------- ENEMY (drawn with iGraphics primitives) ----------------
static int level1_enemyX = 600, level1_enemyY = 80;
static int level1_enemyWidth = 80, level1_enemyHeight = 130;
static int level1_enemyHealth = 100;
static bool level1_enemyEncountered = false; // becomes true once player reaches the enemy
static bool level1_enemyDefeated = false;
static int level1_enemyAttackTimer = 0;

#define ENEMY_TRIGGER_DISTANCE 800   // player must run this far before the enemy blocks the path
#define ENEMY_MAX_HEALTH 100
#define ENEMY_ATTACK_DAMAGE 10
#define ENEMY_ATTACK_DELAY 60        // frames between enemy attacks (~1.2s at 20ms timer)
#define PLAYER_ATTACK_DAMAGE 20      // damage dealt per SPACE press

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
static bool level1_doorsVisible = false; // Simple toggle

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

	level1_energy = 100;
	level1_keyFound = false;
	level1_gameOver = false;
	level1_chosenPath = -1;
	level1_isMoving = false;
	level1_animFrame = 0;
	level1_animTimer = 0;

	// Reset enemy
	level1_enemyHealth = ENEMY_MAX_HEALTH;
	level1_enemyEncountered = false;
	level1_enemyDefeated = false;
	level1_enemyAttackTimer = 0;
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

	// 1. Draw 2 seamless backgrounds sliding left
	iShowImage(level1_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);
	iShowImage(level1_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, desertBg);

	// 2. Draw Player
	int playerImg = level1_isMoving ? runFrames[level1_animFrame] : idleImg;
	iShowImage(level1_playerX, level1_playerY, level1_playerWidth, level1_playerHeight, playerImg);

	// 3. Draw Enemy (plain iGraphics shapes, no image asset needed)
	if (level1_enemyEncountered && !level1_enemyDefeated) {
		// body
		iSetColor(120, 0, 0);
		iFilledRectangle(level1_enemyX, level1_enemyY, level1_enemyWidth, level1_enemyHeight - 35);
		// arms
		iSetColor(90, 0, 0);
		iFilledRectangle(level1_enemyX - 12, level1_enemyY + 40, 12, 50);
		iFilledRectangle(level1_enemyX + level1_enemyWidth, level1_enemyY + 40, 12, 50);
		// head
		iSetColor(200, 30, 30);
		iFilledCircle(level1_enemyX + level1_enemyWidth / 2, level1_enemyY + level1_enemyHeight - 15, 22);
		// angry eyes
		iSetColor(255, 255, 0);
		iFilledCircle(level1_enemyX + level1_enemyWidth / 2 - 8, level1_enemyY + level1_enemyHeight - 15, 3);
		iFilledCircle(level1_enemyX + level1_enemyWidth / 2 + 8, level1_enemyY + level1_enemyHeight - 15, 3);

		// enemy health bar
		int barW = level1_enemyWidth + 20;
		iSetColor(200, 200, 200);
		iFilledRectangle(level1_enemyX - 10, level1_enemyY + level1_enemyHeight + 10, barW, 12);
		iSetColor(255, 0, 0);
		iFilledRectangle(level1_enemyX - 10, level1_enemyY + level1_enemyHeight + 10, barW * level1_enemyHealth / ENEMY_MAX_HEALTH, 12);
		iSetColor(0, 0, 0);
		iRectangle(level1_enemyX - 10, level1_enemyY + level1_enemyHeight + 10, barW, 12);
	}

	// 4. Draw Doors only when player has run far enough
	if (level1_doorsVisible) {
		for (int i = 0; i < 3; i++) {
			int imgToUse = level1_doors[i].visited ? doorOpenImg : doorClosedImg;
			iShowImage(level1_doors[i].x, level1_doors[i].y, level1_doors[i].width, level1_doors[i].height, imgToUse);

			if (i == level1_chosenPath && level1_keyFound) {
				iSetColor(255, 215, 0);
				iText(level1_doors[i].x + 30, level1_doors[i].y + level1_doors[i].height + 10, "KEY!");
			}
		}
	}

	// 5. Energy Bar & Instructions
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(0, 200, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level1_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	iSetColor(0, 0, 0);
	if (level1_enemyEncountered && !level1_enemyDefeated && !level1_gameOver) {
		iText(SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT - 40, "ENEMY! Press SPACE to attack!");
	}
	else if (!level1_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 40, "Hold RIGHT ARROW to run!");
	}
	else if (!level1_gameOver && !level1_keyFound) {
		iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT - 40, "Click a door to open it");
	}

	if (level1_gameOver)
		iText(SCREEN_WIDTH / 2 - 130, 300, "GAME OVER - Press R to restart");
	else if (level1_keyFound)
		iText(SCREEN_WIDTH / 2 - 110, 300, "KEY FOUND! Level complete!");
}

inline void level1_fixedUpdate()
{
	if (level1_gameOver || level1_keyFound) return;

	level1_isMoving = false;

	// Enemy attacks the player automatically while the fight is ongoing
	if (level1_enemyEncountered && !level1_enemyDefeated) {
		level1_enemyAttackTimer++;
		if (level1_enemyAttackTimer >= ENEMY_ATTACK_DELAY) {
			level1_enemyAttackTimer = 0;
			level1_energy -= ENEMY_ATTACK_DAMAGE;
			if (level1_energy <= 0) {
				level1_energy = 0;
				level1_gameOver = true;
			}
		}
	}

	// While doors haven't appeared, holding RIGHT scrolls the background smooth and endless
	// Movement is blocked while an undefeated enemy is in front of the player
	bool blockedByEnemy = level1_enemyEncountered && !level1_enemyDefeated;

	if (!level1_doorsVisible && !blockedByEnemy && isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level1_isMoving = true;

		// Slide background left
		level1_bgX -= level1_playerSpeed;

		// Loop background back when full width leaves screen
		if (level1_bgX <= -SCREEN_WIDTH) {
			level1_bgX = 0;
		}

		// Keep track of how far player ran
		level1_distanceCovered += level1_playerSpeed;

		// Enemy blocks the path once the trigger distance is reached
		if (!level1_enemyEncountered && level1_distanceCovered >= ENEMY_TRIGGER_DISTANCE) {
			level1_enemyEncountered = true;
		}

		// When target distance is hit, stop scrolling and reveal doors!
		if (level1_distanceCovered >= TARGET_DISTANCE) {
			level1_doorsVisible = true;
		}
	}

	// Character Running Animation
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
	if (!level1_doorsVisible || level1_gameOver || level1_keyFound) return;

	for (int i = 0; i < 3; i++) {
		if (!level1_doors[i].visited && level1_isInside(mx, my, level1_doors[i])) {
			level1_doors[i].visited = true;
			level1_chosenPath = i;

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

	// Attack the enemy with SPACE while the fight is active
	if (key == ' ' && level1_enemyEncountered && !level1_enemyDefeated && !level1_gameOver) {
		level1_enemyHealth -= PLAYER_ATTACK_DAMAGE;
		if (level1_enemyHealth <= 0) {
			level1_enemyHealth = 0;
			level1_enemyDefeated = true;
		}
	}
}

#endif
