#include <REG517A.H>

long int t = 0;
unsigned char auxP4 = 0xFF;

void main(){
	
	while(1){
		if(t<18000)t++;
		else{
			auxP4 = ~auxP4;
			P4 = auxP4;
			t = 0;
		}
	}
}	