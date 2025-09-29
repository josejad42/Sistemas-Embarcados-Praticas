#include <REG51F.H>

#define FrClk 12000000
#define FreqTimer0_emHz 100
#define CORRECAO 10
#define TH0_Inicial ((65536-(FrClk/(12*FreqTimer0_emHz))+CORRECAO)>>8)
#define TL0_Inicial ((65536-(FrClk/(12*FreqTimer0_emHz))+CORRECAO)&0xFF)

sbit index_zero = P2^0; // Refer�ncia ao bit na posi��o 0 da porta 2 (P2) 
sbit index_one = P2^1;  // Refer�ncia ao bit na posi��o 1 da porta 2 (P2)

void timer0_inicializa();
void timer0_int();	
void functionA();
void functionB();

int timer_count = 0;
int timer_count2 = 0;
unsigned char aux_p1 = 0xFF;

void main(){
	timer0_inicializa();
	EA=1; // Habilita o tratamento de interrup��es
	while(1){
		functionA();
		functionB();
	}
} 

void timer0_inicializa(){
	TR0 = 0; // Desliga o Timer 0
	TMOD = (TMOD & 0xF0)|0x01; // Timer 0 programado no Modo 1
	TH0 = TH0_Inicial;
	TL0 = TL0_Inicial;
	ET0 = 1; // Habilita interrupcao do timer 0
	TR0 = 1; // Habilita a execu��o do timer 0	 
}

void timer0_int() interrupt 1 using 2{
	TR0 = 0; // Desliga Timer0
	TL0 += TL0_Inicial; // Faz recarga da contagem do Timer0
	TH0 += TH0_Inicial + (unsigned char) CY;
	TR0 = 1; // Habilita contagem do timer 0
	timer_count ++;
	timer_count2 ++;
}

//Fun��o que implementa a m�quina de estados relacionada � substitui��o dos 4 MSB de P1
void functionA(){
	
	static char state = 0; //Vari�vel de estado
	
	//Switch respons�vel pelas transi��es dos estados
		switch (state){
			//Estado inicial: bit 1 de P2 em 1
			case 0: if (~index_one){
				state = 1;
			}
				break;
			//Estado : bit 1 de P2 em 0
			case 1: if (index_one){
				// Mant�m os 4 LSB de aux_p1 e substitui os 4 MSB pelos 4 MSB da porta 0 (P0)
				aux_p1 = (aux_p1 & 0x0f) | (P0 & 0xf0);
				state = 2;
				timer_count = 0;
			} 
				break;
			//Estado: Temporizador. Obs: N�mero m�ximo de t obtido a partir de teste no ambiente de simula��o
			case 2: if (timer_count>=100){
				// Zera os 4 MSB de aux_p1 e mant�m os 4 LSB
				aux_p1 = aux_p1 & 0x0f;
				state = 0;
			}
				break;
		}
		//Atualiza��o da porta P1
		P1 = aux_p1;
		
}	

//Fun��o que implementa a m�quina de estados relacionada � substitui��o dos 4 LSB de P1
void functionB(){
	
	static char state2 = 0; //Vari�vel de estado
	
		//Switch respons�vel pelas transi��es dos estados
		switch (state2){
			//Estado inicial: bit 0 de P2 em 1
			case 0: if (~index_zero){
				state2 = 1;
			}
				break;
			//Estado : bit 0 de P2 em 0
			case 1: if (index_zero){
				// Mant�m os 4 MSB de aux_p1 e substitui os 4 LSB pelos 4 LSB de P0
			  aux_p1 = (aux_p1 & 0xf0) | (P0 & 0x0f);
				state2 = 2;
				timer_count2 = 0;
			} 
				break;
			//Estado: Temporizador. Obs: N�mero m�ximo de t obtido a partir de teste no ambiente de simula��o
			case 2: if (timer_count2>=100){
				// Zera os 4 LSB de aux_p1 e mant�m os 4 MSB
			  aux_p1 = aux_p1 & 0xf0;
				state2 = 0;
			}
				break;
		}
				//Atualiza��o da porta P1
		P1 = aux_p1;
		
}	
	