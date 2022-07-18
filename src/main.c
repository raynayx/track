


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "sim7600.h"




static void uart_setup(uart_inst_t *u, uint baudrate);

uint8_t LED = 25u;
uint8_t pwr_pin = 16u;

SIM7600 m;
int main()
{
	stdio_init_all();

	
	gpio_init(LED);
	gpio_init(pwr_pin);

	gpio_set_dir(LED,true);
	gpio_set_dir(pwr_pin,true);

	uart_setup(uart0,115200);
	sleep_ms(5000);
	
	sim7600_init(&m,uart0,pwr_pin);

	while(1)
	{
		gpio_put(LED,true);
		sleep_ms(100);
		gpio_put(LED,false);
		sleep_ms(100);

	}

	return 0;
}


static void uart_setup(uart_inst_t *u, uint baudrate)
{
	#ifdef _BOARDS_SEEED_XIAO_RP2040_H
		uart_init(u,115200);  //init UART 0
		gpio_set_function(0,GPIO_FUNC_UART); //set GPIO 0 to UART TX found on D6
		gpio_set_function(1,GPIO_FUNC_UART); //set GPIO 1 to UART RX found on D7
	#else //Pico and Pico compatible
		uart_init(u,baudrate);  //init UART 0
		gpio_set_function(0,GPIO_FUNC_UART); //set GPIO 0 to UART TX
		gpio_set_function(1,GPIO_FUNC_UART); //set GPIO 1 to UART RX
	#endif
	uart_set_translate_crlf(u,false);
}