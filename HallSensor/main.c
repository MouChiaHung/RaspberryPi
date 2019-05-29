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
#define SENSOR_0 0
#define SENSOR_1 1
#define SENSOR_2 2
#define SENSOR_3 3
#define SENSOR_4 4
#define SENSOR_5 5
#define SENSOR_6 6
#define SENSOR_7 7

#define	DEBOUNCE_TIME 1000
#define	DELAY_TIME 1000
#define	DELAY_LOG 2000
#define	DELAY_MAGIC 2000

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
int counter_gpio27 = 0;
int counter_gpio22 = 0;
int counter_gpio23 = 0;
int counter_gpio24 = 0;
int counter_gpio25 = 0;
int counter_gpio4 = 0;

int interval_17 = 0;
int interval_18 = 0;
int interval_27 = 0;
int interval_22 = 0;
int interval_23 = 0;
int interval_24 = 0;
int interval_25 = 0;
int interval_4 = 0;

pthread_cond_t cond_show = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_cond_show;
pthread_mutex_t mutex_gpio17; //0
pthread_mutex_t mutex_gpio18; //1
pthread_mutex_t mutex_gpio27; //2
pthread_mutex_t mutex_gpio22; //3
pthread_mutex_t mutex_gpio23; //4
pthread_mutex_t mutex_gpio24; //5
pthread_mutex_t mutex_gpio25; //6
pthread_mutex_t mutex_gpio4;  //7
//flag
int is_reset = 0;

int isPass();
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

void handler_GPIO28(void) { //17
	LOG("%s ********* Got a Button *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
}

void handler_GPIO17(void) { //0
	pthread_mutex_lock(&mutex_gpio17);
	int time = 0;
	time = millis();
	if (time < interval_17) {
		//LOG("%s ********* TOO FAST AROUND GPIO17 *********\n", RED);
		goto END;
	}
	interval_17 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_0) == HIGH) {
		//LOG("%s ********* DON'T STICK AROUND GPIO17 *********\n", RED);
		goto END;
	}
	//Got it
	(counter_gpio17)++;
	LOG("%s ********* Got at GPIO17 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio17);
	return;
}

void handler_GPIO18(void) { //1
	pthread_mutex_lock(&mutex_gpio18);
	int time = 0;
	time = millis();
	if (time < interval_18) {
		//LOG("%s ********* TOO FAST AROUND GPIO18 *********\n", RED);
		goto END;
	}
	interval_18 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_1) == HIGH) {
		//LOG("%s ********* DON'T STICK AROUND GPIO18 *********\n", RED);
		goto END;
	}
	//Got it
	(counter_gpio18)++;
	LOG("%s ********* Got at GPIO18 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);

END:		
	pthread_mutex_unlock(&mutex_gpio18);
	return;
}

void handler_GPIO27(void) { //2
	pthread_mutex_lock(&mutex_gpio27);
	int time = 0;
	time = millis();
	if (time < interval_27) {
		goto END;
	}
	interval_27 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_2) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio27)++;
	LOG("%s ********* Got at GPIO27 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio27);
	return;
}

void handler_GPIO22(void) { //3
	pthread_mutex_lock(&mutex_gpio22);
	int time = 0;
	time = millis();
	if (time < interval_22) {
		goto END;
	}
	interval_22 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_3) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio22)++;
	LOG("%s ********* Got at GPIO22 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio22);
	return;
}

void handler_GPIO23(void) { //4
	pthread_mutex_lock(&mutex_gpio23);
	int time = 0;
	time = millis();
	if (time < interval_23) {
		goto END;
	}
	interval_23 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_4) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio23)++;
	LOG("%s ********* Got at GPIO23 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio23);
	return;
}

void handler_GPIO24(void) { //5
	pthread_mutex_lock(&mutex_gpio24);
	int time = 0;
	time = millis();
	if (time < interval_24) {
		goto END;
	}
	interval_24 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_5) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio24)++;
	LOG("%s ********* Got at GPIO24 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio24);
	return;
}

void handler_GPIO25(void) { //6
	pthread_mutex_lock(&mutex_gpio25);
	int time = 0;
	time = millis();
	if (time < interval_25) {
		goto END;
	}
	interval_25 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_6) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio25)++;
	LOG("%s ********* Got at GPIO25 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio25);
	return;
}

void handler_GPIO4(void) { //7
	pthread_mutex_lock(&mutex_gpio4);
	int time = 0;
	time = millis();
	if (time < interval_4) {
		goto END;
	}
	interval_4 = millis() + DEBOUNCE_TIME;
	
	if (digitalRead(SENSOR_7) == HIGH) {
		goto END;
	}
	//Got it
	(counter_gpio4)++;
	LOG("%s ********* Got at GPIO4 *********\n", BLUE);
	is_reset = 0;
	pthread_cond_signal(&cond_show);
	
END:		
	pthread_mutex_unlock(&mutex_gpio4);
	return;
}

void* taskSensor(void* arg) {
	system ("gpio edge 17 falling");
	system ("gpio edge 18 falling");
	system ("gpio edge 27 falling");
	system ("gpio edge 22 falling");
	system ("gpio edge 23 falling");
	system ("gpio edge 24 falling");
	system ("gpio edge 25 falling");
	system ("gpio edge 4 falling");

	wiringPiISR(BTN, INT_EDGE_FALLING, &handler_GPIO28);
	wiringPiISR(SENSOR_0, INT_EDGE_FALLING, &handler_GPIO17);
	wiringPiISR(SENSOR_1, INT_EDGE_FALLING, &handler_GPIO18);
	wiringPiISR(SENSOR_2, INT_EDGE_FALLING, &handler_GPIO27);
	wiringPiISR(SENSOR_3, INT_EDGE_FALLING, &handler_GPIO22);
	wiringPiISR(SENSOR_4, INT_EDGE_FALLING, &handler_GPIO23);
	wiringPiISR(SENSOR_5, INT_EDGE_FALLING, &handler_GPIO24);
	wiringPiISR(SENSOR_6, INT_EDGE_FALLING, &handler_GPIO25);
	wiringPiISR(SENSOR_7, INT_EDGE_FALLING, &handler_GPIO4);
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
	int interval_show = 0;
	while (1) {
		pthread_mutex_lock(&mutex_cond_show);
		pthread_cond_wait(&cond_show, &mutex_cond_show); 
		pthread_mutex_unlock(&mutex_cond_show);
		interval_show = millis() + DELAY_MAGIC;
		while (1) {
			time = millis();
			if (time <= interval_show) continue;
			else {
				if (isPass() == TEST_TRUE) {
					/*
					digitalWrite(LED, HIGH);
					delay(DELAY_TIME);
					digitalWrite(LED, LOW);
					delay(DELAY_TIME);
					*/
					LOG("%s --------- PASS ---------\n", LIGHT_GREEN);
					resetCounter();
					break;
				}	 
				else {
					if (is_reset) {
						is_reset = 0;
						break;
					}
					else {
						LOG("%s --------- FAIL ---------\n", RED);
						resetCounter();
						break;
					}
				}
			}
		}
	}
	return 0;
}

void* taskReset(void* arg) {
	int time = 0;
	int interval_reset = 0;
	while (1) {
		time = millis();
		if (time > interval_reset) {
			pthread_mutex_lock(&mutex_cond_show);
			pthread_cond_signal(&cond_show);
			pthread_mutex_unlock(&mutex_cond_show);
			resetCounter();
			interval_reset = millis() + 5000;
			is_reset = 1;
		}
	}
	return 0;
}

int isPass() {
	//LOG("%s counter of gpio17:%d\n", DARY_GRAY, counter_gpio17);
	//LOG("%s counter of gpio18:%d\n", DARY_GRAY, counter_gpio18);
#if 0	
	if (counter_gpio17 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio18 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio27 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio22 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio23 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio24 == 0) {
		return TEST_FAIL;
	} 
	if (counter_gpio25 == 0) {
		return TEST_FAIL;
	}
	if (counter_gpio4 == 0) {
		return TEST_FAIL;
	} 
	return TEST_TRUE;
#else 
	if (digitalRead(SENSOR_0) == HIGH) {
		LOG("%s GPIO17 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_1) == HIGH) {
		LOG("%s GPIO18 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_2) == HIGH) {
		LOG("%s GPIO27 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_3) == HIGH) {
		LOG("%s GPIO22 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_4) == HIGH) {
		LOG("%s GPIO23 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_5) == HIGH) {
		LOG("%s GPIO24 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	if (digitalRead(SENSOR_6) == HIGH) {
		LOG("%s GPIO25 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	}
	if (digitalRead(SENSOR_7) == HIGH) {
		LOG("%s GPIO4 HIGH...\n", DARY_GRAY);
		return TEST_FAIL;
	} 
	return TEST_TRUE;
#endif
}

void resetCounter() {
	//LOG("%s reset counters\n", DARY_GRAY);
	counter_gpio17 = 0;
	counter_gpio18 = 0;
	counter_gpio27 = 0;
	counter_gpio22 = 0;
	counter_gpio23 = 0;
	counter_gpio24 = 0;
	counter_gpio25 = 0;
	counter_gpio4 = 0;
}

int main(void) {
	LOG("%s -*-*-*- Amo is cooking Raspberry Pi-*-*-*-\n", LIGHT_GREEN);
	
	wiringPiSetup();
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
	
	pthread_mutex_init(&mutex_gpio17, 0);
	pthread_mutex_init(&mutex_gpio18, 0);
	pthread_mutex_init(&mutex_gpio27, 0);
	pthread_mutex_init(&mutex_gpio22, 0);
	pthread_mutex_init(&mutex_gpio23, 0);
	pthread_mutex_init(&mutex_gpio24, 0);
	pthread_mutex_init(&mutex_gpio25, 0);
	pthread_mutex_init(&mutex_gpio4, 0);
	pthread_mutex_init(&mutex_cond_show, 0);
	
#if 0
	pthread_t tLog;
	pthread_create(&tLog, NULL, taskLog, NULL);
	pthread_join(tLog, NULL);
	
	pthread_t tReset;
	pthread_create(&tReset, NULL, taskReset, NULL);
	pthread_join(tReset, NULL);
#endif

	pthread_t tSensor;
	pthread_t tShow;
	
	pthread_create(&tSensor, NULL, taskSensor, NULL);
	pthread_create(&tShow, NULL, taskShow, NULL);
	
	pthread_join(tSensor, NULL);
	pthread_join(tShow, NULL);
	
	pthread_mutex_destroy(&mutex_gpio17);
	pthread_mutex_destroy(&mutex_gpio18);
	pthread_mutex_destroy(&mutex_gpio27);
	pthread_mutex_destroy(&mutex_gpio22);
	pthread_mutex_destroy(&mutex_gpio23);
	pthread_mutex_destroy(&mutex_gpio24);
	pthread_mutex_destroy(&mutex_gpio25);
	pthread_mutex_destroy(&mutex_gpio4);
	pthread_mutex_destroy(&mutex_cond_show);
	
	LOG("%s -*-*-*- Bye bye -*-*-*-\n", LIGHT_GREEN);
	return 0;
}
