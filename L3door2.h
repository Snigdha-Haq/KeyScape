#ifndef L3DOOR2_H_INCLUDED
#define L3DOOR2_H_INCLUDED

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

// Player Physics & Dimensions
static const float l3d2_groundY = 165.0f;
static float l3d2_playerX = 100.0f;
static float l3d2_playerY = 165.0f;

#define L3D2_PLAYER_NORMAL_W 90
#define L3D2_PLAYER_NORMAL_H 130
#define L3D2_PLAYER_SLIDE_W  120
#define L3D2_PLAYER_SLIDE_H  60

static int l3d2_playerWidth = L3D2_PLAYER_NORMAL_W;
static int l3d2_playerHeight = L3D2_PLAYER_NORMAL_H;

// Scroll & Player speed
static int l3d2_playerSpeed = 11;

// Slide mechanism
static bool l3d2_isSliding = false;
static int l3d2_slideTimer = 0;
#define L3D2_SLIDE_DURATION 28

// Jump physics
static bool l3d2_isJumping = false;
static float l3d2_jumpVelocity = 0.0f;
#define L3D2_JUMP_STRENGTH 16.0f
#define L3D2_GRAVITY 0.8f
static int l3d2_jumpFrameIndex = 0;

// Movement & Background Scroll
static bool l3d2_facingRight = true;
static bool l3d2_isMoving = false;
static int l3d2_bgX = 0;

// Animation Cycle
static int l3d2_animFrame = 0;
static int l3d2_animTimer = 0;
#define L3D2_ANIM_FRAME_DELAY 6

// ==================== COIN LOGIC & REQUIREMENTS ====================
#define L3D2_COIN_SIZE 50
#define L3D2_MAX_COINS 4
static float l3d2_coinBaseSpeed = 5.5f;

// Target Coins
#define L3D2_REQ_BRONZE 5
#define L3D2_REQ_SILVER 6
#define L3D2_REQ_GOLD   7

// Coin Counters
static int l3d2_collectedBronze = 0;
static int l3d2_collectedSilver = 0;
static int l3d2_collectedGold = 0;
static bool l3d2_taskCompleted = false;

// 0: Bronze, 1: Silver, 2: Gold
struct L3D2Coin {
	float x, y;
	int type;
	bool active;
};

static L3D2Coin l3d2_coins[L3D2_MAX_COINS];

// ==================== BOMB LOGIC ====================
#define L3D2_BOMB_W 100
#define L3D2_BOMB_H 65
static float l3d2_bombX = -500.0f;
static float l3d2_bombY = 165.0f;
static bool l3d2_bombActive = false;
static float l3d2_bombSpeed = 6.0f;

inline bool l3d2_checkCollision(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline void l3d2_spawnCoin(int index, float startX)
{
	l3d2_coins[index].active = true;
	l3d2_coins[index].x = startX;

	int lane = rand() % 3;
	if (lane == 0) l3d2_coins[index].y = 170.0f;
	else if (lane == 1) l3d2_coins[index].y = 250.0f;
	else l3d2_coins[index].y = 340.0f;

	bool needBronze = (l3d2_collectedBronze < L3D2_REQ_BRONZE);
	bool needSilver = (l3d2_collectedSilver < L3D2_REQ_SILVER);
	bool needGold = (l3d2_collectedGold   < L3D2_REQ_GOLD);

	int available[3];
	int availCount = 0;
	if (needBronze) available[availCount++] = 0;
	if (needSilver) available[availCount++] = 1;
	if (needGold)   available[availCount++] = 2;

	if (availCount > 0) {
		l3d2_coins[index].type = available[rand() % availCount];
	}
	else {
		l3d2_coins[index].type = rand() % 3;
	}
}

inline void l3d2_spawnBomb(float startX)
{
	l3d2_bombActive = true;
	l3d2_bombX = startX;

	// Random Y Lane: Ground (165), Middle (250), High (330)
	int lane = rand() % 3;
	if (lane == 0) l3d2_bombY = 165.0f;
	else if (lane == 1) l3d2_bombY = 250.0f;
	else l3d2_bombY = 330.0f;
}

inline void L3Door2_GenerateTask()
{
	l3d2_playerX = 100.0f;
	l3d2_playerY = l3d2_groundY;
	l3d2_playerWidth = L3D2_PLAYER_NORMAL_W;
	l3d2_playerHeight = L3D2_PLAYER_NORMAL_H;
	l3d2_isSliding = false;
	l3d2_slideTimer = 0;
	l3d2_isJumping = false;
	l3d2_jumpVelocity = 0.0f;
	l3d2_jumpFrameIndex = 0;
	l3d2_isMoving = false;
	l3d2_facingRight = true;
	l3d2_bgX = 0;
	l3d2_animFrame = 0;
	l3d2_animTimer = 0;

	// Reset Coin Tracking
	l3d2_collectedBronze = 0;
	l3d2_collectedSilver = 0;
	l3d2_collectedGold = 0;
	l3d2_taskCompleted = false;

	// Spawn Initial Coins
	float startOffset = 900.0f;
	for (int i = 0; i < L3D2_MAX_COINS; i++) {
		l3d2_spawnCoin(i, startOffset + i * 520.0f);
	}

	// Spawn Bomb Initially behind the coins
	l3d2_spawnBomb(startOffset + 1200.0f);
}

inline void L3Door2_RenderTask(int screenW, int optW, int optH, int optGap, int optY)
{
	static int insideCaveBg = -1, idleImg = -1, slideImg = -1;
	static int runFrames[8], jumpFrames[3];
	static int coinImgs[3] = { -1, -1, -1 }; // 0: Bronze, 1: Silver, 2: Gold
	static int bombImg = -1;

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

		// Coins Loading
		coinImgs[0] = iLoadImage("Image/bronzeCoin.png");
		if (coinImgs[0] < 0) coinImgs[0] = iLoadImage("bronzeCoin.png");

		coinImgs[1] = iLoadImage("Image/silverCoin.png");
		if (coinImgs[1] < 0) coinImgs[1] = iLoadImage("silverCoin.png");

		coinImgs[2] = iLoadImage("Image/goldCoin.png");
		if (coinImgs[2] < 0) coinImgs[2] = iLoadImage("goldCoin.png");

		// Bomb Loading
		bombImg = iLoadImage("Image/bomb.png");
		if (bombImg < 0) bombImg = iLoadImage("bomb.png");
	}

	// 2. Controlled Frame Tick (~60 FPS)
	static DWORD lastTick = 0;
	DWORD currentTick = GetTickCount();
	bool shouldUpdatePhysics = false;
	if (currentTick - lastTick >= 16) {
		shouldUpdatePhysics = true;
		lastTick = currentTick;
	}

	if (shouldUpdatePhysics && !l3d2_taskCompleted && !level3_gameOver) {
		l3d2_isMoving = false;

		// Slide Input
		if (!l3d2_isJumping && (GetAsyncKeyState(VK_DOWN) & 0x8000)) {
			l3d2_isSliding = true;
			l3d2_slideTimer = L3D2_SLIDE_DURATION;
			l3d2_playerWidth = L3D2_PLAYER_SLIDE_W;
			l3d2_playerHeight = L3D2_PLAYER_SLIDE_H;
		}

		if (l3d2_isSliding) {
			l3d2_slideTimer--;
			if (l3d2_slideTimer <= 0) {
				l3d2_isSliding = false;
				l3d2_playerWidth = L3D2_PLAYER_NORMAL_W;
				l3d2_playerHeight = L3D2_PLAYER_NORMAL_H;
			}
		}

		// Jump Input
		if (!l3d2_isSliding && (GetAsyncKeyState(VK_UP) & 0x8000)) {
			if (!l3d2_isJumping) {
				l3d2_isJumping = true;
				l3d2_jumpVelocity = L3D2_JUMP_STRENGTH;
				l3d2_playerY += 2.0f;
			}
		}

		// Movement & Background Scroll calculation
		float playerMoveShift = 0.0f;

		if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || l3d2_isSliding) {
			l3d2_isMoving = true;
			l3d2_facingRight = true;

			l3d2_bgX -= l3d2_playerSpeed;
			if (l3d2_bgX <= -screenW) {
				l3d2_bgX = 0;
			}
			playerMoveShift = (float)l3d2_playerSpeed;
		}
		else if (!l3d2_isSliding && (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
			l3d2_isMoving = true;
			l3d2_facingRight = false;

			l3d2_bgX += l3d2_playerSpeed;
			if (l3d2_bgX >= 0) {
				l3d2_bgX = -screenW;
			}
			playerMoveShift = -(float)l3d2_playerSpeed;
		}

		// Coins Movement towards Player
		for (int i = 0; i < L3D2_MAX_COINS; i++) {
			l3d2_coins[i].x -= (l3d2_coinBaseSpeed + playerMoveShift);
		}

		// Respawn Coins passing the screen left
		for (int i = 0; i < L3D2_MAX_COINS; i++) {
			if (l3d2_coins[i].x < -100.0f) {
				float furthestX = (float)screenW;
				for (int j = 0; j < L3D2_MAX_COINS; j++) {
					if (l3d2_coins[j].x > furthestX) furthestX = l3d2_coins[j].x;
				}
				l3d2_spawnCoin(i, furthestX + 480.0f + (rand() % 200));
			}
		}

		// Player-Coin Collision Check
		for (int i = 0; i < L3D2_MAX_COINS; i++) {
			if (!l3d2_coins[i].active) continue;

			if (l3d2_checkCollision(l3d2_playerX, l3d2_playerY, l3d2_playerWidth, l3d2_playerHeight,
				l3d2_coins[i].x, l3d2_coins[i].y, L3D2_COIN_SIZE, L3D2_COIN_SIZE)) {

				int cType = l3d2_coins[i].type;
				bool accepted = false;

				if (cType == 0 && l3d2_collectedBronze < L3D2_REQ_BRONZE) {
					l3d2_collectedBronze++;
					accepted = true;
				}
				else if (cType == 1 && l3d2_collectedSilver < L3D2_REQ_SILVER) {
					l3d2_collectedSilver++;
					accepted = true;
				}
				else if (cType == 2 && l3d2_collectedGold < L3D2_REQ_GOLD) {
					l3d2_collectedGold++;
					accepted = true;
				}

				if (accepted) {
					level3_updateScore(25);
					level3_playPlusPointSound();
				}

				// Check Task Finish condition
				if (l3d2_collectedBronze >= L3D2_REQ_BRONZE &&
					l3d2_collectedSilver >= L3D2_REQ_SILVER &&
					l3d2_collectedGold >= L3D2_REQ_GOLD) {
					l3d2_taskCompleted = true;
					l3d2_bombActive = false;
					level3_updateScore(200);
					strcpy_s(level3_message, sizeof(level3_message), "All Coins Collected! Click to Finish.");
					level3_messageTimer = 120;
				}

				// Respawn collected coin ahead
				float furthestX = (float)screenW;
				for (int j = 0; j < L3D2_MAX_COINS; j++) {
					if (l3d2_coins[j].x > furthestX) furthestX = l3d2_coins[j].x;
				}
				l3d2_spawnCoin(i, furthestX + 480.0f + (rand() % 200));
			}
		}

		// ==================== BOMB MOVEMENT & COLLISION ====================
		if (l3d2_bombActive) {
			l3d2_bombX -= (l3d2_bombSpeed + playerMoveShift);

			// Check Bomb Collision with Player -> Game Over
			if (l3d2_checkCollision(l3d2_playerX, l3d2_playerY, l3d2_playerWidth, l3d2_playerHeight,
				l3d2_bombX, l3d2_bombY, L3D2_BOMB_W, L3D2_BOMB_H)) {
				level3_energy = 0;
				level3_gameOver = true;
				level3_playNegPointSound();
			}

			// Screen cross korle abr shamne respawn hobe
			if (l3d2_bombX < -150.0f) {
				float furthestX = (float)screenW;
				for (int j = 0; j < L3D2_MAX_COINS; j++) {
					if (l3d2_coins[j].x > furthestX) furthestX = l3d2_coins[j].x;
				}
				l3d2_spawnBomb(furthestX + 550.0f + (rand() % 350));
			}
		}

		// Gravity Physics
		if (l3d2_isJumping || l3d2_playerY > l3d2_groundY) {
			l3d2_playerY += l3d2_jumpVelocity;
			l3d2_jumpVelocity -= L3D2_GRAVITY;

			if (l3d2_jumpVelocity > 3.0f) l3d2_jumpFrameIndex = 0;
			else if (l3d2_jumpVelocity >= -3.0f) l3d2_jumpFrameIndex = 1;
			else l3d2_jumpFrameIndex = 2;

			if (l3d2_playerY <= l3d2_groundY) {
				l3d2_playerY = l3d2_groundY;
				l3d2_isJumping = false;
				l3d2_jumpVelocity = 0.0f;
				l3d2_jumpFrameIndex = 0;
			}
		}

		// Running Animation Frame Delay
		if (l3d2_isMoving && !l3d2_isSliding) {
			l3d2_animTimer++;
			if (l3d2_animTimer >= L3D2_ANIM_FRAME_DELAY) {
				l3d2_animTimer = 0;
				l3d2_animFrame = (l3d2_animFrame + 1) % 8;
			}
		}
		else {
			l3d2_animFrame = 0;
			l3d2_animTimer = 0;
		}
	}

	// 3. Draw Background Scroll
	if (insideCaveBg >= 0) {
		iShowImage(l3d2_bgX, 0, screenW, 600, insideCaveBg);
		iShowImage(l3d2_bgX + screenW, 0, screenW, 600, insideCaveBg);
	}

	// 4. Draw Coins (50x50)
	for (int i = 0; i < L3D2_MAX_COINS; i++) {
		if (l3d2_coins[i].active && l3d2_coins[i].x >= -60 && l3d2_coins[i].x <= screenW + 60) {
			int t = l3d2_coins[i].type;
			int cImg = coinImgs[t];
			if (cImg >= 0) {
				iShowImage((int)l3d2_coins[i].x, (int)l3d2_coins[i].y, L3D2_COIN_SIZE, L3D2_COIN_SIZE, cImg);
			}
			else {
				if (t == 0) iSetColor(205, 127, 50);      // Bronze
				else if (t == 1) iSetColor(192, 192, 192); // Silver
				else iSetColor(255, 215, 0);               // Gold
				iFilledCircle((int)l3d2_coins[i].x + 25, (int)l3d2_coins[i].y + 25, 25);
			}
		}
	}

	// 5. Draw Bomb (100x65)
	if (l3d2_bombActive && l3d2_bombX >= -120 && l3d2_bombX <= screenW + 100) {
		if (bombImg >= 0) {
			iShowImage((int)l3d2_bombX, (int)l3d2_bombY, L3D2_BOMB_W, L3D2_BOMB_H, bombImg);
		}
		else {
			iSetColor(30, 30, 30);
			iFilledRectangle((int)l3d2_bombX, (int)l3d2_bombY, L3D2_BOMB_W, L3D2_BOMB_H);
		}
	}

	// 6. Draw Player
	int playerImg = l3d2_isSliding ? slideImg : (l3d2_isJumping ? jumpFrames[l3d2_jumpFrameIndex] : (l3d2_isMoving ? runFrames[l3d2_animFrame] : idleImg));
	iShowImage((int)l3d2_playerX, (int)l3d2_playerY, l3d2_playerWidth, l3d2_playerHeight, playerImg);

	// 7. Draw HUD Requirements & Counts (Energy bar-er niche)
	char coinHUD[128];
	sprintf_s(coinHUD, sizeof(coinHUD), "Bronze: %d/%d   Silver: %d/%d   Gold: %d/%d",
		l3d2_collectedBronze, L3D2_REQ_BRONZE,
		l3d2_collectedSilver, L3D2_REQ_SILVER,
		l3d2_collectedGold, L3D2_REQ_GOLD);

	iSetColor(0, 0, 0);
	iFilledRectangle(20, 600 - 95, 330, 26);
	iSetColor(255, 255, 255);
	iRectangle(20, 600 - 95, 330, 26);
	iText(28, 600 - 87, coinHUD);

	// 8. Task Complete Screen Overlay
	if (l3d2_taskCompleted) {
		iSetColor(0, 0, 0);
		for (int i = 0; i < 600; i += 4) {
			iLine(0, i, screenW, i);
		}

		iSetColor(255, 215, 0);
		iText(screenW / 2 - 130, 340, "TASK COMPLETED!", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(255, 255, 255);
		iText(screenW / 2 - 110, 300, "All Coins Collected!");
		iText(screenW / 2 - 125, 260, "Click anywhere to exit.");
	}
}

inline bool L3Door2_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	if (l3d2_taskCompleted) {
		if (doorVisited) *doorVisited = true;
		if (insideTask) *insideTask = false;
		return true;
	}
	return false;
}

#endif