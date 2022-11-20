#ifndef _UTILS_H_
#define _UTILS_H_

#include "structs.h"

double lerp(double a, double b, double t);
double clamp(double x, double a, double b);

// lerp colors
SDL_Color lerp_color(SDL_Color a, SDL_Color b, double t);

int max(int a, int b);
int min(int a, int b);

#endif