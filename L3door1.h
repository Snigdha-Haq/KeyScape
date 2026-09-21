#ifndef L3DOOR1_H_INCLUDED
#define L3DOOR1_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <windows.h>

extern int level3_score;
extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

// Player Physics & Dimensions (Level 3 Exact)
static const float l3d1_groundY = 165.0f;
static float l3d1_playerX = 100.0f;
static float l3d1_playerY = 165.0f;

#define L3D1_PLAYER_NORMAL_W 90
#define L3D1_PLAYER_NORMAL_H 130
#define L3D1_PLAYER_SLIDE_W  120
#define L3D1_PLAYER_SLIDE_H  60

static int l3d1_playerWidth = L3D1_PLAYER_NORMAL_W;
static int l3d1_playerHeight = L3D1_PLAYER_NORMAL_H;

// Scroll & Player speed
static int l3d1_playerSpeed = 11;

// Slide mechanism
static bool l3d1_isSliding = false;
static int l3d1_slideTimer = 0;
#define L3D1_SLIDE_DURATION 28

// Jump physics
static bool l3d1_isJumping = false;
static float l3d1_jumpVelocity = 0.0f;
#define L3D1_JUMP_STRENGTH 16.0f
#define L3D1_GRAVITY 0.8f
static int l3d1_jumpFrameIndex = 0;

// Movement & Background Scroll
static bool l3d1_facingRight = true;
static bool l3d1_isMoving = false;
static int l3d1_bgX = 0;

// Animation Cycle
static int l3d1_animFrame = 0;
static int l3d1_animTimer = 0;
#define L3D1_ANIM_FRAME_DELAY 6

// ==================== BOTTLE SPAWN & MECHANICS ====================
#define L3D1_BOTTLE_SIZE 55
#define L3D1_MAX_BOTTLES 3
static float l3d1_bottleBaseSpeed = 6.0f;

// Types: 0: Poison1, 1: Poison2, 2: Poison3, 3: Medicine1, 4: Medicine2
struct L3D1Bottle {
	float x, y;
	int type;
	bool active;
};

static L3D1Bottle l3d1_bottles[L3D1_MAX_BOTTLES];
static int l3d1_immunityTimer = 0; // 10 seconds shield

// Medicine-2 (Shield) Exact Timed Spawn Flags (10s and 25s at 60 FPS)
static bool l3d1_med2Spawned10s = false;
static bool l3d1_med2Spawned25s = false;

// ==================== HOLE MECHANICS ====================
#define L3D1_HOLE_W 120
#define L3D1_HOLE_H 45
static float l3d1_holeX = -300.0f;
static float l3d1_holeY = 145.0f;
static bool l3d1_holeActive = false;
static int l3d1_holeTimer = 0;
#define L3D1_HOLE_INTERVAL_FRAMES 420 // ~7 seconds por por hole ashbe

// ==================== LETTER EVENT (AFTER 40 SECONDS) ====================
#define L3D1_TASK_DURATION_FRAMES 2400 // 40 seconds at 60 FPS
#define L3D1_LETTER_W 153
#define L3D1_LETTER_H 75
#define L3D1_LETTER_OPEN_W 500
#define L3D1_LETTER_OPEN_H 500

static int l3d1_elapsedFrames = 0;
static bool l3d1_letterSpawned = false;
static float l3d1_letterX = 0.0f;
static float l3d1_letterY = 200.0f;
static bool l3d1_letterOpened = false;

inline void l3d1_spawnBottle(int index, float startX)
{
	l3d1_bottles[index].active = true;
	l3d1_bottles[index].x = startX;

	// Random Y lane: 0: Ground (170), 1: Middle (260), 2: High/Jump (350)
	int lane = rand() % 3;
	if (lane == 0) l3d1_bottles[index].y = 170.0f;
	else if (lane == 1) l3d1_bottles[index].y = 260.0f;
	else l3d1_bottles[index].y = 350.0f;

	// Normal random spawn: Poison (75%), Medicine1 (25%), Medicine2 shudhu timed event-e ashbe
	int r = rand() % 100;
	if (r < 75) {
		l3d1_bottles[index].type = rand() % 3;
	}
	else {
		l3d1_bottles[index].type = 3;
	}
}

inline bool l3d1_checkCollision(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline void L3Door1_GenerateTask()
{
	l3d1_playerX = 100.0f;
	l3d1_playerY = l3d1_groundY;
	l3d1_playerWidth = L3D1_PLAYER_NORMAL_W;
	l3d1_playerHeight = L3D1_PLAYER_NORMAL_H;
	l3d1_isSliding = false;
	l3d1_slideTimer = 0;
	l3d1_isJumping = false;
	l3d1_jumpVelocity = 0.0f;
	l3d1_jumpFrameIndex = 0;
	l3d1_isMoving = false;
	l3d1_facingRight = true;
	l3d1_bgX = 0;

	l3d1_immunityTimer = 0;
	l3d1_med2Spawned10s = false;
	l3d1_med2Spawned25s = false;
	l3d1_elapsedFrames = 0;
	l3d1_letterSpawned = false;
	l3d1_letterOpened = false;

	// Reset Hole
	l3d1_holeActive = false;
	l3d1_holeX = -300.0f;
	l3d1_holeTimer = 0;

	// Initial bottles distribution
	float startOffset = 1100.0f;
	for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
		l3d1_spawnBottle(i, startOffset + i * 550.0f);
	}
}

inline void L3Door1_RenderTask(int screenW, int optW, int optH, int optGap, int optY)
{
	static int insideCaveBg = -1, idleImg = -1, slideImg = -1;
	static int runFrames[8], jumpFrames[3];
	static int poisonImgs[3] = { -1, -1, -1 };
	static int med1Img = -1, med2Img = -1;
	static int letterImg = -1, letterOpnImg = -1;
	static int holeImg = -1;

	// 1. Asset Loading
	if (insideCaveBg == -1) {
		insideCaveBg = iLoadImage("Image/bgInsideCave.png");
		if (insideCaveBg < 0) insideCaveBg = iLoadImage("Image/bgCave.png");

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

		poisonImgs[0] = iLoadImage("Image/poison-1.png");
		if (poisonImgs[0] < 0) poisonImgs[0] = iLoadImage("poison-1.png");
		poisonImgs[1] = iLoadImage("Image/poison-2.png");
		if (poisonImgs[1] < 0) poisonImgs[1] = iLoadImage("poison-2.png");
		poisonImgs[2] = iLoadImage("Image/poison-3.png");
		if (poisonImgs[2] < 0) poisonImgs[2] = iLoadImage("poison-3.png");

		med1Img = iLoadImage("Image/medicine-1.png");
		if (med1Img < 0) med1Img = iLoadImage("medicine-1.png");
		med2Img = iLoadImage("Image/medicine-2.png");
		if (med2Img < 0) med2Img = iLoadImage("medicine-2.png");

		holeImg = iLoadImage("Image/hole.png");
		if (holeImg < 0) holeImg = iLoadImage("hole.png");

		letterImg = iLoadImage("Image/letter.png");
		if (letterImg < 0) letterImg = iLoadImage("letter.png");
		letterOpnImg = iLoadImage("Image/letterOpn.png");
		if (letterOpnImg < 0) letterOpnImg = iLoadImage("letterOpn.png");
	}

	// 2. Controlled Frame Tick
	static DWORD lastTick = 0;
	DWORD currentTick = GetTickCount();
	bool shouldUpdatePhysics = false;
	if (currentTick - lastTick >= 16) {
		shouldUpdatePhysics = true;
		lastTick = currentTick;
	}

	if (shouldUpdatePhysics && !l3d1_letterOpened && !level3_gameOver) {
		l3d1_isMoving = false;

		// Frame tracker
		l3d1_elapsedFrames++;

		// 10 Seconds por (600 frames) Medicine-2 Spawn
		if (l3d1_elapsedFrames >= 600 && !l3d1_med2Spawned10s && !l3d1_letterSpawned) {
			l3d1_med2Spawned10s = true;
			float furthestX = (float)screenW;
			for (int j = 0; j < L3D1_MAX_BOTTLES; j++) {
				if (l3d1_bottles[j].x > furthestX) furthestX = l3d1_bottles[j].x;
			}
			l3d1_spawnBottle(0, furthestX + 500.0f);
			l3d1_bottles[0].type = 4; // Medicine-2
		}

		// 25 Seconds por (1500 frames) Medicine-2 Spawn
		if (l3d1_elapsedFrames >= 1500 && !l3d1_med2Spawned25s && !l3d1_letterSpawned) {
			l3d1_med2Spawned25s = true;
			float furthestX = (float)screenW;
			for (int j = 0; j < L3D1_MAX_BOTTLES; j++) {
				if (l3d1_bottles[j].x > furthestX) furthestX = l3d1_bottles[j].x;
			}
			l3d1_spawnBottle(1, furthestX + 500.0f);
			l3d1_bottles[1].type = 4; // Medicine-2
		}

		// 40 Seconds elapsed tracker (Letter spawn)
		if (l3d1_elapsedFrames >= L3D1_TASK_DURATION_FRAMES && !l3d1_letterSpawned) {
			l3d1_letterSpawned = true;
			l3d1_letterX = (float)(screenW + 50);
			l3d1_letterY = 200.0f;
			l3d1_holeActive = false;
			// Bottles deactivate
			for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
				l3d1_bottles[i].active = false;
			}
		}

		// Immunity Timer Countdown
		if (l3d1_immunityTimer > 0) {
			l3d1_immunityTimer--;
		}

		// Slide Input
		if (!l3d1_isJumping && (GetAsyncKeyState(VK_DOWN) & 0x8000)) {
			l3d1_isSliding = true;
			l3d1_slideTimer = L3D1_SLIDE_DURATION;
			l3d1_playerWidth = L3D1_PLAYER_SLIDE_W;
			l3d1_playerHeight = L3D1_PLAYER_SLIDE_H;
		}

		if (l3d1_isSliding) {
			l3d1_slideTimer--;
			if (l3d1_slideTimer <= 0) {
				l3d1_isSliding = false;
				l3d1_playerWidth = L3D1_PLAYER_NORMAL_W;
				l3d1_playerHeight = L3D1_PLAYER_NORMAL_H;
			}
		}

		// Jump Input
		if (!l3d1_isSliding && (GetAsyncKeyState(VK_UP) & 0x8000)) {
			if (!l3d1_isJumping) {
				l3d1_isJumping = true;
				l3d1_jumpVelocity = L3D1_JUMP_STRENGTH;
				l3d1_playerY += 2.0f;
			}
		}

		// Movement direction calculation
		float playerMoveShift = 0.0f;

		// Forward Movement & Background Scroll + Scoring
		if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || l3d1_isSliding) {
			l3d1_isMoving = true;
			l3d1_facingRight = true;

			l3d1_bgX -= l3d1_playerSpeed;
			if (l3d1_bgX <= -screenW) {
				l3d1_bgX = 0;
			}
			level3_updateScore(1);
			playerMoveShift = (float)l3d1_playerSpeed;
		}
		// Backward Movement & Background Scroll
		else if (!l3d1_isSliding && (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
			l3d1_isMoving = true;
			l3d1_facingRight = false;

			l3d1_bgX += l3d1_playerSpeed;
			if (l3d1_bgX >= 0) {
				l3d1_bgX = -screenW;
			}
			playerMoveShift = -(float)l3d1_playerSpeed;
		}

		// Hole Spawning and Movement
		if (!l3d1_letterSpawned) {
			if (!l3d1_holeActive) {
				l3d1_holeTimer++;
				if (l3d1_holeTimer >= L3D1_HOLE_INTERVAL_FRAMES) {
					l3d1_holeTimer = 0;
					l3d1_holeActive = true;
					l3d1_holeX = (float)(screenW + 100);
				}
			}
			else {
				l3d1_holeX -= (l3d1_bottleBaseSpeed + playerMoveShift);
				if (l3d1_holeX < -200.0f) {
					l3d1_holeActive = false;
					l3d1_holeTimer = 0;
				}

				// Check Hole Collision (Player ground-e thakle game over, jump korle safe)
				float pFeetX = l3d1_playerX + 25.0f;
				float pFeetW = (float)l3d1_playerWidth - 50.0f;
				if (pFeetW < 20.0f) pFeetW = 30.0f;

				if (l3d1_checkCollision(pFeetX, l3d1_playerY, (int)pFeetW, 20,
					l3d1_holeX + 20.0f, l3d1_groundY - 5.0f, L3D1_HOLE_W - 40, 25)) {
					if (l3d1_playerY <= l3d1_groundY + 5.0f) {
						level3_energy = 0;
						level3_gameOver = true;
						level3_playNegPointSound();
					}
				}
			}
		}

		// Bottle or Letter Movement
		if (!l3d1_letterSpawned) {
			for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
				l3d1_bottles[i].x -= (l3d1_bottleBaseSpeed + playerMoveShift);
			}

			for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
				if (l3d1_bottles[i].x < -100.0f) {
					float furthestX = (float)screenW;
					for (int j = 0; j < L3D1_MAX_BOTTLES; j++) {
						if (l3d1_bottles[j].x > furthestX) furthestX = l3d1_bottles[j].x;
					}
					l3d1_spawnBottle(i, furthestX + 500.0f + (rand() % 250));
				}
			}
		}
		else {
			// Letter movement towards player
			l3d1_letterX -= (5.0f + playerMoveShift);

			// Check collision with Letter
			if (l3d1_checkCollision(l3d1_playerX, l3d1_playerY, l3d1_playerWidth, l3d1_playerHeight,
				l3d1_letterX, l3d1_letterY, L3D1_LETTER_W, L3D1_LETTER_H)) {
				l3d1_letterOpened = true;
				level3_updateScore(300);
				level3_playPlusPointSound();
			}
		}

		// Gravity Physics
		if (l3d1_isJumping || l3d1_playerY > l3d1_groundY) {
			l3d1_playerY += l3d1_jumpVelocity;
			l3d1_jumpVelocity -= L3D1_GRAVITY;

			if (l3d1_jumpVelocity > 3.0f) l3d1_jumpFrameIndex = 0;
			else if (l3d1_jumpVelocity >= -3.0f) l3d1_jumpFrameIndex = 1;
			else l3d1_jumpFrameIndex = 2;

			if (l3d1_playerY <= l3d1_groundY) {
				l3d1_playerY = l3d1_groundY;
				l3d1_isJumping = false;
				l3d1_jumpVelocity = 0.0f;
				l3d1_jumpFrameIndex = 0;
			}
		}

		// Running Animation Frame Delay
		if (l3d1_isMoving && !l3d1_isSliding) {
			l3d1_animTimer++;
			if (l3d1_animTimer >= L3D1_ANIM_FRAME_DELAY) {
				l3d1_animTimer = 0;
				l3d1_animFrame = (l3d1_animFrame + 1) % 8;
			}
		}
		else {
			l3d1_animFrame = 0;
			l3d1_animTimer = 0;
		}

		// Bottle Collision Detection (Before letter spawns)
		if (!l3d1_letterSpawned) {
			for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
				if (!l3d1_bottles[i].active) continue;

				if (l3d1_checkCollision(l3d1_playerX, l3d1_playerY, l3d1_playerWidth, l3d1_playerHeight,
					l3d1_bottles[i].x, l3d1_bottles[i].y, L3D1_BOTTLE_SIZE, L3D1_BOTTLE_SIZE)) {

					int bType = l3d1_bottles[i].type;

					if (bType >= 0 && bType <= 2) {
						if (l3d1_immunityTimer <= 0) {
							if (bType == 0) level3_energy -= 10;
							else if (bType == 1) level3_energy -= 15;
							else if (bType == 2) level3_energy -= 20;

							level3_playNegPointSound();

							if (level3_energy <= 0) {
								level3_energy = 0;
								level3_gameOver = true;
							}
						}
						else {
							level3_playPlusPointSound();
						}
					}
					else if (bType == 3) {
						level3_energy += 30;
						if (level3_energy > 100) level3_energy = 100;
						level3_updateScore(50);
						level3_playPlusPointSound();
					}
					else if (bType == 4) {
						l3d1_immunityTimer = 600;
						level3_updateScore(100);
						level3_playPlusPointSound();
					}

					float furthestX = (float)screenW;
					for (int j = 0; j < L3D1_MAX_BOTTLES; j++) {
						if (l3d1_bottles[j].x > furthestX) furthestX = l3d1_bottles[j].x;
					}
					l3d1_spawnBottle(i, furthestX + 500.0f + (rand() % 250));
				}
			}
		}
	}

	// 3. Draw Background
	if (insideCaveBg >= 0) {
		iShowImage(l3d1_bgX, 0, screenW, 600, insideCaveBg);
		iShowImage(l3d1_bgX + screenW, 0, screenW, 600, insideCaveBg);
	}

	// Draw Hole (120x45) on Ground
	if (l3d1_holeActive && l3d1_holeX >= -130 && l3d1_holeX <= screenW + 100) {
		if (holeImg >= 0) {
			iShowImage((int)l3d1_holeX, (int)l3d1_holeY, L3D1_HOLE_W, L3D1_HOLE_H, holeImg);
		}
		else {
			iSetColor(30, 20, 15);
			iFilledEllipse((int)l3d1_holeX + L3D1_HOLE_W / 2, (int)l3d1_holeY + L3D1_HOLE_H / 2, L3D1_HOLE_W / 2, L3D1_HOLE_H / 2);
		}
	}

	// 4. Draw Bottles (55x55) or Letter (153x75)
	if (!l3d1_letterSpawned) {
		for (int i = 0; i < L3D1_MAX_BOTTLES; i++) {
			if (l3d1_bottles[i].active && l3d1_bottles[i].x >= -80 && l3d1_bottles[i].x <= screenW + 80) {
				int imgToDraw = -1;
				int t = l3d1_bottles[i].type;
				if (t >= 0 && t <= 2) imgToDraw = poisonImgs[t];
				else if (t == 3) imgToDraw = med1Img;
				else if (t == 4) imgToDraw = med2Img;

				if (imgToDraw >= 0) {
					iShowImage((int)l3d1_bottles[i].x, (int)l3d1_bottles[i].y, L3D1_BOTTLE_SIZE, L3D1_BOTTLE_SIZE, imgToDraw);
				}
				else {
					if (t <= 2) iSetColor(180, 20, 20);
					else if (t == 3) iSetColor(20, 200, 40);
					else iSetColor(20, 200, 240);
					iFilledRectangle((int)l3d1_bottles[i].x, (int)l3d1_bottles[i].y, L3D1_BOTTLE_SIZE, L3D1_BOTTLE_SIZE);
				}
			}
		}
	}
	else if (!l3d1_letterOpened) {
		// Draw Letter (153x75)
		if (letterImg >= 0) {
			iShowImage((int)l3d1_letterX, (int)l3d1_letterY, L3D1_LETTER_W, L3D1_LETTER_H, letterImg);
		}
		else {
			iSetColor(240, 230, 180);
			iFilledRectangle((int)l3d1_letterX, (int)l3d1_letterY, L3D1_LETTER_W, L3D1_LETTER_H);
			iSetColor(0, 0, 0);
			iRectangle((int)l3d1_letterX, (int)l3d1_letterY, L3D1_LETTER_W, L3D1_LETTER_H);
		}
	}

	// 5. Draw Player
	int playerImg = l3d1_isSliding ? slideImg : (l3d1_isJumping ? jumpFrames[l3d1_jumpFrameIndex] : (l3d1_isMoving ? runFrames[l3d1_animFrame] : idleImg));
	iShowImage((int)l3d1_playerX, (int)l3d1_playerY, l3d1_playerWidth, l3d1_playerHeight, playerImg);

	// Shield Visual Aura
	if (l3d1_immunityTimer > 0) {
		iSetColor(0, 220, 255);
		iCircle((int)l3d1_playerX + l3d1_playerWidth / 2, (int)l3d1_playerY + l3d1_playerHeight / 2, 70);
		iCircle((int)l3d1_playerX + l3d1_playerWidth / 2, (int)l3d1_playerY + l3d1_playerHeight / 2, 72);

		char shieldBuf[32];
		sprintf_s(shieldBuf, sizeof(shieldBuf), "Shield: %ds", (l3d1_immunityTimer / 60) + 1);
		iText((int)l3d1_playerX, (int)l3d1_playerY + l3d1_playerHeight + 12, shieldBuf);
	}

	// 6. Draw HUD Score & Energy Bar
	char scBuf[64];
	sprintf_s(scBuf, sizeof(scBuf), "Score: %d", level3_score);
	iSetColor(255, 255, 255);
	iText(screenW - 150, 600 - 40, scBuf);
	iText(screenW - 149, 600 - 40, scBuf);

	// Energy Bar
	iSetColor(200, 200, 200);
	iFilledRectangle(20, 600 - 40, 200, 20);
	iSetColor(level3_energy > 30 ? 0 : 220, level3_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, 600 - 40, 2 * (level3_energy > 0 ? level3_energy : 0), 20);
	iSetColor(0, 0, 0);
	iRectangle(20, 600 - 40, 200, 20);
	iSetColor(255, 255, 255);
	iText(20, 600 - 55, "Energy");

	// 7. Draw Open Letter in Screen Center (500x500)
	if (l3d1_letterOpened) {
		int cx = (screenW - L3D1_LETTER_OPEN_W) / 2;
		int cy = (600 - L3D1_LETTER_OPEN_H) / 2;

		// Dim background
		iSetColor(0, 0, 0);
		for (int i = 0; i < 600; i += 4) {
			iLine(0, i, screenW, i);
		}

		if (letterOpnImg >= 0) {
			iShowImage(cx, cy, L3D1_LETTER_OPEN_W, L3D1_LETTER_OPEN_H, letterOpnImg);
		}
		else {
			iSetColor(250, 245, 220);
			iFilledRectangle(cx, cy, L3D1_LETTER_OPEN_W, L3D1_LETTER_OPEN_H);
			iSetColor(50, 20, 10);
			iRectangle(cx, cy, L3D1_LETTER_OPEN_W, L3D1_LETTER_OPEN_H);
		}

		iSetColor(255, 255, 255);
		iText(screenW / 2 - 120, cy - 25, "Click on the letter to exit cave!");
	}
}

inline bool L3Door1_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	if (l3d1_letterOpened) {
		int cx = (screenW - L3D1_LETTER_OPEN_W) / 2;
		int cy = (600 - L3D1_LETTER_OPEN_H) / 2;

		if (mx >= cx && mx <= cx + L3D1_LETTER_OPEN_W && my >= cy && my <= cy + L3D1_LETTER_OPEN_H) {
			if (doorVisited) *doorVisited = true;
			if (insideTask) *insideTask = false;
			l3d1_letterOpened = false;
			return true;
		}
	}
	return false;
}

#endif