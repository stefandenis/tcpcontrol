// TODO : optimize level to O1
#define F_CPU 8000000
#include <util/delay.h>

#include <atmel_start.h>
#include <avr/io.h>
 #include <avr/interrupt.h>
 #include <stdbool.h>
 
 #include <string.h>
 #include "winc_init.h"
 #include <stdio.h>
 #include "winc/driver/source/nmasic.h"
 #include "winc/socket/include/socket.h"
 #include "winc/socket/include/m2m_socket_host_if.h"
 #include "motoare.h"
 #include "mqtt/mqtt_core/mqtt_core.h"
 #include "mqtt/mqtt_comm_bsd/mqtt_comm_layer.h"
 #include "adafruit/adafruit_service.h"
 
 int usart_putchar_printf(char var, FILE *stream);
 
 static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);



int usart_putchar_printf(char var, FILE *stream){
	
	USART_1_write(var);
}

/*  Packet size */
#define MAIN_WIFI_M2M_BUFFER_SIZE 1460
//#define MAIN_WLAN_SSID "MCHP.IOT"
#define MAIN_WLAN_SSID "MCHP.IOT"
#define MAIN_WLAN_AUTH M2M_WIFI_SEC_WPA_PSK
//#define MAIN_WLAN_AUTH M2M_WIFI_SEC_OPEN
#define MAIN_WLAN_PSK "microchip"
#define MAIN_WLAN_CHANNEL (6) /* < Channel number */


#define MAIN_WIFI_M2M_SERVER_PORT (6666)
//#define MAIN_WIFI_M2M_SERVER_IP 0xC0A8FFFF /* 192.168.255.255 */atmega4809
#define MAIN_WIFI_M2M_SERVER_IP 0xFFFFFFFF


#define MAX_LENGTH 100

#define MQTT_CID_LENGTH 100
#define MQTT_TOPIC_LENGTH 38

/* Sockets for tcp communication */
static SOCKET tcp_server_socket = -1;
static SOCKET tcp_client_socket = -1;


char msg_wifi_product[40]="Message from server ";

/* Wi-Fi status */
static int8_t wifi_connected;

/* Socket buffer */
static uint8_t gau8SocketTestBuffer[MAIN_WIFI_M2M_BUFFER_SIZE];


volatile char command = 0;
int c;
char buffer[MAX_LENGTH];
int index;
volatile char data_response;
char order[10];

char Password[456]="qwertyhnkl";
char clientID[]="5dad78fc-6853-4115-862e-9de796fe2eff";
char mqttTopic[]="aphosura4ever/feeds/robowifi";
char mqttHost[] = "io.adafruit.com";
char Username[]="aphosura4ever";
char insecurePort[]="1883";


bool sendSubscribe = true;

void received_from_adafruit(uint8_t *topic, uint8_t *payload);

uint8_t pwm=70;
void get_number(char order[]){
	
	int zeci = order[1];
	int unitate = order[2];
	pwm = zeci*10 + unitate;
	
	
	
}


int flag_pwm_control = 0;
void get_command(char *str,tstrSocketRecvMsg *pCommand){

int i=0;
for(i=0;i<sizeof(order);i++){
	order[i]=0x00;
}
i=0;
while(*(pCommand->pu8Buffer+i) != 'x' && i<10){
 	
	*(str+i)=*(pCommand->pu8Buffer+i);
	i++;
	
}



//printf("order: %s",order);

}






/**
 * \brief Callback to get the Data from socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg socket event type. Possible values are:
 *  - SOCKET_MSG_BIND
 *  - SOCKET_MSG_LISTEN
 *  - SOCKET_MSG_ACCEPT
 *  - SOCKET_MSG_CONNECT
 *  - SOCKET_MSG_RECV
 *  - SOCKET_MSG_SEND
 *  - SOCKET_MSG_SENDTO
 *  - SOCKET_MSG_RECVFROM
 * \param[in] pvMsg is a pointer to message structure. Existing types are:
 *  - tstrSocketBindMsg
 *  - tstrSocketListenMsg
 *  - tstrSocketAcceptMsg
 *  - tstrSocketConnectMsg
 *  - tstrSocketRecvMsg
 */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	switch (u8Msg) {
	/* Socket bind */
	case SOCKET_MSG_BIND: {
		tstrSocketBindMsg *pstrBind = (tstrSocketBindMsg *)pvMsg;
		if (pstrBind && pstrBind->status == 0) {
			printf("socket_cb: bind success!\r\n");
			listen(tcp_server_socket, 0);
		} else {
			printf("socket_cb: bind error!\r\n");
			close(tcp_server_socket);
			tcp_server_socket = -1;
		}
	} break;

	/* Socket listen */
	case SOCKET_MSG_LISTEN: {
		tstrSocketListenMsg *pstrListen = (tstrSocketListenMsg *)pvMsg;
		if (pstrListen && pstrListen->status == 0) {
			printf("socket_cb: listen success!\r\n");
			accept(tcp_server_socket, NULL, NULL);
		} else {
			printf("socket_cb: listen error!\r\n");
			close(tcp_server_socket);
			tcp_server_socket = -1;
		}
	} break;

	/* Connect accept */
	case SOCKET_MSG_ACCEPT: {
		tstrSocketAcceptMsg *pstrAccept = (tstrSocketAcceptMsg *)pvMsg;
		if (pstrAccept) {
			printf("socket_cb: accept success!\r\n");
			accept(tcp_server_socket, NULL, NULL);
			tcp_client_socket = pstrAccept->sock;
			recv(tcp_client_socket, gau8SocketTestBuffer, sizeof(gau8SocketTestBuffer), 0);
		} else {
			printf("socket_cb: accept error!\r\n");
			close(tcp_server_socket);
			tcp_server_socket = -1;
		}
	} break;

	/* Message send */
	case SOCKET_MSG_SEND: {//  		printf("socket_cb: send success!\r\n");
 		recv(tcp_client_socket, gau8SocketTestBuffer, sizeof(gau8SocketTestBuffer), 0);
		 
 	} break;


	/* Message receive */
	case SOCKET_MSG_RECV: {
		tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
		if (pstrRecv && pstrRecv->s16BufferSize > 0) {
			printf("socket_cb: recv success!\r\n");
			
			get_command(order, pstrRecv);
			
			
			
			
			
			
			
			
			
			
			if(strcmp(order,"rotate")==0){		
				motor_start_left_wheel(pwm);
				motor_start_right_wheel(pwm);
				motor_reverse_left_wheel(pwm);
				printf("rotating !!");
			}
			if(strcmp(order,"forward")==0){
				motor_start_left_wheel(pwm);
				motor_start_right_wheel(pwm);
				
			}
			if(strcmp(order,"backward")==0){
				motor_start_left_wheel(pwm);
				motor_start_right_wheel(pwm);
				motor_reverse_left_wheel(pwm);
				motor_reverse_right_wheel(pwm);
				
				
			}
			if(strcmp(order,"stop")==0){
				motor_stop_left_wheel();
				motor_stop_right_wheel();
			}
			
			if(strcmp(order,"left")==0){
				motor_stop_left_wheel();
				motor_start_right_wheel(pwm);
			}
			if(strcmp(order,"right")==0){
				motor_start_left_wheel(pwm);
				motor_stop_right_wheel();
			}
					
			
					
					
				if(strcmp(order,"close")==0){
					motor_stop_left_wheel();
					motor_stop_right_wheel();
					close(tcp_client_socket);
					close(tcp_server_socket);
				}
				
				if(order[0] == 'c'){
					get_number(order);
					if(TCB1.CCMPH != 0 && TCB3.CCMPH !=0){
					TCB1.CCMPH = right_pwm_duty(pwm);
					TCB3.CCMPH = left_pwm_duty(pwm);
					}
					if(TCB1.CCMPH != 0 && TCB3.CCMPH == 0 ){
						TCB1.CCMPH = right_pwm_duty(pwm);
						
					}
					if(TCB3.CCMPH != 0 && TCB1.CCMPH == 0 ){
						TCB3.CCMPH = right_pwm_duty(pwm);
						
					}
				
				}
				
			
			printf("%s \n \r ",order);
			
			send(tcp_client_socket, &msg_wifi_product, sizeof(msg_wifi_product), 0);
		
		} else {
			printf("socket_cb: recv error!\r\n");
			close(tcp_server_socket);
			tcp_server_socket = -1;
		}
	}

	break;

	default:
		break;
	}
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_REQ_DHCP_CONF](@ref M2M_WIFI_REQ_DHCP_CONF)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED: {
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			wifi_connected = 1;
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			wifi_connected = 0;
			printf("Station disconnected\r\n");
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF: {
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		wifi_connected = 1; 
		printf("Station connected\r\n");
		printf("Station IP is %u.%u.%u.%u\r\n",pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]); //pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		
		break;
	}

	default: {
		break;
	}
	}
}




bool mqtt_client_connect(){
	bool result;
	MQTT_ClientInitialise();
	mqttConnectPacket adafruitConnectPacket;
	
	memset(&adafruitConnectPacket, 0, sizeof(mqttConnectPacket));
	
	adafruitConnectPacket.connectVariableHeader.connectFlagsByte.All = 0x02;
	adafruitConnectPacket.connectVariableHeader.keepAliveTimer       = 10;
	adafruitConnectPacket.clientID                                   = (uint8_t *)clientID;
	adafruitConnectPacket.password                                   = (uint8_t *)Password;
	adafruitConnectPacket.passwordLength                             = strlen(Password);
	adafruitConnectPacket.username                                   = (uint8_t *) Username;
	adafruitConnectPacket.usernameLength                             = strlen(Username);
	
	
	
	
	result=MQTT_CreateConnectPacket(&adafruitConnectPacket);
	
	return result;
	
}




int main(void){
	
	tstrWifiInitParam param;
	tstrM2MAPConfig   strM2MAPConfig;
	int8_t            ret = 0;
	struct sockaddr_in addr;
	
	// Redirect stream to Tera Term
	stdout = &mystdout;
// 	/* Initializes MCU, drivers and middleware */
	
	atmel_start_init();
 	motor_dir_port_init();
 	LEFT_WHEEL_TCB3_PWM_init();
	RIGHT_WHEEL_TCB1_PWM_init();
 	
	sei();
 
 	/* Initialize BSP */
 	nm_bsp_init();
 
 
 	/* Initialize socket address structure */
 	addr.sin_family = AF_INET;
 	addr.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr.sin_addr.s_addr = _htonl(MAIN_WIFI_M2M_SERVER_IP);




	/* Initialize Wi-Fi parameters structure. */
 	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
 
 	/* Initialize Wi-Fi driver with data and status callbacks. */
 	param.pfAppWifiCb = wifi_cb;
 	wifi_init(&param);



	/* Initialize socket module */
	socketInit();
	registerSocketCallback(socket_cb, NULL);

/* Connect to router. */
	m2m_wifi_connect(	(char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

//PORTB.DIR |= PIN5_bm;





	
while (1) {
	
	
		//PORTB.OUT ^= PIN5_bm;
	//_delay_ms(1000);	
	/* Handle pending events from network controller. */
	
	
	while (m2m_wifi_handle_events(NULL) == M2M_SUCCESS) {
		
		if (wifi_connected == M2M_WIFI_CONNECTED) {
			
			if (tcp_server_socket < 0) {
				/* Open TCP server socket */
				if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
					printf("main: failed to create TCP server socket error!\r\n");
					
					continue;
				}

				/* Bind service*/
				bind(tcp_server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
				
				
			}
		}



	}

}
}







void received_from_adafruit(uint8_t *topic, uint8_t *payload){
	
	
}