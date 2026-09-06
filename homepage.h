#ifndef HOMEPAGE_H_INCLUDED
#define HOMEPAGE_H_INCLUDED

#include <cstdlib>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#endif

extern int gameState;

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
	if (startImg == -1) {
		homepageImg = iLoadImage("Image/homepagebg.png");
		startImg = iLoadImage("Image/play.png");
		optionImg = iLoadImage("Image/options.png");
		highscoreImg = iLoadImage("Image/highscore.png");
		creditImg = iLoadImage("Image/credits.png");
	}

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, homepageImg);

	iShowImage(CENTER_X, START_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, startImg);
	iShowImage(CENTER_X, OPTION_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, optionImg);
	iShowImage(CENTER_X, HIGHSCORE_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, highscoreImg);
	iShowImage(CENTER_X, CREDIT_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, creditImg);
}

// ---------------- CLICK HANDLING ----------------
inline void handleMenuClicks(int mx, int my) {
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