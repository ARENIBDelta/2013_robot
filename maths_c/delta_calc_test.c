#include <stdio.h>

#include "delta_calc.h"

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printf("Usage: %s X Y Z\n", argv[0]);
		return 1;
	}

	delta_params params;
	params.a = 70,
	params.b = 100;
	params.c = 130;
	params.d = 140;
	DAFLOAT alpha, beta, gamma;
	DAFLOAT X, Y, Z;
	sscanf(argv[1], "%lf", &X);
	sscanf(argv[2], "%lf", &Y);
	sscanf(argv[3], "%lf", &Z);
	delta_calc(params, X, Y, Z, 0, &alpha, &beta, &gamma);
	printf("%f %f %f\n", alpha, beta, gamma);
	return 0;
}
