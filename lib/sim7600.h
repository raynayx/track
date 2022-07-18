/***
 * Library for SIM7600E modem
 * using UART on the RP2040 mcu platform
 * 
 * 
 ***/

#ifndef _SIM7600_H_
#define _SIM7600_H_


#include "hardware/uart.h"

/********LITERAL STRINGS************/
#define ok "\r\nOK\r\n"



/*********************************/




typedef struct sLocationTime
{
	float latitude;
	float longitude;
	float altitude;
	float speed_kph;
	float heading;

	//time
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} sLocationTime;


typedef struct SIM7600
{
	uart_inst_t *huart;
	uint8_t pwr_pin; 	//used to reset the module
	__attribute__((aligned(128)))
	char resp_buffer[128];
	sLocationTime location;	//gps location and time
	uint8_t IMEI;
} SIM7600;

bool sim7600_init(SIM7600 *m,uart_inst_t *u,uint8_t pwr_pin);

bool sim7600_setBaudrate(SIM7600 *m,uint16_t baud);



/**
 * SIM CARD 
 ***/
uint8_t sim7600_unlockSIM(char *pin);
uint8_t sim7600_getIMEI(char *imei);


/**
 * NETWORK
 **/

uint8_t sim7600_getNetworkStatus(void);
uint8_t sim7600_getRSSI(void);


/**
 * GPS
 **/

bool sim7600_enableGPS(bool onOff);
int8_t sim7600_gpsStatus();
sLocationTime sim7600_getGPS();


/**
 * GPRS
 **/
bool sim7600_enableGPRS(bool onOff);
uint8_t sim7600_gprsState(void);
void setGPRSNetworkSettings(char *apn,char * uname,char *password);


/**
 * TCP
 **/
bool sim7600_tcpConnect(char *server,uint16_t port);
bool sim7600_tcpClose(void);
bool sim7600_getTcpStatus(void);
bool sim7600_tcpSend(char *data, uint8_t len);
bool sim7600_tcpAvailable(void);
uint16_t simi7600_tcpRead(uint8_t *buffer, uint8_t len);


/**
 * HTTP low-level
 **/
bool sim7600_httpInit();
bool sim7600_httpTerm();
bool sim7600_httpParam_start(char *param,bool quoted);
bool sim7600_httpparam_end(bool quoted);
bool sim7600_httpParam(char * param, int32_t value);
bool sim7600_httpData(uint32_t size, uint32_t maxTime);
bool sim7600_httpAction(uint8_t method,uint16_t *status, uint16_t *datalen, int32_t timeout);
bool sim7600_httpReadAll(uint16_t *datalen);
bool sim7600_httpSsl(bool onOff);

/**
 * HTTP high-level
 **/
bool sim7600_httpGetStart(char *url,uint16_t *status,uint16_t *datalen);
bool sim7600_httpGetEnd(void);
bool sim7600_httpPostStart(char *url,char * contentType, const uint8_t *postdata, uint16_t postdatalen,uint16_t *status,uint16_t *datalen);
bool sim7600_httpPostEnd(void);
void sim7600_httpSetUserAgent(char *userAgent);
void sim7600_httpsSetRedirect(bool onOff);

/**
 * HTTP helpers
 **/
bool sim7600_httpSetup(char *url);


/**
 * Library Helper
 **/
bool sim7600_expectReply(char *reply, uint16_t timeout);


/**
 * Library Low-level
 **/
uint8_t sim7600_readline(uint16_t timeout, bool multiline);
uint8_t sim7600_getReply(char *send,uint16_t timeout);
bool sim7600_sendCheckReply(SIM7600 *m,char *send,char *reply,uint16_t timeout);
bool sim7600_parseReply(char *toReply,uint16_t *parsedReply, char separator,uint8_t index);
bool sim7600_sendParseReply(char *toSend,char * toReply, uint16_t *parsedReply,char separator,uint8_t index);


#endif	//_SIM7600_H_
