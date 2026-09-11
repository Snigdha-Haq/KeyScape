#ifndef L3DOOR2_H_INCLUDED
#define L3DOOR2_H_INCLUDED

#include <cstdlib>
#include <cstdio>

extern int level3_energy;
extern bool level3_gameOver;
extern char level3_message[120];
extern int level3_messageTimer;

void level3_updateScore(int addPoints);
void level3_playPlusPointSound();
void level3_playNegPointSound();

static int l3d2_puzzleSeq[4];
static int l3d2_blankIndex = 0;
static int l3d2_step = 0;
static int l3d2_options[4];
static int l3d2_correctIndex = 0;

inline void L3Door2_GenerateTask()
{
	l3d2_step = 2 + rand() % 5;
	int start = 1 + rand() % 12;
	for (int i = 0; i < 4; i++) l3d2_puzzleSeq[i] = start + i * l3d2_step;
	l3d2_blankIndex = rand() % 4;
	int correct = l3d2_puzzleSeq[l3d2_blankIndex];
	l3d2_correctIndex = rand() % 4;

	for (int i = 0; i < 4; i++) {
		if (i == l3d2_correctIndex) {
			l3d2_options[i] = correct;
		}
		else {
			int fake;
			do {
				fake = correct + (rand() % 9 - 4);
			} while (fake == correct || fake <= 0);
			l3d2_options[i] = fake;
		}
	}
}

inline void L3Door2_RenderTask(int screenW, int optW, int optH, int optGap, int optY)
{
	char buf[64];
	iSetColor(255, 255, 255);
	iText(screenW / 2 - 160, 230, "Find the missing number in sequence:", GLUT_BITMAP_HELVETICA_18);

	int seqX = screenW / 2 - 140;
	for (int i = 0; i < 4; i++) {
		if (i == l3d2_blankIndex) sprintf_s(buf, sizeof(buf), "_");
		else sprintf_s(buf, sizeof(buf), "%d", l3d2_puzzleSeq[i]);
		iText(seqX + i * 70, 260, buf, GLUT_BITMAP_TIMES_ROMAN_24);
	}

	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		iSetColor(255, 255, 255);
		iFilledRectangle(bx, optY, optW, optH);
		iSetColor(60, 60, 60);
		iRectangle(bx, optY, optW, optH);

		sprintf_s(buf, sizeof(buf), "%d", l3d2_options[i]);
		iSetColor(0, 0, 0);
		iText(bx + optW / 2 - 12, optY + optH / 2 - 5, buf, GLUT_BITMAP_HELVETICA_18);
	}
}

inline bool L3Door2_HandleClick(int mx, int my, int screenW, int optW, int optH, int optGap, int optY, bool* doorVisited, bool* insideTask)
{
	int totalW = 4 * optW + 3 * optGap;
	int startX = (screenW - totalW) / 2;

	for (int i = 0; i < 4; i++) {
		int bx = startX + i * (optW + optGap);
		if (mx >= bx && mx <= bx + optW && my >= optY && my <= optY + optH) {
			if (i == l3d2_correctIndex) {
				*doorVisited = true;
				*insideTask = false;
				level3_updateScore(200);
				level3_playPlusPointSound();
				strcpy_s(level3_message, sizeof(level3_message), "Correct! Door 2 Unlocked.");
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
					L3Door2_GenerateTask();
				}
			}
			return true;
		}
	}
	return false;
}

#endif