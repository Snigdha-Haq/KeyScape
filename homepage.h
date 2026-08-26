#ifndef HOMEPAGE_H_INCLUDED
#define HOMEPAGE_H_INCLUDED

#include <cstdlib>   // needed for exit()

// ---------------- SCREEN ----------------
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600

extern int gameState; // Shared variable declared in main.cpp

// ---------------- BUTTON LAYOUT ----------------
// These were MISSING before - that's the main reason nothing worked.
#define BUTTON_WIDTH  200
#define BUTTON_HEIGHT 60
#define BUTTON_GAP    20
#define CENTER_X ((SCREEN_WIDTH - BUTTON_WIDTH) / 2)

// Stack 4 buttons centered vertically. Start ends up on top,
// Credit on the bottom, evenly spaced with BUTTON_GAP between them.
#define CREDIT_BTN_Y    ((SCREEN_HEIGHT - (4 * BUTTON_HEIGHT + 3 * BUTTON_GAP)) / 2)
#define HIGHSCORE_BTN_Y (CREDIT_BTN_Y    + BUTTON_HEIGHT + BUTTON_GAP)
#define OPTION_BTN_Y    (HIGHSCORE_BTN_Y + BUTTON_HEIGHT + BUTTON_GAP)
#define START_BTN_Y     (OPTION_BTN_Y    + BUTTON_HEIGHT + BUTTON_GAP)

// ---------------- RENDER ----------------
inline void renderHomepage() {
	

	// Load the 4 button PNGs ONCE the first time this runs, not every frame.
	// Rename these filenames to match whatever your actual PNG files are called.
	static int homepageImg = -1, startImg = -1, optionImg = -1, highscoreImg = -1, creditImg = -1;
	if (startImg == -1) {
		homepageImg = iLoadImage("Image/homepagebg.png");
		startImg = iLoadImage("Image/play.png");
		optionImg = iLoadImage("Image/options.png");
		highscoreImg = iLoadImage("Image/highscore.png");
		creditImg = iLoadImage("Image/credits.png");
	}
	// Background stays exactly as before

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, homepageImg);

	// Draw all 4 buttons every frame, in the middle of the screen
	iShowImage(CENTER_X, START_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, startImg);
	iShowImage(CENTER_X, OPTION_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, optionImg);
	iShowImage(CENTER_X, HIGHSCORE_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, highscoreImg);
	iShowImage(CENTER_X, CREDIT_BTN_Y, BUTTON_WIDTH, BUTTON_HEIGHT, creditImg);
}

// ---------------- CLICK HANDLING ----------------
// Call this from your iMouse() function in main.cpp, passing the
// mouse click coordinates, e.g.:
//     void iMouse(int button, int state, int mx, int my) {
//         if (state == GLUT_DOWN) handleMenuClicks(mx, my);
//     }
inline void handleMenuClicks(int mx, int my) {
	// Start
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= START_BTN_Y && my <= START_BTN_Y + BUTTON_HEIGHT) {
		gameState = 5; // Start -> now goes to Difficulty select page (was 1)
	}
	// Option
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= OPTION_BTN_Y && my <= OPTION_BTN_Y + BUTTON_HEIGHT) {
		gameState = 2; // Options screen
	}
	// Highscore
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= HIGHSCORE_BTN_Y && my <= HIGHSCORE_BTN_Y + BUTTON_HEIGHT) {
		gameState = 3; // Highscore screen
	}
	// Credit
	if (mx >= CENTER_X && mx <= CENTER_X + BUTTON_WIDTH &&
		my >= CREDIT_BTN_Y && my <= CREDIT_BTN_Y + BUTTON_HEIGHT) {
		gameState = 4; // Credits screen
	}
}

#endif