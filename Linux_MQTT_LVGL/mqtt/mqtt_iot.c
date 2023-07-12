// mqtt_iot.c
#include "mqtt_iot.h"
#include "src/MQTTClient.h"  //��Ҫ��ϵͳ����ǰ��װ��MQTT�����Բο�

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

MQTTClient client;  //����һ��MQTT�ͻ���client
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

//���ݸ�MQTTClient_setCallbacks�Ļص����� ��Ϣ����󣬵��ô˻ص����� 
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
    
	set_temp_humi_data(value);	// �������Ƿ�װ��LVGL���º���

	MQTTClient_freeMessage(&message);	// �ͷ���Ϣ
	MQTTClient_free(topicName);	// �ͷ�������
	return 1; 
}

//���ݸ�MQTTClient_setCallbacks�Ļص����� �����쳣�Ͽ�����ô˻ص����� 
void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf(" cause: %s\n", cause);
}

// ��װ�����Ͽ����ӷ������ĺ���
int mqtt_disconnect(void)
{
    int rc = EXIT_SUCCESS;

	if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)  //�Ͽ��ͷ����������� 
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

// mqtt�����ͻ��ˡ����ӷ���������������ķ�װ��ں���
int mqtt_iot(void)
{
    int rc = EXIT_SUCCESS;
	if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
					MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to create client, return code %d\n", rc);
		goto exit;
	}
	//���ûص�������
	if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd,
					NULL)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to set callbacks, return code %d\n", rc);
		goto destroy_exit;
	}

	conn_opts.keepAliveInterval = 60;
	conn_opts.cleansession = 1;
	//���ӷ�����
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		goto destroy_exit;
	}

    //�������� 
	if ((rc = MQTTClient_subscribe(client, SUB_TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to subscribe, return code %d\n", rc);
        goto destroy_exit;
	}

	printf("MQTT connect success, press 'Q' or 'q' to disconnect mqtt server\n");
    return 0;

destroy_exit:
	MQTTClient_destroy(&client); //�ͷſͻ��˵���Դ 
 		printf("Failed to subscribe, return code %d\n", rc);
    return -1; 
exit:
    return -1;
}
