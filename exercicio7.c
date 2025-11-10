#include <reg51.h>

#define FrClk 12000000
#define FreqTimer0_emHz 100
#define VALOR_TH0 ((65536 - (FrClk / (12 * FreqTimer0_emHz))) >>8)
#define VALOR_TL0 ((65536 - (FrClk / (12 * FreqTimer0_emHz))) & 0xFF)


const int tempo = 299;
int contador;
unsigned char isZero;

void timer0_inicializa() 
  {
  TR0 = 0;              // Desliga Timer0
  TMOD = (TMOD & 0xF0) | 0x01;  // Timer 0 programado como timer de 16 bits
  TH0 = VALOR_TH0;  // Programa contagem do Timer0
  TL0 = VALOR_TL0;
  ET0 = 1;      // Habilita interrupcao do timer 0
  TR0 = 1;      // Habilita contagem do timer 0
  }

void timer0_int (void) interrupt 1 using 2
  {
  TR0 = 0;              // Desliga Timer0
  TL0 += VALOR_TL0;
  TH0 += VALOR_TH0 + (unsigned char) CY;   // Programa contagem do Timer0
  TR0 = 1;      // Habilita contagem do timer 0

  if (contador != 0)
     contador--;
  }
	
unsigned char comparison(){
	EA = 0;
	if (contador == 0) isZero = 1;
	else isZero = 0;
	EA = 1;
	return isZero;
}

void main()
	{
  timer0_inicializa();
	EA = 1; //Habilita o tratamento de interrupções

	while (1)
      {
      contador = tempo;
				
      while(!comparison()); 
			
      if (contador == 0) // Testa se contador é igual a zero.
         P0 = 0;
      else
         P0 = 0xFF;	// ESTA INSTRUÇÃO NUNCA DEVERIA SER EXECUTADA!
      }
	}
	

