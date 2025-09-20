#include <REG51F.H>

sbit index_zero = P2^0; // Referência ao bit na posição 0 da porta 2 (P2) 
sbit index_one = P2^1;  // Referência ao bit na posição 1 da porta 2 (P2)

void main(){
	
	//Definindo variável auxiliar com o valor inicial esperado da porta 1 (P1), servindo para manipular o valor atual de P1 sem usá-la como entrada
	unsigned char aux_p1;
	aux_p1 = 0xff;
	
	while(1){
		
		//Caso onde o valor do bit na posição 1 de P2 é 1
		if(index_one){
			// Mantém os 4 LSB de aux_p1 e substitui os 4 MSB pelos 4 MSB da porta 0 (P0)
			aux_p1 = (aux_p1 & 0x0f) | (P0 & 0xf0);
		} else {
			// Zera os 4 MSB de aux_p1 e mantém os 4 LSB
			aux_p1 = aux_p1 & 0x0f;
		}
		
		P1 = aux_p1; // atualiza o valor de P1
		
		//Caso onde o valor do bit na posição 0 de P2 é 1
		if(index_zero){
			// Mantém os 4 MSB de aux_p1 e substitui os 4 LSB pelos 4 LSB de P0
			aux_p1 = (aux_p1 & 0xf0) | (P0 & 0x0f);
		} else {
			// Zera os 4 LSB de aux_p1 e mantém os 4 MSB
			aux_p1 = aux_p1 & 0xf0;
		}
		
		P1 = aux_p1; // atualiza o valor de P1
	}
}	

