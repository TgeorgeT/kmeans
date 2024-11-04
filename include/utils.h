#include <emmintrin.h>

inline double find_minimum(double a, double b)
{
    double result;

    asm(
        "vminsd %[b], %[a], %[result]"
        : [result] "=x"(result)
        : [a] "x"(a), [b] "x"(b));

    return result;
}

inline double euclidean_distance(double x1, double y1, double x2, double y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}