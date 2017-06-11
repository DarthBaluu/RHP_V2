#include "simuc.h"
#include "io_treiber.h"

typedef struct BHandle_Data {
	BYTE Board_allocated;	// 1=allocated, 0=free
	BYTE Port_A_Direction;	// 1=Output, 0=Input
	BYTE Port_B_Direction;
	BYTE Port_C_Direction;
	BYTE Port_D_Direction;
} BHandle;

typedef BHandle* DSCB;
BHandle BoardHandle_Data;
DSCB GlobalBoardHandle = &BoardHandle_Data;



BYTE Init(DSCB BoardHandle, unsigned long int Steuerwort) {
    BYTE a,b,c,d;

    if (BoardHandle->Board_allocated==0){
        switch(Steuerwort){
            case 0x80: a=1;b=1;c=1;d=1; break;  // 1=Output, 0=Input
            case 0x81: a=1;b=1;c=1;d=0; break;
            case 0x88: a=1;b=1;c=0;d=1; break;
            case 0x89: a=1;b=1;c=0;d=0; break;
            case 0x82: a=1;b=0;c=1;d=1; break;
            case 0x83: a=1;b=0;c=1;d=0; break;
            case 0x8A: a=1;b=0;c=0;d=1; break;
            case 0x8B: a=1;b=0;c=0;d=0; break;
            case 0x90: a=0;b=1;c=1;d=1; break;
            case 0x91: a=0;b=1;c=1;d=0; break;
            case 0x98: a=0;b=1;c=0;d=1; break;
            case 0x99: a=0;b=1;c=0;d=0; break;
            case 0x92: a=0;b=0;c=1;d=1; break;
            case 0x93: a=0;b=0;c=1;d=0; break;
            case 0x9A: a=0;b=0;c=0;d=1; break;
            case 0x9B: a=0;b=0;c=0;d=0; break;
        }
        BoardHandle->Board_allocated=1;
        if(a==1){io_out16(DIR0,0x00ff);}
        if(a==0){io_out16(DIR0,0x0000);}
        if(b==1){io_out16(DIR0, (io_in16(DIR0)|0xff00));}
        if(c==1){io_out16(DIR1,0x00ff);}
        if(c==0){io_out16(DIR1,0x0000);}
        if(d==1){io_out16(DIR1, (io_in16(DIR1)|0xff00));}

        BoardHandle->Port_A_Direction=a;
        BoardHandle->Port_B_Direction=b;
        BoardHandle->Port_C_Direction=c;
        BoardHandle->Port_D_Direction=d;
        /*
        BoardHandle_Data.Port_A_Direction=a;
        BoardHandle_Data.Port_B_Direction=b;
        BoardHandle_Data.Port_C_Direction=c;
        BoardHandle_Data.Port_D_Direction=d;
        */
        return 0;
    }

    else{return 1;}

}

BYTE InputByte(DSCB BoardHandle, BYTE Port, BYTE *DigitalValue) {
    unsigned short int /*value,*/ status=0,port=0;
    //int i, erg=0;
    if(BoardHandle->Board_allocated==1){
        if(Port == 0){
                if(BoardHandle->Port_A_Direction==0){
                    port = io_in16(IN0);
                    port = port & 0xFF;
                }
                else{status=1;}
        }

        else if(Port == 1){
                if(BoardHandle->Port_B_Direction==0){
                    port = io_in16(IN0);
                    port = port >> 8;
                }
                else{status=1;}
        }


        else if(Port == 2){
                if(BoardHandle->Port_C_Direction==0){
                    port = io_in16(IN1);
                    port =  port & 0xFF;
                }
                else{status=1;}
        }

        else if(Port == 3){
                if(BoardHandle->Port_D_Direction==0){
                    port = io_in16(IN1);
                    port = port >> 8;
                }
                else{status=1;}
        }

        else{status=2;}
}else{status=1;}

/*
    for(i=0;i<8;i++){           // berechnen des endwerts(bin -> dez)
        value = port >> i;
        value = value & 0x01;

        erg += (value*(1<<i)); // 1 um i nach links geshiftet = 2^i
    }
*/

        *DigitalValue = port; // wert von erg der Referent zuweisen (Rückgabe)

    return status;
  }





BYTE OutputByte(DSCB BoardHandle, BYTE Port, BYTE DigitalValue) {
	
    unsigned short int muster,akt,digval,port_aus,status=0;
            digval = DigitalValue;
            //A
            //DigitalValue(dez) -> dig(bin);
            //    int dezi = *DigitalValue;
            //    int test;
            //    int zaehler=0;
            //    muster = 0x00;
            //    while (dezi!=0){
            //        test = dezi%2;
            //        test << zaehler;
            //        muster = muster|test;
            //        zaehler++;
            //        dezi = dezi/2;
            //    }
            //A
    if(BoardHandle->Board_allocated==1){
        if(Port == 0/*A*/){
            if(BoardHandle->Port_A_Direction==1){
                akt = io_in16(OUT0);
                muster = digval & 0xFF;     // muster ist digval(Digital Value) mit sicherheit : 00000000xxxxxxxx
                akt = akt & 0xFF00;         // akt = port mit musster xxxxxxxx00000000
                port_aus = akt | muster;    // kombination von muster und akt
                io_out16(OUT0,port_aus);    //Ausgabe auf OUT0
            }
            else{status=1;} //ungültiger BoardHandle
         }

        else if(Port == 1/*B*/){
            if(BoardHandle->Port_B_Direction==1){
                akt = io_in16(OUT0);
                digval = digval<<8;                // Bits auf Porthöhe geshiftet
                muster = digval & 0xFF00;   // muster ist digaval mit sicherheit : xxxxxxxx00000000
                akt = akt & 0xFF;           // akt = port mit muster 00000000xxxxxxxx
                port_aus = akt | muster;    // kombinieren
                io_out16(OUT0,port_aus);    //Ausgabe auf OUT0
            }
            else{status=1;} //ungültiger BoardHandle
        }


        else if(Port == 2/*C*/){
            if(BoardHandle->Port_C_Direction==1){
                akt = io_in16(OUT1);        //Einlesen von OUT1
                muster = digval & 0xFF;     // muster ist digval(Digital Value) mit sicherheit : 00000000xxxxxxxx
                akt = akt & 0xFF00;         // akt = port mit musster xxxxxxxx00000000
                port_aus = akt | muster;    // kombination von muster und akt
                io_out16(OUT1,port_aus);    //Ausgabe auf OUT1
            }
            else{status=1;} //ungültiger BoardHandle
        }

        else if(Port == 3/*D*/){
           if(BoardHandle->Port_D_Direction==1){
                akt = io_in16(OUT1);        //Einlesen von OUT1
                digval = digval<<8;             // Bits auf Porthöhe geshiftet
                muster = digval & 0xFF00;   // muster ist digval mit sicherheit : xxxxxxxx00000000
                akt = akt & 0xFF;           // akt = port mit muster 00000000xxxxxxxx
                port_aus = akt | muster;    // kombinieren
                io_out16(OUT1,port_aus);    //Ausgabe auf OUT1
           }
           else{status=1;} //ungültiger BoardHandle
        }
        else{status=2;} // ungültiger Port
    }else{status=1;}

        return status;
}

BYTE Free(DSCB BoardHandle) {

    BoardHandle->Board_allocated=0;
	
    return 0;
}

