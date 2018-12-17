#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

#include "microphone_setup.h"

// Compilation Instructions
// gcc -Wall -o button microphone_setup.o button_test.o -lwiringPi -lpthread -lasound

// Run Instructions
//sudo ./button

#define GPIO_4_AKA_BUTTON1   7
#define PULL_UP 2
#define PULL_DOWN 1
#define RESISTOR_GONE 0
#define DEBOUNCE_TIME 300
#define LOGIC_LOW 0
#define LOGIC_HIGH 1
#define BUF_SIZE 1024

typedef struct _data_buffers {
	char from_microphone[BUF_SIZE];
	char to_speaker[BUF_SIZE];
} audio_data_t;


void queue_access_callback(void)
{
	// do software debouncing, we are waiting for....??
	unsigned long end = millis() + (unsigned long)DEBOUNCE_TIME;
	unsigned long current = 0;
	while (current <= end)
	{
		current = millis();
	}

	// Query the pin again
 	if (digitalRead(GPIO_4_AKA_BUTTON1) == LOGIC_LOW)
	{
		printf("We are accessing the queue now.\n");
	} else {
		printf("Debounce test failed.\n");
	}
}

void * from_microphone(void * from_mike)
{
	audio_data_t * ad = (audio_data_t *)from_mike;

	int i = 0;

	for (i = 0; i < BUF_SIZE; i++)
	{
		ad->from_microphone[i] = 'f';
	}

	while(1)
	{
		delay(250);
		printf("from_microphone: %c\n", ad->to_speaker[500]);
	}

	// Make sure the thread exits cleanly, can use this return code in pthread_join();
	pthread_exit(NULL);
}

void * to_speaker(void * to_spkr)
{
 	audio_data_t * ad = (audio_data_t *)to_spkr;

	int i = 0;

	for (i = 0; i < BUF_SIZE; i++)
	{
		ad->to_speaker[i] = 't';
	}

	while(1)
	{
		delay(250);
		printf("to_speaker: %c\n", ad->from_microphone[500]);
	}
	// Make sure the thread exits cleanly, can use this return code in pthread_join();
	pthread_exit(NULL);
}

int main(void)
{
	// Variable initializations
	pthread_t from_microphone_t;
	pthread_t to_speaker_t;
	audio_data_t audio_data;

	int pthread_rc = 0;
	int mike_init = 0;

	// Initialize the microphone
	mike_init = initialize_microphone("plughw:1,0");
	if (mike_init != 0)
		printf("Unable to initialize the microphone.\n");

	// Initialize wiringPi
	wiringPiSetup();

	// Initialize the testing button.
	pinMode(GPIO_4_AKA_BUTTON1, INPUT);
	pullUpDnControl(GPIO_4_AKA_BUTTON1, PULL_UP);

	// Initialize the interrupt for the pin.
	wiringPiISR(GPIO_4_AKA_BUTTON1, INT_EDGE_FALLING, &queue_access_callback);


	// Hardware initialized, let's get the audio processing threads going.
	pthread_rc = pthread_create(&from_microphone_t, NULL, from_microphone, &audio_data);
	if (pthread_rc > 0) 
		printf("Unable to create from_microphone thread!\n");
	pthread_rc = pthread_create(&to_speaker_t, NULL, to_speaker, &audio_data);
	if (pthread_rc > 0)
		printf("Unable to create to_speaker thread!\n");

	// while loop to poll the button.
	while(1)
	{
		// put a 250ms delay to give the processor some breathing room
		delay (250);
	}

	return 0;
}

