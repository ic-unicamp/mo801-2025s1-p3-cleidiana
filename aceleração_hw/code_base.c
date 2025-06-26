#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <libbase/uart.h>
#include <libbase/console.h>
#include <generated/csr.h>

#define CLOCK_FREQUENCY 27000000  // 27 MHz

volatile uint32_t botao_user = 0;
#define SIZE 20

int A[SIZE][SIZE];
int B[SIZE][SIZE];
int C[SIZE][SIZE];

int main(void) {

#ifdef CONFIG_CPU_HAS_INTERRUPT
	irq_setmask(0);
	irq_setie(1);
#endif
    uart_init();
    uint32_t tempo_inicio, tempo_fim;
    uint64_t ciclos;
    uint32_t mili_segundos;
    leds_out_write(0x00);
    puts("\nAperte o botão para iniciar o teste\n");
    botao_user = gpio_in_read();

    //inicializa as matrizes
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = i;
            B[i][j] = j;
            C[i][j] = 0;
        }
    }

    while(botao_user){
        botao_user = gpio_in_read();
    }

    leds_out_write(0x3F);
    printf("Iniciando multiplicação de matrizes...\n");

    timer0_reload_write(0xFFFFFFFF);
    timer0_load_write(0xFFFFFFFF);
    timer0_en_write(1);

    timer0_update_value_write(1);
    tempo_inicio = timer0_value_read();

    //multiplicação matrizes A * B
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            for (int k = 0; k < SIZE; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    timer0_update_value_write(1);
    tempo_fim = timer0_value_read();

    ciclos = (uint64_t)(tempo_inicio - tempo_fim);  
    mili_segundos = (ciclos / (CLOCK_FREQUENCY/1000));

    printf("\nTempo decorrido: %lld ciclos miliseg %ld\n", ciclos, mili_segundos);
    
    leds_out_write(0x00);
    puts("\nFim teste\n");

    printf("Resultado Matriz C:\n");
    int PRINT_C = 0;
    if(PRINT_C){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                printf("%3d ", C[i][j]);
            }
            printf("\n");
        }
    }
    return 0;
}

