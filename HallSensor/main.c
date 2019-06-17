 /*************************************************************************************************************************************************
  *  main.cpp
  *  editor: amo
  *************************************************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>


/* -------------------------------------------------------------------- */
/* my define macro                                                     */
/* -------------------------------------------------------------------- */
#define LED 0
#define BTN 17
#if 0 //WiringPi
#define SENSOR_0 0
#define SENSOR_1 1
#define SENSOR_2 2
#define SENSOR_3 3
#define SENSOR_4 4
#define SENSOR_5 5
#define SENSOR_6 6
#define SENSOR_7 7
#else //BCM
#define SENSOR_0 17
#define SENSOR_1 28
#define SENSOR_2 27
#define SENSOR_3 22
#define SENSOR_4 23
#define SENSOR_5 24
#define SENSOR_6 25
#define SENSOR_7 4
#define SERVO_0 18
#define SERVO_1 12
#endif

#define SERV_0_DUTY_90 20
#define PWM_CHANNEL_0_CLOCK 1920
#define PWM_CHANNEL_0_RANGE 200
#define PWM_BASE_FREQ 19200000

#define	DEBOUNCE_TIME 250
#define	DELAY_TIME 1000
#define	DELAY_LOG 2000
#define	DELAY_MAGIC 500

#define	TEST_PASS 1
#define	TEST_FAIL 2
#define	TEST_RETRY 3

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

/* -------------------------------------------------------------------- */
/* global variables                                                     */
/* -------------------------------------------------------------------- */
int counter_sensor_0 = 0;
int counter_sensor_1 = 0;
int counter_sensor_2 = 0;
int counter_sensor_3 = 0;
int counter_sensor_4 = 0;
int counter_sensor_5 = 0;
int counter_sensor_6 = 0;
int counter_sensor_7 = 0;

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
pthread_mutex_t mutex_sensor_0; //0
pthread_mutex_t mutex_sensor_1; //1
pthread_mutex_t mutex_sensor_2; //2
pthread_mutex_t mutex_sensor_3; //3
pthread_mutex_t mutex_sensor_4; //4
pthread_mutex_t mutex_sensor_5; //5
pthread_mutex_t mutex_sensor_6; //6
pthread_mutex_t mutex_sensor_7;  //7

#define SENSOR_0 17
#define SENSOR_1 28
#define SENSOR_2 27
#define SENSOR_3 22
#define SENSOR_4 23
#define SENSOR_5 24
#define SENSOR_6 25
#define SENSOR_7 4
#define SERVO_0 18
#define SERVO_1 12

char sensor_gpio[8][8] = {" GPIO17", " GPIO28", " GPIO27", " GPIO22", " GPIO23", " GPIO24", " GPIO25", " GPIO4"};

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

void servo(int servo, int angle) {
	float period_per_unit = 0.1; //0.1ms;
	int duty = 0; //ms
	int value = 0; //count of units
	switch (servo) {
		case 0:
			period_per_unit = (1/PWM_BASE_FREQ)*PWM_CHANNEL_0_CLOCK*1000;
			duty = (period_per_unit*SERV_0_DUTY_90)-((90-angle)/180)*10*period_per_unit; //1.5ms for 0, 2ms for 90, 1ms for -90
			value = duty/period_per_unit;
			LOG("%s servo_0 going to pwmWrite:%d\n", LIGHT_GRAY, value);
			pwmWrite(SERVO_0, value);
			break;
		case 1:
			period_per_unit = (1/PWM_BASE_FREQ)*PWM_CHANNEL_0_CLOCK*1000;
			duty = (period_per_unit*SERV_0_DUTY_90)-((90-angle)/180)*10*period_per_unit; //1.5ms for 0, 2ms for 90, 1ms for -90
			value = duty/period_per_unit;
			LOG("%s servo_1 going to pwmWrite:%d\n", LIGHT_GRAY, value);
			pwmWrite(SERVO_1, value);
			break;
		default:
			break;
	}
}

void handler_BTN(void) {
	LOG("%s ********* Got a Button *********\n", BLUE);
	pthread_cond_signal(&cond_show);
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
		//LOG("%s ********* FAST SENSOR 1 *********\n", RED);
		goto END;
	}
	interval_sensor_1 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_1) == HIGH) {
		//LOG("%s ********* LOST SENSOR 1 *********\n", RED);
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
	system ("gpio edge 17 falling");
	system ("gpio edge 28 falling");
	system ("gpio edge 27 falling");
	system ("gpio edge 22 falling");
	system ("gpio edge 23 falling");
	system ("gpio edge 24 falling");
	system ("gpio edge 25 falling");
	system ("gpio edge 4 falling");

	wiringPiISR(BTN, INT_EDGE_FALLING, &handler_BTN);
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

void* taskLog(void* arg) {
	arg = NULL;
	int interval_taskLog = 0;
	int time_taskLog = 0;

	while (1) {
		time_taskLog = millis();
		if (time_taskLog < interval_taskLog) continue;
		LOG("%s ********* Testing *********\n", DARY_GRAY);
		interval_taskLog = millis() + DELAY_LOG;
		sleep(DELAY_LOG);	
	}	
	return 0;
}

void* taskShow(void* arg) {
	while (1) {
		pthread_mutex_lock(&mutex_cond_show);
		pthread_cond_wait(&cond_show, &mutex_cond_show);
		delay(DELAY_MAGIC);
		pthread_mutex_unlock(&mutex_cond_show);
		int ret = test();
		if (ret == TEST_PASS) {
			/*
			digitalWrite(LED, HIGH);
			delay(DELAY_TIME);
			digitalWrite(LED, LOW);
			delay(DELAY_TIME);
			*/
			LOG("%s PASS\n", LIGHT_GREEN);
			servo(0, 90);
			delay(100);
			servo(0, 90);

		}
		else if (ret == TEST_RETRY){
			LOG("%s RETRY\n", LIGHT_CYAN);
		}
		else {
			LOG("%s FAIL\n", LIGHT_RED);
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
	else if (level == 8) return TEST_RETRY;
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
   	pinMode (SERVO_0, PWM_OUTPUT) ;
   	pwmSetMode (PWM_MODE_MS);
   	pwmSetRange (200);
   	pwmSetClock (1920);

   	pinMode (SERVO_1, PWM_OUTPUT) ;
   	pwmSetMode (PWM_MODE_MS);
   	pwmSetRange (200);
   	pwmSetClock (1920);

	//pinMode (LED, OUTPUT);
	pinMode (BTN, INPUT);
	pinMode (SENSOR_0, INPUT);
	pinMode (SENSOR_1, INPUT);
	pinMode (SENSOR_2, INPUT);
	pinMode (SENSOR_3, INPUT);
	pinMode (SENSOR_4, INPUT);
	pinMode (SENSOR_5, INPUT);
	pinMode (SENSOR_6, INPUT);
	pinMode (SENSOR_7, INPUT);
	
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

	
#if 0
	pthread_t tLog;
	pthread_create(&tLog, NULL, taskLog, NULL);
	pthread_join(tLog, NULL);
#endif

	pthread_t tSensor;
	pthread_t tShow;
	
	pthread_create(&tSensor, NULL, taskSensor, NULL);
	pthread_create(&tShow, NULL, taskShow, NULL);
	
	pthread_join(tSensor, NULL);
	pthread_join(tShow, NULL);
	
	pthread_mutex_destroy(&mutex_sensor_0);
	pthread_mutex_destroy(&mutex_sensor_1);
	pthread_mutex_destroy(&mutex_sensor_2);
	pthread_mutex_destroy(&mutex_sensor_3);
	pthread_mutex_destroy(&mutex_sensor_4);
	pthread_mutex_destroy(&mutex_sensor_5);
	pthread_mutex_destroy(&mutex_sensor_6);
	pthread_mutex_destroy(&mutex_sensor_7);
	pthread_mutex_destroy(&mutex_cond_show);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
