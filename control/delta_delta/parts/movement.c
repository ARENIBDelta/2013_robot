#include <math.h>
#define M_PI 3.14159265358979323846

#include "movement.h"
#include "../tools/delta_calc.h"

//static float points_x[175] = {0,0.080866,0.32271,0.72331,1.279,1.9845,2.8335,3.8179,4.9289,6.156,7.488,8.9124,10.416,11.986,13.606,15.262,16.939,18.621,20.293,21.939,23.544,25.093,26.572,27.967,29.265,30.455,31.525,32.465,33.267,33.923,34.428,34.776,34.964,34.991,34.856,34.561,34.108,33.502,32.748,31.853,30.825,29.674,28.411,27.047,25.594,24.067,22.479,20.845,19.18,17.5,17.5,17.484,17.437,17.358,17.248,17.107,16.935,16.733,16.5,16.238,15.946,15.626,15.278,14.902,14.499,14.07,13.616,13.137,12.634,12.109,11.562,10.994,10.406,9.7996,9.1754,8.5347,7.8786,7.2083,6.525,5.8299,5.1244,4.4096,3.6869,2.9575,2.2228,1.4841,0.74272,1.0716e-15,-0.74272,-1.4841,-2.2228,-2.9575,-3.6869,-4.4096,-5.1244,-5.8299,-6.525,-7.2083,-7.8786,-8.5347,-9.1754,-9.7996,-10.406,-10.994,-11.562,-12.109,-12.634,-13.137,-13.616,-14.07,-14.499,-14.902,-15.278,-15.626,-15.946,-16.238,-16.5,-16.733,-16.935,-17.107,-17.248,-17.358,-17.437,-17.484,-17.5,-17.5,-19.18,-20.845,-22.479,-24.067,-25.594,-27.047,-28.411,-29.674,-30.825,-31.853,-32.748,-33.502,-34.108,-34.561,-34.856,-34.991,-34.964,-34.776,-34.428,-33.923,-33.267,-32.465,-31.525,-30.455,-29.265,-27.967,-26.572,-25.093,-23.544,-21.939,-20.293,-18.621,-16.939,-15.262,-13.606,-11.986,-10.416,-8.9124,-7.488,-6.156,-4.9289,-3.8179,-2.8335,-1.9845,-1.279,-0.72331,-0.32271,-0.080866,0};
//static float points_y[175] = {-35,-36.68,-38.345,-39.979,-41.567,-43.094,-44.547,-45.911,-47.174,-48.325,-49.353,-50.248,-51.002,-51.608,-52.061,-52.356,-52.491,-52.464,-52.276,-51.928,-51.423,-50.767,-49.965,-49.025,-47.955,-46.765,-45.467,-44.072,-42.593,-41.044,-39.439,-37.793,-36.121,-34.439,-32.762,-31.106,-29.486,-27.916,-26.412,-24.988,-23.656,-22.429,-21.318,-20.333,-19.485,-18.779,-18.223,-17.823,-17.581,-17.5,-17.5,-14.529,-11.564,-8.6088,-5.6699,-2.7525,0.13846,2.9976,5.8198,8.6,11.333,14.014,16.639,19.202,21.698,24.124,26.475,28.747,30.936,33.037,35.047,36.963,38.78,40.496,42.107,43.611,45.005,46.286,47.452,48.501,49.432,50.241,50.929,51.493,51.933,52.248,52.437,52.5,52.437,52.248,51.933,51.493,50.929,50.241,49.432,48.501,47.452,46.286,45.005,43.611,42.107,40.496,38.78,36.963,35.047,33.037,30.936,28.747,26.475,24.124,21.698,19.202,16.639,14.014,11.333,8.6,5.8198,2.9976,0.13846,-2.7525,-5.6699,-8.6088,-11.564,-14.529,-17.5,-17.5,-17.581,-17.823,-18.223,-18.779,-19.485,-20.333,-21.318,-22.429,-23.656,-24.988,-26.412,-27.916,-29.486,-31.106,-32.762,-34.439,-36.121,-37.793,-39.439,-41.044,-42.593,-44.072,-45.467,-46.765,-47.955,-49.025,-49.965,-50.767,-51.423,-51.928,-52.276,-52.464,-52.491,-52.356,-52.061,-51.608,-51.002,-50.248,-49.353,-48.325,-47.174,-45.911,-44.547,-43.094,-41.567,-39.979,-38.345,-36.68,-35};

unsigned char  movement_state = 0;
unsigned char  should_pause   = 0;
        float  pos_x          = 0;
        float  pos_y          = 0;

void calc_control_input_1(
		float X, float Y, float Z,
		unsigned short *alpha_a, unsigned short *beta_a, unsigned short *gamma_a
) {
	delta_params params;
	params.a = 70;
	params.b = 225;
	params.c = 87;
	params.d = 54;
	float alpha, beta, gamma;

	//Ne pas oublier de vérifier la taille de la stack avant d'utiliser ça
	delta_calc(params, X, Y, Z, 0, &alpha, &beta, &gamma);
	*alpha_a = 10092 - ((alpha / M_PI * 180) - 180)/0.1249;
	*beta_a = 10092 - ((beta / M_PI * 180) - 180)/0.1249;
	*gamma_a = 10092 - ((gamma / M_PI * 180) - 180)/0.1249;

	*alpha_a = *alpha_a > 10700 ? 10700 : *alpha_a;
	*alpha_a = *alpha_a < 9800 ? 9800 : *alpha_a;
	*beta_a = *beta_a > 10700 ? 10700 : *beta_a;
	*beta_a = *beta_a < 9800 ? 9800 : *beta_a;
	*gamma_a = *gamma_a > 10700 ? 10700 : *gamma_a;
	*gamma_a = *gamma_a < 9800 ? 9800 : *gamma_a;
}

void calc_control_input_2(
		float X, float Y, float Z,
		unsigned short *alpha_a, unsigned short *beta_a, unsigned short *gamma_a
) {
	delta_params params;
	params.a = 70;
	params.b = 225;
	params.c = 129.8;
	params.d = 143.777;
	float alpha, beta, gamma;

	//Ne pas oublier de vérifier la taille de la stack avant d'utiliser ça
	delta_calc(params, X, -Y, Z, 1, &alpha, &beta, &gamma);
	*alpha_a = 10135 + ((alpha / M_PI * 180) )/0.1249;
	*beta_a = 10135 + ((beta / M_PI * 180) )/0.1249;
	*gamma_a = 10135 + ((gamma / M_PI * 180) )/0.1249;

	*alpha_a = *alpha_a > 10700 ? 10700 : *alpha_a;
	*alpha_a = *alpha_a < 9800 ? 9800 : *alpha_a;
	*beta_a = *beta_a > 10700 ? 10700 : *beta_a;
	*beta_a = *beta_a < 9800 ? 9800 : *beta_a;
	*gamma_a = *gamma_a > 10700 ? 10700 : *gamma_a;
	*gamma_a = *gamma_a < 9800 ? 9800 : *gamma_a;
}

void control_do_lines(
		float X1_from, float X1_to,
		float Y1_from, float Y1_to,
		float Z1_from, float Z1_to,
		float X2_from, float X2_to,
		float Y2_from, float Y2_to,
		float Z2_from, float Z2_to,
		short steps
) {
	unsigned short t;
	unsigned short alpha_a_1, beta_a_1, gamma_a_1;
	unsigned short alpha_a_2, beta_a_2, gamma_a_2;

	float incr_X1 = (X1_to - X1_from) / STEPS;
	float incr_Y1 = (Y1_to - Y1_from) / STEPS;
	float incr_Z1 = (Z1_to - Z1_from) / STEPS;
	float incr_X2 = (X2_to - X2_from) / STEPS;
	float incr_Y2 = (Y2_to - Y2_from) / STEPS;
	float incr_Z2 = (Z2_to - Z2_from) / STEPS;

	for (t=1; t<=steps; t++) {
		//Calcul des points suivants
		calc_control_input_1(
				X1_from+t*incr_X1, Y1_from+t*incr_Y1, Z1_from+t*incr_Z1,
				&alpha_a_1, &beta_a_1, &gamma_a_1
		);
		calc_control_input_2(
				X2_from+t*incr_X2, Y2_from+t*incr_Y2, Z2_from+t*incr_Z2,
				&alpha_a_2, &beta_a_2, &gamma_a_2
		);

		//Attente interruption asserv
		while(!control_event) {SysCtlDelay(1000);} control_event = 0;

		//Commande
		control_set_goal_1(alpha_a_1, beta_a_1, gamma_a_1);
		control_set_goal_2(alpha_a_2, beta_a_2, gamma_a_2);
		control();
	}
}

void movement_stay_put(float X, float Y, float Z) {
	unsigned short alpha_a_1, beta_a_1, gamma_a_1;
	calc_control_input_1(
			X, Y, Z,
			&alpha_a_1, &beta_a_1, &gamma_a_1
	);
	unsigned short alpha_a_2, beta_a_2, gamma_a_2;
	calc_control_input_1(
			X, Y, Z,
			&alpha_a_2, &beta_a_2, &gamma_a_2
	);

	//Attente interruption asserv
	while(!control_event) {SysCtlDelay(1000);} control_event = 0;

	//Commande
	control_set_goal_1(alpha_a_1, beta_a_1, gamma_a_1);
	control_set_goal_2(alpha_a_2, beta_a_2, gamma_a_2);
	control();
}

void control_start_steps(float angle, unsigned char length, unsigned char step_down) {
	angle = angle * M_PI  / 180;
	float dir_x = cos(angle);
	float dir_y = sin(angle);

	float lx = dir_x*length;
	float ly = dir_y*length;

	if (step_down) {
		control_do_lines(
			0, 0,
			0, 0,
			BASE_Z, BASE_Z-DZ,
			0, 0,
			0, 0,
			BASE_Z, BASE_Z,
			STEPS
		);
	}

	control_do_lines(
		0, -0.625*lx,
		0, -0.625*ly,
		BASE_Z-DZ, BASE_Z-DZ,
		0, 0.625*lx,
		0, 0.625*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		-0.625*lx, -0.625*lx,
		-0.625*ly, -0.625*ly,
		BASE_Z-DZ, BASE_Z,
		0.625*lx, 0.625*lx,
		0.625*ly, 0.625*ly,
		BASE_Z, BASE_Z,
		STEPS
	);
}

unsigned char control_do_step(float angle_src, float angle_dst, char length) {
	angle_src = angle_src * M_PI / 180;
	angle_dst = angle_dst * M_PI / 180;
	float dir_x_src = cos(angle_src);
	float dir_y_src = sin(angle_src);
	float dir_x_dst = cos(angle_dst);
	float dir_y_dst = sin(angle_dst);

	float lx = dir_x_src*length;
	float ly = dir_y_src*length;

	float lx2 = dir_x_dst*length;
	float ly2 = dir_y_dst*length;

	control_do_lines(
		-0.625*lx, -lx,
		-0.625*ly, -ly,
		BASE_Z, BASE_Z-DZ,
		0.625*lx, 0.25*lx,
		0.625*ly, 0.25*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		-lx, lx,
		-ly, ly,
		BASE_Z-DZ, BASE_Z-DZ,
		0.25*lx, -0.25*lx,
		0.25*ly, -0.25*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		lx, 0.625*lx,
		ly, 0.625*ly,
		BASE_Z-DZ, BASE_Z,
		-0.25*lx, -0.625*lx,
		-0.25*ly, -0.625*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		0.625*lx, 0.25*lx,
		0.625*ly, 0.25*ly,
		BASE_Z, BASE_Z,
		-0.625*lx, -lx,
		-0.625*ly, -ly,
		BASE_Z, BASE_Z-DZ,
		STEPS
	);

	if (angle_src != angle_dst) {
		control_do_lines(
			0.25*lx, 0,
			0.25*ly, 0,
			BASE_Z, BASE_Z,
			-lx, 0,
			-ly, 0,
			BASE_Z-DZ, BASE_Z-DZ,
			STEPS
		);

		control_do_lines(
			0, -0.25*lx2,
			0, -0.25*ly2,
			BASE_Z, BASE_Z,
			0, lx2,
			0, ly2,
			BASE_Z-DZ, BASE_Z-DZ,
			STEPS
		);
	}
	else {
		control_do_lines(
			0.25*lx, -0.25*lx2,
			0.25*ly, -0.25*ly2,
			BASE_Z, BASE_Z,
			-lx, lx2,
			-ly, ly2,
			BASE_Z-DZ, BASE_Z-DZ,
			STEPS
		);
	}

	control_do_lines(
		-0.25*lx2, -0.625*lx2,
		-0.25*ly2, -0.625*ly2,
		BASE_Z, BASE_Z,
		lx2, 0.625*lx2,
		ly2, 0.625*ly2,
		BASE_Z-DZ, BASE_Z,
		STEPS
	);

	pos_x += 2*lx;
	pos_y += 2*ly;
	pos_x += 2*lx2;
	pos_y += 2*lx2;


	return 1;
}

void control_stop_steps(float angle, unsigned char length, unsigned char step_down) {
	angle = angle * M_PI  / 180;
	float dir_x = cos(angle);
	float dir_y = sin(angle);

	float lx = dir_x*length;
	float ly = dir_y*length;

	control_do_lines(
		-0.625*lx, -0.25*lx,
		-0.625*ly, -0.25*ly,
		BASE_Z, BASE_Z,
		0.625*lx, 0.25*lx,
		0.625*ly, 0.25*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		-0.25*lx, -0.25*lx,
		-0.25*ly, -0.25*ly,
		BASE_Z, BASE_Z-DZ,
		0.25*lx, 0.25*lx,
		0.25*ly, 0.25*ly,
		BASE_Z, BASE_Z,
		STEPS
	);

	control_do_lines(
		-0.25*lx, 0,
		-0.25*ly, 0,
		BASE_Z-DZ, BASE_Z-DZ,
		0.25*lx, 0,
		0.25*ly, 0,
		BASE_Z, BASE_Z,
		STEPS
	);

	if (step_down) {
		control_do_lines(
			0, 0,
			0, 0,
			BASE_Z-DZ, BASE_Z,
			0, 0,
			0, 0,
			BASE_Z, BASE_Z,
			STEPS
		);
	}
}

void movement_pause(void) {
	should_pause = 1;
}

unsigned char movement_is_paused(void) {
	return should_pause;
}

void movement_unpause(void) {
	should_pause = 0;
}

