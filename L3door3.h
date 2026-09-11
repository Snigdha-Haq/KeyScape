#ifndef L3DOOR3_H_INCLUDED
#define L3DOOR3_H_INCLUDED

#include <cstdlib>

extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

static int l3d3_targetColor[3];
static int l3d3_options[4][3];
static int l3d3_correctIndex = 0;

inline void L3Door3_GenerateTask()
{
	l3d3_targetColor[0] = rand() % 256;
	l3d3_targetColor[1] = rand() % 256;
	l3d3_targetColor[2] = rand() % 256;

	l3d3_correctIndex = rand() % 4;
	for (int i = 0; i < 4; i++) {
		if (i == l3d3_correctIndex) {
			l3d3_options[i][0] = l3d3_targetColor[0];
			l3d3_options[i][1] = l3d3_targetColor[1];
			l3d3_options[i][2] = l3d3_targetColor[2];
		}
		else {
			l3d3_options[i][0] = rand() % 256;
			l3d3_options[i][1] = rand() % 256;
			l3d3_options[i][2] = rand() % 256;
		}
	}
}

inline void L3Door3_RenderTask(int screenW, int optW, int optH, int optGap, int optY)
{
	iSetColor(255, 255, 255);
	iText(screenW / 2 - 150, 220, "Select the matching color from below:", GLUT_BITMAP_HELVETICA_18);

	iSetColor(l3d3_targetColor[0], l3d3_targetColor[1], l3d3_targetColor[2]);
	iFilledRectangle(screenW / 2 - 35, 240, 70, 60);
	iSetColor(0, 0, 0);
	iRectangle(screenW / 2 - 35, 240, 70, 60);

	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		iSetColor(l3d3_options[i][0], l3d3_options[i][1], l3d3_options[i][2]);
		iFilledRectangle(bx, optY, optW, optH);
		iSetColor(0, 0, 0);
		iRectangle(bx, optY, optW, optH);
	}
}

inline bool L3Door3_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		if (mx >= bx && mx <= bx + optW && my >= optY && my <= optY + optH) {
			if (i == l3d3_correctIndex) {
				*doorVisited = true;
				*insideTask = false;
				level3_updateScore(200);
				level3_playPlusPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "Correct! Door 3 Unlocked.");
				level3_messageTimer = 90;
			}
			else {
				level3_energy -= 15;
				level3_updateScore(-100);
				level3_playNegPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "Wrong Answer! Try again.");
				level3_messageTimer = 70;
				if (level3_energy <= 0) {
					level3_energy = 0;
					level3_gameOver = true;
				}
				else {
					L3Door3_GenerateTask();
				}
			}
			return true;
		}
	}
	return false;
}

#endif