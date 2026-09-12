#ifndef LEVEL1_H_INCLUDED
#define LEVEL1_H_INCLUDED

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int gameState;   // shared with iMain.cpp (used to hop to Level 2 after winning)

// ---------------- GLOBAL SOUND TOGGLE STATE ----------------
extern bool isSoundMuted;

// ---------------- SOUND EFFECT HELPERS ----------------
inline void level1_playPlusPointSound() {
	if (isSoundMuted) return;
	mciSendString("close l1_sfx_plus", NULL, 0, NULL);
	mciSendString("open \"Audios/plusPoint.MP3\" type mpegvideo alias l1_sfx_plus", NULL, 0, NULL);
	mciSendString("play l1_sfx_plus from 0", NULL, 0, NULL);
}

inline void level1_playNegPointSound() {
	if (isSoundMuted) return;
	mciSendString("close l1_sfx_neg", NULL, 0, NULL);
	mciSendString("open \"Audios/negPoint.MP3\" type mpegvideo alias l1_sfx_neg", NULL, 0, NULL);
	mciSendString("play l1_sfx_neg from 0", NULL, 0, NULL);
}

inline void level1_playWinSound() {
	if (isSoundMuted) return;
	mciSendString("close l1_sfx_win", NULL, 0, NULL);
	mciSendString("open \"Audios/win_and_lose_melodies_-_arranged_win.MP3\" type mpegvideo alias l1_sfx_win", NULL, 0, NULL);
	mciSendString("play l1_sfx_win from 0", NULL, 0, NULL);
}

inline void level1_playLoseSound() {
	if (isSoundMuted) return;
	mciSendString("close l1_sfx_lose", NULL, 0, NULL);
	mciSendString("open \"Audios/win_and_lose_melodies_-_arranged_lose.MP3\" type mpegvideo alias l1_sfx_lose", NULL, 0, NULL);
	mciSendString("play l1_sfx_lose from 0", NULL, 0, NULL);
}

// ---------------- LEVEL STATE MACHINE ----------------
enum Level1State {
	L1_RUNNING,             // holding RIGHT to reach the doors, dodging cacti
	L1_DOOR_SELECT,         // 3 doors visible, waiting for a click
	L1_COMBAT,              // fighting the enemy behind a WRONG door
	L1_WRONG_DOOR_MSG,      // brief "enemy defeated, wrong door" message
	L1_PUZZLE,              // rune-lock memory puzzle (CORRECT door only)
	L1_KEY_FOUND,           // brief "you found the key!" banner
	L1_TREASURE,            // click the treasure box to open it with the key
	L1_RESULT,              // treasure opened - level complete screen
	L1_GAME_OVER            // hero's energy hit 0
};
static Level1State level1_state = L1_RUNNING;
static int level1_stateTimer = 0;   // generic frame counter for transient states
static bool level1_isPaused = false;
static bool level1_hasPlayedEndAudio = false;

// ---------------- IN-GAME SETTINGS POPUP MENU ----------------
static bool level1_showSettingsMenu = false;
#define LEVEL1_SETTING_BTN_X 945
#define LEVEL1_SETTING_BTN_Y 45
#define LEVEL1_SETTING_BTN_R 22

#define LEVEL1_SUB_BTN_R     20
#define LEVEL1_SUB_R_Y       100
#define LEVEL1_SUB_P_Y       150
#define LEVEL1_SUB_M_Y       200
#define LEVEL1_SUB_S_Y       250

enum EnemyType  { ENEMY_SCORPION, ENEMY_MUMMY };
enum WeaponType { WEAPON_SWORD, WEAPON_CLUB };

// ---------------- PLAYER / WORLD ----------------
static int  level1_playerX = 100, level1_playerY = 80;
static int  level1_playerWidth = 90, level1_playerHeight = 130;
static int  level1_playerSpeed = 6;

static int  level1_bgX = 0;
static int  level1_distanceCovered = 0;
#define TARGET_DISTANCE 1800

static bool level1_isMoving = false;
static int  level1_animFrame = 0;
static int  level1_animTimer = 0;
#define ANIM_FRAME_DELAY 6

// ---------------- JUMPING (over cacti) ----------------
#define GROUND_Y        80
#define JUMP_STRENGTH   16
#define GRAVITY_STEP     1
static bool  level1_isJumping = false;
static float level1_jumpVelocity = 0.0f;

// ---------------- CACTUS OBSTACLES ----------------
#define MAX_CACTUS          4
#define CACTUS_W          46
#define CACTUS_H          72
#define CACTUS_MIN_GAP   110
#define CACTUS_MAX_GAP   190
#define CACTUS_HIT_DAMAGE 12
#define CACTUS_HIT_INVULN 45

struct Cactus { float x; bool active; };
static Cactus level1_cacti[MAX_CACTUS];
static int level1_cactusSpawnTimer = 0;
static int level1_playerHurtTimer = 0;

// ---------------- ENERGY & SCORE & HIGHSCORE ----------------
#define PLAYER_MAX_ENERGY 100
static int level1_energy = PLAYER_MAX_ENERGY;
int level1_score = 0;
int level1_highScore = 0;
static bool level1_highScoreLoaded = false;

inline void level1_loadHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level1_highscore.txt", "r");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &level1_highScore);
		fclose(fp);
	}
	level1_highScoreLoaded = true;
}

inline void level1_saveHighScore()
{
	FILE* fp = NULL;
	fopen_s(&fp, "level1_highscore.txt", "w");
	if (fp != NULL) {
		fprintf(fp, "%d", level1_highScore);
		fclose(fp);
	}
}

inline void level1_updateScore(int addPoints)
{
	level1_score += addPoints;
	if (level1_score < 0) level1_score = 0;

	if (level1_score > level1_highScore) {
		level1_highScore = level1_score;
		level1_saveHighScore();
	}
}

// ---------------- DOORS ----------------
struct Level1Door { int x, y, width, height; bool visited; };

#define DOOR_WIDTH  120
#define DOOR_HEIGHT 180
#define DOOR_GAP    60
#define DOORS_START_X ((SCREEN_WIDTH - (3*DOOR_WIDTH + 2*DOOR_GAP)) / 2)
#define DOOR_Y 100

static Level1Door level1_doors[3];
static int level1_correctPath;
static int level1_chosenPath = -1;
static int level1_doorsOpened = 0;
static int level1_wrongDoorsResolved = 0;

// ---------------- RUNE-LOCK MEMORY PUZZLE ----------------
#define PUZZLE_TILES      4
#define PUZZLE_TILE_SIZE  110
#define PUZZLE_TILE_GAP    30
#define SEQUENCE_LEN       5
#define SHOW_LIT_FRAMES   22
#define SHOW_GAP_FRAMES   12
#define INTRO_FRAMES       50

enum PuzzlePhase { PZ_INTRO, PZ_SHOWING, PZ_INPUT };
struct PuzzleTile { int x, y, w, h; int colorIndex; };
static PuzzleTile level1_puzzleTiles[PUZZLE_TILES];

static PuzzlePhase level1_puzzlePhase;
static int  level1_puzzleSequence[SEQUENCE_LEN];
static int  level1_puzzleShowStep;
static int  level1_puzzleShowTimer;
static bool level1_puzzleShowLit;
static int  level1_puzzleInputIndex;

static int  level1_puzzleFlashTile = -1;
static int  level1_puzzleFlashTimer = 0;
static bool level1_puzzleFlashGood = false;

static bool level1_hasKey = false;

struct RGB { int r, g, b; };
static RGB level1_puzzleColors[PUZZLE_TILES] = {
	{ 205, 40, 40 },    // ruby
	{ 40, 160, 90 },    // emerald
	{ 50, 110, 210 },   // sapphire
	{ 225, 180, 30 }    // gold
};

// ---------------- TREASURE BOX ----------------
#define TREASURE_W 220
#define TREASURE_H 190
#define TREASURE_X (SCREEN_WIDTH/2 - TREASURE_W/2)
#define TREASURE_Y 140
static bool level1_treasureOpened = false;
static int  level1_treasureGlowTimer = 0;

// ---------------- COMBAT ----------------
#define SWORD_DMG_TO_SCORPION       15
#define CLUB_DMG_TO_MUMMY           18
#define ATTACK_COOLDOWN             16

#define SCORPION_MAX_ENERGY        100
#define SCORPION_ATK_DMG            10
#define SCORPION_ATK_COOLDOWN       55

#define MUMMY_MAX_ENERGY           120
#define MUMMY_ATK_DMG               12
#define MUMMY_ATK_COOLDOWN          50

#define HURT_FLASH_FRAMES           10

static EnemyType  level1_currentEnemy;
static WeaponType level1_currentWeapon;
static int level1_enemyMaxEnergy;
static int level1_enemyEnergy;
static int level1_attackCooldown;
static int level1_enemyAttackTimer;
static int level1_attackFlashTimer;
static int level1_enemyHurtTimer;
static int level1_enemyIdleTimer;
static int level1_enemyAttackAnimTimer;

#define ARENA_PLAYER_X  170
#define ARENA_PLAYER_Y   90
#define ARENA_ENEMY_Y    85
#define ARENA_ENEMY_W   220
#define ARENA_ENEMY_H   170

#define ARENA_ENEMY_START_X   920
#define ARENA_ENEMY_STOP_X    560
#define ENEMY_APPROACH_SPEED    5

enum CombatPhase { COMBAT_APPROACH, COMBAT_FIGHT };
static CombatPhase level1_combatPhase;
static int level1_enemyArenaX;

inline void level1_startPuzzle();
inline void level1_startCombat(EnemyType type);

// =====================================================================
//  SETUP
// =====================================================================
inline void setupLevel1()
{
	srand((unsigned int)time(0));

	if (!level1_highScoreLoaded) {
		level1_loadHighScore();
	}

	level1_correctPath = rand() % 3;

	level1_doors[0] = { DOORS_START_X, DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };
	level1_doors[1] = { DOORS_START_X + (DOOR_WIDTH + DOOR_GAP), DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };
	level1_doors[2] = { DOORS_START_X + 2 * (DOOR_WIDTH + DOOR_GAP), DOOR_Y, DOOR_WIDTH, DOOR_HEIGHT, false };

	level1_playerX = 100;
	level1_playerY = GROUND_Y;
	level1_bgX = 0;
	level1_distanceCovered = 0;
	level1_isMoving = false;
	level1_animFrame = 0;
	level1_animTimer = 0;

	level1_isJumping = false;
	level1_jumpVelocity = 0.0f;

	for (int i = 0; i < MAX_CACTUS; i++) level1_cacti[i] = { 0.0f, false };
	level1_cactusSpawnTimer = CACTUS_MIN_GAP + rand() % (CACTUS_MAX_GAP - CACTUS_MIN_GAP);
	level1_playerHurtTimer = 0;

	level1_energy = PLAYER_MAX_ENERGY;
	level1_score = 0;
	level1_chosenPath = -1;
	level1_doorsOpened = 0;
	level1_wrongDoorsResolved = 0;

	level1_hasKey = false;
	level1_treasureOpened = false;
	level1_treasureGlowTimer = 0;

	level1_attackCooldown = 0;
	level1_attackFlashTimer = 0;
	level1_enemyHurtTimer = 0;
	level1_enemyIdleTimer = 0;
	level1_enemyAttackAnimTimer = 0;

	level1_isPaused = false;
	level1_showSettingsMenu = false;
	level1_hasPlayedEndAudio = false;

	level1_state = L1_RUNNING;
	level1_stateTimer = 0;
}

// ---------------- TEXT HELPERS ----------------
inline void level1_drawBoldText(int x, int y, const char* str, void* font)
{
	iText(x, y, (char*)str, font);
	iText(x + 1, y, (char*)str, font);
	iText(x, y + 1, (char*)str, font);
	iText(x + 1, y + 1, (char*)str, font);
}

// ---------------- SHARED HUD ----------------
inline void level1_drawHUD()
{
	iSetColor(200, 200, 200);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(level1_energy > 30 ? 0 : 200, level1_energy > 30 ? 200 : 0, 0);
	iFilledRectangle(20, SCREEN_HEIGHT - 40, 2 * level1_energy, 20);
	iSetColor(0, 0, 0);
	iRectangle(20, SCREEN_HEIGHT - 40, 200, 20);
	iText(20, SCREEN_HEIGHT - 55, "Energy");

	char scoreBuf[64];
	iSetColor(0, 0, 0);
	sprintf_s(scoreBuf, sizeof(scoreBuf), "Score: %d", level1_score);
	level1_drawBoldText(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 40, scoreBuf, GLUT_BITMAP_HELVETICA_18);

	if (level1_hasKey) {
		iSetColor(0, 0, 0);
		iText(20, SCREEN_HEIGHT - 75, "You have the KEY");
	}
}

// ---------------- SETTINGS UI ----------------
inline void level1_drawSettingsUI()
{
	static int btnSettings = -1;
	static int btnRestart = -1;
	static int btnPauPlay = -1;
	static int btnPause = -1;
	static int btnMenu = -1;
	static int btnSoundOn = -1;
	static int btnSoundOff = -1;

	if (btnSettings == -1) {
		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnRestart = iLoadImage("Image/restart.png");
		if (btnRestart < 0) btnRestart = iLoadImage("restart.png");

		btnPauPlay = iLoadImage("Image/PauPlay.png");
		if (btnPauPlay < 0) btnPauPlay = iLoadImage("PauPlay.png");

		btnPause = iLoadImage("Image/pause.png");
		if (btnPause < 0) btnPause = iLoadImage("pause.png");

		btnMenu = iLoadImage("Image/menu.png");
		if (btnMenu < 0) btnMenu = iLoadImage("menu.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}

	// Settings Gear Icon (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(LEVEL1_SETTING_BTN_X - 22, LEVEL1_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SETTING_BTN_Y, LEVEL1_SETTING_BTN_R);
	}

	if (level1_showSettingsMenu) {
		// Restart (40x40 px)
		if (btnRestart >= 0) {
			iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_R_Y - 20, 40, 40, btnRestart);
		}
		else {
			iSetColor(220, 50, 50);
			iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_R_Y, LEVEL1_SUB_BTN_R);
			level1_drawBoldText(LEVEL1_SETTING_BTN_X - 6, LEVEL1_SUB_R_Y - 7, "R", GLUT_BITMAP_TIMES_ROMAN_24);
		}

		// Pause / Play (40x40 px)
		if (level1_isPaused) {
			if (btnPause >= 0) iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_P_Y - 20, 40, 40, btnPause);
			else {
				iSetColor(50, 130, 220);
				iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_P_Y, LEVEL1_SUB_BTN_R);
				level1_drawBoldText(LEVEL1_SETTING_BTN_X - 6, LEVEL1_SUB_P_Y - 7, "P", GLUT_BITMAP_TIMES_ROMAN_24);
			}
		}
		else {
			if (btnPauPlay >= 0) iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_P_Y - 20, 40, 40, btnPauPlay);
			else {
				iSetColor(50, 130, 220);
				iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_P_Y, LEVEL1_SUB_BTN_R);
				level1_drawBoldText(LEVEL1_SETTING_BTN_X - 6, LEVEL1_SUB_P_Y - 7, "P", GLUT_BITMAP_TIMES_ROMAN_24);
			}
		}

		// Menu (40x40 px)
		if (btnMenu >= 0) {
			iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_M_Y - 20, 40, 40, btnMenu);
		}
		else {
			iSetColor(45, 175, 75);
			iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_M_Y, LEVEL1_SUB_BTN_R);
			level1_drawBoldText(LEVEL1_SETTING_BTN_X - 8, LEVEL1_SUB_M_Y - 7, "M", GLUT_BITMAP_TIMES_ROMAN_24);
		}

		// Sound On/Off (40x40 px)
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_S_Y, LEVEL1_SUB_BTN_R);
				level1_drawBoldText(LEVEL1_SETTING_BTN_X - 6, LEVEL1_SUB_S_Y - 7, "S", GLUT_BITMAP_TIMES_ROMAN_24);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(LEVEL1_SETTING_BTN_X - 20, LEVEL1_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(LEVEL1_SETTING_BTN_X, LEVEL1_SUB_S_Y, LEVEL1_SUB_BTN_R);
				level1_drawBoldText(LEVEL1_SETTING_BTN_X - 6, LEVEL1_SUB_S_Y - 7, "S", GLUT_BITMAP_TIMES_ROMAN_24);
			}
		}
	}

	if (level1_isPaused && level1_state != L1_GAME_OVER && level1_state != L1_RESULT) {
		iSetColor(0, 0, 0);
		iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 30, "GAME PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
	}
}

// ---------------- VECTOR ART: WEAPONS, KEY, CACTUS ----------------
inline void level1_drawWeaponIcon(int cx, int cy, int size, WeaponType weapon)
{
	if (weapon == WEAPON_SWORD) {
		iSetColor(190, 190, 200);
		iFilledRectangle(cx - size / 10, cy - size / 2, size / 5, (int)(size * 0.75));
		iSetColor(120, 90, 40);
		iFilledRectangle(cx - size / 2, cy + size / 5, size, size / 8);
		iSetColor(90, 60, 20);
		iFilledRectangle(cx - size / 10, cy + size / 3, size / 5, size / 4);
	}
	else {
		iSetColor(110, 75, 35);
		iFilledRectangle(cx - size / 10, cy - size / 2, size / 5, (int)(size * 0.65));
		iSetColor(130, 130, 135);
		iFilledCircle(cx, cy + size / 6, size / 3);
		iSetColor(90, 90, 95);
		iFilledCircle(cx - size / 5, cy + size / 5, size / 10);
		iFilledCircle(cx + size / 5, cy + size / 8, size / 10);
		iFilledCircle(cx, cy + size / 3, size / 10);
	}
}

inline void level1_drawKeyIcon(int cx, int cy, int size)
{
	iSetColor(255, 215, 0);
	iFilledCircle(cx - size / 2, cy, size / 3);
	iFilledRectangle(cx - size / 4, cy - size / 10, size, size / 5);
	iFilledRectangle(cx + size / 2 - size / 6, cy - size / 3, size / 8, size / 5);
	iFilledRectangle(cx + size / 3, cy - size / 3, size / 8, size / 5);
}

inline void level1_drawCactus(float x, int groundY)
{
	int ix = (int)x;
	iSetColor(40, 120, 55);
	iFilledRectangle(ix + 13, groundY, 20, CACTUS_H);
	iFilledRectangle(ix, groundY + 24, 15, 28);
	iFilledRectangle(ix + 31, groundY + 34, 15, 24);
	iSetColor(30, 95, 42);
	iRectangle(ix + 13, groundY, 20, CACTUS_H);
	iSetColor(230, 230, 210);
	for (int s = 0; s < 5; s++) {
		iFilledRectangle(ix + 12, groundY + 8 + s * 13, 2, 4);
		iFilledRectangle(ix + 31, groundY + 8 + s * 13, 2, 4);
	}
}

inline void level1_drawDoorBackdrop(int mainBg, int doorClosedImg, int doorOpenImg)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainBg);
	for (int i = 0; i < 3; i++) {
		int imgToUse = level1_doors[i].visited ? doorOpenImg : doorClosedImg;
		iShowImage(level1_doors[i].x, level1_doors[i].y, level1_doors[i].width, level1_doors[i].height, imgToUse);
	}
}

// =====================================================================
//  RUNE-LOCK PUZZLE
// =====================================================================
inline void level1_setupPuzzleTiles()
{
	int totalW = PUZZLE_TILES * PUZZLE_TILE_SIZE + (PUZZLE_TILES - 1) * PUZZLE_TILE_GAP;
	int startX = SCREEN_WIDTH / 2 - totalW / 2;
	int y = 210;

	for (int i = 0; i < PUZZLE_TILES; i++) {
		level1_puzzleTiles[i].x = startX + i * (PUZZLE_TILE_SIZE + PUZZLE_TILE_GAP);
		level1_puzzleTiles[i].y = y;
		level1_puzzleTiles[i].w = PUZZLE_TILE_SIZE;
		level1_puzzleTiles[i].h = PUZZLE_TILE_SIZE;
		level1_puzzleTiles[i].colorIndex = i;
	}
}

inline void level1_startPuzzle()
{
	level1_setupPuzzleTiles();
	for (int i = 0; i < SEQUENCE_LEN; i++)
		level1_puzzleSequence[i] = rand() % PUZZLE_TILES;

	level1_puzzlePhase = PZ_INTRO;
	level1_puzzleShowStep = 0;
	level1_puzzleShowTimer = 0;
	level1_puzzleShowLit = false;
	level1_puzzleInputIndex = 0;
	level1_puzzleFlashTile = -1;
	level1_puzzleFlashTimer = 0;

	level1_state = L1_PUZZLE;
	level1_stateTimer = 0;
}

inline void level1_renderPuzzle(int mainBg, int doorClosedImg, int doorOpenImg)
{
	level1_drawDoorBackdrop(mainBg, doorClosedImg, doorOpenImg);

	iSetColor(255, 250, 235);
	iFilledRectangle(SCREEN_WIDTH / 2 - 320, 150, 640, 340);
	iSetColor(0, 0, 0);
	iRectangle(SCREEN_WIDTH / 2 - 320, 150, 640, 340);

	iSetColor(0, 0, 0);
	iText(SCREEN_WIDTH / 2 - 170, 460, "The RIGHT door - an ancient rune lock!");

	if (level1_puzzlePhase == PZ_INTRO) {
		iText(SCREEN_WIDTH / 2 - 150, 200, "Watch the glowing gems carefully...");
	}
	else if (level1_puzzlePhase == PZ_SHOWING) {
		iText(SCREEN_WIDTH / 2 - 70, 430, "Memorize this!");
	}
	else {
		char buf[64];
		sprintf(buf, "Repeat it! (%d / %d)", level1_puzzleInputIndex, SEQUENCE_LEN);
		iSetColor(0, 0, 0);
		iText(SCREEN_WIDTH / 2 - 90, 430, buf);
	}

	int litTileThisFrame = -1;
	if (level1_puzzlePhase == PZ_SHOWING && level1_puzzleShowLit)
		litTileThisFrame = level1_puzzleSequence[level1_puzzleShowStep];

	for (int i = 0; i < PUZZLE_TILES; i++) {
		PuzzleTile& t = level1_puzzleTiles[i];
		RGB c = level1_puzzleColors[t.colorIndex];

		bool flashing = (level1_puzzleFlashTile == i && level1_puzzleFlashTimer > 0);
		bool lit = (i == litTileThisFrame);

		if (flashing && level1_puzzleFlashGood)      iSetColor(255, 255, 255);
		else if (flashing && !level1_puzzleFlashGood) iSetColor(40, 40, 40);
		else if (lit)                                 iSetColor(255, 255, 255);
		else                                          iSetColor(c.r, c.g, c.b);

		iFilledRectangle(t.x, t.y, t.w, t.h);
		iSetColor(0, 0, 0);
		iRectangle(t.x, t.y, t.w, t.h);
		iSetColor(c.r, c.g, c.b);
		iRectangle(t.x + 6, t.y + 6, t.w - 12, t.h - 12);
	}

	level1_drawHUD();
}

inline void level1_puzzle_fixedUpdate()
{
	if (level1_puzzleFlashTimer > 0) level1_puzzleFlashTimer--;

	if (level1_puzzlePhase == PZ_INTRO) {
		level1_puzzleShowTimer++;
		if (level1_puzzleShowTimer >= INTRO_FRAMES) {
			level1_puzzlePhase = PZ_SHOWING;
			level1_puzzleShowStep = 0;
			level1_puzzleShowTimer = 0;
			level1_puzzleShowLit = true;
		}
	}
	else if (level1_puzzlePhase == PZ_SHOWING) {
		level1_puzzleShowTimer++;
		if (level1_puzzleShowLit) {
			if (level1_puzzleShowTimer >= SHOW_LIT_FRAMES) {
				level1_puzzleShowLit = false;
				level1_puzzleShowTimer = 0;
			}
		}
		else {
			if (level1_puzzleShowTimer >= SHOW_GAP_FRAMES) {
				level1_puzzleShowStep++;
				level1_puzzleShowTimer = 0;
				if (level1_puzzleShowStep >= SEQUENCE_LEN) {
					level1_puzzlePhase = PZ_INPUT;
					level1_puzzleInputIndex = 0;
				}
				else {
					level1_puzzleShowLit = true;
				}
			}
		}
	}
}

inline bool level1_pointInTile(int mx, int my, PuzzleTile& t)
{
	return (mx >= t.x && mx <= t.x + t.w && my >= t.y && my <= t.y + t.h);
}

inline void level1_handlePuzzleClick(int mx, int my)
{
	if (level1_puzzlePhase != PZ_INPUT) return;

	for (int i = 0; i < PUZZLE_TILES; i++) {
		if (!level1_pointInTile(mx, my, level1_puzzleTiles[i])) continue;

		level1_puzzleFlashTile = i;
		level1_puzzleFlashTimer = 14;

		if (level1_puzzleTiles[i].colorIndex == level1_puzzleSequence[level1_puzzleInputIndex]) {
			level1_puzzleFlashGood = true;
			level1_puzzleInputIndex++;

			if (level1_puzzleInputIndex >= SEQUENCE_LEN) {
				level1_updateScore(200);
				level1_playPlusPointSound();
				level1_hasKey = true;
				level1_state = L1_KEY_FOUND;
				level1_stateTimer = 0;
			}
		}
		else {
			level1_puzzleFlashGood = false;
			level1_puzzleInputIndex = 0;
			level1_playNegPointSound();
		}
		break;
	}
}

// =====================================================================
//  COMBAT
// =====================================================================
inline void level1_startCombat(EnemyType type)
{
	level1_currentEnemy = type;

	if (type == ENEMY_SCORPION) {
		level1_currentWeapon = WEAPON_SWORD;
		level1_enemyMaxEnergy = SCORPION_MAX_ENERGY;
	}
	else {
		level1_currentWeapon = WEAPON_CLUB;
		level1_enemyMaxEnergy = MUMMY_MAX_ENERGY;
	}

	level1_enemyEnergy = level1_enemyMaxEnergy;
	level1_attackCooldown = 0;
	level1_enemyAttackTimer = (type == ENEMY_SCORPION) ? SCORPION_ATK_COOLDOWN : MUMMY_ATK_COOLDOWN;
	level1_attackFlashTimer = 0;
	level1_enemyHurtTimer = 0;
	level1_enemyIdleTimer = 0;
	level1_enemyAttackAnimTimer = 0;
	level1_playerHurtTimer = 0;

	level1_combatPhase = COMBAT_APPROACH;
	level1_enemyArenaX = ARENA_ENEMY_START_X;

	level1_state = L1_COMBAT;
	level1_stateTimer = 0;
}

inline void level1_renderCombat(int pathBg, int heroIdleImg,
	int scorpionIdle, int scorpionAttack, int mummyIdle, int mummyAttack)
{
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, pathBg);

	int playerX = ARENA_PLAYER_X, playerY = ARENA_PLAYER_Y;
	int enemyX = level1_enemyArenaX, enemyY = ARENA_ENEMY_Y;

	// Hero
	iShowImage(playerX, playerY, level1_playerWidth, level1_playerHeight, heroIdleImg);
	if (level1_playerHurtTimer > 0 && (level1_playerHurtTimer / 3) % 2 == 0) {
		iSetColor(255, 60, 60);
		iFilledRectangle(playerX, playerY, level1_playerWidth, level1_playerHeight);
	}
	level1_drawWeaponIcon(playerX + level1_playerWidth + 25, playerY + 70, 60, level1_currentWeapon);

	if (level1_attackFlashTimer > 0) {
		iSetColor(255, 255, 0);
		iFilledRectangle(playerX + level1_playerWidth + 10, playerY + 60, 65, 10);
	}

	// Enemy
	int idleImg = (level1_currentEnemy == ENEMY_SCORPION) ? scorpionIdle : mummyIdle;
	int attackImg = (level1_currentEnemy == ENEMY_SCORPION) ? scorpionAttack : mummyAttack;
	int enemyImg = (level1_enemyAttackAnimTimer > 0) ? attackImg : idleImg;

	bool hurtBlink = (level1_enemyHurtTimer > 0 && (level1_enemyHurtTimer / 3) % 2 == 0);
	if (hurtBlink) {
		iSetColor(255, 90, 70);
		iFilledRectangle(enemyX, enemyY, ARENA_ENEMY_W, ARENA_ENEMY_H);
	}
	else {
		iShowImage(enemyX, enemyY, ARENA_ENEMY_W, ARENA_ENEMY_H, enemyImg);
	}

	level1_drawHUD();

	iSetColor(200, 200, 200);
	iFilledRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, 200, 20);
	iSetColor(200, 0, 0);
	iFilledRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, (int)(2.0 * level1_enemyEnergy * 100 / level1_enemyMaxEnergy), 20);
	iSetColor(0, 0, 0);
	iRectangle(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 40, 200, 20);
	iText(SCREEN_WIDTH - 220, SCREEN_HEIGHT - 55,
		(char*)(level1_currentEnemy == ENEMY_SCORPION ? "Scorpion" : "Mummy"));

	iSetColor(255, 255, 255);
	if (level1_combatPhase == COMBAT_APPROACH)
		iText(SCREEN_WIDTH / 2 - 130, 30,
		(char*)(level1_currentEnemy == ENEMY_SCORPION ? "A scorpion scuttles closer..." : "A mummy shambles closer..."));
	else
		iText(SCREEN_WIDTH / 2 - 110, 30, "SPACE = attack with your weapon");
}

inline void level1_combat_fixedUpdate()
{
	if (level1_attackFlashTimer > 0) level1_attackFlashTimer--;
	if (level1_enemyHurtTimer > 0) level1_enemyHurtTimer--;
	if (level1_playerHurtTimer > 0) level1_playerHurtTimer--;
	if (level1_enemyAttackAnimTimer > 0) level1_enemyAttackAnimTimer--;
	level1_enemyIdleTimer++;

	if (level1_combatPhase == COMBAT_APPROACH) {
		level1_enemyArenaX -= ENEMY_APPROACH_SPEED;
		if (level1_enemyArenaX <= ARENA_ENEMY_STOP_X) {
			level1_enemyArenaX = ARENA_ENEMY_STOP_X;
			level1_combatPhase = COMBAT_FIGHT;
			level1_attackCooldown = 0;
			level1_enemyAttackTimer = (level1_currentEnemy == ENEMY_SCORPION) ? SCORPION_ATK_COOLDOWN : MUMMY_ATK_COOLDOWN;
		}
		return;
	}

	if (level1_attackCooldown > 0) level1_attackCooldown--;

	if (isKeyPressed(' ') && level1_attackCooldown <= 0) {
		int dmg = (level1_currentEnemy == ENEMY_SCORPION) ? SWORD_DMG_TO_SCORPION : CLUB_DMG_TO_MUMMY;
		level1_enemyEnergy -= dmg;
		if (level1_enemyEnergy < 0) level1_enemyEnergy = 0;
		level1_attackCooldown = ATTACK_COOLDOWN;
		level1_attackFlashTimer = 6;
		level1_enemyHurtTimer = HURT_FLASH_FRAMES;
		level1_playPlusPointSound();
	}

	if (level1_enemyEnergy > 0) {
		level1_enemyAttackTimer--;
		if (level1_enemyAttackTimer <= 0) {
			int dmg = (level1_currentEnemy == ENEMY_SCORPION) ? SCORPION_ATK_DMG : MUMMY_ATK_DMG;
			level1_energy -= dmg;
			if (level1_energy < 0) level1_energy = 0;
			level1_playerHurtTimer = HURT_FLASH_FRAMES;
			level1_enemyAttackAnimTimer = 14;
			level1_enemyAttackTimer = (level1_currentEnemy == ENEMY_SCORPION) ? SCORPION_ATK_COOLDOWN : MUMMY_ATK_COOLDOWN;
			level1_playNegPointSound();
		}
	}

	if (level1_energy <= 0) {
		level1_state = L1_GAME_OVER;
		level1_stateTimer = 0;
		return;
	}

	if (level1_enemyEnergy <= 0) {
		level1_updateScore((level1_currentEnemy == ENEMY_SCORPION) ? 50 : 75);
		level1_playPlusPointSound();
		level1_doors[level1_chosenPath].visited = true;
		level1_doorsOpened++;
		level1_wrongDoorsResolved++;
		level1_state = L1_WRONG_DOOR_MSG;
		level1_stateTimer = 0;
	}
}

// =====================================================================
//  TREASURE BOX
// =====================================================================
inline void level1_drawTreasureBox(int treasureImg, bool opened)
{
	iShowImage(TREASURE_X, TREASURE_Y, TREASURE_W, TREASURE_H, treasureImg);

	if (opened) {
		int pulse = 40 + (int)(30 * ((level1_treasureGlowTimer % 40) / 40.0));
		iSetColor(255, 215, 0);
		iFilledCircle(TREASURE_X + TREASURE_W / 2, TREASURE_Y + TREASURE_H / 2, pulse / 6);

		int sparkleOffsets[4][2] = { { -70, 40 }, { 70, 30 }, { -40, -50 }, { 55, -45 } };
		for (int i = 0; i < 4; i++) {
			int sx = TREASURE_X + TREASURE_W / 2 + sparkleOffsets[i][0];
			int sy = TREASURE_Y + TREASURE_H / 2 + sparkleOffsets[i][1];
			iSetColor(255, 250, 200);
			iFilledCircle(sx, sy, 5);
		}
	}
}

// =====================================================================
//  RUNNING PHASE HELPERS
// =====================================================================
inline void level1_updateJumpPhysics()
{
	if (isSpecialKeyPressed(GLUT_KEY_UP) && !level1_isJumping) {
		level1_isJumping = true;
		level1_jumpVelocity = JUMP_STRENGTH;
	}
	if (level1_isJumping) {
		level1_playerY += (int)level1_jumpVelocity;
		level1_jumpVelocity -= GRAVITY_STEP;
		if (level1_playerY <= GROUND_Y) {
			level1_playerY = GROUND_Y;
			level1_isJumping = false;
			level1_jumpVelocity = 0.0f;
		}
	}
}

inline void level1_updateCacti()
{
	level1_cactusSpawnTimer--;
	if (level1_cactusSpawnTimer <= 0) {
		for (int i = 0; i < MAX_CACTUS; i++) {
			if (!level1_cacti[i].active) {
				level1_cacti[i].active = true;
				level1_cacti[i].x = (float)(SCREEN_WIDTH + rand() % 120);
				break;
			}
		}
		level1_cactusSpawnTimer = CACTUS_MIN_GAP + rand() % (CACTUS_MAX_GAP - CACTUS_MIN_GAP);
	}

	for (int i = 0; i < MAX_CACTUS; i++) {
		if (!level1_cacti[i].active) continue;
		level1_cacti[i].x -= level1_playerSpeed;
		if (level1_cacti[i].x + CACTUS_W < 0) level1_cacti[i].active = false;
	}

	if (level1_playerHurtTimer > 0) level1_playerHurtTimer--;

	int hx0 = level1_playerX + 18, hx1 = level1_playerX + level1_playerWidth - 18;
	int hy0 = level1_playerY;

	for (int i = 0; i < MAX_CACTUS; i++) {
		if (!level1_cacti[i].active) continue;
		int cx0 = (int)level1_cacti[i].x, cx1 = (int)level1_cacti[i].x + CACTUS_W;

		bool xOverlap = (hx1 >= cx0 && hx0 <= cx1);
		bool tooLowToClear = (hy0 < GROUND_Y + CACTUS_H - 10);

		if (xOverlap && tooLowToClear && level1_playerHurtTimer <= 0) {
			level1_energy -= CACTUS_HIT_DAMAGE;
			if (level1_energy < 0) level1_energy = 0;
			level1_playerHurtTimer = CACTUS_HIT_INVULN;
			level1_cacti[i].active = false;
			level1_playNegPointSound();

			if (level1_energy <= 0) {
				level1_state = L1_GAME_OVER;
				level1_stateTimer = 0;
				return;
			}
		}
	}
}

// =====================================================================
//  RENDER
// =====================================================================
inline void renderLevel1()
{
	static int mainBg = -1, doorClosedImg = -1, doorOpenImg = -1, idleImg = -1;
	static int pathBg = -1, treasureImg = -1;
	static int bgSeaScoreImg = -1, bgSeaOutImg = -1;
	static int runFrames[8];
	static int scorpionIdle = -1, scorpionAttack = -1, mummyIdle = -1, mummyAttack = -1;

	if (mainBg == -1) {
		mainBg = iLoadImage("Image/gameBackground.png");
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

		pathBg = iLoadImage("Image/pathbackground.png");
		treasureImg = iLoadImage("Image/treasureBox.png");

		bgSeaScoreImg = iLoadImage("Image/bgSeaScore.png");
		bgSeaOutImg = iLoadImage("Image/bgSeaOut.png");

		scorpionIdle = iLoadImage("Image/rsz_gemini_generated_image_mvb0ammvb0ammvb0-removebg-preview.png");
		scorpionAttack = iLoadImage("Image/rsz_gemini_generated_image_1x0e3h1x0e3h1x0e-removebg-preview.png");
		mummyIdle = iLoadImage("Image/rsz_mummy-removebg-preview.png");
		mummyAttack = iLoadImage("Image/mummyAttack.png");
	}

	switch (level1_state) {

	case L1_RUNNING:
	case L1_DOOR_SELECT:
	{
						   iShowImage(level1_bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainBg);
						   iShowImage(level1_bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainBg);
						   iShowImage(level1_bgX - SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainBg);

						   if (level1_state == L1_RUNNING) {
							   for (int i = 0; i < MAX_CACTUS; i++)
							   if (level1_cacti[i].active) level1_drawCactus(level1_cacti[i].x, GROUND_Y);
						   }

						   int playerImg = level1_isMoving ? runFrames[level1_animFrame] : idleImg;
						   iShowImage(level1_playerX, level1_playerY, level1_playerWidth, level1_playerHeight, playerImg);
						   if (level1_playerHurtTimer > 0 && (level1_playerHurtTimer / 3) % 2 == 0) {
							   iSetColor(255, 60, 60);
							   iFilledRectangle(level1_playerX, level1_playerY, level1_playerWidth, level1_playerHeight);
						   }

						   if (level1_state == L1_DOOR_SELECT) {
							   for (int i = 0; i < 3; i++) {
								   int imgToUse = level1_doors[i].visited ? doorOpenImg : doorClosedImg;
								   iShowImage(level1_doors[i].x, level1_doors[i].y, level1_doors[i].width, level1_doors[i].height, imgToUse);
							   }
						   }

						   level1_drawHUD();

						   iSetColor(0, 0, 0);
						   if (level1_state == L1_RUNNING)
							   iText(SCREEN_WIDTH / 2 - 190, SCREEN_HEIGHT - 40, "Hold RIGHT to run - UP to jump over cacti!");
						   else
							   iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 40, "Click an unopened door");
						   break;
	}

	case L1_COMBAT:
		level1_renderCombat(pathBg, idleImg, scorpionIdle, scorpionAttack, mummyIdle, mummyAttack);
		break;

	case L1_WRONG_DOOR_MSG:
	{
							  level1_drawDoorBackdrop(mainBg, doorClosedImg, doorOpenImg);
							  iSetColor(0, 0, 0);
							  iText(SCREEN_WIDTH / 2 - 200, 400,
								  (char*)(level1_currentEnemy == ENEMY_SCORPION
								  ? "Scorpion defeated - nothing here. Try another door!"
								  : "Mummy defeated - nothing here. Try the last door!"));
							  level1_drawHUD();
							  break;
	}

	case L1_PUZZLE:
		level1_renderPuzzle(mainBg, doorClosedImg, doorOpenImg);
		break;

	case L1_KEY_FOUND:
	{
						 level1_drawDoorBackdrop(mainBg, doorClosedImg, doorOpenImg);
						 level1_drawKeyIcon(SCREEN_WIDTH / 2, 350, 120);
						 iSetColor(0, 0, 0);
						 iText(SCREEN_WIDTH / 2 - 90, 250, "YOU FOUND THE KEY!");
						 iText(SCREEN_WIDTH / 2 - 170, 220, "Go find the treasure box to unlock it...");
						 level1_drawHUD();
						 break;
	}

	case L1_TREASURE:
	{
						iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainBg);
						level1_drawTreasureBox(treasureImg, false);
						iSetColor(0, 0, 0);
						iText(SCREEN_WIDTH / 2 - 140, 480, "Click the treasure box to unlock it!");
						level1_drawHUD();
						break;
	}

	case L1_RESULT:
	{
					  if (!level1_hasPlayedEndAudio) {
						  level1_playWinSound();
						  level1_hasPlayedEndAudio = true;
					  }

					  if (bgSeaScoreImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaScoreImg);
					  else {
						  iSetColor(15, 35, 55);
						  iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
					  }

					  char yourScoreBuf[64];
					  sprintf_s(yourScoreBuf, sizeof(yourScoreBuf), "Your Score: %d", level1_score);
					  iSetColor(255, 255, 255);
					  level1_drawBoldText(395, 185, yourScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);

					  char highScoreBuf[64];
					  sprintf_s(highScoreBuf, sizeof(highScoreBuf), "HighScore: %d", level1_highScore);
					  iSetColor(255, 240, 180);
					  level1_drawBoldText(395, 125, highScoreBuf, GLUT_BITMAP_TIMES_ROMAN_24);
					  break;
	}

	case L1_GAME_OVER:
	{
						 if (!level1_hasPlayedEndAudio) {
							 level1_playLoseSound();
							 level1_hasPlayedEndAudio = true;
						 }

						 if (bgSeaOutImg >= 0) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgSeaOutImg);
						 else {
							 iSetColor(225, 230, 235);
							 iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
						 }

						 iSetColor(180, 25, 25);
						 level1_drawBoldText(440, 365, "YOU ARE OUT!", GLUT_BITMAP_TIMES_ROMAN_24);

						 char endScoreBuf[100];
						 sprintf_s(endScoreBuf, sizeof(endScoreBuf), "Score: %d     |     High Score: %d", level1_score, level1_highScore);
						 iSetColor(15, 35, 75);
						 level1_drawBoldText(390, 315, endScoreBuf, GLUT_BITMAP_HELVETICA_18);
						 break;
	}
	}

	level1_drawSettingsUI();
}

// =====================================================================
//  FIXED UPDATE
// =====================================================================
inline void level1_fixedUpdate()
{
	static bool level1_prevRKey = false;
	bool rNow = (isKeyPressed('r') != 0) || (isKeyPressed('R') != 0);
	if (rNow && !level1_prevRKey) {
		setupLevel1();
		level1_prevRKey = rNow;
		return;
	}
	level1_prevRKey = rNow;

	if (level1_isPaused || level1_state == L1_RESULT || level1_state == L1_GAME_OVER) return;

	switch (level1_state) {

	case L1_RUNNING:
		level1_isMoving = false;
		if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
			level1_isMoving = true;
			level1_bgX -= level1_playerSpeed;
			if (level1_bgX <= -SCREEN_WIDTH) level1_bgX = 0;

			level1_distanceCovered += level1_playerSpeed;
			level1_updateScore(1);
			if (level1_distanceCovered >= TARGET_DISTANCE) {
				level1_state = L1_DOOR_SELECT;
			}
		}
		else if (isSpecialKeyPressed(GLUT_KEY_LEFT) && level1_distanceCovered > 0) {
			level1_isMoving = true;
			level1_bgX += level1_playerSpeed;
			if (level1_bgX >= SCREEN_WIDTH) level1_bgX = 0;
			level1_distanceCovered -= level1_playerSpeed;
			if (level1_distanceCovered < 0) level1_distanceCovered = 0;
		}

		if (level1_isMoving) {
			level1_animTimer++;
			if (level1_animTimer >= ANIM_FRAME_DELAY) {
				level1_animTimer = 0;
				level1_animFrame = (level1_animFrame + 1) % 8;
			}
			level1_updateCacti();
		}

		level1_updateJumpPhysics();
		break;

	case L1_DOOR_SELECT:
		break;

	case L1_COMBAT:
		level1_combat_fixedUpdate();
		break;

	case L1_WRONG_DOOR_MSG:
		level1_stateTimer++;
		if (level1_stateTimer >= 70) level1_state = L1_DOOR_SELECT;
		break;

	case L1_PUZZLE:
		level1_puzzle_fixedUpdate();
		break;

	case L1_KEY_FOUND:
		level1_stateTimer++;
		if (level1_stateTimer >= 60) {
			level1_state = L1_TREASURE;
			level1_stateTimer = 0;
		}
		break;

	case L1_TREASURE:
		level1_treasureGlowTimer++;
		break;

	default:
		break;
	}
}

// =====================================================================
//  INPUT
// =====================================================================
inline bool level1_isInside(int px, int py, Level1Door d)
{
	return (px >= d.x && px <= d.x + d.width && py >= d.y && py <= d.y + d.height);
}

inline bool level1_pointInTreasure(int mx, int my)
{
	return (mx >= TREASURE_X && mx <= TREASURE_X + TREASURE_W &&
		my >= TREASURE_Y && my <= TREASURE_Y + TREASURE_H);
}

inline void handleLevel1DoorClicks(int mx, int my)
{
	float distSettings = sqrtf((float)((mx - LEVEL1_SETTING_BTN_X) * (mx - LEVEL1_SETTING_BTN_X) +
		(my - LEVEL1_SETTING_BTN_Y) * (my - LEVEL1_SETTING_BTN_Y)));
	if (distSettings <= LEVEL1_SETTING_BTN_R) {
		level1_showSettingsMenu = !level1_showSettingsMenu;
		return;
	}

	if (level1_showSettingsMenu) {
		float distR = sqrtf((float)((mx - LEVEL1_SETTING_BTN_X) * (mx - LEVEL1_SETTING_BTN_X) +
			(my - LEVEL1_SUB_R_Y) * (my - LEVEL1_SUB_R_Y)));
		if (distR <= LEVEL1_SUB_BTN_R) {
			setupLevel1();
			return;
		}

		float distP = sqrtf((float)((mx - LEVEL1_SETTING_BTN_X) * (mx - LEVEL1_SETTING_BTN_X) +
			(my - LEVEL1_SUB_P_Y) * (my - LEVEL1_SUB_P_Y)));
		if (distP <= LEVEL1_SUB_BTN_R) {
			level1_isPaused = !level1_isPaused;
			return;
		}

		float distM = sqrtf((float)((mx - LEVEL1_SETTING_BTN_X) * (mx - LEVEL1_SETTING_BTN_X) +
			(my - LEVEL1_SUB_M_Y) * (my - LEVEL1_SUB_M_Y)));
		if (distM <= LEVEL1_SUB_BTN_R) {
			gameState = 5;
			return;
		}

		// Sound 'S' Toggle Click
		float distS = sqrtf((float)((mx - LEVEL1_SETTING_BTN_X) * (mx - LEVEL1_SETTING_BTN_X) +
			(my - LEVEL1_SUB_S_Y) * (my - LEVEL1_SUB_S_Y)));
		if (distS <= LEVEL1_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop bgMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	if (level1_isPaused) return;

	if (level1_state == L1_DOOR_SELECT) {
		for (int i = 0; i < 3; i++) {
			if (!level1_doors[i].visited && level1_isInside(mx, my, level1_doors[i])) {
				level1_chosenPath = i;
				if (i == level1_correctPath) {
					level1_doors[i].visited = true;
					level1_doorsOpened++;
					level1_startPuzzle();
				}
				else {
					EnemyType nextEnemy = (level1_wrongDoorsResolved == 0) ? ENEMY_SCORPION : ENEMY_MUMMY;
					level1_startCombat(nextEnemy);
				}
				break;
			}
		}
	}
	else if (level1_state == L1_PUZZLE) {
		level1_handlePuzzleClick(mx, my);
	}
	else if (level1_state == L1_TREASURE) {
		if (level1_pointInTreasure(mx, my)) {
			level1_treasureOpened = true;
			level1_updateScore(500);
			level1_playPlusPointSound();
			level1_treasureGlowTimer = 0;
			level1_state = L1_RESULT;
			level1_stateTimer = 0;
		}
	}
}

inline void handleLevel1Keyboard(unsigned char key)
{
	if (key == 'r' || key == 'R') setupLevel1();
	if (key == 'p' || key == 'P') level1_isPaused = !level1_isPaused;
	if (key == 'm' || key == 'M') gameState = 5;
	if (key == 's' || key == 'S') {
		isSoundMuted = !isSoundMuted;
		if (isSoundMuted) {
			mciSendString("stop bgMusic", NULL, 0, NULL);
		}
	}
	if ((key == 'n' || key == 'N') && level1_state == L1_RESULT) gameState = 6;
}

#endif