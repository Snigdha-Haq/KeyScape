#ifndef L3DOOR4_H_INCLUDED
#define L3DOOR4_H_INCLUDED

#include <cstdlib>
#include <cstdio>

extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

static int l3d4_factorA = 0;
static int l3d4_factorB = 0;
static int l3d4_options[4];
static int l3d4_correctIndex = 0;

inline void L3Door4_GenerateTask()
{
	l3d4_factorA = 3 + rand() % 10;
	l3d4_factorB = 3 + rand() % 10;
	int correct = l3d4_factorA * l3d4_factorB;
	l3d4_correctIndex = rand() % 4;

	for (int i = 0; i < 4; i++) {
		if (i == l3d4_correctIndex) {
			l3d4_options[i] = correct;
		}
		else {
			int fake;
			do {
				fake = correct + (rand() % 15 - 7);
			} while (fake == correct || fake <= 0);
			l3d4_options[i] = fake;
		}
	}
}

inline void L3Door4_RenderTask(int screenW, int optW, int optH, int optGap, int optY)
{
	char buf[64];
	iSetColor(255, 255, 255);
	sprintf_s(buf, sizeof(buf), "%d  x  %d = ?", l3d4_factorA, l3d4_factorB);
	iText(screenW / 2 - 50, 250, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		iSetColor(255, 255, 255);
		iFilledRectangle(bx, optY, optW, optH);
		iSetColor(60, 60, 60);
		iRectangle(bx, optY, optW, optH);

		sprintf_s(buf, sizeof(buf), "%d", l3d4_options[i]);
		iSetColor(0, 0, 0);
		iText(bx + optW / 2 - 12, optY + optH / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
	}
}

inline bool L3Door4_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		if (mx >= bx && mx <= bx + optW && my >= optY && my <= optY + optH) {
			if (i == l3d4_correctIndex) {
				*doorVisited = true;
				*insideTask = false;
				level3_updateScore(200);
				level3_playPlusPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "Correct! Door 4 Unlocked.");
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
					L3Door4_GenerateTask();
				}
			}
			return true;
		}
	}
	return false;
}

#endif