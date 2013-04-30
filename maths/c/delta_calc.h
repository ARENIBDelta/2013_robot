#ifndef DELTA_CALC_H
#define DELTA_CALC_H

#include <math.h>

#define DAFLOAT double


typedef struct {
	DAFLOAT x, y, z;
} v3;

DAFLOAT v3_dot(v3 a, v3 b);
DAFLOAT v3_dot_p(v3 *a, v3 *b);
DAFLOAT v3_norm(v3 a);
DAFLOAT v3_norm_p(v3 *a);
v3 v3_sum(v3 a, v3 b);
void v3_sum_p(v3 *a, v3 *b, v3 *out);
v3 v3_diff(v3 a, v3 b);
void v3_diff_p(v3 *a, v3 *b, v3 *out);
v3 v3_mult(v3 a, DAFLOAT m);
void v3_mult_p(v3 *a, DAFLOAT m, v3 *out);
void v3_cross_p(v3 *a, v3 *b, v3 *out);
void line_projection3d_p(v3 *a, v3 *b, v3 *p, v3 *out);

typedef struct {
	DAFLOAT a, b, c, d;
} delta_params;

int delta_calc(delta_params params, DAFLOAT X, DAFLOAT Y, DAFLOAT Z, int inverted, DAFLOAT *alpha, DAFLOAT *beta, DAFLOAT *gamma);

#endif