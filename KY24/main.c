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
#define	MUTEX_KEY 0
#define	DEBOUNCE_TIME 100
#define	DELAY_TIME 250

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
int* gCounter;

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
	LOG("%s ********* GOT IT *********\n", GREEN);
	digitalWrite(LED, HIGH);
	delay(DELAY_TIME);
	digitalWrite(LED, LOW);
	delay(DELAY_TIME);
	//piLock(MUTEX_KEY);
	LOG("%s going to gCounter++\n", GREEN);
	(*gCounter)++;
	//piUnlock(MUTEX_KEY);
	LOG("%s ********* Right count:%d *********\n", *gCounter, GREEN);
}

#if 0
PI_THREAD(taskKY24) {
	int interval = 0;
	int time = 0;
	
	while (1) {
		time = millis();
		if (time < interval) continue;
		wiringPiISR(SENSOR_0, INT_EDGE_RISING, &handlerKY24);
		interval = millis() + DEBOUNCE_TIME;
	}
}
#else 
void* taskKY24(void* arg) {
	LOG("%s -*-*-*- Waiting... -*-*-*-\n", YELLOW);
	wiringPiISR(SENSOR_0, INT_EDGE_RISING, &handlerKY24);
	return 0;
}
#endif

void* taskLog(void* arg) {
	arg = NULL;
	int interval = 0;
	int time = 0;
	while (1) {
		time = millis();
		if (time < interval) continue;
		LOG("%s -*-*-*- Testing... -*-*-*-\n", YELLOW);
		
		LOG("%s going to gCounter++\n", GREEN);
		(*gCounter)++;
		LOG("%s ********* Right count:%d *********\n", *gCounter, GREEN);
		
		interval = millis() + 1000;
		//sleep(1);
	}
	return 0;
}

int main(void) {
	LOG("%s -*-*-*- Amo is cooking Raspberry Pi-*-*-*-\n", LIGHT_GREEN);
	wiringPiSetup();
	gCounter = (int*) malloc(sizeof(int));
	*gCounter = 10;
	//piThreadCreate(taskKY24);
	pthread_t tKY, tLog;
	pthread_create(&tKY, NULL, taskKY24, NULL);
	pthread_create(&tLog, NULL, taskLog, NULL);
	pthread_join(tKY, NULL);
	pthread_join(tLog, NULL);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
