#include <REG51F.H>

#define TH1_value 204 //Definindo o valor de TH1 baseado na equação TH1 = 256 - ((2^SMOD*10^6) / (32*1200))
#define EOS '$'

char Tx_buffer[16];
char Rx_buffer[16];

unsigned char Tx_in = 0;
unsigned char Tx_out = 0;

unsigned char Rx_in = 0;
unsigned char Rx_out = 0;

unsigned char RecebeuString = 0; //Variável que armazena o valor recebido
unsigned char txOcupado = 0;

char RxBufferVazio(){
	if(Rx_in == Rx_out){
		return 1;
	}else{
		return 0;
	}
}
char ReceiveChar(){
	char rcvd = Rx_buffer[Rx_out];
	Rx_out++;
	Rx_out = Rx_out & 0x0F;
	return rcvd;
}

void ReceiveString(char *s){
	char rcvd = ReceiveChar();
	unsigned char s_index = 0;
	
	while(rcvd != EOS){
		s[s_index] = rcvd;
		s_index++;
		rcvd = ReceiveChar();
	}
	s[s_index] = rcvd;
	
	RecebeuString--;
}

void SendChar(char c){
	Tx_buffer[Tx_in] = c;
	Tx_in++;
	Tx_in = Tx_in & 0x0F;
}

void SendString(char *s){
	unsigned char s_index = 0;
	while(s[s_index] != EOS){
		SendChar(s[s_index]);
		s_index++;
	}
	SendChar(s[s_index]);
	if(!txOcupado){
		txOcupado = 1;
		TI = 1;
	}
	txOcupado = 0;
}
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
	char trsmt = 0;
	
	if(TI){
		TI = 0;
		if(Tx_out != Tx_in){
			trsmt = Tx_buffer[Tx_out];
			SBUF = trsmt;
			Tx_out++;
			Tx_out = Tx_out & 0x0F;
		}
	}
	if(RI){
		//Pega o valor recebido, incrementa em 1 e transmite
		RI = 0;
		if((Rx_in + 1) % 16 != Rx_out){
			Rx_buffer[Rx_in] = SBUF;
			if(Rx_buffer[Rx_in] == EOS){
				RecebeuString++;
			}
			Rx_in++;
			Rx_in = Rx_in & 0x0F;
		}
	}
}


void main(){
	char s[16];
	
	serial_inicialize();
	timer_inicialize();
	EA = 1; //Habilita interrupções do timer
	ES = 1; //Habilita interrupções do serial
	
	while(1){
		if(RecebeuString){
			ReceiveString(s);
			SendString(s);
		}
	}
	
}