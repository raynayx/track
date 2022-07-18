#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "sim7600.h"

#define DEBUG


int data_chan;


static void dma_handler()
{
	dma_hw->ints0 = 1u <<data_chan;
}

void dma_setup(SIM7600 *m)
{

	data_chan = dma_claim_unused_channel(true);

	dma_channel_config c_config = dma_channel_get_default_config(data_chan);
	channel_config_set_transfer_data_size(&c_config,DMA_SIZE_8);
	channel_config_set_read_increment(&c_config,false);
	channel_config_set_write_increment(&c_config,true);
	channel_config_set_dreq(&c_config,DREQ_UART0_RX);
	channel_config_set_ring(&c_config,true,7); //1 << 3 wraps on write pointer

	//chain control channel into data channel
	dma_channel_configure
	(
		data_chan,
		&c_config,
		m->resp_buffer, //initial write address
		&uart0_hw->dr,	//initial read address
		256,
		false
	);

	dma_channel_set_irq0_enabled(data_chan,true);
	irq_set_exclusive_handler(DMA_IRQ_0,dma_handler);
	irq_set_enabled(DMA_IRQ_0,true);

	dma_start_channel_mask(1u << data_chan);
}

/**
 * @brief Send command to SIM7600E module
 * @param u UART peripheral instance
 * @param cmd Command text must end with `\r` according to SIM7600E AT command manual
 **/
static inline void send_cmd(uart_inst_t *u,char *cmd)
{
	uart_puts(u,cmd);
	return;
}

void get_reply(uart_inst_t *u,char *reply_str)
{
	while (uart_is_readable(u))
	{
		*reply_str = uart_getc(u);
		reply_str++;
	}
	
}



/**
 * @brief Initialize SIM7600
 * 
 * @param m SIM7600 struct
 * @param u UART handler
 * @param pwr_pin GPIO pin to reset module
 * @return true on sucessful initialization; false otherwise
 **/
bool sim7600_init(SIM7600 *m,uart_inst_t *u,uint8_t pwr_pin)
{
	
	m->huart = u;
	m->pwr_pin = pwr_pin;

	dma_setup(m);
	gpio_put(m->pwr_pin,true);
	// sleep_ms(10);
	gpio_put(m->pwr_pin,false);
	sleep_ms(500);
	gpio_put(m->pwr_pin,true);
	sleep_ms(10000);
	#ifdef DEBUG
		printf("Attempting initialization with ATs ...\n");
	#endif
	send_cmd(m->huart,"AT\r");

	// sim7600_sendCheckReply(m,"AT\r",ok,10000); //check first contact
	#ifdef DEBUG
		printf("%s\n",m->resp_buffer);
	#endif

	send_cmd(m->huart,"ATE0\r\n");
	// sim7600_sendCheckReply(m,"ATE0\r",ok,10000); // turn off echo

	#ifdef DEBUG
		printf("%s\n",m->resp_buffer);
	#endif
	send_cmd(m->huart,"ATE0\r");

	#ifdef DEBUG
		printf("%s\n",m->resp_buffer);
	#endif

	// if(!sim7600_sendCheckReply(m,"ATE0\r",ok,10000))
	// {
	// 	return false;
	// }

	//turn on hanguptitude
	send_cmd(m->huart,"AT+CVHU=0\r");

	// sim7600_sendCheckReply(m,"AT+CVHU=0\r", ok,10000);
	sleep_ms(10);

	#ifdef DEBUG
		printf("%s\n",m->resp_buffer);
	#endif

	send_cmd(m->huart,"ATI\r");

	//get response back here in sequential code.
	//However using interrupts or DMA, resp_buffer should have
	// the reply after the modem replies
	// get_reply(m->huart,m->resp_buffer);

	#ifdef DEBUG
 		printf("%s\n",m->resp_buffer);
	#endif

	send_cmd(m->huart,"AT+CGSN\r");		//get the device IMEI number
	#ifdef DEBUG
 		printf("%s\n",m->resp_buffer);
	#endif
	return true;
}




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

/**
 * @brief send data and check if response matches an expected value
 * @param m SIM7600 struct
 * @param send data to send
 * @param reply response from modem
 * @param timeout maximum time function could be blocking for
 */
bool sim7600_sendCheckReply(SIM7600 *m,char *send,char *reply,uint16_t timeout)
{
	send_cmd(m->huart,send);
	sleep_ms(30);
	get_reply(m->huart,m->resp_buffer);
	return (strcmp(reply,m->resp_buffer) == 0);
}


bool sim7600_parseReply(char *toReply,uint16_t *parsedReply, char separator,uint8_t index);
bool sim7600_sendParseReply(char *toSend,char * toReply, uint16_t *parsedReply,char separator,uint8_t index);
