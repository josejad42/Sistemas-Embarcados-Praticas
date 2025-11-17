#include <REG517A.H>
#define EOS '$'

char Rx_buffer[16]; //Buffer de recepção

/** Apontadores para inserção e remoçãoo de caracteres nos buffers **/
unsigned char Rx_in = 0;
unsigned char Rx_out = 0;

unsigned char RecebeuString = 0; //Variável que indica que existe uma string recebida nao lida no buffer

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

//Função de inicialização do serial
void serial_inicialize(){
	BD = 1; // Habilita baudrate generator
	PCON |= 0x80; //Inicializa SMOD como 1
	
	//Seta o serial como modo 1
	SM0 = 0;
	SM1 = 1;
	
	SM20 = 0;//Como estamos no modo 1, inicia como 0 para o stop bit funcionar
	REN0 = 1;//Habilita as recepções
}

//Interrupção do serial
void int_serial() interrupt 4 using 2{
	
	if(RI0){
		RI0 = 0;
		if(((Rx_in + 1) % 16) != Rx_out){ // Verifica se o buffer não está cheio
			Rx_buffer[Rx_in] = S0BUF; 
			if(Rx_buffer[Rx_in] == EOS){ // Verifica se é o fim da string
				RecebeuString++; //Incrementa string recebida
			}
			Rx_in++; // Atualiza indice do proximo caracter a ser lido
			Rx_in = Rx_in & 0x0F; // Garante que Rx_in é um indice válido do buffer circular
		}
	}
}


void main(){
	
	//Definição de um duty cycle padrão (100%)
	unsigned char duty_cycle = 9;
	//Definição da lookup table
	int lkup_table [10] = {0xFFFF, 0xFFE2, 0xFFC6, 0xFFA9, 0xFF8D, 0xFF70, 0xFF54, 0xFF37, 0xFF1C, 0xFF00};
	//String de recepção da entrada serial
  char s[16];
	//Configuração do Compare timer no modo 0
	CTCON &= 0xF8;
	CTRELH = 0xFF;
	CTRELL = 0x00;
	CMSEL |= 0xFF;
	CMEN |= 0xFF;
	
	serial_inicialize(); //Inicializa a serial
	EAL = 1; //Habilita interrupcoes 
	ES0 = 1; //Habilita interrupcoes do serial

	
	while(1){
		if(RecebeuString){
			ReceiveString(s); //Recepção da entrada serial
			duty_cycle = s[0] - 48; //Tradução da entrada de caractere para inteiro
			//Setando o valor dos registradores de comparação de acordo com a entrada serial
			CM0 = lkup_table[duty_cycle];
			CM1 = lkup_table[duty_cycle];
			CM2 = lkup_table[duty_cycle];
			CM3 = lkup_table[duty_cycle];
			CM4 = lkup_table[duty_cycle];
			CM5 = lkup_table[duty_cycle];
			CM6 = lkup_table[duty_cycle];
			CM7 = lkup_table[duty_cycle];
		}
	}
	
}
