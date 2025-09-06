#ifndef FSM_SERVICE_H
#define FSM_SERVICE_H
#include <zephyr/sys/util.h>

/*List of App Events & Gates*/
#define EVENT_CENTRAL_CONNECTED BIT(0)
#define EVENT_TEMP_READ 	BIT(1)
#define EVENT_AUDIO_DETECTED 	BIT(2)
#define EVENT_ERROR_OCCURED 	BIT(3)


extern struct k_sem temp_sense_gate; 
extern struct k_sem mic_sense_gate;

/*End List Events & gates*/


void fsm_run_thread(void *arg1 , void *arg2, void *arg3); 
#endif
