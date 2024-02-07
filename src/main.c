#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "drawutil.h"  // draw* functions
#include "flipanimator.h"  // FlipAnimator related functions
#include "reversi.h"  // rev* functions, RevBoard, RevBitboard

uiAreaHandler g_handler;
uiWindow* g_mainwin;
uiCombobox* g_combo_boxes[2];
RevBoard *g_board;

_UI_ENUM(GameState) {
    GAME_INIT = 0,  // Drawing the initialized board.
    GAME_MOVE,  // Waiting for the next move.
    GAME_FLIP,  // Drawing flip animations.
    GAME_END,  // The game is over.
};
GameState g_game_state;

_UI_ENUM(PlayerMode) {
    PLAYER_HUMAN = 0,  // Uses the user inputs
    PLAYER_RANDOM,  // Generates moves randomly
    PLAYER_MCS,  // Generates moves with the monte carlo search
};
PlayerMode g_player[2];

FlipAnimator g_flip_animator;

// This will be called by uiAreaQueueRedrawAll
static void handlerDraw(uiAreaHandler *a, uiArea *area, uiAreaDrawParams *p)
{
    // Draw the board and disks on the canvas
    drawBoard(p);
    drawDisks(p->Context, g_board, DISK_BLACK);
    drawDisks(p->Context, g_board, DISK_WHITE);
    if (g_game_state == GAME_FLIP) {
        drawFlippedDisks(p->Context, g_board, &g_flip_animator);
    }
}

static void handlerMouseEvent(uiAreaHandler *a, uiArea *area, uiAreaMouseEvent *e)
{
    if (g_game_state != GAME_MOVE || g_player[revGetCurrentPlayer(g_board)] != PLAYER_HUMAN) return;

    if (e->Down) {
        int x = (int)((e->X - GRID_OFS) / GRID_SIZE);
        int y = (int)((e->Y - GRID_OFS) / GRID_SIZE);
        if (revIsLegalMoveXY(g_board, x, y)) {
            // Accept the user input
            RevBitboard flipped = revMoveXY(g_board, x, y);
            startFlipAnimation(&g_flip_animator, flipped);
            g_game_state = GAME_FLIP;
        }
    }
}

static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *a, int left)
{
    // do nothing
}

static void handlerDragBroken(uiAreaHandler *ah, uiArea *a)
{
    // do nothing
}

static int handlerKeyEvent(uiAreaHandler *ah, uiArea *a, uiAreaKeyEvent *e)
{
    // reject all keys
    return 0;
}

static int onClosing(uiWindow *w, void *data)
{
    uiQuit();
    return 1;
}

static int onShouldQuit(void *data)
{
    uiWindow *mainwin = uiWindow(data);
    uiControlDestroy(uiControl(mainwin));
    return 1;
}

static void checkWinner()
{
    const char* msg;
    int winner = revGetWinner(g_board);
    if (winner == DISK_BLACK)
        msg = "Black Win!";
    else if (winner == DISK_WHITE)
        msg = "White Win!";
    else
        msg = "Draw.";
    uiMsgBox(g_mainwin, msg, "");
}

static int onMainLoop(void *data)
{
    if (g_game_state == GAME_END) return 1;

    if (g_game_state == GAME_INIT) {
        uiAreaQueueRedrawAll(uiArea(data));  // Call handlerDraw()
        g_game_state = GAME_MOVE;
        return 1;
    }

    if (g_game_state == GAME_FLIP) {
        if (isFinished(&g_flip_animator)) {
            // End the flip animation
            g_game_state = GAME_MOVE;
            initFlipAnimator(&g_flip_animator);
            if (!revHasLegalMoves(g_board)) {
                // The next player has no legal moves.
                revChangePlayer(g_board);
                if (!revHasLegalMoves(g_board)) {
                    // No one has legal moves.
                    g_game_state = GAME_END;
                    checkWinner();
                }
            }
        }
        uiAreaQueueRedrawAll(uiArea(data));  // Call handlerDraw()
        stepAnimation(&g_flip_animator);
    }

    PlayerMode player_mode = g_player[revGetCurrentPlayer(g_board)];
    if (g_game_state == GAME_MOVE && player_mode != PLAYER_HUMAN) {
        // The next player is CPU.
        int move = 0;
        if (player_mode == PLAYER_RANDOM) {
            move = revGenMoveRandom(g_board);
        } else if (player_mode == PLAYER_MCS) {
            move = revGenMoveMonteCarlo(g_board, 20000);
        }
        RevBitboard flipped = revMove(g_board, move);
        startFlipAnimation(&g_flip_animator, flipped);
        g_game_state = GAME_FLIP;
    }

    return 1;
}

static void onInit(uiButton *b, void *data)
{
    g_player[DISK_BLACK] = uiComboboxSelected(g_combo_boxes[DISK_BLACK]);
    g_player[DISK_WHITE] = uiComboboxSelected(g_combo_boxes[DISK_WHITE]);
    initFlipAnimator(&g_flip_animator);
    revInitBoard(g_board);
    g_game_state = GAME_INIT;
}

void createWindow()
{
    // Main window
    g_mainwin = uiNewWindow("libui Reversi Demo", 400, 440, 1);
    uiWindowOnClosing(g_mainwin, onClosing, NULL);
    uiOnShouldQuit(onShouldQuit, g_mainwin);
    uiWindowSetMargined(g_mainwin, 1);

    // Main container
    uiBox* vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    uiWindowSetChild(g_mainwin, uiControl(vbox));

    // Drawing area
    g_handler.Draw = handlerDraw;
    g_handler.MouseEvent = handlerMouseEvent;
    g_handler.MouseCrossed = handlerMouseCrossed;
    g_handler.DragBroken = handlerDragBroken;
    g_handler.KeyEvent = handlerKeyEvent;

    uiArea* area = uiNewArea(&g_handler);
    uiBoxAppend(vbox, uiControl(area), 1);
    uiTimer(10, onMainLoop, area);

    // Combo boxes and Buttons
    uiBox* ctrl_box = uiNewHorizontalBox();
    uiBoxSetPadded(ctrl_box, 1);

    uiCombobox *combo1 = uiNewCombobox();
    uiComboboxAppend(combo1, "Player1");
    uiComboboxAppend(combo1, "CPU (Random)");
    uiComboboxAppend(combo1, "CPU (MCS)");
    uiComboboxSetSelected(combo1, PLAYER_HUMAN);
    uiBoxAppend(ctrl_box, uiControl(combo1), 0);
    g_combo_boxes[DISK_BLACK] = combo1;

    uiLabel* vs = uiNewLabel("vs");
    uiBoxAppend(ctrl_box, uiControl(vs), 0);

    uiCombobox* combo2 = uiNewCombobox();
    uiComboboxAppend(combo2, "Player2");
    uiComboboxAppend(combo2, "CPU (Random)");
    uiComboboxAppend(combo2, "CPU (MCS)");
    uiComboboxSetSelected(combo2, PLAYER_MCS);
    uiBoxAppend(ctrl_box, uiControl(combo2), 0);
    g_combo_boxes[DISK_WHITE] = combo2;

    uiButton *button = uiNewButton("Restart");
    uiButtonOnClicked(button, onInit, NULL);
    uiBoxAppend(ctrl_box, uiControl(button), 0);

    uiBoxAppend(vbox, uiControl(ctrl_box), 0);

    // Make them visible
    uiControlShow(uiControl(g_mainwin));
}

int main(void)
{
    // Initialize libui
    uiInitOptions options;
    const char *err;

    memset(&options, 0, sizeof (uiInitOptions));
    err = uiInit(&options);
    if (err != NULL) {
        fprintf(stderr, "error initializing libui: %s", err);
        uiFreeInitError(err);
        return 1;
    }

    // Create a new board
    g_board = revNewBoard();
    if (g_board == NULL) {
        printf("Failed to create a reversi board.\n");
        return 1;
    }
    revInitGenRandom((unsigned)time(NULL));

    // Create main window
    createWindow();

    // Initialize the game state
    onInit(NULL, NULL);

    // Start main loop
    uiMain();

    // Free a board
    revFreeBoard(g_board);

    return 0;
}
