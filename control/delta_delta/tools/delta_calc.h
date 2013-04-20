#ifndef DELTA_CALC_H
#define DELTA_CALC_H

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
	float x, y, z;
} v3;

inline float v3_dot(v3 a, v3 b);
inline float v3_dot_p(v3 *a, v3 *b);
inline float v3_norm(v3 a);
inline float v3_norm_p(v3 *a);
inline v3 v3_sum(v3 a, v3 b);
inline void v3_sum_p(v3 *a, v3 *b, v3 *out);
inline v3 v3_diff(v3 a, v3 b);
inline void v3_diff_p(v3 *a, v3 *b, v3 *out);
inline v3 v3_mult(v3 a, float m);
inline void v3_mult_p(v3 *a, float m, v3 *out);
inline void v3_cross_p(v3 *a, v3 *b, v3 *out);
inline void line_projection3d_p(v3 *a, v3 *b, v3 *p, v3 *out);

typedef struct {
	float a, b, c, d;
} delta_params;

int delta_calc(delta_params params, float X, float Y, float Z, int inverted, float *alpha, float *beta, float *gamma);

#endif
