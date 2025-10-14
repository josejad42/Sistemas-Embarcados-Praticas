#include <REG51F.H>

#define TH1_value 204 //Definindo o valor de TH1 baseado na equação TH1 = 256 - ((2^SMOD*10^6) / (32*1200))
#define EOS '$'

char Tx_buffer[16]; //Buffer de transmissão
char Rx_buffer[16]; //Buffer de recepção

/** Apontadores para inserção e remoção de caracteres nos buffers **/
unsigned char Tx_in = 0;
unsigned char Tx_out = 0;

unsigned char Rx_in = 0;
unsigned char Rx_out = 0;

unsigned char RecebeuString = 0; //Variável que indica que há uma string recebida não lida no buffer
unsigned char txOcupado = 0; //Variável de controle para saber se o transmissor está livre 

//Função que retorna se o buffer está vazio ou não
char RxBufferVazio(){
	if(Rx_in == Rx_out){
		return 1;
	}else{
		return 0;
	}
}

//Função que retorna um caracter de RxBuffer
char ReceiveChar(){
	char rcvd = Rx_buffer[Rx_out]; 
	Rx_out++; //Atualiza o indice do proximo caractere a ser lido de RxBuffer
	Rx_out = Rx_out & 0x0F; //Garante que Rx_out é um indice válido do buffer circular
	return rcvd;
}

//Função que copia uma string de RxBuffer para o local apontado por '*s'
void ReceiveString(char *s){
	char rcvd = ReceiveChar();
	unsigned char s_index = 0; 
	
	while(rcvd != EOS){ // Lê caracteres até chegar no fim da string
		s[s_index] = rcvd; // adiciona caractere lido
		s_index++;
		rcvd = ReceiveChar();
	}
	s[s_index] = rcvd; //Adiciona caractere de fim da string
	
	RecebeuString--;
}

//Função que recebe caractere e insere em TxBuffer
void SendChar(char c){
	Tx_buffer[Tx_in] = c;
	Tx_in++;
	Tx_in = Tx_in & 0x0F; // Garante que Tx_in é um indice válido do buffer circular
}

//Função que insere a string apontada por '*s' em TxBuffer
void SendString(char *s){
	unsigned char s_index = 0;
	while(s[s_index] != EOS){ // Adiciona caracteres ao buffer enquanto a string não chega ao fim
		SendChar(s[s_index]);
		s_index++;
	}
	SendChar(s[s_index]);
	if(!txOcupado){ 
		txOcupado = 1; // Indica que o transmissor está ocupado
		TI = 1; // Gera um pedido de interrupção
	}
	txOcupado = 0; // Indica que o transmissor está livre
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
		if(Tx_out != Tx_in){ //Verifica se o buffer não está vazio
			trsmt = Tx_buffer[Tx_out];
			Tx_out++; //Atualiza indice do próximo caractere a ser transmitido
			Tx_out = Tx_out & 0x0F; // Garante que Tx_out é um indice válido do buffer circular
			SBUF = trsmt; //transmite caracter
		}
	}
	if(RI){
		RI = 0;
		if(((Rx_in + 1) % 16) != Rx_out){ // Verifica se o buffer não está cheio
			Rx_buffer[Rx_in] = SBUF; 
			if(Rx_buffer[Rx_in] == EOS){ // Verifica se é o fim da string
				RecebeuString++; //Incrementa string recebida
			}
			Rx_in++; // Atualiza indice do proximo caracter a ser lido
			Rx_in = Rx_in & 0x0F; // Garante que Rx_in é um indice válido do buffer circular
		}
	}
}


void main(){
	char s[16];
	
	serial_inicialize();
	timer_inicialize();
	EA = 1; //Habilita interrupções 
	ES = 1; //Habilita interrupções do serial
	
	while(1){
		if(RecebeuString){
			ReceiveString(s);
			SendString(s);
		}
	}
	
}
