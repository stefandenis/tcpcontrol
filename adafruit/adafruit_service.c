#include "mqtt/mqtt_core/mqtt_core.h"
#include "../mqtt/mqtt_comm_bsd/mqtt_comm_layer.h"
#include "adafruit_service.h"
#include <stdbool.h>
#include "adafruit/bsd_adapter/bsdWINC.h"
#include "mqtt/mqtt_packetTransfer_interface.h"

publishReceptionHandler_t imqtt_publishReceiveCallBackTable[NUM_TOPICS_SUBSCRIBE];

uint32_t mqttAdafruitIP;

bool sendSubscribe = true;

packetReceptionHandler_t adafruit_packetReceiveCallBackTable[ADAFRUIT_PACKET_RECV_TABLE_SIZE];

void           received_from_adafruit(uint8_t *topic, uint8_t *payload);

static int8_t             connectMQTTSocket(void);

char Feed_Key[FEED_KEY_LENGTH];
char Adafruit_IO_Username[ADAFRUIT_USERNAME_LENGTH];
char mqttSubscribeTopic[TOPIC_SIZE];



static int8_t             connectMQTTSocket(void)
{
	int8_t ret = false;

	if (mqttAdafruitIP > 0) {
		struct bsd_sockaddr_in addr;

		addr.sin_family      = PF_INET;
		addr.sin_port        = BSD_htons(1883);
		addr.sin_addr.s_addr = mqttAdafruitIP;

		mqttContext * context     = MQTT_GetClientConnectionInfo();
		socketState_t socketState = BSD_GetSocketState(*context->tcpClientSocket);

		// Todo: Check - Are we supposed to call close on the socket here to ensure we do not leak ?
		if (socketState == NOT_A_SOCKET) {
			*context->tcpClientSocket = BSD_socket(PF_INET, BSD_SOCK_STREAM, 1);

			if (*context->tcpClientSocket >= 0) {
				packetReceptionHandler_t *sockInfo = getSocketInfo(*context->tcpClientSocket);
				if (sockInfo != NULL) {
					sockInfo->socketState = SOCKET_CLOSED;
				}
			}
		}

		socketState = BSD_GetSocketState(*context->tcpClientSocket);
		if (socketState == SOCKET_CLOSED) {
			printf("CLOUD: Connect socket");
			ret = BSD_connect(*context->tcpClientSocket, (struct bsd_sockaddr *)&addr, sizeof(struct bsd_sockaddr_in));

			if (ret != BSD_SUCCESS) {
				printf("CLOUD connect received %d", ret);
				
				//shared_networking_params.haveERROR = 1;
				BSD_close(*context->tcpClientSocket);
			}
		}
	}
	return ret;
}



void ADAFRUIT_subscribe(void)
{
	mqttSubscribePacket adafruitSubscribePacket;
	uint8_t             topicCount = 0;

	// Variable header
	adafruitSubscribePacket.packetIdentifierLSB = 1;
	adafruitSubscribePacket.packetIdentifierMSB = 0;

	// Payload
	for (topicCount = 0; topicCount < NUM_TOPICS_SUBSCRIBE; topicCount++) {
		sprintf(mqttSubscribeTopic, "%s/feeds/%s", Adafruit_IO_Username, Feed_Key );
		adafruitSubscribePacket.subscribePayload[topicCount].topic        = (uint8_t *)mqttSubscribeTopic;
		adafruitSubscribePacket.subscribePayload[topicCount].topicLength  = strlen(mqttSubscribeTopic);
		adafruitSubscribePacket.subscribePayload[topicCount].requestedQoS = 0;

		imqtt_publishReceiveCallBackTable[0].topic                         = mqttSubscribeTopic;
		imqtt_publishReceiveCallBackTable[0].mqttHandlePublishDataCallBack = received_from_adafruit;
		MQTT_SetPublishReceptionHandlerTable(imqtt_publishReceiveCallBackTable);
	}

	if (MQTT_CreateSubscribePacket(&adafruitSubscribePacket) == true) {
		debug_printInfo("ADAFRUIT: SUBSCRIBE packet created");
		sendSubscribe = false;
	}
}


void ADAFRUIT_publishData(uint8_t *data, unsigned int len){
	
	MQTT_CLIENT_publish(data,len);
	
}

static void dnsHandler(uint8_t *domainName, uint32_t serverIP)
{
	if (serverIP != 0) {
		mqttAdafruitIP = serverIP;
		
	}
}
