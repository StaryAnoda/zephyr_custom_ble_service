#ifndef APP_COMMON_H
#define APP_COMMON_H
#include <zephyr/kernel.h>

/*Make a mssq  common to the whole app*/ 
struct current_temp_msg {
	float value; 
};

#define TEMPQSIZE 	2
#define TEMP_MSG_SIZE	sizeof(struct current_temp_msg)

extern struct current_temp_msg curr_msg;

#endif
