#ifndef FSM_SERVICE_H
#define FSM_SERVICE_H


/*List of App Events & Gates*/

#define EVENT_CENTRAL_CONNECTED BIT(0)
#define EVENT_TEMP_READ 	BIT(1)
#define EVENT_AUDIO_DETECTED 	BIT(2)
#define EVENT_ERROR_OCCURED 	BIT(3)


/*End List Events & gates*/


void fsm_run_thread(void *arg1 , void *arg2, void *arg3); 
#endif
