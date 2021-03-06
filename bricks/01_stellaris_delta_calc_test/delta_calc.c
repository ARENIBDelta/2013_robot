#include <math.h>

#include "delta_calc.h"

float vsqrtf_stellaris(float);

inline float v3_dot(v3 a, v3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline float v3_dot_p(v3 *a, v3 *b) {
	return a->x*b->x + a->y*b->y + a->z*b->z;
}

inline float v3_norm(v3 a) {
	return vsqrtf_stellaris(a.x*a.x + a.y*a.y + a.z*a.z);
}

inline float v3_norm_p(v3 *a) {
	return vsqrtf_stellaris(a->x*a->x + a->y*a->y + a->z*a->z);
}

inline float v3_norm_squared(v3 a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

inline float v3_norm_squared_p(v3 *a) {
	return a->x*a->x + a->y*a->y + a->z*a->z;
}

inline v3 v3_sum(v3 a, v3 b) {
	v3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

inline void v3_sum_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
}

inline v3 v3_diff(v3 a, v3 b) {
	v3 res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;
	return res;
}

inline void v3_diff_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
}

inline v3 v3_mult(v3 a, float m) {
	v3 res;
	res.x = a.x * m;
	res.y = a.y * m;
	res.z = a.z * m;
	return res;
}

inline void v3_mult_p(v3 *a, float m, v3 *out) {
	out->x = a->x * m;
	out->y = a->y * m;
	out->z = a->z * m;
}

inline void v3_cross_p(v3 *a, v3 *b, v3 *out) {
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

inline void line_projection3d_p(v3 *a, v3 *b, v3 *p, v3 *out) {
	v3 BmA, PmA, res;
	float coef, normed, doted;
	v3_diff_p(b, a, &BmA);
	v3_diff_p(p, a, &PmA);
	doted = v3_dot_p(&BmA, &PmA);
	normed = v3_norm_p(&BmA);
	normed = normed * normed;
	coef = doted / normed;
	v3_mult_p(&BmA, coef, &res);
	v3_sum_p(a, &res, out);
}

int delta_calc(delta_params params, float X, float Y, float Z, int inverted, float *alpha, float *beta, float *gamma) {
	//
	// alpha
	//
	v3 C1, D1, orthoC1, Dp1;
	C1.x = params.c * -0.750000000000000; //sqrt(3)/2. * cos(-150./180.*M_PI);
	C1.y = params.c * -0.433012701892219; //sqrt(3)/2. * sin(-150./180.*M_PI);
	C1.z = 0;

	D1.x = X + (params.d * -0.750000000000000);
	D1.y = Y + (params.d * -0.433012701892219);
	D1.z = Z;

	orthoC1.x = -0.500000000000000;
	orthoC1.y = 0.866025403784439;
	orthoC1.z = 0;

	v3 lp1, lp2, D1mDp1;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	float bp;
	v3_sum_p(&lp1, &C1, &lp2);
	line_projection3d_p(&lp1, &lp2, &D1, &Dp1);
	v3_diff_p(&D1, &Dp1, &D1mDp1);
	float D1Dp1norm = v3_norm_squared_p(&D1mDp1);
	bp = vsqrtf_stellaris(params.b*params.b - D1Dp1norm);

	v3 Dp1u, Dp1ut, C1mDp1;
	float C1mDp1_norm;
	v3_diff_p(&C1, &Dp1, &C1mDp1);
	C1mDp1_norm = v3_norm_p(&C1mDp1);
	v3_mult_p(&C1mDp1, 1/C1mDp1_norm, &Dp1u);
	v3_cross_p(&Dp1u, &orthoC1, &Dp1ut);
	if (inverted) {
		v3_mult_p(&Dp1ut, -1, &Dp1ut);
	}

	float cc_a, cc_h;
	v3 P1, buffer;
	cc_a = (bp*bp - params.a*params.a + C1mDp1_norm*C1mDp1_norm) / (2 * C1mDp1_norm);
	cc_h = vsqrtf_stellaris(bp*bp - cc_a * cc_a);
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
	C2.x = params.c * 0; //sqrt(3)/2. * cos(90./180.*M_PI);
	C2.y = params.c * 0.866025403784439; //sqrt(3)/2. * sin(90./180.*M_PI);
	C2.z = 0;

	D2.x = X + (params.d * 0);
	D2.y = Y + (params.d * 0.866025403784439);
	D2.z = Z;

	orthoC2.x = 1;
	orthoC2.y = 0;
	orthoC2.z = 0;

	v3 D2mDp2;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	v3_sum_p(&lp1, &C2, &lp2);
	line_projection3d_p(&lp1, &lp2, &D2, &Dp2);
	v3_diff_p(&D2, &Dp2, &D2mDp2);
	float D2Dp2norm = v3_norm_squared_p(&D2mDp2);
	bp = vsqrtf_stellaris(params.b*params.b - D2Dp2norm);

	v3 Dp2u, Dp2ut, C2mDp2;
	float C2mDp2_norm;
	v3_diff_p(&C2, &Dp2, &C2mDp2);
	C2mDp2_norm = v3_norm_p(&C2mDp2);
	v3_mult_p(&C2mDp2, 1/C2mDp2_norm, &Dp2u);
	v3_cross_p(&Dp2u, &orthoC2, &Dp2ut);
	if (inverted) {
		v3_mult_p(&Dp2ut, -1, &Dp2ut);
	}

	v3 P2;
	cc_a = (bp*bp - params.a*params.a + C2mDp2_norm*C2mDp2_norm) / (2 * C2mDp2_norm);
	cc_h = vsqrtf_stellaris(bp*bp - cc_a * cc_a);
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
	C3.x = params.c * 0.750000000000000; //sqrt(3)/2. * cos(-30./180.*M_PI);
	C3.y = params.c * -0.433012701892219; //sqrt(3)/2. * sin(-30./180.*M_PI);
	C3.z = 0;

	D3.x = X + (params.d * 0.750000000000000);
	D3.y = Y + (params.d * -0.433012701892219);
	D3.z = Z;

	orthoC3.x = -0.5;
	orthoC3.y = -0.866025403784439;
	orthoC3.z = 0;

	v3 D3mDp3;
	lp1.x = 0; lp1.y = 0; lp1.z = Z;
	v3_sum_p(&lp1, &C3, &lp2);
	line_projection3d_p(&lp1, &lp2, &D3, &Dp3);
	v3_diff_p(&D3, &Dp3, &D3mDp3);
	float D3Dp3norm = v3_norm_squared_p(&D3mDp3);
	bp = vsqrtf_stellaris(params.b*params.b - D3Dp3norm);

	v3 Dp3u, Dp3ut, C3mDp3;
	float C3mDp3_norm;
	v3_diff_p(&C3, &Dp3, &C3mDp3);
	C3mDp3_norm = v3_norm_p(&C3mDp3);
	v3_mult_p(&C3mDp3, 1/C3mDp3_norm, &Dp3u);
	v3_cross_p(&Dp3u, &orthoC3, &Dp3ut);
	if (inverted) {
		v3_mult_p(&Dp3ut, -1, &Dp3ut);
	}

	v3 P3;
	cc_a = (bp*bp - params.a*params.a + C3mDp3_norm*C3mDp3_norm) / (2 * C3mDp3_norm);
	cc_h = vsqrtf_stellaris(bp*bp - cc_a * cc_a);
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
