#ifndef __EYES_H__
#define __EYES_H__

#include <Arduino.h>
#include "corestats.h"
#include "config.h"

#define SMOOTH_FACTOR 0.8 // high values towards 1 = slow response, very smooth. 0 = input reading

typedef struct Eye {
    int iopin;
    float weight;
    float reading;
    float smooth;
    float vmin, vmax;
};

extern Eye leye, reye;;

void eyes_init();
void eyes_read();
float eyes_diff();
float eyes_absdiff();
void eyes_debug();


#endif // __EYES_H__
