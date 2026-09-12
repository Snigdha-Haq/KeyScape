#ifndef PLAYBUTTON_H_INCLUDED
#define PLAYBUTTON_H_INCLUDED

#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int gameState; //shared variable from main.cpp 
extern bool isSoundMuted;

// ---------------- MENU SETTINGS UI CONSTANTS ----------------
static bool pb_showSettingsMenu = false;
#define PB_SETTING_BTN_X 945
#define PB_SETTING_BTN_Y 45
#define PB_SETTING_BTN_R 22

#define PB_SUB_BTN_R     20
#define PB_SUB_S_Y       100

// ---------------- DIFFICULTY BUTTON LAYOUT ----------------
#define DIFF_BTN_WIDTH  200
#define DIFF_BTN_HEIGHT 60
#define DIFF_BTN_Y      330          //  common Y 
#define EASY_BTN_X     50             //  left key 
#define EASY_BTN_Y     DIFF_BTN_Y     
#define MEDIUM_BTN_X  412             // middle key 
#define MEDIUM_BTN_Y  DIFF_BTN_Y     
#define HARD_BTN_X    770             // right key
#define HARD_BTN_Y    DIFF_BTN_Y     
// ---------------- BACK BUTTON (bottom-right now) ----------------
#define PLAY_BACK_BTN_X 412
#define PLAY_BACK_BTN_Y 0            
#define PLAY_BACK_BTN_WIDTH 200
#define PLAY_BACK_BTN_HEIGHT 60
// ---------------- RENDER ----------------
inline void renderPlayButtonPage()
{
	static int bgImg = -1, backBtnImg = -1;
	static int easyImg = -1, mediumImg = -1, hardImg = -1;
	static int btnSettings = -1, btnSoundOn = -1, btnSoundOff = -1;

	if (bgImg == -1) {
		bgImg = iLoadImage("Image/playbutton.png");
		backBtnImg = iLoadImage("Image/backbutton.png");
		easyImg = iLoadImage("Image/easy.png");
		mediumImg = iLoadImage("Image/medium.png");
		hardImg = iLoadImage("Image/hard.png");

		btnSettings = iLoadImage("Image/settings.png");
		if (btnSettings < 0) btnSettings = iLoadImage("settings.png");

		btnSoundOn = iLoadImage("Image/soundOn.png");
		if (btnSoundOn < 0) btnSoundOn = iLoadImage("soundOn.png");

		btnSoundOff = iLoadImage("Image/soundOff.png");
		if (btnSoundOff < 0) btnSoundOff = iLoadImage("soundOff.png");
	}
	// background
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImg);
	// EASY / MEDIUM / HARD
	iShowImage(EASY_BTN_X, EASY_BTN_Y, DIFF_BTN_WIDTH, DIFF_BTN_HEIGHT, easyImg);
	iShowImage(MEDIUM_BTN_X, MEDIUM_BTN_Y, DIFF_BTN_WIDTH, DIFF_BTN_HEIGHT, mediumImg);
	iShowImage(HARD_BTN_X, HARD_BTN_Y, DIFF_BTN_WIDTH, DIFF_BTN_HEIGHT, hardImg);
	// Back button
	if (backBtnImg != -1) {
		iShowImage(PLAY_BACK_BTN_X, PLAY_BACK_BTN_Y, PLAY_BACK_BTN_WIDTH, PLAY_BACK_BTN_HEIGHT, backBtnImg);
	}
	else {
		iSetColor(255, 0, 0);
		iFilledRectangle(PLAY_BACK_BTN_X, PLAY_BACK_BTN_Y, PLAY_BACK_BTN_WIDTH, PLAY_BACK_BTN_HEIGHT);
		iSetColor(255, 255, 255);
		iText(PLAY_BACK_BTN_X + 10, PLAY_BACK_BTN_Y + 12, "<--", GLUT_BITMAP_HELVETICA_18);
	}

	// Settings Button (44x44 px)
	if (btnSettings >= 0) {
		iShowImage(PB_SETTING_BTN_X - 22, PB_SETTING_BTN_Y - 22, 44, 44, btnSettings);
	}
	else {
		iSetColor(30, 45, 65);
		iFilledCircle(PB_SETTING_BTN_X, PB_SETTING_BTN_Y, PB_SETTING_BTN_R);
	}

	// Pop-up Sound On/Off Button (40x40 px)
	if (pb_showSettingsMenu) {
		if (isSoundMuted) {
			if (btnSoundOff >= 0) iShowImage(PB_SETTING_BTN_X - 20, PB_SUB_S_Y - 20, 40, 40, btnSoundOff);
			else {
				iSetColor(120, 120, 120);
				iFilledCircle(PB_SETTING_BTN_X, PB_SUB_S_Y, PB_SUB_BTN_R);
			}
		}
		else {
			if (btnSoundOn >= 0) iShowImage(PB_SETTING_BTN_X - 20, PB_SUB_S_Y - 20, 40, 40, btnSoundOn);
			else {
				iSetColor(230, 140, 20);
				iFilledCircle(PB_SETTING_BTN_X, PB_SUB_S_Y, PB_SUB_BTN_R);
			}
		}
	}
}
// ---------------- INPUT ----------------
inline void handlePlayButtonClicks(int mx, int my)
{
	// Settings Click
	float distSettings = sqrtf((float)((mx - PB_SETTING_BTN_X) * (mx - PB_SETTING_BTN_X) +
		(my - PB_SETTING_BTN_Y) * (my - PB_SETTING_BTN_Y)));
	if (distSettings <= PB_SETTING_BTN_R) {
		pb_showSettingsMenu = !pb_showSettingsMenu;
		return;
	}

	// Sound Toggle Click
	if (pb_showSettingsMenu) {
		float distS = sqrtf((float)((mx - PB_SETTING_BTN_X) * (mx - PB_SETTING_BTN_X) +
			(my - PB_SUB_S_Y) * (my - PB_SUB_S_Y)));
		if (distS <= PB_SUB_BTN_R) {
			isSoundMuted = !isSoundMuted;
			if (isSoundMuted) {
				mciSendString("stop mainMusic", NULL, 0, NULL);
			}
			return;
		}
	}

	if (mx >= EASY_BTN_X && mx <= EASY_BTN_X + DIFF_BTN_WIDTH &&
		my >= EASY_BTN_Y && my <= EASY_BTN_Y + DIFF_BTN_HEIGHT) {
		gameState = 1; // Easy -> Level1
	}
	if (mx >= MEDIUM_BTN_X && mx <= MEDIUM_BTN_X + DIFF_BTN_WIDTH &&
		my >= MEDIUM_BTN_Y && my <= MEDIUM_BTN_Y + DIFF_BTN_HEIGHT) {
		gameState = 6; // Medium -> Level2
	}
	if (mx >= HARD_BTN_X && mx <= HARD_BTN_X + DIFF_BTN_WIDTH &&
		my >= HARD_BTN_Y && my <= HARD_BTN_Y + DIFF_BTN_HEIGHT) {
		gameState = 7; // Hard -> Level3
	}
	if (mx >= PLAY_BACK_BTN_X && mx <= PLAY_BACK_BTN_X + PLAY_BACK_BTN_WIDTH &&
		my >= PLAY_BACK_BTN_Y && my <= PLAY_BACK_BTN_Y + PLAY_BACK_BTN_HEIGHT) {
		gameState = 0;
	}
}
#endif