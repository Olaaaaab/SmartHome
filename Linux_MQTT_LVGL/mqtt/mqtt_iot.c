// mqtt_iot.c
#include "mqtt_iot.h"
#include "src/MQTTClient.h"  //需要在系统中提前安装好MQTT，可以参考

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define ADDRESS     "tcp://121.41.60.67:1883"
#define CLIENTID    "LVGL_MQTT"
#define SUB_TOPIC   "LVGL_MQTT"
#define QOS 0
#define TIMEOUT 	10000L


extern void set_temp_humi_data(unsigned short value);

MQTTClient client;  //定义一个MQTT客户端client
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

//传递给MQTTClient_setCallbacks的回调函数 消息到达后，调用此回调函数 
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	printf("Message arrived\n");
	printf(" topic: %s\n", topicName);
	printf(" message: %.*s\n", message->payloadlen, (char*)message->payload);

	unsigned short value = 0;
	unsigned short len = message->payloadlen;
	char *buf = (char*)message->payload;
	for(unsigned short i=0; i<len; i++)
	{
		if(buf[i] == '\0')	break;
		if(buf[i]<='9' && buf[i]>='0')
			value = value*10 + buf[i] - '0';
	}
    
	set_temp_humi_data(value);	// 调用我们封装的LVGL更新函数

	MQTTClient_freeMessage(&message);	// 释放消息
	MQTTClient_free(topicName);	// 释放主题名
	return 1; 
}

//传递给MQTTClient_setCallbacks的回调函数 连接异常断开后调用此回调函数 
void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf(" cause: %s\n", cause);
}

// 封装主动断开连接服务器的函数
int mqtt_disconnect(void)
{
    int rc = EXIT_SUCCESS;

	if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)  //断开和服务器的连接 
	{
		printf("Failed to disconnect, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}
	else
	{
		printf("MQTT disconnect success\n");
		MQTTClient_destroy(&client);
	}

	return rc;
}

// mqtt建立客户端、连接服务器、订阅主题的封装入口函数
int mqtt_iot(void)
{
    int rc = EXIT_SUCCESS;
	if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
					MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to create client, return code %d\n", rc);
		goto exit;
	}
	//设置回调函数，
	if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd,
					NULL)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to set callbacks, return code %d\n", rc);
		goto destroy_exit;
	}

	conn_opts.keepAliveInterval = 60;
	conn_opts.cleansession = 1;
	//连接服务器
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		goto destroy_exit;
	}

    //订阅主题 
	if ((rc = MQTTClient_subscribe(client, SUB_TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to subscribe, return code %d\n", rc);
        goto destroy_exit;
	}

	printf("MQTT connect success, press 'Q' or 'q' to disconnect mqtt server\n");
    return 0;

destroy_exit:
	MQTTClient_destroy(&client); //释放客户端的资源 
 		printf("Failed to subscribe, return code %d\n", rc);
    return -1; 
exit:
    return -1;
}
