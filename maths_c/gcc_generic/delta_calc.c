#include <math.h>

#include "delta_calc.h"

DAFLOAT v3_dot(v3 a, v3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

DAFLOAT v3_dot_p(v3 *a, v3 *b) {
	return a->x*b->x + a->y*b->y + a->z*b->z;
}

DAFLOAT v3_norm(v3 a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

DAFLOAT v3_norm_p(v3 *a) {
	return sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
}

v3 v3_sum(v3 a, v3 b) {
	v3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

void v3_sum_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
}

v3 v3_diff(v3 a, v3 b) {
	v3 res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;
	return res;
}

void v3_diff_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
}

v3 v3_mult(v3 a, DAFLOAT m) {
	v3 res;
	res.x = a.x * m;
	res.y = a.y * m;
	res.z = a.z * m;
	return res;
}

void v3_mult_p(v3 *a, DAFLOAT m, v3 *out) {
	out->x = a->x * m;
	out->y = a->y * m;
	out->z = a->z * m;
}

void v3_cross_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

void line_projection3d_p(v3 *a, v3 *b, v3 *p, v3 *out) {
	v3 BmA, PmA, res;
	DAFLOAT coef, normed, doted;
	v3_diff_p(b, a, &BmA);
	v3_diff_p(p, a, &PmA);
	doted = v3_dot_p(&BmA, &PmA);
	normed = v3_norm(BmA);
	normed = normed * normed;
	coef = doted / normed;
	v3_mult_p(&BmA, coef, &res);
	v3_sum_p(a, &res, out);
}

int delta_calc(delta_params params, DAFLOAT X, DAFLOAT Y, DAFLOAT Z, int inverted, DAFLOAT *alpha, DAFLOAT *beta, DAFLOAT *gamma) {
	//
	// alpha
	//
	v3 C1, D1, orthoC1, Dp1;
	C1.x = params.c * sqrt(3)/2. * cos(-150./180.*M_PI);
	C1.y = params.c * sqrt(3)/2. * sin(-150./180.*M_PI);
	C1.z = 0;

	D1.x = X + (params.d * sqrt(3)/2. * cos(-150./180.*M_PI));
	D1.y = Y + (params.d * sqrt(3)/2. * sin(-150./180.*M_PI));
	D1.z = Z;

	orthoC1.x = sin(-150./180.*M_PI);
	orthoC1.y = -cos(-150./180.*M_PI);
	orthoC1.z = 0;

	v3 lp1, lp2, D1mDp1;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	DAFLOAT bp;
	v3_sum_p(&lp1, &C1, &lp2);
	line_projection3d_p(&lp1, &lp2, &D1, &Dp1);
	v3_diff_p(&D1, &Dp1, &D1mDp1);
	DAFLOAT D1Dp1norm = v3_norm_p(&D1mDp1);
	bp = sqrt(params.b*params.b - D1Dp1norm*D1Dp1norm);

	v3 Dp1u, Dp1ut, C1mDp1;
	DAFLOAT C1mDp1_norm;
	v3_diff_p(&C1, &Dp1, &C1mDp1);
	C1mDp1_norm = v3_norm_p(&C1mDp1);
	v3_mult_p(&C1mDp1, 1/C1mDp1_norm, &Dp1u);
	v3_cross_p(&Dp1u, &orthoC1, &Dp1ut);
	if (inverted) {
		v3_mult_p(&Dp1ut, -1, &Dp1ut);
	}

	DAFLOAT cc_a, cc_h;
	v3 P1, res, buffer;
	cc_a = (bp*bp - params.a*params.a + C1mDp1_norm*C1mDp1_norm) / (2 * C1mDp1_norm);
	cc_h = sqrt(bp*bp - cc_a * cc_a);
	v3_mult_p(&Dp1u, cc_a, &buffer);
	v3_sum_p(&Dp1, &buffer, &P1);
	v3_mult_p(&Dp1ut, cc_h, &buffer);
	v3_diff_p(&P1, &buffer, &P1);

	if (inverted)
		*alpha = asin(P1.z / params.a);
	else
		*alpha = M_PI - asin(P1.z / params.a);

	//
	// beta
	//
	v3 C2, D2, orthoC2, Dp2;
	C2.x = params.c * sqrt(3)/2. * cos(90./180.*M_PI);
	C2.y = params.c * sqrt(3)/2. * sin(90./180.*M_PI);
	C2.z = 0;

	D2.x = X + (params.d * sqrt(3)/2. * cos(90./180.*M_PI));
	D2.y = Y + (params.d * sqrt(3)/2. * sin(90./180.*M_PI));
	D2.z = Z;

	orthoC2.x = sin(90./180.*M_PI);
	orthoC2.y = -cos(90./180.*M_PI);
	orthoC2.z = 0;

	v3 D2mDp2;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	v3_sum_p(&lp1, &C2, &lp2);
	line_projection3d_p(&lp1, &lp2, &D2, &Dp2);
	v3_diff_p(&D2, &Dp2, &D2mDp2);
	DAFLOAT D2Dp2norm = v3_norm_p(&D2mDp2);
	bp = sqrt(params.b*params.b - D2Dp2norm*D2Dp2norm);

	v3 Dp2u, Dp2ut, C2mDp2;
	DAFLOAT C2mDp2_norm;
	v3_diff_p(&C2, &Dp2, &C2mDp2);
	C2mDp2_norm = v3_norm_p(&C2mDp2);
	v3_mult_p(&C2mDp2, 1/C2mDp2_norm, &Dp2u);
	v3_cross_p(&Dp2u, &orthoC2, &Dp2ut);
	if (inverted) {
		v3_mult_p(&Dp2ut, -1, &Dp2ut);
	}

	v3 P2;
	cc_a = (bp*bp - params.a*params.a + C2mDp2_norm*C2mDp2_norm) / (2 * C2mDp2_norm);
	cc_h = sqrt(bp*bp - cc_a * cc_a);
	v3_mult_p(&Dp2u, cc_a, &buffer);
	v3_sum_p(&Dp2, &buffer, &P2);
	v3_mult_p(&Dp2ut, cc_h, &buffer);
	v3_diff_p(&P2, &buffer, &P2);

	if (inverted)
		*beta = asin(P2.z / params.a);
	else
		*beta = M_PI - asin(P2.z / params.a);

	//
	// gamma
	//
	v3 C3, D3, orthoC3, Dp3;
	C3.x = params.c * sqrt(3)/2. * cos(-30./180.*M_PI);
	C3.y = params.c * sqrt(3)/2. * sin(-30./180.*M_PI);
	C3.z = 0;

	D3.x = X + (params.d * sqrt(3)/2. * cos(-30./180.*M_PI));
	D3.y = Y + (params.d * sqrt(3)/2. * sin(-30./180.*M_PI));
	D3.z = Z;

	orthoC3.x = sin(-30./180.*M_PI);
	orthoC3.y = -cos(-30./180.*M_PI);
	orthoC3.z = 0;

	v3 D3mDp3;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	v3_sum_p(&lp1, &C3, &lp2);
	line_projection3d_p(&lp1, &lp2, &D3, &Dp3);
	v3_diff_p(&D3, &Dp3, &D3mDp3);
	DAFLOAT D3Dp3norm = v3_norm_p(&D3mDp3);
	bp = sqrt(params.b*params.b - D3Dp3norm*D3Dp3norm);

	v3 Dp3u, Dp3ut, C3mDp3;
	DAFLOAT C3mDp3_norm;
	v3_diff_p(&C3, &Dp3, &C3mDp3);
	C3mDp3_norm = v3_norm_p(&C3mDp3);
	v3_mult_p(&C3mDp3, 1/C3mDp3_norm, &Dp3u);
	v3_cross_p(&Dp3u, &orthoC3, &Dp3ut);
	if (inverted) {
		v3_mult_p(&Dp3ut, -1, &Dp3ut);
	}

	v3 P3;
	cc_a = (bp*bp - params.a*params.a + C3mDp3_norm*C3mDp3_norm) / (2 * C3mDp3_norm);
	cc_h = sqrt(bp*bp - cc_a * cc_a);
	v3_mult_p(&Dp3u, cc_a, &buffer);
	v3_sum_p(&Dp3, &buffer, &P3);
	v3_mult_p(&Dp3ut, cc_h, &buffer);
	v3_diff_p(&P3, &buffer, &P3);

	if (inverted)
		*gamma = asin(P3.z / params.a);
	else
		*gamma = M_PI - asin(P3.z / params.a);

	return 0;
}
