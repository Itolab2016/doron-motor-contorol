/*
   Itolab drone motor control sample

   build
   make

   Usage
   sudo sixad -start &
   sudo ./dronemotor

   Copyright K.ITO,2016

*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/input/js0"

using namespace std;

#include <unistd.h>
#include "Navio/PWM.h"
#include "Navio/Util.h"
#include <stdio.h>

#define PWM_OUTPUT 0
#define SERVO_MIN 1.200 /*mS*/
#define SERVO_MAX 1.300 /*mS*/

int main()
{
	PWM pwm;

	int joy_fd(-1), num_of_axis(0), num_of_buttons(0);
	char name_of_joystick[80];
	vector<char> joy_button;
	vector<int> joy_axis;

	if((joy_fd = open(JOY_DEV, O_RDONLY)) < 0) {
		printf("Failed to open %s", JOY_DEV);
		cerr << "Failed to open " << JOY_DEV << endl;
		return -1;
	}

	ioctl(joy_fd, JSIOCGAXES, &num_of_axis);
	ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);
	ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);

	joy_button.resize(num_of_buttons, 0);
	joy_axis.resize(num_of_axis, 0);

	printf("Joystick: %s axis: %d buttons: %d\n", name_of_joystick, num_of_axis, num_of_buttons);

	fcntl(joy_fd, F_SETFL, O_NONBLOCK); // using non-blocking mode



	if (check_apm()) {
		return 1;
	}

	if (!pwm.init(PWM_OUTPUT)) {
		fprintf(stderr, "Output Enable not set. Are you root?\n");
		return 0;
	}

	pwm.enable(PWM_OUTPUT);
	pwm.set_period(PWM_OUTPUT, 50);


	while(true) {
		js_event js;

		read(joy_fd, &js, sizeof(js_event));

		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_AXIS:
				joy_axis[(int)js.number] = js.value;
				break;
			case JS_EVENT_BUTTON:
				joy_button[(int)js.number] = js.value;
				break;
		}
#if 0        
		printf( "LX: %6d  LY: %6d  ", joy_axis[0], joy_axis[1] );

		if( num_of_axis > 2 )
			printf("RX: %6d  ", joy_axis[2] );

		if( num_of_axis > 3 )
			printf("RY: %6d  ", joy_axis[3] );

		for(int x=0 ; x<num_of_buttons ; ++x )
			printf("B%d: %d  ", x, joy_button[x] );

		printf("\n");
		fflush(stdout);
#endif
		float duty=(-(float)joy_axis[3]/32768.0)*1.0+1.0;

		if (duty<1.0)duty=1.0;

		pwm.set_duty_cycle(PWM_OUTPUT, duty);
		usleep(100);
	}

	return 0;
}
