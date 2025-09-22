#include <REG51F.H>

sbit index_zero = P2^0; // Referência ao bit na posição 0 da porta 2 (P2) 
sbit index_one = P2^1;  // Referência ao bit na posição 1 da porta 2 (P2)

//Definindo variável auxiliar com o valor inicial esperado da porta 1 (P1), servindo para manipular o valor atual de P1 sem usá-la como entrada
unsigned char aux_p1;

//Assinatura das funções que implementam as máquinas de estado
void functionA();
void functionB();

void main(){
	
	//Definindo o valor inicial da variável como o mesmo da P1
	aux_p1 = 0xff;
	
	while(1){
		functionA();
		functionB();
	}
}	

//Função que implementa a máquina de estados relacionada à substituição dos 4 MSB de P1
void functionA(){
	
	static char state = 0; //Variável de estado
	static long int t = 0; //Variável de temporizador
	
	//Switch responsável pelas transições dos estados
		switch (state){
			//Estado inicial: bit 1 de P2 em 1
			case 0: if (!index_one){
				state = 1;
			}
				break;
			//Estado : bit 1 de P2 em 0
			case 1: if (index_one){
				// Mantém os 4 LSB de aux_p1 e substitui os 4 MSB pelos 4 MSB da porta 0 (P0)
				aux_p1 = (aux_p1 & 0x0f) | (P0 & 0xf0);
				state = 2;
			} 
				break;
			//Estado: Temporizador. Obs: Número máximo de t obtido a partir de teste no ambiente de simulação
			case 2: if (t<13636){
				t++;
			} else {
				t = 0;
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
	static long int t2 = 0; //Variável de temporizador
	
		//Switch responsável pelas transições dos estados
		switch (state2){
			//Estado inicial: bit 0 de P2 em 1
			case 0: if (!index_zero){
				state2 = 1;
			}
				break;
			//Estado : bit 0 de P2 em 0
			case 1: if (index_zero){
				// Mantém os 4 MSB de aux_p1 e substitui os 4 LSB pelos 4 LSB de P0
			  aux_p1 = (aux_p1 & 0xf0) | (P0 & 0x0f);
				state2 = 2;
			} 
				break;
			//Estado: Temporizador. Obs: Número máximo de t obtido a partir de teste no ambiente de simulação
			case 2: if (t2<13636){
				t2++;
			} else {
				t2 = 0;
				// Zera os 4 LSB de aux_p1 e mantém os 4 MSB
			  aux_p1 = aux_p1 & 0xf0;
				state2 = 0;
			}
				break;
		}
				//Atualização da porta P1
		P1 = aux_p1;
		
}	
