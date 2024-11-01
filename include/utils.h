#include <emmintrin.h>

inline double find_minimum(double a, double b)
{
    __m128d xmm0 = _mm_set_sd(a);
    __m128d xmm1 = _mm_set_sd(b);
    xmm0 = _mm_min_sd(xmm0, xmm1);
    return _mm_cvtsd_f64(xmm0);
}

inline double euclidean_distance(const Point &p1, const Point &p2)
{
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}