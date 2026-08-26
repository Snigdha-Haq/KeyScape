#ifndef PLAYBUTTON_H_INCLUDED
#define PLAYBUTTON_H_INCLUDED

extern int gameState; //shared variable from  main.cpp 

// ---------------- DIFFICULTY BUTTON LAYOUT ----------------

#define DIFF_BTN_WIDTH  200
#define DIFF_BTN_HEIGHT 60
#define DIFF_BTN_Y      330          //  common Y 

#define EASY_BTN_X    50             //  left key 
#define EASY_BTN_Y    DIFF_BTN_Y      

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

	if (bgImg == -1) {
		bgImg = iLoadImage("Image/playbutton.png");
		backBtnImg = iLoadImage("Image/backbutton.png");
		easyImg = iLoadImage("Image/easy.png");
		mediumImg = iLoadImage("Image/medium.png");
		hardImg = iLoadImage("Image/hard.png");
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
}

// ---------------- INPUT ----------------

inline void handlePlayButtonClicks(int mx, int my)
{
	if (mx >= EASY_BTN_X && mx <= EASY_BTN_X + DIFF_BTN_WIDTH &&
		my >= EASY_BTN_Y && my <= EASY_BTN_Y + DIFF_BTN_HEIGHT) {
		gameState = 1;
	}

	if (mx >= MEDIUM_BTN_X && mx <= MEDIUM_BTN_X + DIFF_BTN_WIDTH &&
		my >= MEDIUM_BTN_Y && my <= MEDIUM_BTN_Y + DIFF_BTN_HEIGHT) {
		
	}

	if (mx >= HARD_BTN_X && mx <= HARD_BTN_X + DIFF_BTN_WIDTH &&
		my >= HARD_BTN_Y && my <= HARD_BTN_Y + DIFF_BTN_HEIGHT) {
		
	}

	if (mx >= PLAY_BACK_BTN_X && mx <= PLAY_BACK_BTN_X + PLAY_BACK_BTN_WIDTH &&
		my >= PLAY_BACK_BTN_Y && my <= PLAY_BACK_BTN_Y + PLAY_BACK_BTN_HEIGHT) {
		gameState = 0;
	}
}
#endif