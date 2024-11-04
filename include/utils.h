#include <emmintrin.h>
#include <types.h>

inline double find_minimum(double a, double b)
{
    double result;

    asm(
        "vminsd %[b], %[a], %[result]"
        : [result] "=x"(result)
        : [a] "x"(a), [b] "x"(b));

    return result;
}

inline double euclidean_distance(const Point &p1, const Point &p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

inline double euclidean_distance(double x1, double y1, double x2, double y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}