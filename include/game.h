#ifndef GAME_H
#define GAME_H

#include <gba.h>

typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;

typedef enum {
    MODE_TITLE = 0,
    MODE_HUB = 1,
    MODE_DASH = 2,
    MODE_MEMORY = 3,
    MODE_RHYTHM = 4,
    MODE_RESULTS = 5
} GameMode;

typedef struct {
    int level;
    int score;
    int stars;
    int timer;
    int combo;
    int successCount;
    int failCount;
    bool paused;
    bool roundWon;
    bool gameCompleted;
    GameMode mode;

    Rect player;
    Rect goal;
    Rect bonus;
    Rect hazard;

    int memorySequence[16];
    int memoryLength;
    int memoryIndex;
    int rhythmBeat;
    int rhythmWindow;
    int frame;
} GameState;

void game_init(GameState* g);
void game_update(GameState* g, u16 keysDown, u16 keysHeld);
void game_draw(const GameState* g);

#endif
