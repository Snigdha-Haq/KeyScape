#ifndef L3DOOR4_H_INCLUDED
#define L3DOOR4_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <windows.h>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

#ifndef LEVEL3_PLAYER_NORMAL_W
#define LEVEL3_PLAYER_NORMAL_W 90
#define LEVEL3_PLAYER_NORMAL_H 130
#endif

#ifndef LEVEL3_ANIM_FRAME_DELAY
#define LEVEL3_ANIM_FRAME_DELAY 6
#endif

extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();
void level3_drawBoldText(int x, int y, const char* str, void* font);

static bool level3door4_active = false;

// ---------------- image files ----------------
#define L3D4_BG_FILE      "Image/bgInsideCave.png"
#define L3D4_ROCK1_FILE   "Image/door4_rock1.png"
#define L3D4_ROCK2_FILE   "Image/door4_rock2.png"
#define L3D4_SPIKE1_FILE  "Image/door4_spike1.png"
#define L3D4_SPIKE2_FILE  "Image/door4_spike2.png"

// ---------------- tuning knobs ----------------
#define L3D4_SURVIVE_SECONDS   60.0f
#define L3D4_WAVE_SECONDS      20.0f
#define L3D4_SURVIVE_SCORE     300

#define L3D4_ROCK_DAMAGE       10
#define L3D4_SPIKE_DAMAGE      5

#define L3D4_GROUND_Y          150.0f
#define L3D4_PLAYER_SPEED      8.0f
#define L3D4_HIT_HALF_W        25
#define L3D4_HIT_H             110

#define L3D4_MAX_HAZARDS       24
#define L3D4_ROCK_W            90
#define L3D4_ROCK_H            90
#define L3D4_SPIKE_W           50
#define L3D4_SPIKE_H           75
#define L3D4_HAZARD_INSET      6
#define L3D4_ROCK_BASE_SPEED   5.0f
#define L3D4_SPIKE_BASE_SPEED  10.0f

#define L3D4_SPAWN_BASE        55
#define L3D4_SPAWN_MIN         20

// ---------------- state ----------------
struct Level3Door4Hazard {
	bool  active;
	bool  isSpike;
	int   variant;
	float x;
	float y;
	float speed;
};

static Level3Door4Hazard level3door4_hazards[L3D4_MAX_HAZARDS];

static float   level3door4_playerX = 500.0f;
static bool    level3door4_started = false;
static float   level3door4_timeLeft = L3D4_SURVIVE_SECONDS;
static clock_t level3door4_lastClock = 0;
static int     level3door4_spawnTimer = 0;
static bool    level3door4_isMoving = false;
static int     level3door4_animFrame = 0;
static int     level3door4_animTimer = 0;

// ---------------- helpers ----------------
inline int level3door4_currentWave()
{
	int wave = (int)((L3D4_SURVIVE_SECONDS - level3door4_timeLeft) / L3D4_WAVE_SECONDS);
	if (wave < 0) wave = 0;
	if (wave > 5) wave = 5;
	return wave;
}

inline int level3door4_hazardW(const Level3Door4Hazard &h) { return h.isSpike ? L3D4_SPIKE_W : L3D4_ROCK_W; }
inline int level3door4_hazardH(const Level3Door4Hazard &h) { return h.isSpike ? L3D4_SPIKE_H : L3D4_ROCK_H; }

inline void startL3Door4Path()
{
	level3door4_active = true;
	level3door4_playerX = SCREEN_WIDTH / 2.0f;
	level3door4_started = false;
	level3door4_timeLeft = L3D4_SURVIVE_SECONDS;
	level3door4_lastClock = clock();
	level3door4_spawnTimer = 0;
	level3door4_isMoving = false;
	level3door4_animFrame = 0;
	level3door4_animTimer = 0;
	for (int i = 0; i < L3D4_MAX_HAZARDS; i++) level3door4_hazards[i].active = false;
}

inline void level3door4_spawnHazard()
{
	int slot = -1;
	for (int i = 0; i < L3D4_MAX_HAZARDS; i++) {
		if (!level3door4_hazards[i].active) { slot = i; break; }
	}
	if (slot < 0) return;

	int wave = level3door4_currentWave();
	Level3Door4Hazard &h = level3door4_hazards[slot];

	h.active = true;
	h.isSpike = (rand() % 100) < (25 + wave * 6);
	h.variant = rand() % 2;
	int halfW = level3door4_hazardW(h) / 2;

	float x;
	if (rand() % 100 < 40) x = level3door4_playerX + (float)(rand() % 201 - 100);
	else x = (float)(halfW + 10 + rand() % (SCREEN_WIDTH - 2 * halfW - 20));
	if (x < halfW + 5) x = (float)(halfW + 5);
	if (x > SCREEN_WIDTH - halfW - 5) x = (float)(SCREEN_WIDTH - halfW - 5);
	h.x = x;

	h.y = (float)SCREEN_HEIGHT;
	float base = h.isSpike ? L3D4_SPIKE_BASE_SPEED : L3D4_ROCK_BASE_SPEED;
	h.speed = base + wave * 0.6f + (rand() % 20) / 10.0f;
}

inline void level3door4_updateHazards()
{
	float pl = level3door4_playerX - L3D4_HIT_HALF_W;
	float pr = level3door4_playerX + L3D4_HIT_HALF_W;
	float pb = L3D4_GROUND_Y;
	float pt = L3D4_GROUND_Y + L3D4_HIT_H;

	for (int i = 0; i < L3D4_MAX_HAZARDS; i++) {
		Level3Door4Hazard &h = level3door4_hazards[i];
		if (!h.active) continue;

		h.y -= h.speed;

		float left = h.x - level3door4_hazardW(h) / 2.0f + L3D4_HAZARD_INSET;
		float right = h.x + level3door4_hazardW(h) / 2.0f - L3D4_HAZARD_INSET;
		float bottom = h.y + L3D4_HAZARD_INSET;
		float top = h.y + level3door4_hazardH(h) - L3D4_HAZARD_INSET;

		if (left < pr && right > pl && bottom < pt && top > pb) {
			h.active = false;
			level3_energy -= h.isSpike ? L3D4_SPIKE_DAMAGE : L3D4_ROCK_DAMAGE;
			level3_playNegPointSound();
			if (level3_energy <= 0) {
				level3_energy = 0;
				level3_gameOver = true;
				return;
			}
			continue;
		}

		if (h.y <= L3D4_GROUND_Y) h.active = false;
	}
}

// ---------------- render ----------------
inline void renderL3Door4Path()
{
	static bool loaded = false;
	static int bgImg = -1, idleImg = -1, runFrames[8];
	static int rockImgs[2] = { -1, -1 }, spikeImgs[2] = { -1, -1 };

	if (!loaded) {
		loaded = true;
		bgImg = iLoadImage(L3D4_BG_FILE);
		rockImgs[0] = iLoadImage(L3D4_ROCK1_FILE);
		rockImgs[1] = iLoadImage(L3D4_ROCK2_FILE);
		spikeImgs[0] = iLoadImage(L3D4_SPIKE1_FILE);
		spikeImgs[1] = iLoadImage(L3D4_SPIKE2_FILE);
		idleImg = iLoadImage("Image/idle_1.png");
		for (int i = 0; i < 8; i++) {
			char path[64];
			sprintf_s(path, sizeof(path), "Image/run_%d.png", i + 1);
			runFrames[i] = iLoadImage(path);
		}
	}

	if (bgImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImg);

	for (int i = 0; i < L3D4_MAX_HAZARDS; i++) {
		const Level3Door4Hazard &h = level3door4_hazards[i];
		if (!h.active) continue;
		int img = h.isSpike ? spikeImgs[h.variant] : rockImgs[h.variant];
		int w = level3door4_hazardW(h);
		if (img >= 0) iShowImage((int)(h.x - w / 2.0f), (int)h.y, w, level3door4_hazardH(h), img);
	}

	int playerImg = level3door4_isMoving ? runFrames[level3door4_animFrame] : idleImg;
	if (playerImg >= 0) {
		iShowImage((int)level3door4_playerX - LEVEL3_PLAYER_NORMAL_W / 2, (int)L3D4_GROUND_Y,
			LEVEL3_PLAYER_NORMAL_W, LEVEL3_PLAYER_NORMAL_H, playerImg);
	}

	int secs = (int)ceilf(level3door4_timeLeft);
	if (secs < 0) secs = 0;
	char buf[32];
	sprintf_s(buf, sizeof(buf), "Time: %d:%02d", secs / 60, secs % 60);
	if (level3door4_started && secs <= 10) iSetColor(255, 80, 60); else iSetColor(255, 255, 255);
	level3_drawBoldText(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 80, buf, GLUT_BITMAP_TIMES_ROMAN_24);
}

// ---------------- update ----------------
inline void updateL3Door4PathIfActive()
{
	if (level3_gameOver) {
		level3door4_active = false;
		return;
	}

	if (level3_messageTimer > 0) level3_messageTimer--;

	bool goLeft = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
	bool goRight = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
	if (goLeft) level3door4_playerX -= L3D4_PLAYER_SPEED;
	if (goRight) level3door4_playerX += L3D4_PLAYER_SPEED;
	float minX = LEVEL3_PLAYER_NORMAL_W / 2.0f;
	float maxX = SCREEN_WIDTH - LEVEL3_PLAYER_NORMAL_W / 2.0f;
	if (level3door4_playerX < minX) level3door4_playerX = minX;
	if (level3door4_playerX > maxX) level3door4_playerX = maxX;

	level3door4_isMoving = (goLeft != goRight);
	if (level3door4_isMoving) {
		level3door4_animTimer++;
		if (level3door4_animTimer >= LEVEL3_ANIM_FRAME_DELAY) {
			level3door4_animTimer = 0;
			level3door4_animFrame = (level3door4_animFrame + 1) % 8;
		}
	}
	else {
		level3door4_animFrame = 0;
		level3door4_animTimer = 0;
	}

	if (!level3door4_started) {
		if (!level3door4_isMoving) return;
		level3door4_started = true;
		level3door4_lastClock = clock();
		level3door4_spawnTimer = 30;
	}

	clock_t now = clock();
	float dt = (float)(now - level3door4_lastClock) / (float)CLOCKS_PER_SEC;
	level3door4_lastClock = now;
	if (dt < 0.0f) dt = 0.0f;
	if (dt > 0.1f) dt = 0.1f;
	level3door4_timeLeft -= dt;

	level3door4_spawnTimer--;
	if (level3door4_spawnTimer <= 0) {
		int wave = level3door4_currentWave();
		int count = 1;
		if (wave >= 2 && rand() % 3 == 0) count++;
		if (wave >= 4 && rand() % 3 == 0) count++;
		for (int i = 0; i < count; i++) level3door4_spawnHazard();

		int interval = L3D4_SPAWN_BASE - wave * 7;
		if (interval < L3D4_SPAWN_MIN) interval = L3D4_SPAWN_MIN;
		level3door4_spawnTimer = interval + rand() % 15;
	}

	level3door4_updateHazards();

	if (level3_gameOver) {
		level3door4_active = false;
		return;
	}

	if (level3door4_timeLeft <= 0.0f) {
		level3_updateScore(L3D4_SURVIVE_SCORE);
		level3_playPlusPointSound();
		strcpy_s(level3_message, sizeof(level3_message), "You survived the Cave Storm!");
		level3_messageTimer = 90;
		level3door4_active = false;
	}
}

#endif