#ifndef L3DOOR4_H_INCLUDED
#define L3DOOR4_H_INCLUDED

// =================================================================
//  DOOR 4 - "CAVE STORM"
//  The player can only move LEFT / RIGHT. The 60-second timer starts
//  the moment the player first moves. Falling rocks (slower) and
//  spikes (much faster) come down from the top; every hit removes
//  energy from the MAIN game energy (level3_energy), which Level3.h
//  already draws. Energy 0 -> game over. Timer reaches 0 -> the door
//  ends and the game returns to the cave doors (no chest here).
//
//  Hooks (same names as door 5):
//     level3door4_active / startL3Door4Path()
//     renderL3Door4Path() / updateL3Door4PathIfActive()
// =================================================================

static bool level3door4_active = false;

// ---------------- image files (change the names to match yours) ----------------
#define L3D4_BG_FILE      "Image/bgInsideCave.png"
#define L3D4_ROCK1_FILE   "Image/door4_rock1.png"
#define L3D4_ROCK2_FILE   "Image/door4_rock2.png"
#define L3D4_SPIKE1_FILE  "Image/door4_spike1.png"
#define L3D4_SPIKE2_FILE  "Image/door4_spike2.png"

// ---------------- tuning knobs ----------------
#define L3D4_SURVIVE_SECONDS   60.0f
#define L3D4_WAVE_SECONDS      40.0f   // difficulty goes up every 20 s
#define L3D4_SURVIVE_SCORE     300     // bonus for surviving (0 = none)

#define L3D4_ROCK_DAMAGE       10
#define L3D4_SPIKE_DAMAGE      5

#define L3D4_GROUND_Y          150.0f  // where the character's feet are
#define L3D4_PLAYER_SPEED      8.0f
#define L3D4_HIT_HALF_W        25      // player hitbox (narrower than the sprite)
#define L3D4_HIT_H             110

#define L3D4_MAX_HAZARDS       24
#define L3D4_ROCK_W            70      // drawn size of a falling stone
#define L3D4_ROCK_H            70
#define L3D4_SPIKE_W           30      // drawn size of a falling spike
#define L3D4_SPIKE_H           75
#define L3D4_HAZARD_INSET      6       // hitbox is this much smaller than the image on every side
#define L3D4_ROCK_BASE_SPEED   5.0f
#define L3D4_SPIKE_BASE_SPEED  10.0f

#define L3D4_SPAWN_BASE        55      // frames between spawns in the first wave
#define L3D4_SPAWN_MIN         20

// ---------------- state ----------------
struct Level3Door4Hazard {
	bool  active;
	bool  isSpike;
	int   variant;  // 0 or 1 = which of the two images
	float x;        // horizontal centre
	float y;        // bottom edge
	float speed;
};

static Level3Door4Hazard level3door4_hazards[L3D4_MAX_HAZARDS];

static float   level3door4_playerX = 500.0f;
static bool    level3door4_started = false;   // becomes true on the first LEFT/RIGHT press
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
	return wave; // 0..5
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
	h.isSpike = (rand() % 100) < (25 + wave * 6); // spikes get more common each wave
	h.variant = rand() % 2;
	int halfW = level3door4_hazardW(h) / 2;

	// 40% of hazards are aimed near the player so nobody can hide in a corner
	float x;
	if (rand() % 100 < 40) x = level3door4_playerX + (float)(rand() % 201 - 100);
	else x = (float)(halfW + 10 + rand() % (SCREEN_WIDTH - 2 * halfW - 20));
	if (x < halfW + 5) x = (float)(halfW + 5);
	if (x > SCREEN_WIDTH - halfW - 5) x = (float)(SCREEN_WIDTH - halfW - 5);
	h.x = x;

	h.y = (float)SCREEN_HEIGHT; // starts just above the top edge
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

		if (h.y <= L3D4_GROUND_Y) h.active = false; // hit the floor
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
		for (int i = 0; i < 4; i++) {
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

	// Countdown (the energy bar, score and messages are drawn by Level3.h)
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

	// Left / right movement only
	bool goLeft = isSpecialKeyPressed(GLUT_KEY_LEFT);
	bool goRight = isSpecialKeyPressed(GLUT_KEY_RIGHT);
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

	// Nothing falls and the clock stays at 1:00 until the character first moves
	if (!level3door4_started) {
		if (!level3door4_isMoving) return;
		level3door4_started = true;
		level3door4_lastClock = clock();
		level3door4_spawnTimer = 30;
	}

	// Real-time countdown (clamped so pausing the game can't skip seconds)
	clock_t now = clock();
	float dt = (float)(now - level3door4_lastClock) / (float)CLOCKS_PER_SEC;
	level3door4_lastClock = now;
	if (dt < 0.0f) dt = 0.0f;
	if (dt > 0.1f) dt = 0.1f;
	level3door4_timeLeft -= dt;

	// Spawn hazards - faster and in bigger groups as time passes
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

	// Survived the full minute -> back to the doors
	if (level3door4_timeLeft <= 0.0f) {
		level3_updateScore(L3D4_SURVIVE_SCORE);
		level3_playPlusPointSound();
		strcpy_s(level3_message, sizeof(level3_message), "You survived the Cave Storm!");
		level3_messageTimer = 90;
		level3door4_active = false;
	}
}

#endif