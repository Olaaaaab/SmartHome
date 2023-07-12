// mqtt_iot.h
#ifndef __MQTT_IOT_H__
#define __MQTT_IOT_H__

#include <pthread.h>
#include <semaphore.h>
#include <sys/msg.h>

typedef enum{
    DisconThread,
    PubThread
}Mqtt_Thread;

typedef struct  
{
	long mtype;       /* message type, must be > 0 */
	unsigned int value;    /* message data */
}msgbuf;

int mqtt_disconnect(void);
int mqtt_iot(void);

#endif /* __MQTT_IOT_H__ */