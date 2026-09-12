#ifndef SCORE_BOARD_H_INCLUDED
#define SCORE_BOARD_H_INCLUDED

#include <cstdio>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int gameState;
extern int level1_highScore;
extern int level2_highScore;
extern bool isSoundMuted;

// ---------------- MENU SETTINGS UI CONSTANTS ----------------
static bool sb_showSettingsMenu = false;
#define SB_SETTING_BTN_X 945
#define SB_SETTING_BTN_Y 45
#define SB_SETTING_BTN_R 22

#define SB_SUB_BTN_R     20
#define SB_SUB_S_Y       100

// Back Button Position & Size
#define HS_BACK_BTN_X 412
#define HS_BACK_BTN_Y 0
#define HS_BACK_BTN_WIDTH 200
#define HS_BACK_BTN_HEIGHT 60

// lvl1 highscore load func
inline void fetchLevel1HighScore() {
	FILE* fp = NULL;
	fopen_s(&fp, "level1_highscore.txt", "r");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &level1_highScore);
		fclose(fp);
	}
}

// lvl2 highscore load func
inline void fetchLevel2HighScore() {
	FILE* fp = NULL;
	fopen_s(&fp, "level2_highscore.txt", "r");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &level2_highScore);
		fclose(fp);
	}
}

// important func
inline void renderHighscorePage() {
	static int bgScoreBoardImg = -1, backBtnImg = -1;
	static int btnSettings = -1, btnSoundOn = -1, btnSoundOff = -1;

	if (bgScoreBoardImg == -1) {
		bgScoreBoardImg = iLoadImage("Image/BgScoreBoard.png");
		backBtnImg = iLoadImage("Image/backbutton.png");

		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}

	// evrytime in highscore pg, read updated highscore
	fetchLevel1HighScore();
	fetchLevel2HighScore();

	if (bgScoreBoardImg >= 0) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgScoreBoardImg);
	}
	else {
		iSetColor(220, 220, 220);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	// 3 lvl string making
	char easyBuf[80];
	char mediumBuf[80];
	char hardBuf[80];

	//text show
	sprintf_s(easyBuf, sizeof(easyBuf), "EASY LEVEL HIGHSCORE   :  %d", level1_highScore);
	sprintf_s(mediumBuf, sizeof(mediumBuf), "MEDIUM LEVEL HIGHSCORE :  %d", level2_highScore);
	sprintf_s(hardBuf, sizeof(hardBuf), "HARD LEVEL HIGHSCORE   :  0");

	// Black
	iSetColor(0, 0, 0);

	// 1. Easy Level real highscore
	iText(280, 360, easyBuf, GLUT_BITMAP_TIMES_ROMAN_24);
	iText(281, 360, easyBuf, GLUT_BITMAP_TIMES_ROMAN_24);

	// 2. Medium Level real highscore
	iText(280, 300, mediumBuf, GLUT_BITMAP_TIMES_ROMAN_24);
	iText(281, 300, mediumBuf, GLUT_BITMAP_TIMES_ROMAN_24);

	// 3. Hard Level
	iText(280, 240, hardBuf, GLUT_BITMAP_TIMES_ROMAN_24);
	iText(281, 240, hardBuf, GLUT_BITMAP_TIMES_ROMAN_24);

	// Back Button
	if (backBtnImg != -1) {
		iShowImage(HS_BACK_BTN_X, HS_BACK_BTN_Y, HS_BACK_BTN_WIDTH, HS_BACK_BTN_HEIGHT, backBtnImg);
	}
	else {
		iSetColor(255, 0, 0);
		iFilledRectangle(HS_BACK_BTN_X, HS_BACK_BTN_Y, HS_BACK_BTN_WIDTH, HS_BACK_BTN_HEIGHT);
		iSetColor(255, 255, 255);
		iText(HS_BACK_BTN_X + 10, HS_BACK_BTN_Y + 12, "<--", GLUT_BITMAP_HELVETICA_18);
	}

	// Settings Button (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(SB_SETTING_BTN_X - 22, SB_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(SB_SETTING_BTN_X, SB_SETTING_BTN_Y, SB_SETTING_BTN_R);
	}

	// Pop-up Sound On/Off Button (40x40 px)
	if (sb_showSettingsMenu) {
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(SB_SETTING_BTN_X - 20, SB_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(SB_SETTING_BTN_X, SB_SUB_S_Y, SB_SUB_BTN_R);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(SB_SETTING_BTN_X - 20, SB_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(SB_SETTING_BTN_X, SB_SUB_S_Y, SB_SUB_BTN_R);
			}
		}
	}
}

inline void handleHighscoreClicks(int mx, int my) {
	// Settings Click
	float distSettings = sqrtf((float)((mx - SB_SETTING_BTN_X) * (mx - SB_SETTING_BTN_X) +
		(my - SB_SETTING_BTN_Y) * (my - SB_SETTING_BTN_Y)));
	if (distSettings <= SB_SETTING_BTN_R) {
		sb_showSettingsMenu = !sb_showSettingsMenu;
		return;
	}

	// Sound Toggle Click
	if (sb_showSettingsMenu) {
		float distS = sqrtf((float)((mx - SB_SETTING_BTN_X) * (mx - SB_SETTING_BTN_X) +
			(my - SB_SUB_S_Y) * (my - SB_SUB_S_Y)));
		if (distS <= SB_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop mainMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	if (mx >= HS_BACK_BTN_X && mx <= HS_BACK_BTN_X + HS_BACK_BTN_WIDTH &&
		my >= HS_BACK_BTN_Y && my <= HS_BACK_BTN_Y + HS_BACK_BTN_HEIGHT) {
		gameState = 0; // Return to Home Screen
	}
}

#endif