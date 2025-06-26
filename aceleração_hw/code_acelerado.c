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
#define SUB_SIZE 2


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


    //percorre todos os blocos SUB_SIZExSUB_SIZE da matriz C
    for (int bi = 0; bi < SIZE; bi += SUB_SIZE) {
        for (int bj = 0; bj < SIZE; bj += SUB_SIZE) {
            int Cblk[SUB_SIZE][SUB_SIZE] = {0};  

            //soma dos produtos Ablk * Bblk
            for (int bk = 0; bk < SIZE; bk += SUB_SIZE) {
                int Ablk[SUB_SIZE][SUB_SIZE];
                int Bblk[SUB_SIZE][SUB_SIZE];

                //montagem da submatriz A
                for (int i = 0; i < SUB_SIZE; i++) {
                    for (int j = 0; j < SUB_SIZE; j++) {
                        Ablk[i][j] = A[bi + i][bk + j];
                    }
                }

                //montagem da submatriz B
                for (int i = 0; i < SUB_SIZE; i++) {
                    for (int j = 0; j < SUB_SIZE; j++) {
                        Bblk[i][j] = B[bk + i][bj + j];
                    }
                }

                //multiplicação submatrizes Ablk * Bblk
                for (int i = 0; i < SUB_SIZE; i++) {
                    for (int j = 0; j < SUB_SIZE; j++) {
                        for (int k = 0; k < SUB_SIZE; k++) {
                            Cblk[i][j] += Ablk[i][k] * Bblk[k][j];
                        }
                    }
                }
            }

            //copia o resultado Cblk para C
            for (int i = 0; i < SUB_SIZE; i++) {
                for (int j = 0; j < SUB_SIZE; j++) {
                    C[bi + i][bj + j] = Cblk[i][j];
                }
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
    int PRINT_C = 1;
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
