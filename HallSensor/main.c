 /*************************************************************************************************************************************************
  *  main.cpp
  *  editor: amo
  *************************************************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softServo.h>
#include <softPwm.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>

/* -------------------------------------------------------------------- */
/* my define macro                                                     */
/* -------------------------------------------------------------------- */
#define NONECOLOR 	"\033[m"
#define RED 		"\033[0;32;31m"
#define LIGHT_RED 	"\033[1;31m"
#define GREEN 		"\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE 		"\033[0;32;34m"
#define LIGHT_BLUE 	"\033[1;34m"
#define DARY_GRAY 	"\033[1;30m"
#define CYAN 		"\033[0;36m"
#define LIGHT_CYAN 	"\033[1;36m"
#define PURPLE 		"\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN 		"\033[0;33m"
#define YELLOW 		"\033[1;33m"
#define LIGHT_GRAY 	"\033[0;37m"
#define WHITE 		"\033[1;37m"


#if 1 //BCM
#define LED 5
#define BTN 6
#define SENSOR_0 17
#define SENSOR_1 26
#define SENSOR_2 27
#define SENSOR_3 22
#define SENSOR_4 23
#define SENSOR_5 24
#define SENSOR_6 25
#define SENSOR_7 20
#define SERVO_0 18
#else //wPi
#define LED 21
#define BTN 22
#define SENSOR_0 0
#define SENSOR_1 25
#define SENSOR_2 2
#define SENSOR_3 3
#define SENSOR_4 4
#define SENSOR_5 5
#define SENSOR_6 6
#define SENSOR_7 28
#define SERVO_0 1
#endif

#define SERV_0_DUTY_90 20
#define PWM_CHANNEL_0_CLOCK 1920
#define PWM_CHANNEL_0_RANGE 200
#define PWM_BASE_FREQ 19200000

#define RANGE 100

#define	DEBOUNCE_TIME 250
#define	DELAY_TIME 1000
#define	DELAY_LOG 2000
#define	DELAY_MAGIC 600

#define	TEST_PASS 1
#define	TEST_FAIL 2
#define	TEST_RETRY 3
/* -------------------------------------------------------------------- */
/* global variables                                                     */
/* -------------------------------------------------------------------- */
char sensor_gpio[8][8] = {" GPIO17", " GPIO26", " GPIO27", " GPIO22", " GPIO23", " GPIO24", " GPIO25", " GPIO20"};

int counter_sensor_0 = 0;
int counter_sensor_1 = 0;
int counter_sensor_2 = 0;
int counter_sensor_3 = 0;
int counter_sensor_4 = 0;
int counter_sensor_5 = 0;
int counter_sensor_6 = 0;
int counter_sensor_7 = 0;

int interval_btn = 0;
int interval_sensor_0 = 0;
int interval_sensor_1 = 0;
int interval_sensor_2 = 0;
int interval_sensor_3 = 0;
int interval_sensor_4 = 0;
int interval_sensor_5 = 0;
int interval_sensor_6 = 0;
int interval_sensor_7 = 0;

pthread_cond_t cond_show = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_cond_show;
pthread_cond_t cond_fail_check = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_cond_fail_check;
pthread_mutex_t mutex_sensor_0; //0
pthread_mutex_t mutex_sensor_1; //1
pthread_mutex_t mutex_sensor_2; //2
pthread_mutex_t mutex_sensor_3; //3
pthread_mutex_t mutex_sensor_4; //4
pthread_mutex_t mutex_sensor_5; //5
pthread_mutex_t mutex_sensor_6; //6
pthread_mutex_t mutex_sensor_7;  //7

int test();
void resetCounter();
/* -------------------------------------------------------------------- */
/* implements                                                           */
/* -------------------------------------------------------------------- */
void LOG(const char* format, ...)
{
	char str[255];
	va_list ap;
	va_start(ap,format);
	vsprintf(str,format,ap);
	sprintf(str,"%s%s",str,NONECOLOR);
	printf("%s",str);
	va_end(ap); 
}

#if 1
void servo_init_(int servo, int pwmc, int pwmr) {
	switch (servo) {
		case 0:
			LOG("%s initialize servo_0 at pin:%d, pwmc:%d, pwmr:%d\n", LIGHT_GRAY, SERVO_0, pwmc, pwmr);
			pinMode(SERVO_0, PWM_OUTPUT) ;
			pwmSetMode(PWM_MODE_MS);
			pwmSetClock(pwmc);
			pwmSetRange(pwmr);
			break;
		default:
			break;
	}
}
void servo_init() {
	servo_init_(0, PWM_CHANNEL_0_CLOCK, PWM_CHANNEL_0_RANGE);
}
#elif 0
void servo_init() {
  softServoSetup (SERVO_0, SERVO_1, -1, -1, -1, -1, -1, -1) ;
}

#else 
void servo_init() {
  int servo[1] = {SERVO_0};
	for (int i=0;i<1;i++) {
		if (softPwmCreate(servo[i], 0, RANGE) >= 0) {
			LOG("%s soft pwm create servo:%d, range:%d\n", LIGHT_GRAY, servo[i], RANGE);
		}
		else {
			LOG("%s failed to soft pwm create servo:%d, range:%d\n", RED, servo[i], RANGE);
		}		
  }
}

#endif

#if 1
void servo(int servo, int angle) {
/*	
	float period_per_unit = 0.1; //0.1ms;
	float duty = 0; //ms
	int value = 0; //count of units
	switch (servo) {
		case 0:
			period_per_unit = (1.0f/PWM_BASE_FREQ)*PWM_CHANNEL_0_CLOCK*1000.0f; //ms
			duty = (period_per_unit*SERV_0_DUTY_90)-((90.0f-angle)/180.0f)*(SERV_0_DUTY_90/2.0f)*period_per_unit; //1.5ms for 0, 2ms for 90, 1ms for -90
			value = duty/period_per_unit;
			LOG("%s pwm write servo_0 value:%d, duty:%f, unit:%f\n", LIGHT_GRAY, value, duty, period_per_unit);
			pwmWrite(SERVO_0, value);
			break;
		default:
			break;
	}
*/
	int value = 17;
	switch (servo) {
		case 0:
			servo = SERVO_0;
			break;
		default:
			break;
	}
	switch (angle) {
		case 90:
			value = 25;
			break;
		case -90:
			value = 5;
			break;
		case 0:
			value = 15;
			break;	
		default:
			break;
	}
	LOG("%s soft pwm write servo:%d, value:%d\n", LIGHT_GRAY, servo, value);
	pwmWrite(SERVO_0, value);
}
#elif 0
void servo(int servo, int angle) {
	int min = -250;
	int max = 1250;
	int value = min+(max-min)*((angle-(-90))/(90-(-90)));
	switch (servo) {
		case 0:
			servo = SERVO_0;
			break;
		case 1:
			servo = SERVO_1;
			break;
		default:
			break;
	}
	LOG("%s soft servo write servo:%d, value:%d\n", LIGHT_GRAY, servo, value);
	softServoWrite (servo, value) ;
}

#else 
void servo(int servo, int angle) {
	int value = 0+RANGE*((angle-(-90))/(90-(-90)));
	switch (servo) {
		case 0:
			servo = SERVO_0;
			break;
		default:
			break;
	}
	LOG("%s soft pwm write servo:%d, value:%d\n", LIGHT_GRAY, servo, value);
	softPwmWrite (servo, value) ;
}

#endif

void handler_BTN(void) {
	int time = millis();
	if (time < interval_btn) {
		return;
	}
	interval_btn = millis() + DEBOUNCE_TIME;
	LOG("%s BTN\n", LIGHT_GRAY);
	pthread_cond_signal(&cond_fail_check);
}

void handler_sensor_0(void) {
	pthread_mutex_lock(&mutex_sensor_0);
	int time = 0;
	time = millis();
	if (time < interval_sensor_0) {
		//LOG("%s ********* FAST SENSOR 0 *********\n", RED);
		goto END;
	}
	interval_sensor_0 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 0 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_0) == HIGH) {
		//LOG("%s ********* LOST SENSOR 0 *********\n", RED);
		goto END;
	}
	(counter_sensor_0)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_0);
	return;
}

void handler_sensor_1(void) {
	pthread_mutex_lock(&mutex_sensor_1);
	int time = 0;
	time = millis();
	if (time < interval_sensor_1) {
		goto END;
	}
	interval_sensor_1 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 1 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_1) == HIGH) {
		goto END;
	}
	(counter_sensor_1)++;
	pthread_cond_signal(&cond_show);

END:		
	pthread_mutex_unlock(&mutex_sensor_1);
	return;
}

void handler_sensor_2(void) {
	pthread_mutex_lock(&mutex_sensor_2);
	int time = 0;
	time = millis();
	if (time < interval_sensor_2) {
		goto END;
	}
	interval_sensor_2 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 2 \n", LIGHT_GRAY);	
	if (digitalRead(SENSOR_2) == HIGH) {
		goto END;
	}
	(counter_sensor_2)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_2);
	return;
}

void handler_sensor_3(void) {
	pthread_mutex_lock(&mutex_sensor_3);
	int time = 0;
	time = millis();
	if (time < interval_sensor_3) {
		goto END;
	}
	interval_sensor_3 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 3 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_3) == HIGH) {
		goto END;
	}
	(counter_sensor_3)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_3);
	return;
}

void handler_sensor_4(void) {
	pthread_mutex_lock(&mutex_sensor_4);
	int time = 0;
	time = millis();
	if (time < interval_sensor_4) {
		goto END;
	}
	interval_sensor_4 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 4 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_4) == HIGH) {
		goto END;
	}
	(counter_sensor_4)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_4);
	return;
}

void handler_sensor_5(void) {
	pthread_mutex_lock(&mutex_sensor_5);
	int time = 0;
	time = millis();
	if (time < interval_sensor_5) {
		goto END;
	}
	interval_sensor_5 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 5 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_5) == HIGH) {
		goto END;
	}
	(counter_sensor_5)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_5);
	return;
}

void handler_sensor_6(void) {
	pthread_mutex_lock(&mutex_sensor_6);
	int time = 0;
	time = millis();
	if (time < interval_sensor_6) {
		goto END;
	}
	interval_sensor_6 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 6 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_6) == HIGH) {
		goto END;
	}
	(counter_sensor_6)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_6);
	return;
}

void handler_sensor_7(void) {
	pthread_mutex_lock(&mutex_sensor_7);
	int time = 0;
	time = millis();
	if (time < interval_sensor_7) {
		goto END;
	}
	interval_sensor_7 = millis() + DEBOUNCE_TIME;
	LOG("%s SENSOR 7 \n", LIGHT_GRAY);
	if (digitalRead(SENSOR_7) == HIGH) {
		goto END;
	}
	(counter_sensor_7)++;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_sensor_7);
	return;
}

void* taskSensor(void* arg) {
#if 0	
	system ("gpio edge 17 falling");
	system ("gpio edge 26 falling");
	system ("gpio edge 27 falling");
	system ("gpio edge 22 falling");
	system ("gpio edge 23 falling");
	system ("gpio edge 24 falling");
	system ("gpio edge 25 falling");
	system ("gpio edge 4 falling");
	system ("gpio edge 6 rising");
#endif

	wiringPiISR(BTN, INT_EDGE_RISING, &handler_BTN);
	wiringPiISR(SENSOR_0, INT_EDGE_FALLING, &handler_sensor_0);
	wiringPiISR(SENSOR_1, INT_EDGE_FALLING, &handler_sensor_1);
	wiringPiISR(SENSOR_2, INT_EDGE_FALLING, &handler_sensor_2);
	wiringPiISR(SENSOR_3, INT_EDGE_FALLING, &handler_sensor_3);
	wiringPiISR(SENSOR_4, INT_EDGE_FALLING, &handler_sensor_4);
	wiringPiISR(SENSOR_5, INT_EDGE_FALLING, &handler_sensor_5);
	wiringPiISR(SENSOR_6, INT_EDGE_FALLING, &handler_sensor_6);
	wiringPiISR(SENSOR_7, INT_EDGE_FALLING, &handler_sensor_7);
	return 0;
}

void* taskBlink(void* arg) {
	while (1) {
		digitalWrite (LED, HIGH) ;	// On
		delay (500) ;		// mS
		digitalWrite (LED, LOW) ;	// Off
		delay (500) ;
	}
	return 0;
}

void* taskShow(void* arg) {
	while (1) {
		pthread_mutex_lock(&mutex_cond_show);
		pthread_cond_wait(&cond_show, &mutex_cond_show);
		LOG("%s *DETECTED\n", LIGHT_GRAY);
		delay(DELAY_MAGIC);
		pthread_mutex_unlock(&mutex_cond_show);
		int ret = test();
		if (ret == TEST_PASS) {
			LOG("%s [PASS]\n", LIGHT_GREEN);
			servo(0, 0);
			delay(DELAY_MAGIC);
			servo(0, 90);
			delay(DELAY_MAGIC);
			servo(0, 0);
			delay(DELAY_MAGIC/2);
			servo(1, 0);
		}
#if 0 //no retry case		
		else if (ret == TEST_RETRY){
			LOG("%s [RETRY]\n", LIGHT_GRAY);
		}
#endif		
		else {
			LOG("%s [Please CHECK]\n", YELLOW);
		}
	}
	return 0;
}

void* taskCheck(void* arg) {
	//struct timeval tv;
    //struct timespec ts;
	//int timeInMs = 3000;
	while (1) {
		pthread_mutex_lock(&mutex_cond_fail_check);
		//gettimeofday(&tv, NULL);
		//ts.tv_sec = time(NULL) + timeInMs / 1000;
		//ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
		//ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
		//ts.tv_nsec %= (1000 * 1000 * 1000);
		//pthread_cond_timedwait(&cond_fail_check, &mutex_cond_fail_check, &ts);
		pthread_cond_wait(&cond_fail_check, &mutex_cond_fail_check);
		LOG("%s *PRESSED\n", LIGHT_GRAY);
		delay(DELAY_MAGIC);
		pthread_mutex_unlock(&mutex_cond_fail_check);
		int ret = test();
		if (ret == TEST_PASS) {
			LOG("%s [CHECK and PASS]\n", LIGHT_GREEN);
			servo(0, 0);
			delay(DELAY_MAGIC);
			servo(0, 90);
			delay(DELAY_MAGIC);
			servo(0, 0);
			delay(DELAY_MAGIC/2);
			servo(1, 0);
		}
		else {
			LOG("%s [CHECK and FAIL]\n", LIGHT_RED);
			servo(0, 0);
			delay(DELAY_MAGIC);
			servo(0, 90);
			delay(DELAY_MAGIC);
			servo(0, 0);
			
			delay(DELAY_MAGIC/2);
			servo(1, 0);
			delay(DELAY_MAGIC);
			servo(0, 90);
			delay(DELAY_MAGIC);
			servo(0, 0);
		}
	}
	return 0;
}

int test() {
	int level = 0;
	char str[128] = "";
	char str2[8] = "";
	if (digitalRead(SENSOR_0) == HIGH) {
		strcat(str, sensor_gpio[0]);
		level++;
	} 
	if (digitalRead(SENSOR_1) == HIGH) {
		strcat(str, sensor_gpio[1]);
		level++;
	} 
	if (digitalRead(SENSOR_2) == HIGH) {
		strcat(str, sensor_gpio[2]);
		level++;
	} 
	if (digitalRead(SENSOR_3) == HIGH) {
		strcat(str, sensor_gpio[3]);
		level++;
	} 
	if (digitalRead(SENSOR_4) == HIGH) {
		strcat(str, sensor_gpio[4]);
		level++;
	} 
	if (digitalRead(SENSOR_5) == HIGH) {
		strcat(str, sensor_gpio[5]);
		level++;
	} 
	if (digitalRead(SENSOR_6) == HIGH) {
		strcat(str, sensor_gpio[6]);
		level++;
	}
	if (digitalRead(SENSOR_7) == HIGH) {
		strcat(str, sensor_gpio[7]);
		level++;
	}
	if (level == 0) return TEST_PASS;
	//else if (level == 8) return TEST_RETRY;
	strcat(str2, "BAD");
	LOG("%s %s%s\n", LIGHT_GRAY, str2, str);
	return TEST_FAIL;
}

void resetCounter() {
	//LOG("%s reset counters\n", DARY_GRAY);
	counter_sensor_0 = 0;
	counter_sensor_1 = 0;
	counter_sensor_2 = 0;
	counter_sensor_3 = 0;
	counter_sensor_4 = 0;
	counter_sensor_5 = 0;
	counter_sensor_6 = 0;
	counter_sensor_7 = 0;
}

int main(void) {
	LOG("%s -*-*-*- Amo is cooking Raspberry Pi-*-*-*-\n", LIGHT_GREEN);
	
	//wiringPiSetup();
	wiringPiSetupGpio();
	servo_init();

	pinMode (LED, OUTPUT);
	pinMode (BTN, INPUT);
	pinMode (SENSOR_0, INPUT);
	pinMode (SENSOR_1, INPUT);
	pinMode (SENSOR_2, INPUT);
	pinMode (SENSOR_3, INPUT);
	pinMode (SENSOR_4, INPUT);
	pinMode (SENSOR_5, INPUT);
	pinMode (SENSOR_6, INPUT);
	pinMode (SENSOR_7, INPUT);
	
	pullUpDnControl(BTN, PUD_DOWN);
	pullUpDnControl(SENSOR_0, PUD_UP);
	pullUpDnControl(SENSOR_1, PUD_UP);
	pullUpDnControl(SENSOR_2, PUD_UP);
	pullUpDnControl(SENSOR_3, PUD_UP);
	pullUpDnControl(SENSOR_4, PUD_UP);
	pullUpDnControl(SENSOR_5, PUD_UP);
	pullUpDnControl(SENSOR_6, PUD_UP);
	pullUpDnControl(SENSOR_7, PUD_UP);
	
	pthread_mutex_init(&mutex_sensor_0, 0);
	pthread_mutex_init(&mutex_sensor_1, 0);
	pthread_mutex_init(&mutex_sensor_2, 0);
	pthread_mutex_init(&mutex_sensor_3, 0);
	pthread_mutex_init(&mutex_sensor_4, 0);
	pthread_mutex_init(&mutex_sensor_5, 0);
	pthread_mutex_init(&mutex_sensor_6, 0);
	pthread_mutex_init(&mutex_sensor_7, 0);
	pthread_mutex_init(&mutex_cond_show, 0);
	pthread_mutex_init(&mutex_cond_fail_check, 0);

	pthread_t tSensor;
	pthread_t tBlink;
	pthread_t tShow;
	pthread_t tCheck;
	
	pthread_create(&tSensor, NULL, taskSensor, NULL);
	pthread_create(&tBlink, NULL, taskBlink, NULL);
	pthread_create(&tShow, NULL, taskShow, NULL);
	pthread_create(&tCheck, NULL, taskCheck, NULL);
	
	pthread_join(tSensor, NULL);
	pthread_join(tBlink, NULL);
	pthread_join(tShow, NULL);
	pthread_join(tCheck, NULL);
	
	pthread_mutex_destroy(&mutex_sensor_0);
	pthread_mutex_destroy(&mutex_sensor_1);
	pthread_mutex_destroy(&mutex_sensor_2);
	pthread_mutex_destroy(&mutex_sensor_3);
	pthread_mutex_destroy(&mutex_sensor_4);
	pthread_mutex_destroy(&mutex_sensor_5);
	pthread_mutex_destroy(&mutex_sensor_6);
	pthread_mutex_destroy(&mutex_sensor_7);
	pthread_mutex_destroy(&mutex_cond_show);
	pthread_mutex_destroy(&mutex_cond_fail_check);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
