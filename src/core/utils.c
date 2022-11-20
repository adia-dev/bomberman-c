#include "utils.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

double clamp(double x, double a, double b)
{
    return x < a ? a : (x > b ? b : x);
}

SDL_Color lerp_color(SDL_Color a, SDL_Color b, double t)
{
    SDL_Color color;
    color.r = lerp(a.r, b.r, t);
    color.g = lerp(a.g, b.g, t);
    color.b = lerp(a.b, b.b, t);
    color.a = lerp(a.a, b.a, t);
    return color;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}
