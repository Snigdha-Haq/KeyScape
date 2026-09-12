#ifndef HOMEPAGE_H_INCLUDED
#define HOMEPAGE_H_INCLUDED

#include <cstdlib>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int gameState;
extern bool isSoundMuted;

// ---------------- MENU SETTINGS UI CONSTANTS ----------------
static bool home_showSettingsMenu = false;
#define HOME_SETTING_BTN_X 945
#define HOME_SETTING_BTN_Y 45
#define HOME_SETTING_BTN_R 22

#define HOME_SUB_BTN_R     20
#define HOME_SUB_S_Y       100

// ---------------- BUTTON LAYOUT ----------------
#define BUTTON_WIDTH  200
#define BUTTON_HEIGHT 60
#define BUTTON_GAP    20
#define CENTER_X ((SCREEN_WIDTH - BUTTON_WIDTH) / 2)

#define CREDIT_BTN_Y    ((SCREEN_HEIGHT - (4 * BUTTON_HEIGHT + 3 * BUTTON_GAP)) / 2)
#define HIGHSCORE_BTN_Y (CREDIT_BTN_Y    + BUTTON_HEIGHT + BUTTON_GAP)
#define OPTION_BTN_Y    (HIGHSCORE_BTN_Y + BUTTON_HEIGHT + BUTTON_GAP)
#define START_BTN_Y     (OPTION_BTN_Y    + BUTTON_HEIGHT + BUTTON_GAP)

// ---------------- RENDER HOMEPAGE ----------------
inline void renderHomepage() {
	static int homepageImg = -1, startImg = -1, optionImg = -1, highscoreImg = -1, creditImg = -1;
	static int btnSettings = -1, btnSoundOn = -1, btnSoundOff = -1;

	if (startImg == -1) {
		homepageImg = iLoadImage("Image/homepagebg.png");
		startImg = iLoadImage("Image/play.png");
		optionImg = iLoadImage("Image/options.png");
		highscoreImg = iLoadImage("Image/highscore.png");
		creditImg = iLoadImage("Image/credits.png");

		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, homepageImg);

	iShowImage(CENTER_X, START_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, startImg);
	iShowImage(CENTER_X, OPTION_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, optionImg);
	iShowImage(CENTER_X, HIGHSCORE_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, highscoreImg);
	iShowImage(CENTER_X, CREDIT_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, creditImg);

	// Settings Button (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(HOME_SETTING_BTN_X - 22, HOME_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(HOME_SETTING_BTN_X, HOME_SETTING_BTN_Y, HOME_SETTING_BTN_R);
	}

	// Pop-up Sound On/Off Button (40x40 px)
	if (home_showSettingsMenu) {
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(HOME_SETTING_BTN_X - 20, HOME_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(HOME_SETTING_BTN_X, HOME_SUB_S_Y, HOME_SUB_BTN_R);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(HOME_SETTING_BTN_X - 20, HOME_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(HOME_SETTING_BTN_X, HOME_SUB_S_Y, HOME_SUB_BTN_R);
			}
		}
	}
}

// ---------------- CLICK HANDLING ----------------
inline void handleMenuClicks(int mx, int my) {
	// Settings Click
	float distSettings = sqrtf((float)((mx - HOME_SETTING_BTN_X) * (mx - HOME_SETTING_BTN_X) +
		(my - HOME_SETTING_BTN_Y) * (my - HOME_SETTING_BTN_Y)));
	if (distSettings <= HOME_SETTING_BTN_R) {
		home_showSettingsMenu = !home_showSettingsMenu;
		return;
	}

	// Sound Toggle Click
	if (home_showSettingsMenu) {
		float distS = sqrtf((float)((mx - HOME_SETTING_BTN_X) * (mx - HOME_SETTING_BTN_X) +
			(my - HOME_SUB_S_Y) * (my - HOME_SUB_S_Y)));
		if (distS <= HOME_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop mainMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= START_BTN_Y && my <= START_BTN_Y + BUTTON_HEIGHT) {
		gameState = 5;
	}
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= OPTION_BTN_Y && my <= OPTION_BTN_Y + BUTTON_HEIGHT) {
		gameState = 2;
	}
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= HIGHSCORE_BTN_Y && my <= HIGHSCORE_BTN_Y + BUTTON_HEIGHT) {
		gameState = 3;
	}
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= CREDIT_BTN_Y && my <= CREDIT_BTN_Y + BUTTON_HEIGHT) {
		gameState = 4;
	}
}

#endif