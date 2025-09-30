#include <REG51F.H>

#define FrClk 12000000
#define FreqTimer0_emHz 100
#define CORRECAO 9
#define TH0_Inicial ((65536-(FrClk/(12*FreqTimer0_emHz))+CORRECAO)>>8)
#define TL0_Inicial ((65536-(FrClk/(12*FreqTimer0_emHz))+CORRECAO)&0xFF)

sbit index_zero = P2^0; // Referência ao bit na posição 0 da porta 2 (P2) 
sbit index_one = P2^1;  // Referência ao bit na posição 1 da porta 2 (P2)

void timer0_inicializa();
void timer0_int();	
void functionA();
void functionB();

int timer_count = 0; // Contador de tempo (ou seja, do número de interrupções) da função da máquina de estados A
int timer_count2 = 0; // Contador de tempo (ou seja, do número de interrupções) da função da máquina de estados B
unsigned char aux_p1 = 0xFF;

void main(){
	timer0_inicializa();
	EA=1; // Habilita o tratamento de interrupções
	while(1){
		functionA();
		functionB();
	}
} 

// Função que inicializa o registrador de modo (TMOD) e de controle (TCON) no modo timer, com valores esperados
void timer0_inicializa(){
	TR0 = 0; // Desliga o Timer 0
	TMOD = (TMOD & 0xF0)|0x01; // Timer 0 programado no Modo 1
	TH0 = TH0_Inicial;
	TL0 = TL0_Inicial;
	ET0 = 1; // Habilita interrupcao do timer 0
	TR0 = 1; // Habilita a execução do timer 0	 
}

// Função que implementa o tratamento de interrupções do timer 0 (usando o banco de registradores 2)
void timer0_int() interrupt 1 using 2{
	TR0 = 0; // Desliga Timer0
	TL0 += TL0_Inicial; // Faz recarga da contagem do Timer0
	TH0 += TH0_Inicial + (unsigned char) CY;
	TR0 = 1; // Habilita contagem do timer 0
	timer_count ++;
	timer_count2 ++;
}

//Função que implementa a máquina de estados relacionada à substituição dos 4 MSB de P1
void functionA(){
	
	static char state = 0; //Variável de estado
	
	//Switch responsável pelas transições dos estados
		switch (state){
			//Estado inicial: bit 1 de P2 em 1
			case 0: if (~index_one){
				state = 1;
			}
				break;
			//Estado : bit 1 de P2 em 0
			case 1: if (index_one){
				//Zera o contador dessa função antes de entrar no estado do temporizador
				timer_count = 0;
				// Mantém os 4 LSB de aux_p1 e substitui os 4 MSB pelos 4 MSB da porta 0 (P0)
				aux_p1 = (aux_p1 & 0x0f) | (P0 & 0xf0);
				state = 2;
			} 
				break;
			// Estado: Temporizador de 1s
			case 2: if (timer_count>=100){
				// Zera os 4 MSB de aux_p1 e mantém os 4 LSB
				aux_p1 = aux_p1 & 0x0f;
				state = 0;
			}
				break;
		}
		//Atualização da porta P1
		P1 = aux_p1;
		
}	

//Função que implementa a máquina de estados relacionada à substituição dos 4 LSB de P1
void functionB(){
	
	static char state2 = 0; //Variável de estado
	
		//Switch responsável pelas transições dos estados
		switch (state2){
			//Estado inicial: bit 0 de P2 em 1
			case 0: if (~index_zero){
				state2 = 1;
			}
				break;
			//Estado : bit 0 de P2 em 0
			case 1: if (index_zero){
				//Zera o contador dessa função antes de entrar no estado do temporizador
				timer_count2 = 0;
				// Mantém os 4 MSB de aux_p1 e substitui os 4 LSB pelos 4 LSB de P0
			  aux_p1 = (aux_p1 & 0xf0) | (P0 & 0x0f);
				state2 = 2;
			} 
				break;
			// Estado: Temporizador de 1s
			case 2: if (timer_count2>=100){
				// Zera os 4 LSB de aux_p1 e mantém os 4 MSB
			  aux_p1 = aux_p1 & 0xf0;
				state2 = 0;
			}
				break;
		}
				//Atualização da porta P1
		P1 = aux_p1;
		
}	
	
