#include "mqtt_packetPopulate.h"
#include "../mqtt/mqtt_core/mqtt_core.h"
#include "../mqtt/mqtt_comm_bsd/mqtt_comm_layer.h"




void MQTT_CLIENT_publish(uint8_t *data, uint16_t len){
	
	mqttPublishPacket adafruitPublishPacket;

	// Fixed header
	adafruitPublishPacket.publishHeaderFlags.duplicate = 0;
	adafruitPublishPacket.publishHeaderFlags.qos       = 0;
	adafruitPublishPacket.publishHeaderFlags.retain    = 0;

	// Variable header
	adafruitPublishPacket.topic = (uint8_t *)mqttTopic;

	// Payload
	adafruitPublishPacket.payload = data;
	// ToDo Check whether sizeof can be used for integers and strings
	adafruitPublishPacket.payloadLength = len;

	if (MQTT_CreatePublishPacket(&adafruitPublishPacket) != true) {
		debug_printError("MQTT: Connection lost PUBLISH failed");
	}

	
	
}