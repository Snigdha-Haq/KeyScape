#include "iGraphics.h"
#include "homepage.h"
#include "level1.h"
#include "level2.h"
#include "option.h"
#include "playbutton.h"   
int x = 0;
int y = 0;
int gameState = 0;   // 0 = Home, 1 = Level1, 2 = Option, 3 = Highscore, 4 = Credit, 5 = Play/Difficulty select, 6 = Level2
void iDraw()
{
	iClear();
	static int lastGameState = -1;
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
		iSetColor(255, 255, 255);
		iText(410, 300, "Highscore Screen (placeholder)");
	}
	else if (gameState == 4) {
		iSetColor(255, 255, 255);
		iText(430, 300, "Credits Screen (placeholder)");
	}
	else if (gameState == 5) {           // <-- ADDED
		renderPlayButtonPage();
	}
	else if (gameState == 6) {           // <-- ADDED (Level2)
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
		else if (gameState == 5) {       // <-- ADDED
			handlePlayButtonClicks(mx, my);
		}
		else if (gameState == 6) {       // <-- ADDED (Level2)
			handleLevel2DoorClicks(mx, my);
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {}
}
void iSpecialKeyboard(unsigned char key)
{
	/*if (gameState == 1) {
	handleLevel1Keyboard(key);
	}*/
}
void iKeyboard(unsigned char key)
{
	if (gameState == 1) {
		handleLevel1Keyboard(key);
	}
	if (gameState == 6) {                // <-- ADDED (Level2)
		handleLevel2Keyboard(key);
	}
	if (key == 27) {
		gameState = 0;
	}
}
void fixedUpdate() {
	level1_fixedUpdate();
	level2_fixedUpdate();                // <-- ADDED (Level2)
}
int main()
{
	iSetTimer(20, fixedUpdate);
	iInitialize(1000, 600, "KeyScape");
	iStart();
	return 0;
}
