#ifndef OPTION_H_INCLUDED
#define OPTION_H_INCLUDED

extern int gameState; // shared variable from main.cpp

// Back Button Position & Size
#define BACK_BTN_X 750
#define BACK_BTN_Y 30            
#define BACK_BTN_WIDTH 200       
#define BACK_BTN_HEIGHT 60       

inline void renderOptionPage() {
	static int optionBgImg = -1, backBtnImg = -1;
	// bg load 1st time
	if (optionBgImg == -1) {
		optionBgImg = iLoadImage("Image/options_bg.png");
		backBtnImg = iLoadImage("Image/backbutton.png");
	}
	// bg image
	iShowImage(0, 0, 1000, 600, optionBgImg);

	// মাঝ বরাবর কন্ট্রোল নির্দেশিকা টেক্সট (কালো রঙ, একই সাইজ ও ফন্ট)
	iSetColor(0, 0, 0);

	// 1. UP : Jump
	iText(420, 360, "UP     :  Jump", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(421, 360, "UP     :  Jump", GLUT_BITMAP_TIMES_ROMAN_24); // বোল্ড এফেক্ট

	// 2. DN : Slide
	iText(420, 300, "DN     :  Slide", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(421, 300, "DN     :  Slide", GLUT_BITMAP_TIMES_ROMAN_24);

	// 3. RIGHT : Run
	iText(420, 240, "RIGHT  :  Run", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(421, 240, "RIGHT  :  Run", GLUT_BITMAP_TIMES_ROMAN_24);

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
}

// click at Back Button to back Home Screen
inline void handleOptionClicks(int mx, int my) {
	if (mx >= BACK_BTN_X && mx <= BACK_BTN_X + BACK_BTN_WIDTH &&
		my >= BACK_BTN_Y && my <= BACK_BTN_Y + BACK_BTN_HEIGHT) {
		gameState = 0; // Go back to Home Screen
	}
}

#endif