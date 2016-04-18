;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		transmitter.inc
;   Dato:		04.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:

; ******* HOVEDPROGRAM ********************************************************************

TRANSMITTERSETUP
; Start med at sæt de forskellige bits, som er associeret med "asynchronous transmission"
	
    ; Sæt baud rate generatoren
    BANKSEL TX1STA ; Gå til bank
    BSF TX1STA,BRGH	; Sættes til høj da det giver den mindste fejl med 9600 baud rate 
    BANKSEL BAUD1CON ; Gå til bank
    BCF BAUD1CON,BRG16 ; Sættes lav, da det giver den mindste fejl med 9600 baud rate
	
    BANKSEL SP1BRG ; Gå til bank
    MOVLW D'207' ; Ved 32 MHz
    MOVWF SP1BRG ; Flyt W til SPBRG så boud rate generator får en error% på 0.16
	
    BANKSEL TX1STA ; Gå til bank hvor registret befinder sig
    BCF TX1STA,SYNC ; Konfigur til asynchronous
    BANKSEL RC1STA ; Gå til bank hvor registret befinder sig
    BSF RC1STA,SPEN ; Aktiver EUSART
	
    BANKSEL TX1STA ; Gå til bank hvor registret befinder sig
    BCF TX1STA,CSRC ; Clock source ignoreret i asynchronous mode
    BCF TX1STA,TX9 ; Vi skal ikke bruge den niende bit
    BCF TX1STA,SENDB ; Vi sender ikke SYNC BREAK transmission
    BSF TX1STA,TRMT	; Transmit data bit sættes til 1. Cleares når data er tilgængeligt
    BCF TX1STA,TX9D	; 9ende bit sættes til 0, men anvendes ikke i dette program
	
    BANKSEL BAUD1CON ; Gå til bank hvor registret befinder sig
    BCF BAUD1CON,SCKP ; Inverteret bit transmission er ikke ønsket
	
    BANKSEL TX1STA ; Gå til bank hvor registret befinder sig
    BSF TX1STA,TXEN ; Aktiver den serielle port
	
    BANKSEL PIE1 ; Gå til banken hvor PIE1 ligger
    BSF PIE1,TXIE ; Vi ønsker interupts
	
    BANKSEL INTCON ; Gå til banken hvor INTCON ligger
    BSF INTCON,GIE ; Vi ønsker interupts
    BSF INTCON,PEIE ; Vi ønsker interupts
	
    BANKSEL PIR1 ; Gå til banken hvor PIR1 ligger
    BCF PIR1,TXIF ; Sætter interuptflaget til 0, 1 betyder byte modtaget
	
    RETURN ; Setup er færdig, vend tilbage til hovedprogram

; ******* PROGRAM AFSLUTTET ***************************************************************


