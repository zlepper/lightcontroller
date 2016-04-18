;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		transmitter.inc
;   Dato:		04.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:

; ******* HOVEDPROGRAM ********************************************************************

TRANSMITTERSETUP
; Start med at s�t de forskellige bits, som er associeret med "asynchronous transmission"
	
    ; S�t baud rate generatoren
    BANKSEL TX1STA ; G� til bank
    BSF TX1STA,BRGH	; S�ttes til h�j da det giver den mindste fejl med 9600 baud rate 
    BANKSEL BAUD1CON ; G� til bank
    BCF BAUD1CON,BRG16 ; S�ttes lav, da det giver den mindste fejl med 9600 baud rate
	
    BANKSEL SP1BRG ; G� til bank
    MOVLW D'207' ; Ved 32 MHz
    MOVWF SP1BRG ; Flyt W til SPBRG s� boud rate generator f�r en error% p� 0.16
	
    BANKSEL TX1STA ; G� til bank hvor registret befinder sig
    BCF TX1STA,SYNC ; Konfigur til asynchronous
    BANKSEL RC1STA ; G� til bank hvor registret befinder sig
    BSF RC1STA,SPEN ; Aktiver EUSART
	
    BANKSEL TX1STA ; G� til bank hvor registret befinder sig
    BCF TX1STA,CSRC ; Clock source ignoreret i asynchronous mode
    BCF TX1STA,TX9 ; Vi skal ikke bruge den niende bit
    BCF TX1STA,SENDB ; Vi sender ikke SYNC BREAK transmission
    BSF TX1STA,TRMT	; Transmit data bit s�ttes til 1. Cleares n�r data er tilg�ngeligt
    BCF TX1STA,TX9D	; 9ende bit s�ttes til 0, men anvendes ikke i dette program
	
    BANKSEL BAUD1CON ; G� til bank hvor registret befinder sig
    BCF BAUD1CON,SCKP ; Inverteret bit transmission er ikke �nsket
	
    BANKSEL TX1STA ; G� til bank hvor registret befinder sig
    BSF TX1STA,TXEN ; Aktiver den serielle port
	
    BANKSEL PIE1 ; G� til banken hvor PIE1 ligger
    BSF PIE1,TXIE ; Vi �nsker interupts
	
    BANKSEL INTCON ; G� til banken hvor INTCON ligger
    BSF INTCON,GIE ; Vi �nsker interupts
    BSF INTCON,PEIE ; Vi �nsker interupts
	
    BANKSEL PIR1 ; G� til banken hvor PIR1 ligger
    BCF PIR1,TXIF ; S�tter interuptflaget til 0, 1 betyder byte modtaget
	
    RETURN ; Setup er f�rdig, vend tilbage til hovedprogram

; ******* PROGRAM AFSLUTTET ***************************************************************

