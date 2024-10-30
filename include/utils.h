#include <emmintrin.h>

inline double FindMinimum(double a, double b)
{
    __m128d xmm0 = _mm_set_sd(a);
    __m128d xmm1 = _mm_set_sd(b);

    xmm0 = _mm_min_sd(_mm_set_sd(a), _mm_set_sd(b));

    return _mm_cvtsd_f64(xmm0);
}

inline double euclideanDistance(const Point &p1, const Point &p2)
{
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}