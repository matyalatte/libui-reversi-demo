#ifndef __REVERSI_DEMO_INCLUDE_UIUTIL_H__
#define __REVERSI_DEMO_INCLUDE_UIUTIL_H__
#include "ui.h"
#include "flipanimator.h"  // FlipAnimator related functions
#include "reversi.h"  // rev* functions, RevBoard, RevBitboard

// helper to quickly set a brush color
static void setSolidBrush(uiDrawBrush *brush, uint32_t color, double alpha)
{
    uint8_t component;

    brush->Type = uiDrawBrushTypeSolid;
    component = (uint8_t) ((color >> 16) & 0xFF);
    brush->R = ((double) component) / 255;
    component = (uint8_t) ((color >> 8) & 0xFF);
    brush->G = ((double) component) / 255;
    component = (uint8_t) (color & 0xFF);
    brush->B = ((double) component) / 255;
    brush->A = alpha;
}

static void drawLine(uiDrawContext *context,
                     double x1, double y1, double x2, double y2,
                     double thickness, uint32_t color)
{
    uiDrawPath *path;
    uiDrawBrush brush;
    setSolidBrush(&brush, color, 1.0);

    uiDrawStrokeParams params;
    // clear params to avoid passing garbage to uiDrawStroke()
    memset(&params, 0, sizeof (uiDrawStrokeParams));
    params.Cap = uiDrawLineCapFlat;
    params.Join = uiDrawLineJoinMiter;
    params.Thickness = thickness;
    params.MiterLimit = uiDrawDefaultMiterLimit;

    path = uiDrawNewPath(uiDrawFillModeWinding);
    uiDrawPathNewFigure(path, x1, y1);
    uiDrawPathLineTo(path, x2, y2);
    uiDrawPathEnd(path);

    uiDrawStroke(context, path, &brush, &params);
    uiDrawFreePath(path);
}

static void drawCircle(uiDrawContext *context,
                       double x, double y, double radius, uint32_t color)
{
    uiDrawPath *path;
    uiDrawBrush brush;
    setSolidBrush(&brush, color, 1.0);

    path = uiDrawNewPath(uiDrawFillModeWinding);
    uiDrawPathNewFigureWithArc(path, x, y, radius, 0, uiPi * 2, 0);
    uiDrawPathEnd(path);

    uiDrawFill(context, path, &brush);
    uiDrawFreePath(path);
}

static void drawOval(uiDrawContext *context,
                       double x, double y, double a, double b, uint32_t color)
{
    double sx = 1.0;
    double sy = b / a;
    if (sy < 0.00001)
        return;

    uiDrawSave(context);
    uiDrawMatrix m;
    uiDrawMatrixSetIdentity(&m);
    uiDrawMatrixScale(&m, x, y, sx, sy);
    uiDrawTransform(context, &m);
    drawCircle(context, x, y, a, color);
    uiDrawRestore(context);
}

#define COLOR_GREEN 0x44CC44
#define COLOR_BLACK 0x444444
#define COLOR_WHITE 0xCCCCCC

const double GRID_SIZE = 45.0;
const double GRID_OFS = 8.0;

static void drawBoard(uiAreaDrawParams *p)
{
    // fill the area
    uiDrawPath *path;
    uiDrawBrush brush;
    setSolidBrush(&brush, COLOR_GREEN, 1.0);
    path = uiDrawNewPath(uiDrawFillModeWinding);
    uiDrawPathAddRectangle(path, 0, 0, p->AreaWidth, p->AreaHeight);
    uiDrawPathEnd(path);
    uiDrawFill(p->Context, path, &brush);
    uiDrawFreePath(path);

    // Draw lines
    for (int i = 0; i < 9; i++) {
        double y = GRID_OFS + i * GRID_SIZE;
        drawLine(p->Context, GRID_OFS, y, GRID_OFS + GRID_SIZE * 8, y, 2, COLOR_BLACK);
        drawLine(p->Context, y, GRID_OFS, y, GRID_OFS + GRID_SIZE * 8, 2, COLOR_BLACK);
    }
}

const uint32_t DISK_COLORS[2] = { COLOR_BLACK, COLOR_WHITE };

static void drawDisks(uiDrawContext *context, RevBoard *board, RevDiskType disk_type)
{
    // Draw static disks
    uint32_t color = DISK_COLORS[disk_type];
    double cs = GRID_SIZE * 0.45;
    int *disks = revGetBitboardAsArray(board, disk_type);
    for (int *dptr = disks; dptr < disks + revCountDisks(board, disk_type); dptr++) {
        int pos = dptr[0];
        int x = pos % 8;
        int y = pos / 8;
        double cx = GRID_OFS + (x + 0.5) * GRID_SIZE;
        double cy = GRID_OFS + (y + 0.5) * GRID_SIZE;
        drawCircle(context, cx, cy, cs, color);  // draw a disk
    }
    free(disks);
}

static void drawFlippedDisks(uiDrawContext *context, RevBoard *board, FlipAnimator *animator)
{
    // Draw the flipping disks
    RevDiskType disk_type = revGetCurrentPlayer(board);
    if (isFirstHalf(animator)) {
        // Use the previous color in the first half of the animation.
        disk_type = !disk_type;
    }
    uint32_t color = DISK_COLORS[disk_type];
    double flip_scale = getYScale(animator);
    double ca = GRID_SIZE * 0.45;
    double cb = ca * flip_scale;
    int *dptr = animator->flipped;
    for (; dptr < animator->flipped + animator->flipped_count; dptr++) {
        int pos = dptr[0];
        int x = pos % 8;
        int y = pos / 8;
        double cx = GRID_OFS + (x + 0.5) * GRID_SIZE;
        double cy = GRID_OFS + (y + 0.5) * GRID_SIZE;
        drawCircle(context, cx, cy, ca + 1, COLOR_GREEN);
        drawOval(context, cx, cy, ca, cb, color);
    }
}

#endif  // __REVERSI_DEMO_INCLUDE_UIUTIL_H__
