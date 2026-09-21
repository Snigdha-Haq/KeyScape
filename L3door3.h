#ifndef L3DOOR3_H_INCLUDED
#define L3DOOR3_H_INCLUDED



#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <windows.h>

// Level 3 state variables
extern int level3_score;
extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

// ---------------------------------------------------------------- phases
#define L3D3_PHASE_INTRO    0
#define L3D3_PHASE_PLAY     1
#define L3D3_PHASE_UNLOCKED 2
#define L3D3_PHASE_DONE     3

#define L3D3_MIN_FRAMES   3600     // 60 seconds @ ~60 fps
#define L3D3_NEED_CRYSTAL 3

// cave floor bounds (hero bottom-left corner)
#define L3D3_MIN_X  60
#define L3D3_MAX_X  855
#define L3D3_MIN_Y  70
#define L3D3_MAX_Y  325

// ---------------------------------------------------------------- hero
#define L3D3_HERO_W 74
#define L3D3_HERO_H 104

static float l3d3_heroX = 120.0f;
static float l3d3_heroY = 120.0f;
static float l3d3_heroSpeed = 5.2f;
static bool  l3d3_heroMoving = false;
static int   l3d3_animFrame = 0;
static int   l3d3_animTimer = 0;
#define L3D3_ANIM_DELAY 6

static int   l3d3_phase = L3D3_PHASE_INTRO;
static int   l3d3_phaseTimer = 0;
static int   l3d3_elapsed = 0;
static int   l3d3_collected = 0;
static int   l3d3_invuln = 0;          // damage cooldown
static int   l3d3_boostTimer = 0;      // speed boost from the special crystal
static int   l3d3_attackTimer = 0;     // knife swing visual (14 frames -> 7 attack frames)
static int   l3d3_attackCooldown = 0;
static bool  l3d3_prevSpace = false;
static float l3d3_lightRadius = 330.0f;

// ---------------------------------------------------------------- crystals
#define L3D3_MAX_CRYSTALS 2
#define L3D3_CRYSTAL_W 34
#define L3D3_CRYSTAL_H 45

struct L3D3Crystal { float x, y; bool active; int bob; };
static L3D3Crystal l3d3_crystals[L3D3_MAX_CRYSTALS];

// special crystal
#define L3D3_SPECIAL_W 42
#define L3D3_SPECIAL_H 45
static bool  l3d3_specialActive = false;
static float l3d3_specialX = 0.0f, l3d3_specialY = 0.0f;
static int   l3d3_specialTimer = 0;
static int   l3d3_specialSpawnTimer = 0;

// ---------------------------------------------------------------- rocks
#define L3D3_MAX_ROCKS 4
#define L3D3_ROCK_W 52
#define L3D3_ROCK_H 62

struct L3D3Rock { float x, y, landY, speed; bool active; int warn; };
static L3D3Rock l3d3_rocks[L3D3_MAX_ROCKS];
static int l3d3_rockSpawnTimer = 0;

// ---------------------------------------------------------------- scorpions
#define L3D3_MAX_SCORPS 4
#define L3D3_SCORP_W 66
#define L3D3_SCORP_H 52

struct L3D3Scorp { float x, y, vx, vy; bool active; int respawn; };
static L3D3Scorp l3d3_scorps[L3D3_MAX_SCORPS];
static int l3d3_activeScorps = 2;

// ---------------------------------------------------------------- helpers
inline int l3d3_loadImg(const char* path)
{
	FILE* fp = NULL;
	fopen_s(&fp, path, "rb");
	if (fp == NULL) return -1;
	fclose(fp);
	return (int)iLoadImage((char*)path);
}

// root folder ebong Image/ folder duita thekei check kore load korbe
inline int l3d3_smartLoad(const char* fileName)
{
	int img = l3d3_loadImg(fileName);
	if (img >= 0) return img;

	char folderPath[260];
	sprintf_s(folderPath, sizeof(folderPath), "Image/%s", fileName);
	img = l3d3_loadImg(folderPath);
	if (img >= 0) return img;

	char doubleExt[260];
	sprintf_s(doubleExt, sizeof(doubleExt), "%s.png", fileName);
	img = l3d3_loadImg(doubleExt);
	if (img >= 0) return img;

	sprintf_s(doubleExt, sizeof(doubleExt), "Image/%s.png", fileName);
	return l3d3_loadImg(doubleExt);
}

inline bool l3d3_overlap(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh)
{
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}

inline void l3d3_placeCrystal(int i)
{
	for (int tries = 0; tries < 12; tries++) {
		float cx = (float)(L3D3_MIN_X + 40 + rand() % (L3D3_MAX_X - L3D3_MIN_X - 80));
		float cy = (float)(L3D3_MIN_Y + 20 + rand() % (L3D3_MAX_Y - L3D3_MIN_Y - 40));
		float dx = cx - l3d3_heroX, dy = cy - l3d3_heroY;
		if (dx * dx + dy * dy > 260.0f * 260.0f || tries == 11) {
			l3d3_crystals[i].x = cx;
			l3d3_crystals[i].y = cy;
			l3d3_crystals[i].active = true;
			l3d3_crystals[i].bob = rand() % 100;
			return;
		}
	}
}

inline void l3d3_spawnRock()
{
	for (int i = 0; i < L3D3_MAX_ROCKS; i++) {
		if (!l3d3_rocks[i].active) {
			l3d3_rocks[i].active = true;
			l3d3_rocks[i].x = (float)(L3D3_MIN_X + rand() % (L3D3_MAX_X - L3D3_MIN_X));
			l3d3_rocks[i].y = 600.0f;
			l3d3_rocks[i].landY = (float)(L3D3_MIN_Y + rand() % (L3D3_MAX_Y - L3D3_MIN_Y));
			l3d3_rocks[i].speed = 5.0f + (float)(rand() % 40) / 10.0f;
			l3d3_rocks[i].warn = 40;
			return;
		}
	}
}

inline void l3d3_spawnScorp(int i)
{
	l3d3_scorps[i].active = true;
	l3d3_scorps[i].respawn = 0;
	if (rand() % 2 == 0) {
		l3d3_scorps[i].x = (float)L3D3_MIN_X;
		l3d3_scorps[i].y = (float)(L3D3_MIN_Y + rand() % (L3D3_MAX_Y - L3D3_MIN_Y));
	}
	else {
		l3d3_scorps[i].x = (float)L3D3_MAX_X;
		l3d3_scorps[i].y = (float)(L3D3_MIN_Y + rand() % (L3D3_MAX_Y - L3D3_MIN_Y));
	}
	l3d3_scorps[i].vx = (rand() % 2 == 0) ? 1.6f : -1.6f;
	l3d3_scorps[i].vy = (rand() % 2 == 0) ? 1.1f : -1.1f;
}

// ---------------------------------------------------------------- setup
inline void L3Door3_GenerateTask()
{
	l3d3_heroX = 110.0f;
	l3d3_heroY = 110.0f;
	l3d3_heroMoving = false;
	l3d3_animFrame = 0;
	l3d3_animTimer = 0;

	l3d3_phase = L3D3_PHASE_INTRO;
	l3d3_phaseTimer = 150;
	l3d3_elapsed = 0;
	l3d3_collected = 0;
	l3d3_invuln = 0;
	l3d3_boostTimer = 0;
	l3d3_attackTimer = 0;
	l3d3_attackCooldown = 0;
	l3d3_prevSpace = false;
	l3d3_lightRadius = 330.0f;

	for (int i = 0; i < L3D3_MAX_CRYSTALS; i++) l3d3_placeCrystal(i);

	l3d3_specialActive = false;
	l3d3_specialTimer = 0;
	l3d3_specialSpawnTimer = 300;

	for (int i = 0; i < L3D3_MAX_ROCKS; i++) l3d3_rocks[i].active = false;
	l3d3_rockSpawnTimer = 120;

	l3d3_activeScorps = 2;
	for (int i = 0; i < L3D3_MAX_SCORPS; i++) {
		l3d3_scorps[i].active = false;
		l3d3_scorps[i].respawn = 0;
	}
	for (int i = 0; i < l3d3_activeScorps; i++) l3d3_spawnScorp(i);
}

// ---------------------------------------------------------------- damage
inline void l3d3_hurt(int amount, const char* why)
{
	if (l3d3_invuln > 0) return;
	level3_energy -= amount;
	l3d3_invuln = 110;
	level3_updateScore(-20);
	level3_playNegPointSound();
	strcpy_s(level3_message, sizeof(level3_message), why);
	level3_messageTimer = 70;
	if (level3_energy <= 0) {
		level3_energy = 0;
		level3_gameOver = true;
	}
}

// ---------------------------------------------------------------- darkness
inline void l3d3_drawDarkness(int screenW, float cx, float cy, float radius)
{
	iSetColor(0, 0, 0);
	for (int y = 0; y < 600; y += 6) {
		float dy = (float)(y + 3) - cy;
		if (fabsf(dy) >= radius) {
			iFilledRectangle(0, y, screenW, 6);
		}
		else {
			float half = sqrtf(radius * radius - dy * dy);
			int left = (int)(cx - half);
			int right = (int)(cx + half);
			if (left > 0) iFilledRectangle(0, y, left, 6);
			if (right < screenW) iFilledRectangle(right, y, screenW - right, 6);
		}
	}
}

// ---------------------------------------------------------------- render
inline void L3Door3_RenderTask(int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited = NULL, bool* insideTask = NULL)
{
	static int bgLocked = -2, bgOpen = -2;
	static int crystalImg = -1, specialImg = -1, rockImg = -1, scorpImg = -1;
	static int idleImg = -1;
	static int knifeFrames[7];
	static int runFrames[8];

	// ---------------- asset loading (once) ----------------
	if (bgLocked == -2) {
		bgLocked = l3d3_smartLoad("l3d31.png");
		bgOpen = l3d3_smartLoad("l3d32.png");
		crystalImg = l3d3_smartLoad("crystal.png");
		specialImg = l3d3_smartLoad("special_crystal.png");
		rockImg = l3d3_smartLoad("largeFallingRock.png");

		scorpImg = l3d3_smartLoad("rsz_gemini_generated_image_mvb0ammvb0ammvb0-removebg-preview.png");
		if (scorpImg < 0) scorpImg = l3d3_smartLoad("desert enemy 1.png");
		if (scorpImg < 0) scorpImg = l3d3_smartLoad("crab.png");

		idleImg = l3d3_smartLoad("idle_1.png");

		char framePath[64];
		for (int i = 0; i < 7; i++) {
			sprintf_s(framePath, sizeof(framePath), "knife_%d.png", i + 1);
			knifeFrames[i] = l3d3_smartLoad(framePath);
		}

		for (int i = 0; i < 8; i++) {
			sprintf_s(framePath, sizeof(framePath), "run_%d.png", i + 1);
			runFrames[i] = l3d3_smartLoad(framePath);
		}
	}

	// ---------------- fixed 60 fps tick ----------------
	static DWORD l3d3_lastTick = 0;
	DWORD now = GetTickCount();
	bool tick = false;
	if (now - l3d3_lastTick >= 16) { tick = true; l3d3_lastTick = now; }

	if (tick && !level3_gameOver && l3d3_phase != L3D3_PHASE_DONE) {

		if (l3d3_phase == L3D3_PHASE_INTRO) {
			l3d3_phaseTimer--;
			if (l3d3_phaseTimer <= 0) l3d3_phase = L3D3_PHASE_PLAY;
		}
		else {
			l3d3_elapsed++;
			if (l3d3_invuln > 0) l3d3_invuln--;
			if (l3d3_attackTimer > 0) l3d3_attackTimer--;
			if (l3d3_attackCooldown > 0) l3d3_attackCooldown--;
			if (l3d3_boostTimer > 0) l3d3_boostTimer--;

			// cave slowly gets darker : 330 -> 150 over the first 60 seconds
			float t = (float)l3d3_elapsed / (float)L3D3_MIN_FRAMES;
			if (t > 1.0f) t = 1.0f;
			l3d3_lightRadius = 330.0f - 180.0f * t;

			// ---------- hero movement ----------
			float spd = l3d3_heroSpeed * (l3d3_boostTimer > 0 ? 1.85f : 1.0f);
			l3d3_heroMoving = false;

			if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { l3d3_heroX += spd; l3d3_heroMoving = true; }
			if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { l3d3_heroX -= spd; l3d3_heroMoving = true; }
			if (GetAsyncKeyState(VK_UP) & 0x8000)    { l3d3_heroY += spd; l3d3_heroMoving = true; }
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { l3d3_heroY -= spd; l3d3_heroMoving = true; }

			float maxX = (float)L3D3_MAX_X;
			if (l3d3_heroX < (float)L3D3_MIN_X) l3d3_heroX = (float)L3D3_MIN_X;
			if (l3d3_heroX > maxX) l3d3_heroX = maxX;
			if (l3d3_heroY < (float)L3D3_MIN_Y) l3d3_heroY = (float)L3D3_MIN_Y;
			if (l3d3_heroY >(float)L3D3_MAX_Y) l3d3_heroY = (float)L3D3_MAX_Y;

			// ---------- knife swing ----------
			bool space = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
			if (space && !l3d3_prevSpace && l3d3_attackCooldown <= 0) {
				l3d3_attackTimer = 14;
				l3d3_attackCooldown = 45;

				float hcx = l3d3_heroX + L3D3_HERO_W / 2.0f;
				float hcy = l3d3_heroY + L3D3_HERO_H / 2.0f;
				for (int i = 0; i < L3D3_MAX_SCORPS; i++) {
					if (!l3d3_scorps[i].active) continue;
					float sx = l3d3_scorps[i].x + L3D3_SCORP_W / 2.0f;
					float sy = l3d3_scorps[i].y + L3D3_SCORP_H / 2.0f;
					float dx = sx - hcx, dy = sy - hcy;
					if (dx * dx + dy * dy <= 95.0f * 95.0f) {
						l3d3_scorps[i].active = false;
						l3d3_scorps[i].respawn = 200;
						level3_updateScore(75);
						level3_playPlusPointSound();
						strcpy_s(level3_message, sizeof(level3_message), "Scorpion crushed!");
						level3_messageTimer = 55;
						break;
					}
				}
			}
			l3d3_prevSpace = space;

			// ---------- crystals ----------
			if (l3d3_phase == L3D3_PHASE_PLAY) {
				for (int i = 0; i < L3D3_MAX_CRYSTALS; i++) {
					if (!l3d3_crystals[i].active) continue;
					l3d3_crystals[i].bob++;
					if (l3d3_overlap(l3d3_heroX + 12, l3d3_heroY, L3D3_HERO_W - 24, L3D3_HERO_H - 30,
						l3d3_crystals[i].x, l3d3_crystals[i].y, L3D3_CRYSTAL_W, L3D3_CRYSTAL_H)) {

						l3d3_crystals[i].active = false;
						l3d3_collected++;
						level3_energy += 8;
						if (level3_energy > 100) level3_energy = 100;
						level3_updateScore(120);
						level3_playPlusPointSound();

						if (l3d3_collected >= L3D3_NEED_CRYSTAL) {
							strcpy_s(level3_message, sizeof(level3_message), "3 crystals! Now survive until 60 seconds.");
							level3_messageTimer = 110;
						}
						else {
							char m[80];
							sprintf_s(m, sizeof(m), "Crystal %d of %d collected!", l3d3_collected, L3D3_NEED_CRYSTAL);
							strcpy_s(level3_message, sizeof(level3_message), m);
							level3_messageTimer = 80;
						}
						if (l3d3_collected < L3D3_NEED_CRYSTAL) l3d3_placeCrystal(i);
					}
				}
			}

			// ---------- special crystal ----------
			if (!l3d3_specialActive) {
				l3d3_specialSpawnTimer--;
				if (l3d3_specialSpawnTimer <= 0) {
					l3d3_specialActive = true;
					l3d3_specialTimer = 500;
					l3d3_specialX = (float)(L3D3_MIN_X + 40 + rand() % (L3D3_MAX_X - L3D3_MIN_X - 80));
					l3d3_specialY = (float)(L3D3_MIN_Y + 20 + rand() % (L3D3_MAX_Y - L3D3_MIN_Y - 40));
				}
			}
			else {
				l3d3_specialTimer--;
				if (l3d3_specialTimer <= 0) {
					l3d3_specialActive = false;
					l3d3_specialSpawnTimer = 350 + rand() % 150;
				}
				else if (l3d3_overlap(l3d3_heroX + 12, l3d3_heroY, L3D3_HERO_W - 24, L3D3_HERO_H - 30,
					l3d3_specialX, l3d3_specialY, L3D3_SPECIAL_W, L3D3_SPECIAL_H)) {
					l3d3_specialActive = false;
					l3d3_specialSpawnTimer = 350 + rand() % 150;
					l3d3_boostTimer = 400;
					level3_energy += 15;
					if (level3_energy > 100) level3_energy = 100;
					level3_updateScore(150);
					level3_playPlusPointSound();
					strcpy_s(level3_message, sizeof(level3_message), "Star crystal! Speed boost + Healed!");
					level3_messageTimer = 90;
				}
			}

			// ---------- falling rocks ----------
			l3d3_rockSpawnTimer--;
			if (l3d3_rockSpawnTimer <= 0) {
				l3d3_spawnRock();
				int base = 150 - (l3d3_elapsed / 40);
				if (base < 75) base = 75;
				l3d3_rockSpawnTimer = base + rand() % 45;
			}

			for (int i = 0; i < L3D3_MAX_ROCKS; i++) {
				if (!l3d3_rocks[i].active) continue;
				if (l3d3_rocks[i].warn > 0) { l3d3_rocks[i].warn--; continue; }

				l3d3_rocks[i].y -= l3d3_rocks[i].speed;

				if (l3d3_overlap(l3d3_heroX + 14, l3d3_heroY, L3D3_HERO_W - 28, L3D3_HERO_H,
					l3d3_rocks[i].x, l3d3_rocks[i].y, L3D3_ROCK_W, L3D3_ROCK_H)) {
					l3d3_rocks[i].active = false;
					l3d3_hurt(6, "A rock hit you! -6 HP"); // Stone porle energy loss 6 HP
				}
				else if (l3d3_rocks[i].y <= l3d3_rocks[i].landY) {
					l3d3_rocks[i].active = false;
				}
			}

			// ---------- scorpions ----------
			if (l3d3_elapsed == 1200 && l3d3_activeScorps < 3) { l3d3_activeScorps = 3; l3d3_spawnScorp(2); }
			if (l3d3_elapsed == 2400 && l3d3_activeScorps < 4) { l3d3_activeScorps = 4; l3d3_spawnScorp(3); }

			for (int i = 0; i < L3D3_MAX_SCORPS; i++) {
				if (!l3d3_scorps[i].active) {
					if (l3d3_scorps[i].respawn > 0) {
						l3d3_scorps[i].respawn--;
						if (l3d3_scorps[i].respawn == 0 && i < l3d3_activeScorps) l3d3_spawnScorp(i);
					}
					continue;
				}

				float hcx = l3d3_heroX + L3D3_HERO_W / 2.0f;
				float hcy = l3d3_heroY + L3D3_HERO_H / 2.0f;
				float sx = l3d3_scorps[i].x + L3D3_SCORP_W / 2.0f;
				float sy = l3d3_scorps[i].y + L3D3_SCORP_H / 2.0f;

				if (hcx > sx) l3d3_scorps[i].vx += 0.035f; else l3d3_scorps[i].vx -= 0.035f;
				if (hcy > sy) l3d3_scorps[i].vy += 0.030f; else l3d3_scorps[i].vy -= 0.030f;

				if (l3d3_scorps[i].vx > 2.3f)   l3d3_scorps[i].vx = 2.3f;
				if (l3d3_scorps[i].vx < -2.3f)  l3d3_scorps[i].vx = -2.3f;
				if (l3d3_scorps[i].vy > 1.9f)   l3d3_scorps[i].vy = 1.9f;
				if (l3d3_scorps[i].vy < -1.9f)  l3d3_scorps[i].vy = -1.9f;

				l3d3_scorps[i].x += l3d3_scorps[i].vx;
				l3d3_scorps[i].y += l3d3_scorps[i].vy;

				if (l3d3_scorps[i].x < (float)L3D3_MIN_X) { l3d3_scorps[i].x = (float)L3D3_MIN_X; l3d3_scorps[i].vx = -l3d3_scorps[i].vx; }
				if (l3d3_scorps[i].x >(float)L3D3_MAX_X) { l3d3_scorps[i].x = (float)L3D3_MAX_X; l3d3_scorps[i].vx = -l3d3_scorps[i].vx; }
				if (l3d3_scorps[i].y < (float)L3D3_MIN_Y) { l3d3_scorps[i].y = (float)L3D3_MIN_Y; l3d3_scorps[i].vy = -l3d3_scorps[i].vy; }
				if (l3d3_scorps[i].y >(float)L3D3_MAX_Y) { l3d3_scorps[i].y = (float)L3D3_MAX_Y; l3d3_scorps[i].vy = -l3d3_scorps[i].vy; }

				if (l3d3_overlap(l3d3_heroX + 14, l3d3_heroY, L3D3_HERO_W - 28, L3D3_HERO_H - 20,
					l3d3_scorps[i].x, l3d3_scorps[i].y, L3D3_SCORP_W, L3D3_SCORP_H)) {
					l3d3_hurt(4, "The scorpion stung you! -4 HP"); // Scorpion attack-e energy loss 4 HP
				}
			}

			// ---------- WIN CONDITION CHECK (AUTO-COMPLETE & EXIT) ----------
			if (l3d3_phase == L3D3_PHASE_PLAY &&
				l3d3_collected >= L3D3_NEED_CRYSTAL && l3d3_elapsed >= L3D3_MIN_FRAMES) {
				l3d3_phase = L3D3_PHASE_DONE;

				if (doorVisited) *doorVisited = true;
				if (insideTask) *insideTask = false;

				level3_updateScore(550);
				level3_playPlusPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "You escaped the cave! Door 3 unlocked.");
				level3_messageTimer = 130;
				for (int i = 0; i < L3D3_MAX_CRYSTALS; i++) l3d3_crystals[i].active = false;
				l3d3_specialActive = false;
			}

			// running animation
			if (l3d3_heroMoving) {
				l3d3_animTimer++;
				if (l3d3_animTimer >= L3D3_ANIM_DELAY) {
					l3d3_animTimer = 0;
					l3d3_animFrame = (l3d3_animFrame + 1) % 8;
				}
			}
			else { l3d3_animFrame = 0; l3d3_animTimer = 0; }
		}
	}

	// ================= DRAWING =================
	int bg = bgLocked;
	if (bg >= 0) iShowImage(0, 0, screenW, 600, bg);
	else { iSetColor(10, 12, 26); iFilledRectangle(0, 0, screenW, 600); }

	// ---- rock landing warnings (drawn on the floor) ----
	for (int i = 0; i < L3D3_MAX_ROCKS; i++) {
		if (l3d3_rocks[i].active && l3d3_rocks[i].warn > 0 && (l3d3_rocks[i].warn / 5) % 2 == 0) {
			iSetColor(230, 80, 60);
			iCircle(l3d3_rocks[i].x + L3D3_ROCK_W / 2, l3d3_rocks[i].landY + 10, 26);
			iCircle(l3d3_rocks[i].x + L3D3_ROCK_W / 2, l3d3_rocks[i].landY + 10, 22);
		}
	}

	// ---- scorpions ----
	for (int i = 0; i < L3D3_MAX_SCORPS; i++) {
		if (!l3d3_scorps[i].active) continue;
		if (scorpImg >= 0) {
			iShowImage((int)l3d3_scorps[i].x, (int)l3d3_scorps[i].y, L3D3_SCORP_W, L3D3_SCORP_H, scorpImg);
		}
		else {
			iSetColor(190, 90, 30);
			iFilledRectangle((int)l3d3_scorps[i].x, (int)l3d3_scorps[i].y, L3D3_SCORP_W, L3D3_SCORP_H);
		}
	}

	// ---- falling rocks ----
	for (int i = 0; i < L3D3_MAX_ROCKS; i++) {
		if (!l3d3_rocks[i].active || l3d3_rocks[i].warn > 0) continue;
		if (rockImg >= 0) {
			iShowImage((int)l3d3_rocks[i].x, (int)l3d3_rocks[i].y, L3D3_ROCK_W, L3D3_ROCK_H, rockImg);
		}
		else {
			iSetColor(120, 110, 100);
			iFilledCircle(l3d3_rocks[i].x + L3D3_ROCK_W / 2, l3d3_rocks[i].y + L3D3_ROCK_H / 2, 26);
		}
	}

	// ---- hero ----
	int heroImg = idleImg;
	if (l3d3_attackTimer > 0) {
		int frameIdx = (14 - l3d3_attackTimer) / 2;
		if (frameIdx < 0) frameIdx = 0;
		if (frameIdx > 6) frameIdx = 6;
		if (knifeFrames[frameIdx] >= 0) heroImg = knifeFrames[frameIdx];
	}
	else if (l3d3_heroMoving && runFrames[l3d3_animFrame] >= 0) {
		heroImg = runFrames[l3d3_animFrame];
	}

	bool blink = (l3d3_invuln > 0 && (l3d3_invuln / 5) % 2 == 0);
	if (!blink) {
		if (heroImg >= 0) iShowImage((int)l3d3_heroX, (int)l3d3_heroY, L3D3_HERO_W, L3D3_HERO_H, heroImg);
		else {
			iSetColor(235, 215, 160);
			iFilledRectangle((int)l3d3_heroX, (int)l3d3_heroY, L3D3_HERO_W, L3D3_HERO_H);
		}
	}

	// knife swing arc
	if (l3d3_attackTimer > 0) {
		iSetColor(255, 245, 180);
		iCircle(l3d3_heroX + L3D3_HERO_W / 2, l3d3_heroY + L3D3_HERO_H / 2, 95);
		iCircle(l3d3_heroX + L3D3_HERO_W / 2, l3d3_heroY + L3D3_HERO_H / 2, 92);
	}

	// speed boost aura
	if (l3d3_boostTimer > 0) {
		iSetColor(255, 220, 60);
		iCircle(l3d3_heroX + L3D3_HERO_W / 2, l3d3_heroY + L3D3_HERO_H / 2, 62);
	}

	// ---- darkness (small light radius around the hero) ----
	if (l3d3_phase != L3D3_PHASE_INTRO) {
		float flicker = (float)((l3d3_elapsed / 4) % 10) - 5.0f;
		l3d3_drawDarkness(screenW,
			l3d3_heroX + L3D3_HERO_W / 2.0f,
			l3d3_heroY + L3D3_HERO_H / 2.0f,
			l3d3_lightRadius + flicker);
	}

	// ---- crystals glow through the dark (drawn AFTER the darkness) ----
	for (int i = 0; i < L3D3_MAX_CRYSTALS; i++) {
		if (!l3d3_crystals[i].active) continue;
		float bob = sinf((float)l3d3_crystals[i].bob * 0.08f) * 4.0f;
		iSetColor(90, 190, 255);
		iCircle(l3d3_crystals[i].x + L3D3_CRYSTAL_W / 2, l3d3_crystals[i].y + bob + L3D3_CRYSTAL_H / 2, 26);
		if (crystalImg >= 0) {
			iShowImage((int)l3d3_crystals[i].x, (int)(l3d3_crystals[i].y + bob), L3D3_CRYSTAL_W, L3D3_CRYSTAL_H, crystalImg);
		}
		else {
			iSetColor(80, 200, 255);
			iFilledRectangle((int)l3d3_crystals[i].x, (int)(l3d3_crystals[i].y + bob), L3D3_CRYSTAL_W, L3D3_CRYSTAL_H);
		}
	}

	if (l3d3_specialActive) {
		iSetColor(255, 220, 70);
		iCircle(l3d3_specialX + L3D3_SPECIAL_W / 2, l3d3_specialY + L3D3_SPECIAL_H / 2, 30);
		iCircle(l3d3_specialX + L3D3_SPECIAL_W / 2, l3d3_specialY + L3D3_SPECIAL_H / 2, 26);
		if (specialImg >= 0) {
			iShowImage((int)l3d3_specialX, (int)l3d3_specialY, L3D3_SPECIAL_W, L3D3_SPECIAL_H, specialImg);
		}
		else {
			iSetColor(255, 220, 70);
			iFilledRectangle((int)l3d3_specialX, (int)l3d3_specialY, L3D3_SPECIAL_W, L3D3_SPECIAL_H);
		}
	}

	// ================= HUD =================
	char buf[120];

	if (l3d3_phase == L3D3_PHASE_INTRO) {
		iSetColor(255, 235, 150);
		iText(screenW / 2 - 215, 500, (char*)"THE MONSTER CAVE - collect 3 crystals and stay alive!", GLUT_BITMAP_HELVETICA_18);
		iSetColor(220, 220, 240);
		iText(screenW / 2 - 200, 472, (char*)"ARROW KEYS = move     SPACE = swing knife     Watch for falling rocks!", GLUT_BITMAP_HELVETICA_12);
	}

	// crystal counter
	for (int i = 0; i < L3D3_NEED_CRYSTAL; i++) {
		int cx = 260 + i * 30;
		if (i < l3d3_collected) iSetColor(90, 200, 255);
		else iSetColor(80, 80, 95);
		iFilledCircle(cx, 570, 10);
		iSetColor(230, 230, 250);
		iCircle(cx, 570, 10);
	}
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "Crystals %d/%d", (l3d3_collected > L3D3_NEED_CRYSTAL ? L3D3_NEED_CRYSTAL : l3d3_collected), L3D3_NEED_CRYSTAL);
	iText(250, 548, buf, GLUT_BITMAP_HELVETICA_12);

	// survival timer
	int secs = l3d3_elapsed / 60;
	if (secs > 60) secs = 60;
	sprintf_s(buf, sizeof(buf), "Survived: %02d / 60 s", secs);
	iSetColor(255, 255, 255);
	iText(screenW / 2 + 40, 570, buf, GLUT_BITMAP_HELVETICA_12);
	iSetColor(70, 70, 90);
	iFilledRectangle(screenW / 2 + 40, 550, 180, 12);
	iSetColor(90, 200, 255);
	iFilledRectangle(screenW / 2 + 40, 550, (180 * secs) / 60, 12);
	iSetColor(230, 230, 255);
	iRectangle(screenW / 2 + 40, 550, 180, 12);

	if (l3d3_boostTimer > 0) {
		iSetColor(255, 220, 60);
		sprintf_s(buf, sizeof(buf), "SPEED BOOST %ds", (l3d3_boostTimer / 60) + 1);
		iText(screenW / 2 - 60, 520, buf, GLUT_BITMAP_HELVETICA_12);
	}

	// score + energy
	sprintf_s(buf, sizeof(buf), "Score: %d", level3_score);
	iSetColor(255, 255, 255);
	iText(screenW - 150, 560, buf, GLUT_BITMAP_HELVETICA_18);

	iSetColor(200, 200, 200);
	iFilledRectangle(20, 560, 200, 20);
	iSetColor(level3_energy > 30 ? 0 : 220, level3_energy > 30 ? 200 : 20, 0);
	iFilledRectangle(20, 560, 2 * (level3_energy > 0 ? level3_energy : 0), 20);
	iSetColor(0, 0, 0);
	iRectangle(20, 560, 200, 20);
	iSetColor(255, 255, 255);
	iText(20, 545, (char*)"Energy", GLUT_BITMAP_HELVETICA_12);
}

// ---------------------------------------------------------------- click
inline bool L3Door3_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	if (l3d3_phase == L3D3_PHASE_DONE) {
		if (doorVisited) *doorVisited = true;
		if (insideTask) *insideTask = false;
		return true;
	}
	return false;
}

#endif