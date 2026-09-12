#ifndef OPTION_H_INCLUDED
#define OPTION_H_INCLUDED

#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

extern int gameState; // shared variable from main.cpp
extern bool isSoundMuted;

// ---------------- MENU SETTINGS UI CONSTANTS ----------------
static bool opt_showSettingsMenu = false;
#define OPT_SETTING_BTN_X 945
#define OPT_SETTING_BTN_Y 45
#define OPT_SETTING_BTN_R 22

#define OPT_SUB_BTN_R     20
#define OPT_SUB_S_Y       100

// Back Button Position & Size
#define BACK_BTN_X 412
#define BACK_BTN_Y 0            
#define BACK_BTN_WIDTH 200       
#define BACK_BTN_HEIGHT 60       

inline void renderOptionPage() {
	static int optionBgImg = -1, backBtnImg = -1;
	static int btnSettings = -1, btnSoundOn = -1, btnSoundOff = -1;

	// bg load 1st time
	if (optionBgImg == -1) {
		optionBgImg = iLoadImage("Image/options_bg.png");
		backBtnImg = iLoadImage("Image/backbutton.png");

		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}
	// bg image
	iShowImage(0, 0, 1000, 600, optionBgImg);

	iSetColor(0, 0, 0);

	// 1. UP : Jump
	iText(400, 310, "UP          :  Jump", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 310, "UP          :  Jump", GLUT_BITMAP_TIMES_ROMAN_24); // bold

	// 2. DN : Slide
	iText(400, 268, "DN          :  Slide", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 268, "DN          :  Slide", GLUT_BITMAP_TIMES_ROMAN_24);

	// 3. RIGHT : Run
	iText(400, 226, "RIGHT       :  Run", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 226, "RIGHT       :  Run", GLUT_BITMAP_TIMES_ROMAN_24);

	// 4. LEFT/RIGHT : Move
	iText(400, 184, "LEFT/RIGHT  :  Move", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 184, "LEFT/RIGHT  :  Move", GLUT_BITMAP_TIMES_ROMAN_24);

	// 5. SPACE : Knife
	iText(400, 142, "SPACE       :  Knife", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 142, "SPACE       :  Knife", GLUT_BITMAP_TIMES_ROMAN_24);

	// 6. F : Shoot
	iText(400, 100, "F            :  Shoot", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(401, 100, "F            :  Shoot", GLUT_BITMAP_TIMES_ROMAN_24);

	// Back Button render
	if (backBtnImg != -1) {
		iShowImage(BACK_BTN_X, BACK_BTN_Y, BACK_BTN_WIDTH, BACK_BTN_HEIGHT, backBtnImg);
	}
	else {
		// backRectangle
		iSetColor(255, 0, 0);
		iFilledRectangle(BACK_BTN_X, BACK_BTN_Y, BACK_BTN_WIDTH, BACK_BTN_HEIGHT);
		iSetColor(255, 255, 255);
		iText(BACK_BTN_X + 10, BACK_BTN_Y + 12, "<--", GLUT_BITMAP_HELVETICA_18);
	}

	// Settings Button (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(OPT_SETTING_BTN_X - 22, OPT_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(OPT_SETTING_BTN_X, OPT_SETTING_BTN_Y, OPT_SETTING_BTN_R);
	}

	// Pop-up Sound On/Off Button (40x40 px)
	if (opt_showSettingsMenu) {
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(OPT_SETTING_BTN_X - 20, OPT_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(OPT_SETTING_BTN_X, OPT_SUB_S_Y, OPT_SUB_BTN_R);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(OPT_SETTING_BTN_X - 20, OPT_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(OPT_SETTING_BTN_X, OPT_SUB_S_Y, OPT_SUB_BTN_R);
			}
		}
	}
}

// click at Back Button to back Home Screen
inline void handleOptionClicks(int mx, int my) {
	// Settings Click
	float distSettings = sqrtf((float)((mx - OPT_SETTING_BTN_X) * (mx - OPT_SETTING_BTN_X) +
		(my - OPT_SETTING_BTN_Y) * (my - OPT_SETTING_BTN_Y)));
	if (distSettings <= OPT_SETTING_BTN_R) {
		opt_showSettingsMenu = !opt_showSettingsMenu;
		return;
	}

	// Sound Toggle Click
	if (opt_showSettingsMenu) {
		float distS = sqrtf((float)((mx - OPT_SETTING_BTN_X) * (mx - OPT_SETTING_BTN_X) +
			(my - OPT_SUB_S_Y) * (my - OPT_SUB_S_Y)));
		if (distS <= OPT_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop mainMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	if (mx >= BACK_BTN_X && mx <= BACK_BTN_X + BACK_BTN_WIDTH &&
		my >= BACK_BTN_Y && my <= BACK_BTN_Y + BACK_BTN_HEIGHT) {
		gameState = 0; // Go back to Home Screen
	}
}

#endif