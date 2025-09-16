#include <REG51F.H>

sbit index_zero = P2^0;
sbit index_one = P2^1;

void main(){
	
	//Definindo vari�vel auxiliar com o valor inicial esperado da porta 1
	unsigned char aux_p1;
	aux_p1 = 0xff;
	
	while(1){
		
		//Caso onde o valor da porta 2 na posi��o 1 � 1
		if(index_one){
			// Mant�m os LSB de aux_p1 e substitui os MSB pelos MSB de P0
			aux_p1 = (aux_p1 & 0x0f) | (P0 & 0xf0);
		} else {
			// Zera os MSB de aux_p1
			aux_p1 = aux_p1 & 0x0f;
		}
		
		P1 = aux_p1; // atualiza o valor de P1
		
		//Caso onde o valor da porta 2 na posi��o 0 � 1
		if(index_zero){
			// Mant�m os MSB de aux_p1 e substitui os LSB pelos LSB de P0
			aux_p1 = (aux_p1 & 0xf0) | (P0 & 0x0f);
		} else {
			// Zera os LSB de aux_p1
			aux_p1 = aux_p1 & 0xf0;
		}
		
		P1 = aux_p1; // atualiza o valor de P1
	}
}	
