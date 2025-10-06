#include <REG51F.H>

#define TH1_value 204 //Definindo o valor de TH1 baseado na equação TH1 = 256 - ((2^SMOD*10^6) / (32*1200))

char rcvd = 0; //Variável que armazena o valor recebido

//Função de inicialização do timer
void timer_inicialize(){
	TR1 = 0; //Desliga o timer 1
	TMOD = (TMOD & 0x0F)|0x20; //Timer 1 programado no modo 2
	TH1 = TH1_value; //Inicializa TH1 como o valor calculado anteriormente
	ET1 = 0; // Desliga as interrupções do timer 1
	PCON |= 0x80; //Inicializa SMOD como 1
	TR1 = 1; //Inicia o timer 1
}

//Função de inicialização do serial
void serial_inicialize(){
	//Seta o serial como modo 1
	SM0 = 0;
	SM1 = 1;
	
	SM2 = 0;//Como estamos no modo 1, inicia como 0 para o stop bit funcionar
	REN = 1;//Habilita as recepções
}

//Interrupção do serial
void int_serial() interrupt 4 using 2{
	
	if(TI){
		TI = 0;
	}
	if(RI){
		//Pega o valor recebido, incrementa em 1 e transmite
		RI = 0;
		rcvd = SBUF;
		rcvd++;
		SBUF = rcvd;
	}
}

void main(){
	serial_inicialize();
	timer_inicialize();
	EA = 1; //Habilita interrupções do timer
	ES = 1; //Habilita interrupções do serial
	
	while(1); //Loop infinito
	
}


