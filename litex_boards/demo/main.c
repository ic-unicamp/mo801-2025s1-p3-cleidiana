// This file is Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
// License: BSD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <libbase/uart.h>
#include <libbase/console.h>
#include <generated/csr.h>

#define CLOCK_FREQUENCY 27000000  // 27 MHz
#define QUANT_REP       50000

volatile uint32_t botao_user = 0;

static void teste_exec(void)
{
	uint32_t i;
    uint32_t tempo_inicio, tempo_fim;
    uint64_t ciclos;
    int segundos;
    leds_out_write(0x00);
    puts("\nAperte o botão para iniciar o teste\n");
    botao_user = gpio_in_read();
    while(botao_user){
        botao_user = gpio_in_read();
    }
    leds_out_write(0x3F);
    puts("\nInicio teste\n");

    timer0_reload_write(0xFFFFFFFF);
    timer0_load_write(0xFFFFFFFF);
    timer0_en_write(1);

    timer0_update_value_write(1);
    tempo_inicio = timer0_value_read();

    for (i = 0; i < QUANT_REP; i++) {
        printf("%ld ", i); 
    }
    timer0_update_value_write(1);
    tempo_fim = timer0_value_read();

    ciclos = (uint64_t)(tempo_inicio - tempo_fim);  
    segundos = (ciclos / CLOCK_FREQUENCY);

    printf("\nTempo decorrido: %d segundos\n", segundos);
    
    leds_out_write(0x00);
    puts("\nFim teste\n");
}

int main(void)
{
	
#ifdef CONFIG_CPU_HAS_INTERRUPT
	irq_setmask(0);
	irq_setie(1);
#endif
	uart_init();
	teste_exec();

	return 0;
}
