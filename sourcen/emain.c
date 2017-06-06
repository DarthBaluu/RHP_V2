#include "simuc.h"
#include "io_treiber.h"

// Die Verwendung der folgenden Zustandsnamen ist verbindlich
typedef enum{Steht,StarteRunter,StarteRauf,Notaus,FahreRauf_MIT_OF,FahreRauf_OHNE_OF,FahreRunter_MIT_OF,FahreRunter_OHNE_OF} state_t;

// Die folgenden Defines muessen sinnvoll genutzt werden
#define BIT_T2		5
#define BIT_T1		4
#define BIT_ESR		3
#define BIT_ESL		2

#define BIT_M_Re	2
#define BIT_M_Li	1
#define BIT_M_An

long int startpos;
long int endpos;
state_t the_state=Steht;
// Hier die Steuerungsfunktion definieren
// ...

void steuerungsfunktion(int notaus, int t2, int esr, int esl, int* m_re, int* m_li, int* m_an, long int akt_pos, state_t* the_state){

    switch(*the_state){
    case Steht:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if(esr==1&&esl==0){
            *the_state=StarteRauf;
        }
        else if (esr==0&&esl==1){
            *the_state=StarteRunter;
        }
        break;
    case StarteRauf:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if(akt_pos>=endpos){
            *the_state=FahreRauf_OHNE_OF;
        }
        else if(akt_pos<endpos){
            *the_state=FahreRauf_MIT_OF;
        }
        break;
    case StarteRunter:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if(akt_pos>endpos){
            *the_state=FahreRunter_MIT_OF;
        }
        else if (akt_pos<=endpos){
            *the_state=FahreRunter_OHNE_OF;
        }
        break;
    case Notaus:
       if(t2==1&&notaus==0){
           *the_state=Steht;
       }
        break;
    case FahreRauf_MIT_OF:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if (akt_pos<endpos && akt_pos>startpos){
            *the_state=Steht;
        }
        else if (esr==1){
            *the_state=StarteRauf;
        }
        break;
    case FahreRauf_OHNE_OF:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if (akt_pos<endpos || akt_pos>startpos){
            *the_state=Steht;
        }
        else if(esr==1){
            *the_state=StarteRauf;
        }
        break;
    case FahreRunter_MIT_OF:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if(akt_pos>endpos && akt_pos<startpos){
            *the_state=Steht;
        }
        else if(esl==1){
            *the_state=StarteRunter;
        }
        break;
    case FahreRunter_OHNE_OF:
        if (notaus==1){
            *the_state=Notaus;
            break;
        }
        else if(akt_pos>endpos || akt_pos<startpos){
            *the_state=Steht;
        }
        else if(esl==1){
            *the_state=StarteRunter;
        }
        break;
    }


    switch(*the_state){
    case Steht:
        *m_li=0;
        *m_re=0;
        *m_an=0;
        break;
    case StarteRauf:
        *m_li=1;
        *m_re=0;
        *m_an=1;
        startpos=akt_pos;
        endpos = (startpos-60000)%65535;
        if(endpos<0){
            endpos=endpos+65535;
        }
        break;
    case StarteRunter:
        *m_li=0;
        *m_re=1;
        *m_an=1;
        startpos=akt_pos;
        endpos = (startpos+60000)%65535;
        break;
    case Notaus:
        *m_li=0;
        *m_re=0;
        *m_an=0;
        break;
    case FahreRauf_MIT_OF:
        *m_li=1;
        *m_re=0;
        *m_an=1;
        break;
    case FahreRauf_OHNE_OF:
        *m_li=1;
        *m_re=0;
        *m_an=1;
        break;
    case FahreRunter_MIT_OF:
        *m_li=0;
        *m_re=1;
        *m_an=1;
        break;
    case FahreRunter_OHNE_OF:
        *m_li=0;
        *m_re=1;
        *m_an=1;
        break;
    }
}

void emain(void* arg) 
{
	// Hier alle benoetigten Variablen deklarieren
	// ...

    unsigned short int wert=1;
    unsigned short int *DigitalValue,temp;
    DigitalValue = &wert;


	INIT_BM_WITH_REGISTER_UI; // Hier unbedingt einen Break-Point setzen !!!

	
	// Hier die Treiberfunktionen aufrufen und testen (Aufgabe 1)
	// ...

  // io_out16(OUT1,wert);
  // io_out16(DIR1,wert);
   // Init(GlobalBoardHandle,0x89);

/*
    OutputByte(GlobalBoardHandle,3,5);
    OutputByte(GlobalBoardHandle,2,6);
    OutputByte(GlobalBoardHandle,1,255);
    OutputByte(GlobalBoardHandle,0,255);
/*
        InputByte(GlobalBoardHandle,3,&wert);
        InputByte(GlobalBoardHandle,2,&wert);
        InputByte(GlobalBoardHandle,1,&wert);
        InputByte(GlobalBoardHandle,0,&wert);

*/

	// Ab hier beginnt die Endlosschleife fuer den Automaten (Aufgabe 2)
    Free(GlobalBoardHandle);
    Init(GlobalBoardHandle,0x9A);

	while(1) {

		SYNC_SIM; 
        unsigned short int portd, portc, portb, porta;
		// Hier die Eingabesignale einlesen
		// ...

        InputByte(GlobalBoardHandle,2,&portc);
        InputByte(GlobalBoardHandle,1,&portb);
        InputByte(GlobalBoardHandle,0,&porta);
        int i;
        int notaus, t2, esr, esl, m_re, m_li, m_an;
        long int akt_pos = 0;
        esr=(portc>>3)&0x1;
        esl=(portc>>2)&0x1;
        t2=(portc>>5)&0x1;
        notaus=(portc>>4)&0x1;
        akt_pos = (portb<<8)|porta;
		// Hier die Steuerungsfunktion aufrufen
		// ...

        steuerungsfunktion(notaus, t2, esr, esl, &m_re, &m_li, &m_an, akt_pos, &the_state);


		// Hier die Ausgabesignale ausgeben
		// ...
        portd= m_re<<2|m_li<<1|m_an;
        OutputByte(GlobalBoardHandle,3,portd);

	} // while(1)..
    Free(GlobalBoardHandle);
	
}

  
