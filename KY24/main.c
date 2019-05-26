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
#define SENSOR_0 0
#define SENSOR_1 1
#define	DEBOUNCE_TIME 500
#define	DELAY_TIME 1000
#define	DELAY_LOG 2000

#define	TEST_TRUE 1
#define	TEST_FAIL 0

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
int counter_gpio17 = 0;
int counter_gpio18 = 0;
int interval = 0;
pthread_cond_t cond_show = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_cond_show = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_gpio17 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_gpio18 = PTHREAD_MUTEX_INITIALIZER;

int isPass();
void reset();
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

void handlerKY24_GPIO17(void) {
	pthread_mutex_lock(&mutex_gpio17);
	int time = 0;
	time = millis();
	if (time < interval) {
		LOG("%s ********* TOO FAST AROUND GPIO17 *********\n", RED);
		goto END;
	}
	interval = millis() + DEBOUNCE_TIME;
	
	if (digitalRead (SENSOR_0) == HIGH) {
		LOG("%s ********* DON'T STICK AROUND GPIO17 *********\n", RED);
		goto END;
	}
	//Got it
	(counter_gpio17)++;
	LOG("%s ********* Got it and count:%d *********\n", LIGHT_GREEN, counter_gpio17);
	pthread_mutex_lock(&mutex_cond_show);
	pthread_cond_signal(&cond_show);
	pthread_mutex_unlock(&mutex_cond_show);
END:		
	pthread_mutex_unlock(&mutex_gpio17);
	return;
}

void handlerKY24_GPIO18(void) {
	pthread_mutex_lock(&mutex_gpio18);
	int time = 0;
	time = millis();
	if (time < interval) {
		LOG("%s ********* TOO FAST AROUND GPIO18 *********\n", RED);
		goto END;
	}
	interval = millis() + DEBOUNCE_TIME;
	
	if (digitalRead (SENSOR_0) == HIGH) {
		LOG("%s ********* DON'T STICK AROUND GPIO18 *********\n", RED);
		goto END;
	}
	//Got it
	(counter_gpio18)++;
	LOG("%s ********* Got it and count:%d *********\n", LIGHT_GREEN, counter_gpio18);
	pthread_mutex_lock(&mutex_cond_show);
	pthread_cond_signal(&cond_show);
	pthread_mutex_unlock(&mutex_cond_show);
END:		
	pthread_mutex_unlock(&mutex_gpio18);
	return;
}

void* taskKY24(void* arg) {
	system ("gpio edge 18 rising") ;
	wiringPiISR(SENSOR_0, INT_EDGE_RISING, &handlerKY24_GPIO17);
	wiringPiISR(SENSOR_1, INT_EDGE_RISING, &handlerKY24_GPIO18);
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
	int time = 0;
	while (1) {
		pthread_mutex_lock(&mutex_cond_show);
		pthread_cond_wait(&cond_show, &mutex_cond_show); 
		if (isPass()) {
			/*
			digitalWrite(LED, HIGH);
			delay(DELAY_TIME);
			digitalWrite(LED, LOW);
			delay(DELAY_TIME);
			*/
			LOG("%s ********* PASS *********\n", LIGHT_GREEN);
		} 
		else {
			LOG("%s ********* FAIL *********\n", RED);
		}
		time = millis();
		if (time > interval) {
			LOG("%s ********* reset counters *********\n", DARY_GRAY);
			reset();
		}
		pthread_mutex_unlock(&mutex_cond_show);
	}
	return 0;
}

int isPass() {
	int ret = TEST_FAIL;
	ret &= counter_gpio17;
	ret &= counter_gpio18;
	return ret > 0 ? TEST_TRUE : TEST_FAIL;
}

void restPass() {
	counter_gpio17 = 0;
	counter_gpio18 = 0;
}

int main(void) {
	LOG("%s -*-*-*- Amo is cooking Raspberry Pi-*-*-*-\n", LIGHT_GREEN);
	
	wiringPiSetup();
	//pinMode (LED, OUTPUT);
	pinMode (SENSOR_0, INPUT);
	pullUpDnControl(SENSOR_0, PUD_DOWN);
	
	pthread_t tKY, tLog, tShow;
	LOG("%s going to pthread_create(&tKY, NULL, taskKY24, NULL)\n", GREEN);
	pthread_create(&tKY, NULL, taskKY24, NULL);
	LOG("%s going to pthread_create(&tLog, NULL, taskLog, NULL)\n", GREEN);
	pthread_create(&tLog, NULL, taskLog, NULL);
	LOG("%s going to pthread_create(&tKY, NULL, taskKY24, NULL)\n", GREEN);
	pthread_create(&tShow, NULL, taskShow, NULL);
	pthread_join(tKY, NULL);
	pthread_join(tLog, NULL);
	pthread_join(tShow, NULL);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
