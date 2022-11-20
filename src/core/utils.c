#include "utils.h"

double lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

double clamp(double x, double a, double b)
{
    return x < a ? a : (x > b ? b : x);
}
