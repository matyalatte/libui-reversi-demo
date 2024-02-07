#ifndef __REVERSI_DEMO_INCLUDE_FLIPANIMATOR_H__
#define __REVERSI_DEMO_INCLUDE_FLIPANIMATOR_H__

#include "reversi.h"

#define MAX_ANIM_COUNT 16
#define HALF_ANIM_COUNT 8

// Make 16-frame animation for flipping disks.
typedef struct FlipAnimator FlipAnimator;
struct FlipAnimator {
    int* flipped;  // positions of flipped disks
    int flipped_count;  // The number of flipped disks
    int anim_count;  // Count the frames from 16 to 0
};

void initFlipAnimator(FlipAnimator* animator)
{
    if (animator->flipped)
        free(animator->flipped);
    animator->flipped = NULL;
    animator->flipped_count = 0;
    animator->anim_count = 0;
}

void startFlipAnimation(FlipAnimator* animator, RevBitboard flipped)
{
    animator->flipped = revBitboardToArray(flipped);
    animator->flipped_count = revCountOnes(flipped);
    animator->anim_count = MAX_ANIM_COUNT;
}

double getYScale(FlipAnimator* animator)
{
    // Calculate the y scale for the current frame.
    return (double) abs(animator->anim_count - HALF_ANIM_COUNT) / HALF_ANIM_COUNT;
}

int isFirstHalf(FlipAnimator* animator)
{
    return animator->anim_count < HALF_ANIM_COUNT;
}

void stepAnimation(FlipAnimator* animator)
{
    animator->anim_count--;
}

int isFinished(FlipAnimator* animator)
{
    return animator->anim_count == 0;
}

#endif  // __REVERSI_DEMO_INCLUDE_FLIPANIMATOR_H__
