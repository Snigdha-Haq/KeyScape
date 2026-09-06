#include "iGraphics.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "level1.h"
#include "level2.h"
#include "option.h"
#include "playbutton.h"
#include "score-board.h"
#include "homepage.h"

int x = 0;
int y = 0;
int gameState = 0; // 0 = Home, 1 = Level1, 2 = Option, 3 = Highscore, 4 = Credit, 5 = Play/Difficulty select, 6 = Level2

static bool isBgMusicPlaying = false;
static bool isMainMenuMusicPlaying = false;

// ---------------- ইন-গেম ব্যাকগ্রাউন্ড মিউজিক ----------------
inline void playBgMusic() {
	if (!isBgMusicPlaying) {
		mciSendString("close bgMusic", NULL, 0, NULL);
		mciSendString("open \"Audios/movement.mp3\" type mpegvideo alias bgMusic", NULL, 0, NULL);
		mciSendString("play bgMusic repeat", NULL, 0, NULL);
		isBgMusicPlaying = true;
	}
}

inline void stopBgMusic() {
	if (isBgMusicPlaying) {
		mciSendString("stop bgMusic", NULL, 0, NULL);
		mciSendString("close bgMusic", NULL, 0, NULL);
		isBgMusicPlaying = false;
	}
}

// ---------------- মেইন মেনু ও অপশন ব্যাকগ্রাউন্ড মিউজিক ----------------
inline void playMainMenuMusic() {
	if (!isMainMenuMusicPlaying) {
		mciSendString("close mainMusic", NULL, 0, NULL);
		mciSendString("open \"Audios/mainSound.MP3\" type mpegvideo alias mainMusic", NULL, 0, NULL);
		mciSendString("play mainMusic repeat", NULL, 0, NULL);
		isMainMenuMusicPlaying = true;
	}
}

inline void stopMainMenuMusic() {
	if (isMainMenuMusicPlaying) {
		mciSendString("stop mainMusic", NULL, 0, NULL);
		mciSendString("close mainMusic", NULL, 0, NULL);
		isMainMenuMusicPlaying = false;
	}
}

void iDraw()
{
	iClear();
	static int lastGameState = -1;

	// অডিও ট্রানজিশন ম্যানেজমেন্ট:
	// ১. গেমপ্লে স্ক্রিন (লেভেল ১ বা লেভেল ২)
	if ((gameState == 1 || gameState == 6) && !level2_gameOver && !level2_keyFound) {
		stopMainMenuMusic();
		playBgMusic();
	}
	// ২. মেনু ও অন্যান্য সব পেজ (Home, Option, Highscore, Credit, Difficulty Select)
	else if (gameState == 0 || gameState == 2 || gameState == 3 || gameState == 4 || gameState == 5) {
		stopBgMusic();
		playMainMenuMusic();
	}
	// ৩. গেম ওভার বা উইন হলে সব ব্যাকগ্রাউন্ড মিউজিক থামবে
	else {
		stopBgMusic();
		stopMainMenuMusic();
	}

	if (gameState == 0) {
		renderHomepage();
	}
	else if (gameState == 1) {
		if (lastGameState != 1) {
			setupLevel1();
		}
		renderLevel1();
	}
	else if (gameState == 2) {
		renderOptionPage();
	}
	else if (gameState == 3) {
		renderHighscorePage();
	}
	else if (gameState == 4) {
		iSetColor(255, 255, 255);
		iText(430, 300, "Credits Screen (placeholder)");
	}
	else if (gameState == 5) {
		renderPlayButtonPage();
	}
	else if (gameState == 6) {
		if (lastGameState != 6) {
			setupLevel2();
		}
		renderLevel2();
	}

	lastGameState = gameState;
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (gameState == 0) {
			handleMenuClicks(mx, my);
		}
		else if (gameState == 1) {
			handleLevel1DoorClicks(mx, my);
		}
		else if (gameState == 2) {
			handleOptionClicks(mx, my);
		}
		else if (gameState == 3) {
			handleHighscoreClicks(mx, my);
		}
		else if (gameState == 5) {
			handlePlayButtonClicks(mx, my);
		}
		else if (gameState == 6) {
			handleLevel2DoorClicks(mx, my);
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {}
}

void iSpecialKeyboard(unsigned char key)
{
	if (gameState == 6) {
		handleLevel2SpecialKeyboard(key);
	}
}

void iKeyboard(unsigned char key)
{
	if (gameState == 1) {
		handleLevel1Keyboard(key);
	}
	if (gameState == 6) {
		handleLevel2Keyboard(key);
	}
	if (key == 27) { // ESC চেপে হোমে ফেরা
		gameState = 0;
	}
}

void fixedUpdate()
{
	if (gameState == 1) level1_fixedUpdate();
	if (gameState == 6) level2_fixedUpdate();
}

int main()
{
	iSetTimer(20, fixedUpdate);
	iInitialize(1000, 600, "KeyScape");
	iStart();

	// গেম উইন্ডো বন্ধ হলে অডিও ক্লিনআপ
	stopBgMusic();
	stopMainMenuMusic();
	return 0;
}