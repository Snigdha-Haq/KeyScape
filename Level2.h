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
static int level2_playerX = 100, level2_playerY = 80;
static int level2_playerWidth = 90, level2_playerHeight = 130;
static int level2_playerSpeed = 5;
static int level2_groundY = 80; // player's resting (non-jump) Y

// Background sliding position
static int level2_bgX = 0;
static int level2_distanceCovered = 0;
// NOTE: increased so the player has enough time to collect ALL keys
// before the doors show up. (A key spawned at the far end still needs
// roughly ~830px of on-screen travel to reach the player, so the target
// distance must be bigger than the last key's spawn distance + that gap.)
#define LEVEL2_TARGET_DISTANCE 2500

// Animation
static bool level2_isMoving = false;
static int level2_animFrame = 0;
static int level2_animTimer = 0;
#define LEVEL2_ANIM_FRAME_DELAY 6

// Jump (used to dodge ground obstacles / difficulties)
static bool level2_isJumping = false;
static float level2_jumpVelocity = 0.0f;
#define LEVEL2_JUMP_STRENGTH -14.0f
#define LEVEL2_GRAVITY 0.9f
// which of the 3 jump images to show: 0 = launching up, 1 = near the peak, 2 = falling down
static int level2_jumpFrameIndex = 0;

// Energy & Game State
static int level2_energy = 100;
static bool level2_gameOver = false;
static bool level2_keyFound = false; // now means: the REAL key was found -> level complete

// ---------------- KEYS (4 collectibles, drawn with iDraw shapes) ----------------
#define LEVEL2_NUM_KEYS 4
#define LEVEL2_KEY_SIZE 40

struct Level2Key {
	float x;
	int y, size;
	int id;          // 0..3
	bool spawned;     // has this key appeared yet in the run
	bool active;      // currently visible/collidable on screen
	bool collected;   // picked up by player
};

static Level2Key level2_keys[LEVEL2_NUM_KEYS];

// Distance (in pixels run) at which each key appears.
// Spread out and kept well before LEVEL2_TARGET_DISTANCE so the player
// always has time to grab every key before the doors appear.
static int level2_keySpawnDistance[LEVEL2_NUM_KEYS] = { 200, 650, 1100, 1550 };

// One distinct color per key id (used for the HUD icon)
static int level2_keyColor[LEVEL2_NUM_KEYS][3] = {
	{ 255, 215, 0 },   // Key 0 - Gold
	{ 65, 145, 220 },  // Key 1 - Blue
	{ 60, 200, 90 },   // Key 2 - Green
	{ 225, 60, 60 }    // Key 3 - Red
};

static bool level2_keyCollected[LEVEL2_NUM_KEYS] = { false, false, false, false };

// ---------------- OBSTACLES / DIFFICULTIES (must jump over) - drawn as red crabs ----------------
#define LEVEL2_NUM_OBSTACLES 5

struct Level2Obstacle {
	float x;
	int y, width, height;
	bool spawned;
	bool active;
	bool hit; // energy already deducted for this one
};

static Level2Obstacle level2_obstacles[LEVEL2_NUM_OBSTACLES];
static int level2_obstacleSpawnDistance[LEVEL2_NUM_OBSTACLES] = { 100, 400, 800, 1200, 1600 };
#define LEVEL2_OBSTACLE_WIDTH 50
#define LEVEL2_OBSTACLE_DAMAGE 15

// ---------------- DOORS ----------------
struct Level2Door {
	int x, y, width, height;
	bool visited; // true = already opened (real key claimed OR wrong-door task solved)
};

#define LEVEL2_DOOR_WIDTH  120
#define LEVEL2_DOOR_HEIGHT 180
#define LEVEL2_DOOR_GAP    60
#define LEVEL2_DOORS_START_X ((SCREEN_WIDTH - (4*LEVEL2_DOOR_WIDTH + 3*LEVEL2_DOOR_GAP)) / 2)
#define LEVEL2_DOOR_Y 100

static Level2Door level2_doors[4];
static bool level2_doorsVisible = false; // Simple toggle

// ---- REAL KEY / MINI-GAME SYSTEM ----
// Exactly one door (random every game) actually contains the real key.
// The other 3 doors each get one different mini-game task.
static int level2_realKeyDoor = -1;

enum Level2TaskType { LEVEL2_TASK_NONE = 0, LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
static int level2_doorTaskType[4];

// Distinct "room" background shown behind each door's mini-game
static int level2_doorRoomBgId[4];

// Are we currently "inside" a door solving its task?
static bool level2_insideTask = false;
static int level2_currentTaskDoor = -1;

// --- Math (addition) task state ---
static int level2_mathA = 0, level2_mathB = 0;
static int level2_mathOptions[4];
static int level2_mathCorrectIndex = 0;

// --- Puzzle (missing number in sequence) task state ---
static int level2_puzzleSeq[4];
static int level2_puzzleBlankIndex = 0;
static int level2_puzzleStep = 0;
static int level2_puzzleOptions[4];
static int level2_puzzleCorrectIndex = 0;

// --- Color match task state ---
static int level2_colorTarget[3];
static int level2_colorOptions[4][3];
static int level2_colorCorrectIndex = 0;

// Option-box layout used by all 3 mini-games (4 clickable choices)
#define LEVEL2_OPT_W   150
#define LEVEL2_OPT_H   70
#define LEVEL2_OPT_GAP 30
#define LEVEL2_OPT_Y   330
#define LEVEL2_WRONG_TASK_PENALTY 8

// Small on-screen feedback message
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
	level2_puzzleStep = 2 + rand() % 4;         // step of 2..5
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

	level2_playerX = 100;
	level2_playerY = level2_groundY;
	level2_bgX = 0;
	level2_distanceCovered = 0;
	level2_doorsVisible = false;

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

	for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
		level2_keys[i].id = i;
		level2_keys[i].spawned = false;
		level2_keys[i].active = false;
		level2_keys[i].collected = false;
		level2_keys[i].size = LEVEL2_KEY_SIZE;
		level2_keyCollected[i] = false;
	}

	for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
		level2_obstacles[i].spawned = false;
		level2_obstacles[i].active = false;
		level2_obstacles[i].hit = false;
		level2_obstacles[i].width = LEVEL2_OBSTACLE_WIDTH;
		level2_obstacles[i].height = 28 + (rand() % 3) * 6; // a little variety in crab size
	}

	// ---- Randomly decide which door has the REAL key, and randomly
	//      assign the 3 mini-game types to the other 3 doors ----
	level2_realKeyDoor = rand() % 4;
	{
		int types[3] = { LEVEL2_TASK_MATH, LEVEL2_TASK_PUZZLE, LEVEL2_TASK_COLOR };
		for (int i = 2; i > 0; i--) {          // small Fisher-Yates shuffle
			int j = rand() % (i + 1);
			int tmp = types[i]; types[i] = types[j]; types[j] = tmp;
		}
		int ti = 0;
		for (int i = 0; i < 4; i++) {
			if (i == level2_realKeyDoor) level2_doorTaskType[i] = LEVEL2_TASK_NONE;
			else level2_doorTaskType[i] = types[ti++];
			level2_doorRoomBgId[i] = i; // room-bg index, matches doorRoomBg[] loaded in render
		}
	}
	level2_insideTask = false;
	level2_currentTaskDoor = -1;
}

// Generic AABB overlap check (float x so keys/obstacles can move smoothly)
inline bool level2_rectOverlap(float ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
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
	iFilledCircle(x + size * 0.3f, y + size * 0.3f, size * 0.28f);   // key head
	iSetColor(255, 255, 255);
	iFilledCircle(x + size * 0.3f, y + size * 0.3f, size * 0.12f);   // hole in the head
	iSetColor(r, g, b);
	iFilledRectangle(x + size * 0.28f, y + size * 0.22f, size * 0.55f, size * 0.16f); // shaft
	iFilledRectangle(x + size * 0.68f, y + size * 0.05f, size * 0.1f, size * 0.17f);  // tooth 1
	iFilledRectangle(x + size * 0.84f, y + size * 0.05f, size * 0.1f, size * 0.22f);  // tooth 2
	iSetColor(0, 0, 0);
	iCircle(x + size * 0.3f, y + size * 0.3f, size * 0.28f);
}

// Obstacle now drawn as a little red crab (pure iDraw shapes, no images)
inline void level2_drawCrab(float x, int y, int w, int h)
{
	float cx = x + w / 2.0f;
	float cy = y + h / 2.0f;

	// legs (drawn first so the body sits on top of them)
	iSetColor(150, 20, 20);
	for (int side = -1; side <= 1; side += 2) {
		iLine((int)(cx + side * w * 0.10f), (int)(cy), (int)(cx + side * w * 0.55f), (int)(cy - h * 0.35f));
		iLine((int)(cx + side * w * 0.10f), (int)(cy + h * 0.10f), (int)(cx + side * w * 0.60f), (int)(cy + h * 0.05f));
		iLine((int)(cx + side * w * 0.10f), (int)(cy + h * 0.20f), (int)(cx + side * w * 0.55f), (int)(cy + h * 0.45f));
	}

	// claws
	iSetColor(220, 40, 40);
	iFilledCircle(cx - w * 0.48f, cy, w * 0.16f);
	iFilledCircle(cx + w * 0.48f, cy, w * 0.16f);
	iSetColor(150, 20, 20);
	iCircle(cx - w * 0.48f, cy, w * 0.16f);
	iCircle(cx + w * 0.48f, cy, w * 0.16f);

	// body
	iSetColor(220, 40, 40);
	iFilledCircle(cx, cy, w * 0.36f);
	iSetColor(150, 20, 20);
	iCircle(cx, cy, w * 0.36f);

	// eye stalks
	iSetColor(220, 40, 40);
	iFilledRectangle(cx - w * 0.18f, cy + h * 0.15f, 4, 12);
	iFilledRectangle(cx + w * 0.14f, cy + h * 0.15f, 4, 12);
	iSetColor(255, 255, 255);
	iFilledCircle(cx - w * 0.16f, cy + h * 0.30f, 4);
	iFilledCircle(cx + w * 0.16f, cy + h * 0.30f, 4);
	iSetColor(0, 0, 0);
	iFilledCircle(cx - w * 0.16f, cy + h * 0.30f, 2);
	iFilledCircle(cx + w * 0.16f, cy + h * 0.30f, 2);
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
	static int doorRoomBg[4];
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

		// 3 jump-pose images: 0 = launch (crouch/push-off), 1 = peak (legs tucked),
		// 2 = falling/landing prep. Save your 3 chosen frames with these names.
		jumpFrames[0] = iLoadImage("Image/jump_1.png");
		jumpFrames[1] = iLoadImage("Image/jump_2.png");
		jumpFrames[2] = iLoadImage("Image/jump_3.png");

		// NEW: if any jump image failed to load, don't let the player vanish -
		// fall back to the idle sprite while still doing the jump physics.
		jumpImagesOK = (jumpFrames[0] >= 0 && jumpFrames[1] >= 0 && jumpFrames[2] >= 0);

		// 4 distinct backgrounds shown behind each door once it is opened
		doorRoomBg[0] = iLoadImage("Image/DoorBg1.png");
		doorRoomBg[1] = iLoadImage("Image/DoorBg2.png");
		doorRoomBg[2] = iLoadImage("Image/DoorBg3.png");
		doorRoomBg[3] = iLoadImage("Image/DoorBg4.png");
	}

	// 1. Draw 2 seamless backgrounds sliding left (running phase) OR the
	//    room background of the door currently being solved
	if (level2_doorsVisible && level2_insideTask && level2_currentTaskDoor >= 0) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, doorRoomBg[level2_currentTaskDoor]);
	}
	else {
		iShowImage(level2_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
		iShowImage(level2_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnderSeaBg);
	}

	// 2 & 3. Obstacles (crabs) and keys - only during the running phase
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (level2_obstacles[i].active) {
				level2_drawCrab(level2_obstacles[i].x, level2_obstacles[i].y,
					level2_obstacles[i].width, level2_obstacles[i].height);
			}
		}

		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (level2_keys[i].active && !level2_keys[i].collected) {
				level2_drawKey(level2_keys[i].x, level2_keys[i].y, level2_keys[i].size,
					level2_keyColor[i][0], level2_keyColor[i][1], level2_keyColor[i][2]);
			}
		}
	}

	// 4. Draw Player (only during the running phase - once at the doors, player sprite is hidden)
	if (!level2_doorsVisible) {
		int playerImg;
		if (level2_isJumping) playerImg = jumpImagesOK ? jumpFrames[level2_jumpFrameIndex] : idleImg; // NEW fallback
		else if (level2_isMoving) playerImg = runFrames[level2_animFrame];
		else playerImg = idleImg;

		iShowImage(level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight, playerImg);
	}

	// 5. Doors screen
	if (level2_doorsVisible) {
		if (!level2_insideTask) {
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
		else {
			// ---- Mini-game UI for the door currently being solved ----
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

	// 6. Keys-collected HUD (top-left) - only meaningful during running phase but kept visible
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

	// 7. Energy Bar & Instructions
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(0, 200, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level2_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	iSetColor(0, 0, 0);
	if (!level2_doorsVisible) {
		iText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 40, "Hold RIGHT to run, SPACE to jump obstacles!");
	}
	else if (!level2_insideTask && !level2_gameOver && !level2_keyFound) {
		iText(SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT - 40, "Ekta dorjaay ASHOL chabi ache - beche nao!");
	}

	// 8. Feedback message
	if (level2_messageTimer > 0) {
		iSetColor(220, 20, 20);
		iText(SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT - 70, level2_message);
	}

	if (level2_gameOver)
		iText(SCREEN_WIDTH / 2 - 130, 480, "GAME OVER - Press R to restart");
	else if (level2_keyFound)
		iText(SCREEN_WIDTH / 2 - 130, 480, "ASHOL CHABI PAWA GECHE! Level Complete!");
}

inline void level2_fixedUpdate()
{
	if (level2_gameOver || level2_keyFound) return;

	if (level2_messageTimer > 0) level2_messageTimer--;

	level2_isMoving = false;

	// While doors haven't appeared, holding RIGHT scrolls the background smooth and endless
	if (!level2_doorsVisible && isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		level2_isMoving = true;

		level2_bgX -= level2_playerSpeed;
		if (level2_bgX <= -SCREEN_WIDTH) {
			level2_bgX = 0;
		}

		level2_distanceCovered += level2_playerSpeed;

		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (!level2_keys[i].spawned && level2_distanceCovered >= level2_keySpawnDistance[i]) {
				level2_keys[i].spawned = true;
				level2_keys[i].active = true;
				level2_keys[i].x = (float)SCREEN_WIDTH + 20;
				level2_keys[i].y = level2_groundY + 35;
			}
		}

		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].spawned && level2_distanceCovered >= level2_obstacleSpawnDistance[i]) {
				level2_obstacles[i].spawned = true;
				level2_obstacles[i].active = true;
				level2_obstacles[i].x = (float)SCREEN_WIDTH + 20;
				level2_obstacles[i].y = level2_groundY + level2_playerHeight - level2_obstacles[i].height;
			}
		}

		if (level2_distanceCovered >= LEVEL2_TARGET_DISTANCE) {
			level2_doorsVisible = true;
		}
	}

	// Jump physics (usable while running, to dodge obstacles)
	if (level2_isJumping) {
		level2_playerY += (int)lroundf(level2_jumpVelocity);   // CHANGED: round instead of truncate
		level2_jumpVelocity += LEVEL2_GRAVITY;

		// Pick the jump sprite based on vertical velocity:
		// strongly negative = still launching upward, near zero = at the peak,
		// positive = falling back down.
		if (level2_jumpVelocity < -4.0f) level2_jumpFrameIndex = 0;
		else if (level2_jumpVelocity <= 4.0f) level2_jumpFrameIndex = 1;
		else level2_jumpFrameIndex = 2;

		if (level2_playerY < 0) level2_playerY = 0; // NEW: don't let it fly off-screen

		if (level2_playerY >= level2_groundY) {
			level2_playerY = level2_groundY;   // back to the exact original resting position
			level2_isJumping = false;
			level2_jumpVelocity = 0.0f;
			level2_jumpFrameIndex = 0;
		}
	}

	// Move & resolve keys (collect on touch)
	if (!level2_doorsVisible) {
		for (int i = 0; i < LEVEL2_NUM_KEYS; i++) {
			if (!level2_keys[i].active) continue;

			level2_keys[i].x -= level2_playerSpeed;

			if (!level2_keys[i].collected &&
				level2_rectOverlap(level2_keys[i].x, level2_keys[i].y, level2_keys[i].size, level2_keys[i].size,
				level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight)) {
				level2_keys[i].collected = true;
				level2_keys[i].active = false;
				level2_keyCollected[level2_keys[i].id] = true;
			}

			if (level2_keys[i].x + level2_keys[i].size < 0) {
				level2_keys[i].active = false;
			}
		}

		for (int i = 0; i < LEVEL2_NUM_OBSTACLES; i++) {
			if (!level2_obstacles[i].active) continue;

			level2_obstacles[i].x -= level2_playerSpeed;

			if (!level2_obstacles[i].hit &&
				level2_rectOverlap(level2_obstacles[i].x, level2_obstacles[i].y, level2_obstacles[i].width, level2_obstacles[i].height,
				level2_playerX, level2_playerY, level2_playerWidth, level2_playerHeight)) {
				level2_obstacles[i].hit = true;
				level2_energy -= LEVEL2_OBSTACLE_DAMAGE;
				if (level2_energy <= 0) {
					level2_energy = 0;
					level2_gameOver = true;
				}
			}

			if (level2_obstacles[i].x + level2_obstacles[i].width < 0) {
				level2_obstacles[i].active = false;
			}
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

inline void handleLevel2DoorClicks(int mx, int my)
{
	if (level2_gameOver || level2_keyFound) return;
	if (!level2_doorsVisible) return;

	// ---- Currently solving a wrong-door mini-game: check the 4 option boxes ----
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
						level2_generateTaskForDoor(level2_currentTaskDoor); // fresh numbers/colors to try again
					}
				}
				return;
			}
		}
		return;
	}

	// ---- Outside: choosing which door to open ----
	for (int i = 0; i < 4; i++) {
		if (level2_isInside(mx, my, level2_doors[i])) {
			if (i == level2_realKeyDoor) {
				level2_doors[i].visited = true;
				level2_keyFound = true; // WIN - real key found
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

inline void handleLevel2Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R') {
		setupLevel2();
		return;
	}

	if (key == ' ') {
		if (level2_gameOver || level2_keyFound) return;
		if (level2_doorsVisible) return;
		if (level2_isJumping) return;

		level2_isJumping = true;
		level2_jumpVelocity = LEVEL2_JUMP_STRENGTH;
	}
}

#endif