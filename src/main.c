#include <gba.h>
#include <stdio.h>
#include <string.h>

#include "game.h"

#define SCREEN_W 240
#define SCREEN_H 160

#define RGB15(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#define MODE3 0x0003
#define BG2_ENABLE 0x0400

static u16* const videoBuffer = (u16*)0x06000000;

static inline void plotPixel(int x, int y, u16 color) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        videoBuffer[y * SCREEN_W + x] = color;
    }
}

static void fillRect(Rect r, u16 color) {
    for (int yy = 0; yy < r.h; yy++) {
        for (int xx = 0; xx < r.w; xx++) {
            plotPixel(r.x + xx, r.y + yy, color);
        }
    }
}

static bool overlaps(Rect a, Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

static void clearScreen(u16 color) {
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
        videoBuffer[i] = color;
    }
}

static int nextRand(int seed) {
    return (seed * 1103515245 + 12345) & 0x7fffffff;
}

static void resetDashCourse(GameState* g) {
    g->player = (Rect){14, 70, 10, 10};
    g->goal = (Rect){206, 63, 20, 24};
    g->bonus = (Rect){98 + (g->level * 7 % 70), 36 + (g->level * 11 % 60), 8, 8};
    g->hazard = (Rect){100 + (g->level * 5 % 60), 100 + (g->level * 3 % 20), 14, 10};
    g->timer = 60 * 20;
    g->roundWon = false;
}

static void startMemoryRound(GameState* g) {
    g->timer = 60 * 15;
    g->memoryLength = 3 + (g->level % 4);
    if (g->memoryLength > 8) g->memoryLength = 8;
    g->memoryIndex = 0;

    int seed = 7 + g->level * 13 + g->score;
    for (int i = 0; i < g->memoryLength; i++) {
        seed = nextRand(seed);
        g->memorySequence[i] = seed % 4;
    }
}

static void startRhythmRound(GameState* g) {
    g->timer = 60 * 18;
    g->rhythmBeat = 0;
    g->rhythmWindow = 0;
    g->combo = 0;
}

static void enterHub(GameState* g) {
    g->mode = MODE_HUB;
    g->timer = 60 * 30;
    g->paused = false;
}

void game_init(GameState* g) {
    memset(g, 0, sizeof(*g));
    g->level = 1;
    g->mode = MODE_TITLE;
    g->timer = 0;
}

static void updateTitle(GameState* g, u16 keysDown) {
    if (keysDown & KEY_A) {
        g->score = 0;
        g->stars = 0;
        g->successCount = 0;
        g->failCount = 0;
        g->level = 1;
        enterHub(g);
    }
}

static void updateHub(GameState* g, u16 keysDown) {
    if (keysDown & KEY_LEFT) {
        g->mode = MODE_DASH;
        resetDashCourse(g);
    } else if (keysDown & KEY_UP) {
        g->mode = MODE_MEMORY;
        startMemoryRound(g);
    } else if (keysDown & KEY_RIGHT) {
        g->mode = MODE_RHYTHM;
        startRhythmRound(g);
    }
}

static void completeMiniGame(GameState* g, bool success) {
    if (success) {
        g->successCount++;
        g->stars++;
        g->score += 100 + g->combo * 5;
    } else {
        g->failCount++;
        if (g->score >= 20) g->score -= 20;
    }

    if (g->successCount >= 5) {
        g->mode = MODE_RESULTS;
        g->gameCompleted = true;
    } else {
        g->level++;
        enterHub(g);
    }
}

static void updateDash(GameState* g, u16 keysHeld) {
    const int speed = 1 + (g->level / 4);

    if (keysHeld & KEY_LEFT) g->player.x -= speed;
    if (keysHeld & KEY_RIGHT) g->player.x += speed;
    if (keysHeld & KEY_UP) g->player.y -= speed;
    if (keysHeld & KEY_DOWN) g->player.y += speed;

    if (g->player.x < 0) g->player.x = 0;
    if (g->player.y < 12) g->player.y = 12;
    if (g->player.x + g->player.w > SCREEN_W) g->player.x = SCREEN_W - g->player.w;
    if (g->player.y + g->player.h > SCREEN_H) g->player.y = SCREEN_H - g->player.h;

    if (--g->timer <= 0) {
        completeMiniGame(g, false);
        return;
    }

    if (overlaps(g->player, g->bonus)) {
        g->score += 25;
        g->combo++;
        g->bonus.x = (g->bonus.x + 43) % (SCREEN_W - g->bonus.w);
        g->bonus.y = 12 + ((g->bonus.y + 17) % (SCREEN_H - 12 - g->bonus.h));
    }

    if (overlaps(g->player, g->hazard)) {
        g->combo = 0;
        if (g->score >= 10) g->score -= 10;
        g->hazard.x = (g->hazard.x + 67) % (SCREEN_W - g->hazard.w);
        g->hazard.y = 12 + ((g->hazard.y + 29) % (SCREEN_H - 12 - g->hazard.h));
    }

    if (overlaps(g->player, g->goal)) {
        completeMiniGame(g, true);
    }
}

static void updateMemory(GameState* g, u16 keysDown) {
    if (--g->timer <= 0) {
        completeMiniGame(g, false);
        return;
    }

    int pick = -1;
    if (keysDown & KEY_LEFT) pick = 0;
    else if (keysDown & KEY_UP) pick = 1;
    else if (keysDown & KEY_RIGHT) pick = 2;
    else if (keysDown & KEY_DOWN) pick = 3;

    if (pick >= 0) {
        if (pick == g->memorySequence[g->memoryIndex]) {
            g->memoryIndex++;
            g->combo++;
            g->score += 20;
            if (g->memoryIndex >= g->memoryLength) {
                completeMiniGame(g, true);
            }
        } else {
            completeMiniGame(g, false);
        }
    }
}

static void updateRhythm(GameState* g, u16 keysDown) {
    if (--g->timer <= 0) {
        completeMiniGame(g, g->combo >= 8);
        return;
    }

    g->rhythmWindow = (g->frame % 40);

    if (keysDown & KEY_A) {
        int d = g->rhythmWindow - 20;
        if (d < 0) d = -d;

        if (d <= 4) {
            g->combo += 2;
            g->score += 30;
            g->rhythmBeat++;
        } else if (d <= 8) {
            g->combo += 1;
            g->score += 15;
            g->rhythmBeat++;
        } else {
            if (g->combo > 0) g->combo--;
        }

        if (g->rhythmBeat >= 10) {
            completeMiniGame(g, true);
        }
    }
}

void game_update(GameState* g, u16 keysDown, u16 keysHeld) {
    g->frame++;

    if (keysDown & KEY_START && g->mode != MODE_TITLE && g->mode != MODE_RESULTS) {
        g->paused = !g->paused;
    }

    if (g->paused) return;

    if (g->mode == MODE_TITLE) {
        updateTitle(g, keysDown);
    } else if (g->mode == MODE_HUB) {
        updateHub(g, keysDown);
    } else if (g->mode == MODE_DASH) {
        updateDash(g, keysHeld);
    } else if (g->mode == MODE_MEMORY) {
        updateMemory(g, keysDown);
    } else if (g->mode == MODE_RHYTHM) {
        updateRhythm(g, keysDown);
    } else if (g->mode == MODE_RESULTS) {
        if (keysDown & KEY_A) {
            game_init(g);
        }
    }
}

static void drawHud(const GameState* g) {
    Rect hud = {0, 0, SCREEN_W, 12};
    fillRect(hud, RGB15(3, 3, 3));
    iprintf("\x1b[1;1HLv:%d Score:%d Stars:%d Combo:%d   ", g->level, g->score, g->stars, g->combo);
}

static void drawTitle(void) {
    clearScreen(RGB15(25, 25, 31));
    iprintf("\x1b[4;3HPOTTY & UNPOTTY DELUXE 2000s");
    iprintf("\x1b[6;3HKid-safe retro challenge set");
    iprintf("\x1b[9;3HPress A to Start");
    iprintf("\x1b[11;3H5 wins unlock Deluxe Complete");
}

static void drawDash(const GameState* g) {
    clearScreen(RGB15(28, 28, 31));
    fillRect(g->goal, RGB15(16, 16, 16));
    fillRect(g->player, RGB15(0, 0, 31));
    fillRect(g->bonus, RGB15(31, 31, 0));
    fillRect(g->hazard, RGB15(0, 24, 24));
    drawHud(g);
    iprintf("\x1b[3;1HDASH! Reach goal before time:%d ", g->timer / 60);
}

static void drawMemory(const GameState* g) {
    clearScreen(RGB15(31, 28, 24));
    drawHud(g);
    iprintf("\x1b[3;1HMEMORY: repeat with D-Pad");
    iprintf("\x1b[4;1HNeed:%d Step:%d Time:%d", g->memoryLength, g->memoryIndex + 1, g->timer / 60);
    iprintf("\x1b[6;1HSequence: ");
    for (int i = 0; i < g->memoryLength; i++) {
        iprintf("%d ", g->memorySequence[i] + 1);
    }
}

static void drawRhythm(const GameState* g) {
    clearScreen(RGB15(22, 24, 31));
    drawHud(g);
    int barW = 180;
    int markerX = 30 + (g->rhythmWindow * barW / 40);

    fillRect((Rect){30, 70, barW, 8}, RGB15(5, 5, 5));
    fillRect((Rect){30 + (20 * barW / 40) - 4, 66, 8, 16}, RGB15(0, 31, 0));
    fillRect((Rect){markerX, 64, 3, 20}, RGB15(31, 0, 0));

    iprintf("\x1b[3;1HRHYTHM: Press A on green center");
    iprintf("\x1b[4;1HBeats:%d/10 Time:%d", g->rhythmBeat, g->timer / 60);
}

static void drawResults(const GameState* g) {
    clearScreen(RGB15(31, 31, 24));
    iprintf("\x1b[4;3HDeluxe Complete!");
    iprintf("\x1b[6;3HSuccess:%d  Fails:%d", g->successCount, g->failCount);
    iprintf("\x1b[7;3HStars:%d   Score:%d", g->stars, g->score);
    iprintf("\x1b[10;3HGreat job! Press A to restart");
}

void game_draw(const GameState* g) {
    if (g->mode == MODE_TITLE) {
        drawTitle();
    } else if (g->mode == MODE_HUB) {
        clearScreen(RGB15(24, 28, 24));
        drawHud(g);
        iprintf("\x1b[3;1HChoose a mini-game:");
        iprintf("\x1b[5;1HLEFT  : Dash to Potty");
        iprintf("\x1b[6;1HUP    : Memory Signals");
        iprintf("\x1b[7;1HRIGHT : Rhythm Clap");
        iprintf("\x1b[9;1HWin 5 rounds for Deluxe Complete");
    } else if (g->mode == MODE_DASH) {
        drawDash(g);
    } else if (g->mode == MODE_MEMORY) {
        drawMemory(g);
    } else if (g->mode == MODE_RHYTHM) {
        drawRhythm(g);
    } else if (g->mode == MODE_RESULTS) {
        drawResults(g);
    }

    if (g->paused) {
        iprintf("\x1b[18;1HPAUSED (START)");
    }
}

int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;
    consoleDemoInit();

    GameState game;
    game_init(&game);

    while (1) {
        VBlankIntrWait();
        scanKeys();

        game_update(&game, keysDown(), keysHeld());
        game_draw(&game);
    }

    return 0;
}
