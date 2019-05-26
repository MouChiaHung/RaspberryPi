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
#define SENSOR_0 1
#define	DEBOUNCE_TIME 500
#define	DELAY_TIME 1000
#define	DELAY_LOG 2000

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
int gCounter;
int interval = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
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

void handlerKY24(void) {
	/*
	digitalWrite(LED, HIGH);
	delay(DELAY_TIME);
	digitalWrite(LED, LOW);
	delay(DELAY_TIME);
	*/
	
	int time = 0;
	time = millis();
	if (time < interval) return;
	interval = millis() + DEBOUNCE_TIME;
	
	pthread_mutex_lock(&mutex1);
	(gCounter)++;
	LOG("%s ********* Got it and count:%d *********\n", LIGHT_GREEN, gCounter);	
	while (digitalRead (SENSOR_0) == HIGH) {
		LOG("%s ********* HIGH INT and Delay *********\n", RED);
		delay(1000);
	}	
	pthread_mutex_unlock(&mutex1);
}

#if 0
PI_THREAD(taskKY24) {
	
}
#else 
void* taskKY24(void* arg) {
	system ("gpio edge 18 rising") ;
	wiringPiISR(SENSOR_0, INT_EDGE_RISING, &handlerKY24);
	return 0;
}
#endif

void* taskLog(void* arg) {
	arg = NULL;
	int interval_taskLog = 0;
	int time_taskLog = 0;

	while (1) {
		time_taskLog = millis();
		if (time_taskLog < interval_taskLog) continue;
		pthread_mutex_lock(&mutex1);
		LOG("%s ********* Testing and now count:%d *********\n", DARY_GRAY, gCounter);
		pthread_mutex_unlock(&mutex1);
		interval_taskLog = millis() + DELAY_LOG;
		sleep(1);
	}
	return 0;
}

int main(void) {
	LOG("%s -*-*-*- Amo is cooking Raspberry Pi-*-*-*-\n", LIGHT_GREEN);
	
	wiringPiSetup();
	pinMode (LED, OUTPUT);
	pinMode (SENSOR_0, INPUT);
	pullUpDnControl(SENSOR_0, PUD_DOWN);
	
	pthread_mutex_lock( &mutex1 );
	gCounter = 0;
	pthread_mutex_unlock( &mutex1 );
#if 0	
	piThreadCreate(taskKY24);
#endif	
	pthread_t tKY, tLog;
	LOG("%s going to pthread_create(&tKY, NULL, taskKY24, NULL)\n", GREEN);
	pthread_create(&tKY, NULL, taskKY24, NULL);
	LOG("%s going to pthread_create(&tLog, NULL, taskLog, NULL)\n", GREEN);
	pthread_create(&tLog, NULL, taskLog, NULL);
	pthread_join(tKY, NULL);
	pthread_join(tLog, NULL);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
