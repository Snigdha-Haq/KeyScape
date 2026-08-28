#ifndef LEVEL2_H_INCLUDED
#define LEVEL2_H_INCLUDED

#include <cstdlib>
#include <ctime>
#include <cstdio>


#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

// ---------------- BASIC LEVEL STATE ----------------
static int level2_playerX = 100, level2_playerY = 80;
static int level2_playerWidth = 90, level2_playerHeight = 130;
static int level2_playerSpeed = 5;

// Background sliding position
static int level2_bgX = 0;
static int level2_distanceCovered = 0;
#define LEVEL2_TARGET_DISTANCE 1500 // Run 1500 pixels to make doors appear

// Animation
static bool level2_isMoving = false;
static int level2_animFrame = 0;
static int level2_animTimer = 0;
#define LEVEL2_ANIM_FRAME_DELAY 6

// Energy & Game State
static int level2_energy = 100;
static bool level2_gameOver = false;
static bool level2_keyFound = false;

// Doors
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
static int level2_correctPath;
static int level2_chosenPath = -1;
static bool level2_doorsVisible = false; // Simple toggle

inline void setupLevel2()
{
	srand((unsigned int)time(0));
	level2_correctPath = rand() % 4;

	level2_doors[0] = { LEVEL2_DOORS_START_X, LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[1] = { LEVEL2_DOORS_START_X + (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[2] = { LEVEL2_DOORS_START_X + 2 * (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };
	level2_doors[3] = { LEVEL2_DOORS_START_X + 3 * (LEVEL2_DOOR_WIDTH + LEVEL2_DOOR_GAP), LEVEL2_DOOR_Y, LEVEL2_DOOR_WIDTH, LEVEL2_DOOR_HEIGHT, false };

	level2_playerX = 100;
	level2_playerY = 80;
	level2_bgX = 0;
	level2_distanceCovered = 0;
	level2_doorsVisible = false;

	level2_energy = 100;
	level2_keyFound = false;
	level2_gameOver = false;
	level2_chosenPath = -1;
	level2_isMoving = false;
	level2_animFrame = 0;
	level2_animTimer = 0;
}

inline void renderLevel2()
{
	static int UnderSeaBg = -1, doorClosedImg = -1, doorOpenImg = -1, idleImg = -1;
	static int runFrames[8];

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
	}

	// 1. Draw 2 seamless backgrounds sliding left
	iShowImage(level2_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
	iShowImage(level2_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);

	// 2. Draw Player
	int playerImg = level2_isMoving ? runFrames[level2_animFrame] : idleImg;
	iShowImage(level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight, playerImg);

	// 3. Draw Doors only when player has run far enough
	if (level2_doorsVisible) {
		for (int i = 0; i < 4; i++) {
			int imgToUse = level2_doors[i].visited ? doorOpenImg : doorClosedImg;
			iShowImage(level2_doors[i].x, level2_doors[i].y, level2_doors[i].width, level2_doors[i].height, imgToUse);

			if (i == level2_chosenPath && level2_keyFound) {
				iSetColor(255, 215, 0);
				iText(level2_doors[i].x + 30, level2_doors[i].y + level2_doors[i].height + 10, "KEY!");
			}
		}
	}

	// 4. Energy Bar & Instructions
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(0, 200, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level2_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	iSetColor(0, 0, 0);
	if (!level2_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 40, "Hold RIGHT ARROW to run!");
	}
	else if (!level2_gameOver && !level2_keyFound) {
		iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT - 40, "Click a door to open it");
	}

	if (level2_gameOver)
		iText(SCREEN_WIDTH / 2 - 130, 300, "GAME OVER - Press R to restart");
	else if (level2_keyFound)
		iText(SCREEN_WIDTH / 2 - 110, 300, "KEY FOUND! Level complete!");
}

inline void level2_fixedUpdate()
{
	if (level2_gameOver || level2_keyFound) return;

	level2_isMoving = false;

	// While doors haven't appeared, holding RIGHT scrolls the background smooth and endless
	if (!level2_doorsVisible && isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level2_isMoving = true;

		// Slide background left
		level2_bgX -= level2_playerSpeed;

		// Loop background back when full width leaves screen
		if (level2_bgX <= -SCREEN_WIDTH) {
			level2_bgX = 0;
		}

		// Keep track of how far player ran
		level2_distanceCovered += level2_playerSpeed;

		// When target distance is hit, stop scrolling and reveal doors!
		if (level2_distanceCovered >= LEVEL2_TARGET_DISTANCE) {
			level2_doorsVisible = true;
		}
	}

	// Character Running Animation
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

inline bool level2_isInside(int px, int py, Level2Door d)
{
	return (px >= d.x && px <= d.x + d.width && py >= d.y && py <= d.y + d.height);
}

inline void handleLevel2DoorClicks(int mx, int my)
{
	if (!level2_doorsVisible || level2_gameOver || level2_keyFound) return;

	for (int i = 0; i < 4; i++) {
		if (!level2_doors[i].visited && level2_isInside(mx, my, level2_doors[i])) {
			level2_doors[i].visited = true;
			level2_chosenPath = i;

			if (i == level2_correctPath) {
				level2_keyFound = true;
			}
			else {
				level2_energy -= 40;
				if (level2_energy <= 0) {
					level2_energy = 0;
					level2_gameOver = true;
				}
			}
		}
	}
}

inline void handleLevel2Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R')
		setupLevel2();
}

#endif
