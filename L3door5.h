#ifndef L3DOOR5_H_INCLUDED
#define L3DOOR5_H_INCLUDED

// =================================================================
//  DOOR 5 - "THE MYSTICAL MINE" (smooth-camera version)
//  10 platforms hover over lava and slide LEFT / RIGHT. Press UP to
//  hop to the next one. If the next platform is close enough when you
//  press UP, you leap onto it; if it has swung too far away, the
//  character hops short and drops into the lava (lose a life).
//
//  - The camera EASES toward the current platform instead of snapping.
//  - The three backgrounds are one wide panorama that slides slowly
//    with the camera (no hard switch / crossfade zones any more).
//  - The hop is done in world coordinates and homes in on the moving
//    platform, so it always lands exactly on it.
//
//  ENDING: landing on the final ledge pops up a treasure chest. Click it
//  -> sum-lock puzzle (3 dials must add up to the target rune)
//  -> opened chest -> win page + reward.
//
//  Hooks: level3door5_active, startL3Door5Path(), renderL3Door5Path(),
//  updateL3Door5PathIfActive(), and NEW level3door5_handleClick(mx, my)
//  (see the one-line edit needed in handleLevel3Clicks in Level3.h)
// =================================================================

static bool level3door5_active = false;

#define L3D5_NUM_STEPS 10

// ---- image files (change the names to match yours) ----
#define L3D5_HEARTS3_FILE "Image/3_heart.png"   // 3 hearts filled
#define L3D5_HEARTS2_FILE "Image/2_heart.png"   // 2 filled, 1 blank
#define L3D5_HEARTS1_FILE "Image/1_heart.png"   // 1 filled, 2 blank
#define L3D5_CHEST_CLOSED_FILE "Image/treasure_closed.png"
#define L3D5_CHEST_OPEN_FILE   "Image/treasure_opened.png"

// ---- hearts (top-left, under the Energy bar) ----
#define L3D5_HEARTS_X 20
#define L3D5_HEARTS_Y (SCREEN_HEIGHT - 125)
#define L3D5_HEARTS_W 150
#define L3D5_HEARTS_H 50

// ---- treasure chest pop-up (centre of the screen) ----
#define L3D5_CHEST_W 220
#define L3D5_CHEST_H 180
#define L3D5_CHEST_CY 300
#define L3D5_POP_FRAMES 20              // length of the pop-in animation
#define L3D5_OPEN_DELAY 110             // frames the opened chest stays before the win page
#define L3D5_CROSS_SCORE 400            // reward for reaching the ledge
#define L3D5_TREASURE_SCORE 1000        // reward for opening the chest

// ---- sum-lock puzzle: click the 3 dials until their digits add up to the target rune ----
#define L3D5_PUZZLE_BOARD_FILE "Image/door5_puzzle_board.png"
#define L3D5_TARGET_FILE_FMT   "Image/%d.png"  // door5_target_21.png, door5_target_25.png, door5_target_16.png
#define L3D5_DIGIT_FILE_FMT    "Image/%d.png"   // door5_digit_0.png ... door5_digit_9.png
#define L3D5_NUM_TARGETS 3
#define L3D5_NUM_DIALS 3
static const int level3door5_targetList[L3D5_NUM_TARGETS] = { 21, 25, 16 };  // one is picked at random

// puzzle layout (game pixels, origin bottom-left)
#define L3D5_BOARD_W 640
#define L3D5_BOARD_H 265
#define L3D5_BOARD_X (SCREEN_WIDTH / 2 - L3D5_BOARD_W / 2)
#define L3D5_BOARD_Y 110
#define L3D5_RUNE_W 120                 // target rune picture (sits on top of the board)
#define L3D5_RUNE_H 180
#define L3D5_RUNE_Y 283                 // bottom edge of the rune picture
#define L3D5_DIAL_SIZE 100             // each digit picture is drawn DIAL_SIZE x DIAL_SIZE
#define L3D5_DIAL_SPACING 190           // distance between dial centres
#define L3D5_DIAL_CY 209                // dial centre height
#define L3D5_SOLVED_DELAY 45            // frames the solved board stays before the chest opens

#define L3D5_CRACK_DURATION 500.0f      // frames you can stand on a step before it crumbles
#define L3D5_PLAYER_SCREEN_X 220        // where the camera keeps the character on screen

// ---- jump feel ----
#define L3D5_JUMP_DURATION 28           // frames a hop lasts (bigger = slower / floatier)
#define L3D5_JUMP_ARC_HEIGHT 70.0f
#define L3D5_JUMP_REACH 210.0f          // max distance from you to the NEAR EDGE of the next step
// for the hop to succeed (bigger = easier)

// ---- camera ----
#define L3D5_CAMERA_EASE 0.08f          // 0.05 = lazy, 0.15 = snappy

// ---- falling into lava ----
#define L3D5_LAVA_Y 80.0f               // height of the lava surface on screen
#define L3D5_GRAVITY 0.7f

// ---- layout (world units) ----
#define L3D5_GROUND_TRACK 0.0f
#define L3D5_GROUND_Y 150.0f
#define L3D5_FIRST_STEP_X 240.0f
#define L3D5_STEP_SPACING 280.0f        // distance between the steps' centre lines
#define L3D5_FINAL_TRACK (L3D5_FIRST_STEP_X + L3D5_NUM_STEPS * L3D5_STEP_SPACING)
#define L3D5_FINAL_Y 150.0f

static int level3door5_lives = 3;
static int level3door5_currentStep = -1; // -1 = on ground, 0..9 = on that step, 10 = on final ledge

static float level3door5_camX = 0.0f;       // world x the camera is centred on (eased)
static float level3door5_camTarget = 0.0f;  // world x the camera is moving toward
static float level3door5_playerX = 0.0f;    // player's world x
static float level3door5_playerY = 150.0f;

static bool  level3door5_jumping = false;
static bool  level3door5_jumpHits = true;   // false = the hop falls short
static int   level3door5_jumpTarget = 0;
static float level3door5_jumpTimer = 0.0f;
static float level3door5_jumpFromX = 0.0f, level3door5_jumpFromY = 0.0f;

static bool  level3door5_prevUpPressed = false;
static float level3door5_crackTimer = 0.0f;

static bool  level3door5_falling = false;
static bool  level3door5_fallCrack = false; // true = the step crumbled under you (step is hidden)
static float level3door5_fallVel = 0.0f;

static float level3door5_globalClock = 0.0f;

// Ending: 0 = platforming, 1 = chest popped up (click it), 2 = puzzle, 3 = chest opened
static int level3door5_stage = 0;
static int level3door5_stageTimer = 0;

// Puzzle state
static int  level3door5_targetIdx = 0;
static int  level3door5_targetSum = 0;
static int  level3door5_dial[L3D5_NUM_DIALS];   // digit (0-9) currently shown on each dial
static bool level3door5_puzzleSolved = false;
static int  level3door5_solvedTimer = 0;

// Steps slide horizontally around their home position.
// Group 1 (0-2): gentle. Group 2 (3-6): faster. Group 3 (7-9): fastest.
static float level3door5_stepY[L3D5_NUM_STEPS] = { 166, 235, 166, 235, 166, 235, 166, 235, 166, 235 };
static float level3door5_stepAmp[L3D5_NUM_STEPS] = { 35, 45, 50, 55, 60, 60, 65, 65, 70, 70 };
static float level3door5_stepSpeed[L3D5_NUM_STEPS] = { 0.028f, 0.032f, 0.030f, 0.045f, 0.048f, 0.043f, 0.050f, 0.065f, 0.070f, 0.068f };
static float level3door5_stepPhase[L3D5_NUM_STEPS] = { 0.0f, 1.2f, 2.4f, 0.6f, 1.8f, 3.0f, 2.1f, 0.9f, 2.7f, 1.5f };
static int   level3door5_stepW = 190, level3door5_stepH = 50;

// ---------------- helpers ----------------
// i: -1 = ground, 0..9 = step, 10 = final ledge
inline float level3door5_homeOf(int i)
{
	if (i < 0) return L3D5_GROUND_TRACK;
	if (i >= L3D5_NUM_STEPS) return L3D5_FINAL_TRACK;
	return L3D5_FIRST_STEP_X + i * L3D5_STEP_SPACING;
}

// Current world x of a step (it slides left/right around its home)
inline float level3door5_xOf(int i)
{
	if (i < 0 || i >= L3D5_NUM_STEPS) return level3door5_homeOf(i);
	return level3door5_homeOf(i) + sinf(level3door5_globalClock * level3door5_stepSpeed[i] + level3door5_stepPhase[i]) * level3door5_stepAmp[i];
}

inline float level3door5_yOf(int i)
{
	if (i < 0) return L3D5_GROUND_Y;
	if (i >= L3D5_NUM_STEPS) return L3D5_FINAL_Y;
	return level3door5_stepY[i];
}

inline float level3door5_screenXFor(float worldX)
{
	return worldX - level3door5_camX + L3D5_PLAYER_SCREEN_X;
}

inline void startL3Door5Path()
{
	level3door5_active = true;
	level3door5_lives = 3;
	level3door5_currentStep = -1;
	level3door5_camX = L3D5_GROUND_TRACK;
	level3door5_camTarget = L3D5_GROUND_TRACK;
	level3door5_playerX = L3D5_GROUND_TRACK;
	level3door5_playerY = L3D5_GROUND_Y;
	level3door5_jumping = false;
	level3door5_jumpTimer = 0.0f;
	level3door5_prevUpPressed = false;
	level3door5_crackTimer = 0.0f;
	level3door5_falling = false;
	level3door5_fallCrack = false;
	level3door5_fallVel = 0.0f;
	level3door5_globalClock = 0.0f;
	level3door5_stage = 0;
	level3door5_stageTimer = 0;
}

inline void level3door5_respawn()
{
	level3door5_currentStep = -1;
	level3door5_camX = L3D5_GROUND_TRACK;
	level3door5_camTarget = L3D5_GROUND_TRACK;
	level3door5_playerX = L3D5_GROUND_TRACK;
	level3door5_playerY = L3D5_GROUND_Y;
	level3door5_crackTimer = 0.0f;
	level3door5_falling = false;
	level3door5_fallCrack = false;
	level3door5_fallVel = 0.0f;
	level3door5_jumping = false;
}

// ---------------- render ----------------
inline void level3door5_drawBackground()
{
	static int bgImgs[3] = { -2, -2, -2 };
	if (bgImgs[0] == -2) {
		bgImgs[0] = iLoadImage("Image/door5_first_bg.png");
		bgImgs[1] = iLoadImage("Image/door5_middle_bg.png");
		bgImgs[2] = iLoadImage("Image/door5_last_bg.png");
	}

	// The three backgrounds sit side by side as one panorama. As the camera
	// travels from the ground to the final ledge the panorama slides by two
	// screen widths, so the last background is fully in view at the end.
	float progress = level3door5_camX / L3D5_FINAL_TRACK;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;
	float offset = progress * 2.0f * SCREEN_WIDTH;

	for (int k = 0; k < 3; k++) {
		int x = (int)(k * SCREEN_WIDTH - offset);
		if (x >= SCREEN_WIDTH || x + SCREEN_WIDTH <= 0) continue;
		if (bgImgs[k] >= 0) iShowImage(x, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImgs[k]);
	}
}

inline void level3door5_drawStep(int i)
{
	static int stepNormalImg = -2, crackImgs[4] = { -2, -2, -2, -2 };
	if (stepNormalImg == -2) {
		stepNormalImg = iLoadImage("Image/door5_step1.png");
		crackImgs[0] = iLoadImage("Image/door5_step1.png");
		crackImgs[1] = iLoadImage("Image/door5_step2.png");
		crackImgs[2] = iLoadImage("Image/door5_step3.png");
		crackImgs[3] = iLoadImage("Image/door5_step4.png");
	}

	// the step you were standing on crumbled away
	if (level3door5_falling && level3door5_fallCrack && i == level3door5_currentStep) return;

	float screenX = level3door5_screenXFor(level3door5_xOf(i));
	if (screenX < -level3door5_stepW || screenX > SCREEN_WIDTH + level3door5_stepW) return; // off-screen, skip

	float y = level3door5_yOf(i);

	int imgToUse = stepNormalImg;
	if (i == level3door5_currentStep && !level3door5_falling && !level3door5_jumping) {
		int stage = (int)(level3door5_crackTimer / (L3D5_CRACK_DURATION / 4.0f));
		if (stage > 3) stage = 3;
		if (crackImgs[stage] >= 0) imgToUse = crackImgs[stage];
	}

	if (imgToUse >= 0) {
		iShowImage((int)(screenX - level3door5_stepW / 2), (int)y, level3door5_stepW, level3door5_stepH, imgToUse);
	}
	else {
		if (i == level3door5_currentStep && !level3door5_falling) {
			float pct = level3door5_crackTimer / L3D5_CRACK_DURATION;
			iSetColor((int)(140 + pct * 100), (int)(120 - pct * 90), (int)(90 - pct * 70));
		}
		else {
			iSetColor(140, 120, 90);
		}
		iFilledRectangle((int)(screenX - level3door5_stepW / 2), (int)y, level3door5_stepW, level3door5_stepH);
		iSetColor(70, 55, 40);
		iRectangle((int)(screenX - level3door5_stepW / 2), (int)y, level3door5_stepW, level3door5_stepH);
	}
}

// ---------------- ending: treasure chest + sum-lock puzzle ----------------
inline int level3door5_dialX(int k) { return SCREEN_WIDTH / 2 + (k - L3D5_NUM_DIALS / 2) * L3D5_DIAL_SPACING; }

inline int level3door5_dialSum()
{
	int sum = 0;
	for (int k = 0; k < L3D5_NUM_DIALS; k++) sum += level3door5_dial[k];
	return sum;
}

inline void level3door5_startPuzzle()
{
	// random target (21 / 25 / 16) and random starting digits that are NOT already the answer
	level3door5_targetIdx = rand() % L3D5_NUM_TARGETS;
	level3door5_targetSum = level3door5_targetList[level3door5_targetIdx];
	do {
		for (int k = 0; k < L3D5_NUM_DIALS; k++) level3door5_dial[k] = rand() % 10;
	} while (level3door5_dialSum() == level3door5_targetSum);

	level3door5_puzzleSolved = false;
	level3door5_solvedTimer = 0;
	level3door5_stage = 2;
	level3door5_stageTimer = 0;
}

inline void level3door5_drawChestImage(int img, float scale, int r, int g, int b)
{
	int w = (int)(L3D5_CHEST_W * scale);
	int h = (int)(L3D5_CHEST_H * scale);
	if (w < 2 || h < 2) return;
	int x = SCREEN_WIDTH / 2 - w / 2;
	int y = L3D5_CHEST_CY - h / 2;
	if (img >= 0) {
		iShowImage(x, y, w, h, img);
	}
	else { // placeholder box until your picture exists
		iSetColor(r, g, b);
		iFilledRectangle(x, y, w, h);
		iSetColor(0, 0, 0);
		iRectangle(x, y, w, h);
	}
}

inline void level3door5_drawPuzzle()
{
	static bool loaded = false;
	static int boardImg = -1, targetImgs[L3D5_NUM_TARGETS], digitImgs[10];
	if (!loaded) {
		loaded = true;
		char path[80];
		boardImg = iLoadImage(L3D5_PUZZLE_BOARD_FILE);
		for (int i = 0; i < L3D5_NUM_TARGETS; i++) {
			sprintf_s(path, sizeof(path), L3D5_TARGET_FILE_FMT, level3door5_targetList[i]);
			targetImgs[i] = iLoadImage(path);
		}
		for (int d = 0; d < 10; d++) {
			sprintf_s(path, sizeof(path), L3D5_DIGIT_FILE_FMT, d);
			digitImgs[d] = iLoadImage(path);
		}
	}

	// wooden board
	if (boardImg >= 0) {
		iShowImage(L3D5_BOARD_X, L3D5_BOARD_Y, L3D5_BOARD_W, L3D5_BOARD_H, boardImg);
	}
	else {
		iSetColor(110, 75, 40);
		iFilledRectangle(L3D5_BOARD_X, L3D5_BOARD_Y, L3D5_BOARD_W, L3D5_BOARD_H);
		iSetColor(50, 30, 15);
		iRectangle(L3D5_BOARD_X, L3D5_BOARD_Y, L3D5_BOARD_W, L3D5_BOARD_H);
	}

	// the three dials, each showing its current digit
	for (int k = 0; k < L3D5_NUM_DIALS; k++) {
		int cx = level3door5_dialX(k);
		int d = level3door5_dial[k];
		if (digitImgs[d] >= 0) {
			iShowImage(cx - L3D5_DIAL_SIZE / 2, L3D5_DIAL_CY - L3D5_DIAL_SIZE / 2, L3D5_DIAL_SIZE, L3D5_DIAL_SIZE, digitImgs[d]);
		}
		else {
			iSetColor(130, 125, 115);
			iFilledCircle(cx, L3D5_DIAL_CY, L3D5_DIAL_SIZE / 2);
			iSetColor(40, 40, 40);
			iCircle(cx, L3D5_DIAL_CY, L3D5_DIAL_SIZE / 2);
			char b[4];
			sprintf_s(b, sizeof(b), "%d", d);
			iSetColor(255, 255, 255);
			level3_drawBoldText(cx - 6, L3D5_DIAL_CY - 8, b, GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}

	// target rune (drawn last so it overlaps the top of the board)
	int rx = SCREEN_WIDTH / 2 - L3D5_RUNE_W / 2;
	if (targetImgs[level3door5_targetIdx] >= 0) {
		iShowImage(rx, L3D5_RUNE_Y, L3D5_RUNE_W, L3D5_RUNE_H, targetImgs[level3door5_targetIdx]);
	}
	else {
		char b[8];
		sprintf_s(b, sizeof(b), "%d", level3door5_targetSum);
		iSetColor(255, 190, 60);
		iFilledCircle(SCREEN_WIDTH / 2, L3D5_RUNE_Y + L3D5_RUNE_H / 2, L3D5_RUNE_W / 2);
		iSetColor(90, 40, 10);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 14, L3D5_RUNE_Y + L3D5_RUNE_H / 2 - 8, b, GLUT_BITMAP_TIMES_ROMAN_24);
	}

	if (level3door5_puzzleSolved) {
		iSetColor(255, 215, 0);
		iText(SCREEN_WIDTH / 2 - 75, L3D5_BOARD_Y - 25, "The lock clicks open!");
	}
	else {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 190, L3D5_BOARD_Y - 25, "Click the dials until their sum equals the rune!");
	}
}

inline void level3door5_drawTreasureOverlay()
{
	if (level3door5_stage == 0) return;

	static int closedImg = -2, openImg = -2;
	if (closedImg == -2) {
		closedImg = iLoadImage(L3D5_CHEST_CLOSED_FILE);
		openImg = iLoadImage(L3D5_CHEST_OPEN_FILE);
	}

	if (level3door5_stage == 1) {
		// pop-in with a little overshoot ("ease out back")
		float t = (float)level3door5_stageTimer / L3D5_POP_FRAMES;
		if (t > 1.0f) t = 1.0f;
		float u = t - 1.0f;
		float scale = 1.0f + 2.70158f * u * u * u + 1.70158f * u * u;
		level3door5_drawChestImage(closedImg, scale, 150, 95, 40);

		if (t >= 1.0f) {
			iSetColor(255, 255, 255);
			iFilledRectangle(SCREEN_WIDTH / 2 - 150, L3D5_CHEST_CY - L3D5_CHEST_H / 2 - 55, 300, 38);
			iSetColor(0, 0, 0);
			iRectangle(SCREEN_WIDTH / 2 - 150, L3D5_CHEST_CY - L3D5_CHEST_H / 2 - 55, 300, 38);
			iText(SCREEN_WIDTH / 2 - 105, L3D5_CHEST_CY - L3D5_CHEST_H / 2 - 42, "Click the treasure to open it!");
		}
	}
	else if (level3door5_stage == 2) {
		level3door5_drawPuzzle();
	}
	else if (level3door5_stage == 3) {
		level3door5_drawChestImage(openImg, 1.0f, 240, 190, 60);
		iSetColor(255, 215, 0);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 100, L3D5_CHEST_CY - L3D5_CHEST_H / 2 - 45, "Treasure unlocked!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
}

// Called from handleLevel3Clicks (Level3.h) while door 5 is active
inline void level3door5_handleClick(int mx, int my)
{
	if (level3door5_stage == 1) {
		bool popped = level3door5_stageTimer >= L3D5_POP_FRAMES;
		bool onChest = abs(mx - SCREEN_WIDTH / 2) <= L3D5_CHEST_W / 2 && abs(my - L3D5_CHEST_CY) <= L3D5_CHEST_H / 2;
		if (popped && onChest) level3door5_startPuzzle();
		return;
	}

	if (level3door5_stage == 2 && !level3door5_puzzleSolved) {
		for (int k = 0; k < L3D5_NUM_DIALS; k++) {
			int dx = mx - level3door5_dialX(k);
			int dy = my - L3D5_DIAL_CY;
			if (dx * dx + dy * dy >(L3D5_DIAL_SIZE / 2) * (L3D5_DIAL_SIZE / 2)) continue;

			// each click turns that dial to the next digit: 0,1,2 ... 9, then back to 0
			level3door5_dial[k] = (level3door5_dial[k] + 1) % 10;

			if (level3door5_dialSum() == level3door5_targetSum) {
				level3door5_puzzleSolved = true;
				level3door5_solvedTimer = L3D5_SOLVED_DELAY;
				level3_playPlusPointSound();
			}
			return;
		}
	}
}

inline void level3door5_updateTreasure()
{
	level3door5_stageTimer++;

	if (level3door5_stage == 2) {
		if (level3door5_puzzleSolved) {
			level3door5_solvedTimer--;
			if (level3door5_solvedTimer <= 0) {
				level3door5_stage = 3;      // show the opened chest
				level3door5_stageTimer = 0;
			}
		}
	}
	else if (level3door5_stage == 3 && level3door5_stageTimer >= L3D5_OPEN_DELAY) {
		// Hand over to the win page (same flags the clam/pearl ending in Level3.h uses)
		level3_finishStage = 3;
		level3_keyFound = true;
		level3_updateScore(L3D5_TREASURE_SCORE);
		level3_playPlusPointSound();
		level3door5_active = false;
	}
}

inline void renderL3Door5Path()
{
	static int idleImg = -1, jumpFrames[3] = { -1, -1, -1 };
	if (idleImg == -1) {
		idleImg = iLoadImage("Image/idle_1.png");
		jumpFrames[0] = iLoadImage("Image/jump_1.png");
		jumpFrames[1] = iLoadImage("Image/jump_2.png");
		jumpFrames[2] = iLoadImage("Image/jump_3.png");
	}

	level3door5_drawBackground();

	// steps 0..9, plus the final ledge (i == L3D5_NUM_STEPS) with the same step picture
	for (int i = 0; i <= L3D5_NUM_STEPS; i++) {
		level3door5_drawStep(i);
	}

	// Player (disappears once it has sunk into the lava)
	bool sunk = level3door5_falling && level3door5_playerY < L3D5_LAVA_Y;
	if (!sunk) {
		int pImg = idleImg;
		if (level3door5_falling) {
			pImg = jumpFrames[2];
		}
		else if (level3door5_jumping) {
			int frame = (int)((level3door5_jumpTimer / L3D5_JUMP_DURATION) * 3.0f);
			if (frame > 2) frame = 2;
			if (frame >= 0 && jumpFrames[frame] >= 0) pImg = jumpFrames[frame];
		}
		iShowImage((int)level3door5_screenXFor(level3door5_playerX) - 45, (int)level3door5_playerY, 90, 130, pImg);
	}

	// Lives: one picture per life count (3 hearts -> 2 -> 1)
	static int heartImgs[3] = { -2, -2, -2 };
	if (heartImgs[0] == -2) {
		heartImgs[0] = iLoadImage(L3D5_HEARTS1_FILE);
		heartImgs[1] = iLoadImage(L3D5_HEARTS2_FILE);
		heartImgs[2] = iLoadImage(L3D5_HEARTS3_FILE);
	}
	if (level3door5_lives >= 1 && level3door5_lives <= 3) {
		int hImg = heartImgs[level3door5_lives - 1];
		if (hImg >= 0) {
			iShowImage(L3D5_HEARTS_X, L3D5_HEARTS_Y, L3D5_HEARTS_W, L3D5_HEARTS_H, hImg);
		}
		else {
			char livesBuf[24];
			sprintf_s(livesBuf, sizeof(livesBuf), "Lives: %d", level3door5_lives);
			iSetColor(255, 255, 255);
			iText(L3D5_HEARTS_X, SCREEN_HEIGHT - 85, livesBuf);
		}
	}

	if (level3door5_stage == 0) {
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH / 2 - 200, 40, "Press UP to jump when the next platform is close enough!");
	}

	level3door5_drawTreasureOverlay();

	if (level3_messageTimer > 0) {
		iSetColor(255, 220, 120);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 170, 70, level3_message, GLUT_BITMAP_HELVETICA_18);
	}

	if (level3door5_falling) {
		iSetColor(255, 80, 40);
		level3_drawBoldText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2, "You fell!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
}

// ---------------- update ----------------
inline void level3door5_startJump(int target)
{
	// Is the next platform close enough RIGHT NOW? (distance to its near edge)
	float nearEdge = level3door5_xOf(target) - level3door5_stepW / 2.0f;
	float gap = nearEdge - level3door5_playerX;
	level3door5_jumpHits = (target == L3D5_NUM_STEPS) || (gap <= L3D5_JUMP_REACH); // final ledge always succeeds

	level3door5_jumpFromX = level3door5_playerX;
	level3door5_jumpFromY = level3door5_playerY;
	level3door5_jumpTarget = target;
	level3door5_jumpTimer = 0.0f;
	level3door5_jumping = true;

	if (level3door5_jumpHits) {
		level3door5_currentStep = target;
		level3door5_crackTimer = 0.0f;
		level3door5_camTarget = level3door5_homeOf(target); // camera starts gliding to the new step
	}
}

inline void updateL3Door5PathIfActive()
{
	if (level3_gameOver) {
		level3door5_active = false;
		return;
	}

	if (level3_messageTimer > 0) level3_messageTimer--;
	level3door5_globalClock += 1.0f;

	// Camera glides toward its target every frame (never snaps)
	float camDiff = level3door5_camTarget - level3door5_camX;
	if (fabsf(camDiff) < 0.5f) level3door5_camX = level3door5_camTarget;
	else level3door5_camX += camDiff * L3D5_CAMERA_EASE;

	// Fresh UP press this frame?
	bool upNow = isSpecialKeyPressed(GLUT_KEY_UP);
	bool jumpPressed = upNow && !level3door5_prevUpPressed;
	level3door5_prevUpPressed = upNow;

	// Ending (chest pop-up / puzzle / opened chest)
	if (level3door5_stage > 0) {
		level3door5_updateTreasure();
		return;
	}

	// Falling toward the lava
	if (level3door5_falling) {
		level3door5_fallVel -= L3D5_GRAVITY;
		level3door5_playerY += level3door5_fallVel;
		if (level3door5_playerY <= L3D5_LAVA_Y - 50.0f) {
			level3door5_lives--;
			if (level3door5_lives <= 0) {
				level3_gameOver = true;
				level3door5_active = false;
				return;
			}
			level3door5_respawn();
		}
		return;
	}

	// The hop: X glides toward the (moving) target, Y follows a parabola
	if (level3door5_jumping) {
		level3door5_jumpTimer += 1.0f;
		float t = level3door5_jumpTimer / L3D5_JUMP_DURATION;
		if (t > 1.0f) t = 1.0f;

		float toX, toY;
		if (level3door5_jumpHits) {
			toX = level3door5_xOf(level3door5_jumpTarget); // follows the moving platform
			toY = level3door5_yOf(level3door5_jumpTarget);
		}
		else {
			toX = level3door5_jumpFromX + L3D5_JUMP_REACH;  // falls short of the platform
			toY = level3door5_jumpFromY;
		}

		level3door5_playerX = level3door5_jumpFromX + (toX - level3door5_jumpFromX) * t;
		float straightY = level3door5_jumpFromY + (toY - level3door5_jumpFromY) * t;
		level3door5_playerY = straightY + sinf(t * 3.14159f) * L3D5_JUMP_ARC_HEIGHT;

		if (t >= 1.0f) {
			level3door5_jumping = false;

			if (!level3door5_jumpHits) {
				// No platform under us - drop into the lava
				level3door5_falling = true;
				level3door5_fallCrack = false;
				level3door5_fallVel = -3.0f;
				return;
			}

			// Landed - check for the win condition now that the hop is done
			if (level3door5_currentStep == L3D5_NUM_STEPS) {
				level3_updateScore(L3D5_CROSS_SCORE);
				level3_playPlusPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "You crossed the Mystical Mine!");
				level3_messageTimer = 90;
				level3door5_stage = 1;        // the treasure chest pops up
				level3door5_stageTimer = 0;
			}
		}
		return;
	}

	// Standing: ride the step as it slides, and let it crack
	if (level3door5_currentStep >= 0 && level3door5_currentStep < L3D5_NUM_STEPS) {
		level3door5_playerX = level3door5_xOf(level3door5_currentStep);
		level3door5_playerY = level3door5_yOf(level3door5_currentStep);

		level3door5_crackTimer += 1.0f;
		if (level3door5_crackTimer >= L3D5_CRACK_DURATION) {
			level3door5_falling = true;
			level3door5_fallCrack = true;
			level3door5_fallVel = 0.0f;
			return;
		}
	}
	else {
		level3door5_playerX = level3door5_homeOf(level3door5_currentStep);
		level3door5_playerY = level3door5_yOf(level3door5_currentStep);
	}

	if (jumpPressed && level3door5_currentStep < L3D5_NUM_STEPS) {
		level3door5_startJump(level3door5_currentStep + 1);
	}
}

#endif