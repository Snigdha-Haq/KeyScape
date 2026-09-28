#ifndef CREDIT_H_INCLUDED
#define CREDIT_H_INCLUDED

#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

extern int gameState;
extern bool isSoundMuted;

// ---------------- MENU SETTINGS UI CONSTANTS ----------------
static bool credit_showSettingsMenu = false;
#define CREDIT_SETTING_BTN_X 945
#define CREDIT_SETTING_BTN_Y 45
#define CREDIT_SETTING_BTN_R 22

#define CREDIT_SUB_BTN_R     20
#define CREDIT_SUB_S_Y       100

// Back Button Position & Size
#define CREDIT_BACK_BTN_X 412
#define CREDIT_BACK_BTN_Y 0            
#define CREDIT_BACK_BTN_WIDTH 200       
#define CREDIT_BACK_BTN_HEIGHT 60       

inline void renderCreditPage() {
	static int creditBgImg = -1, backBtnImg = -1;
	static int btnSettings = -1, btnSoundOn = -1, btnSoundOff = -1;

	// Load images once
	if (creditBgImg == -1) {
		creditBgImg = iLoadImage("Image/bgCredit.png");
		if (creditBgImg < 0) creditBgImg = iLoadImage("bgCredit.png");

		backBtnImg = iLoadImage("Image/backbutton.png");
		if (backBtnImg < 0) backBtnImg = iLoadImage("backbutton.png");

		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}

	// 1. Background Image
	iShowImage(0, 0, 1000, 600, creditBgImg);

	// 2. Back Button
	if (backBtnImg != -1) {
		iShowImage(CREDIT_BACK_BTN_X, CREDIT_BACK_BTN_Y, CREDIT_BACK_BTN_WIDTH, CREDIT_BACK_BTN_HEIGHT, backBtnImg);
	}
	else {
		iSetColor(255, 0, 0);
		iFilledRectangle(CREDIT_BACK_BTN_X, CREDIT_BACK_BTN_Y, CREDIT_BACK_BTN_WIDTH, CREDIT_BACK_BTN_HEIGHT);
		iSetColor(255, 255, 255);
		iText(CREDIT_BACK_BTN_X + 10, CREDIT_BACK_BTN_Y + 12, "<--", GLUT_BITMAP_HELVETICA_18);
	}

	// 3. Settings Button (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(CREDIT_SETTING_BTN_X - 22, CREDIT_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(CREDIT_SETTING_BTN_X, CREDIT_SETTING_BTN_Y, CREDIT_SETTING_BTN_R);
	}

	// 4. Pop-up Sound On/Off Button (40x40 px)
	if (credit_showSettingsMenu) {
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(CREDIT_SETTING_BTN_X - 20, CREDIT_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(CREDIT_SETTING_BTN_X, CREDIT_SUB_S_Y, CREDIT_SUB_BTN_R);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(CREDIT_SETTING_BTN_X - 20, CREDIT_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(CREDIT_SETTING_BTN_X, CREDIT_SUB_S_Y, CREDIT_SUB_BTN_R);
			}
		}
	}
}

// Click handling for Credit Page
inline void handleCreditClicks(int mx, int my) {
	// Settings Button Click
	float distSettings = sqrtf((float)((mx - CREDIT_SETTING_BTN_X) * (mx - CREDIT_SETTING_BTN_X) +
		(my - CREDIT_SETTING_BTN_Y) * (my - CREDIT_SETTING_BTN_Y)));
	if (distSettings <= CREDIT_SETTING_BTN_R) {
		credit_showSettingsMenu = !credit_showSettingsMenu;
		return;
	}

	// Sound Toggle Click
	if (credit_showSettingsMenu) {
		float distS = sqrtf((float)((mx - CREDIT_SETTING_BTN_X) * (mx - CREDIT_SETTING_BTN_X) +
			(my - CREDIT_SUB_S_Y) * (my - CREDIT_SUB_S_Y)));
		if (distS <= CREDIT_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop mainMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	// Back Button Click (Go to Homepage)
	if (mx >= CREDIT_BACK_BTN_X && mx <= CREDIT_BACK_BTN_X + CREDIT_BACK_BTN_WIDTH &&
		my >= CREDIT_BACK_BTN_Y && my <= CREDIT_BACK_BTN_Y + CREDIT_BACK_BTN_HEIGHT) {
		gameState = 0;
	}
}

#endif