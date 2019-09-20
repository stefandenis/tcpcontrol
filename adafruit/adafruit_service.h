/*
 * adafruit_service.h
 *
 * Created: 9/3/2019 3:50:47 PM
 *  Author: M51763
 */ 


#ifndef ADAFRUIT_SERVICE_H_
#define ADAFRUIT_SERVICE_H_
#include <stdbool.h>


#define ADAFRUIT_PACKET_RECV_TABLE_SIZE 2
#define ADAFRUIT_MAX_DEVICEID_LENGTH 30
#define PASSWORD_SPACE 456
#define ADAFRUIT_USERNAME_LENGTH 13
#define FEED_KEY_LENGTH 8

void ADAFRUIT_reset(void);
void ADAFRUIT_init(char *deviceId);
void ADAFRUIT_subscribe(void);
void ADAFRUIT_disconnect(void);
bool ADAFRUIT_isConnected(void);
void ADAFRUIT_publishData(uint8_t *data, unsigned int len);


#endif /* ADAFRUIT_SERVICE_H_ */