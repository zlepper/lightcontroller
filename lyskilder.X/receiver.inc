;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		reciever.inc
;   Dato:		04.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:

; ******* HOVEDPROGRAM ********************************************************************

RECEIVERSETUP
	; S�t de forskellige bits, som er associeret med "asynchronous reception"
	BANKSEL PIR1 ; G� til banken hvor PIR1 ligger
	BCF PIR1,RCIF	; S�tter interuptflaget til 0, 1 betyder byte modtaget
	
	BANKSEL TX1STA ; G� til banken hvor TX1STA ligger
	BCF TX1STA,SYNC ; Clear SYNC n�r asynchronous reception skal k�re
	
	BANKSEL PIE1 ; G� til banken hvor PIE1 ligger
	BSF PIE1,RCIE ; Vi �nsker interupts
	
	BANKSEL INTCON ; G� til banken hvor INTCON ligger
	BSF INTCON,GIE ; Vi �nsker interupts
	BSF INTCON,PEIE ; Vi �nsker interupts
	
	; S�t baud rate generatoren
	BANKSEL TX1STA ; G� til bank
	BSF TX1STA,BRGH	; S�ttes til h�j da det giver den mindste fejl med 9600 baud rate 
	BANKSEL BAUD1CON ; G� til bank
	BCF BAUD1CON,BRG16 ; S�ttes lav, da det giver den mindste fejl med 9600 baud rate
	
	BANKSEL SP1BRG ; G� til bank
	MOVLW D'207' ; Ved en MHz p� 32
	MOVWF SP1BRG ; Flyt W til SPBRG s� boud rate generator f�r en error% p� 0.16
	
	BANKSEL RC1STA ; G� til banken hvor RC1STA ligger
	BSF RC1STA,SPEN	; S�t SPEN bit for at aktivere den serielle port
	BCF RC1STA,RX9 ; Vi skal ikke bruge den niende bit
	BCF RC1STA,OERR	; Denne bit skal cleares, ellers vil intet data kunne modtages!
	BCF RC1STA,RX9D	; 9ende bit s�ttes til 0, men anvendes ikke i dette program
	BCF RC1STA,FERR	; Denne bit cleares, men hvis et stop bit er opfanget som clearet, s� s�ttes denne bit
	BSF RC1STA,CREN ; Aktiver reception
	
	CLRW ; Clear W, s� nyt data kan blive lageret i W
	BANKSEL RC1REG ; G� til banken hvor RCREG befinder sig 
	CLRF RC1REG ; Clear registeringsregistret, s� nyt data kan blive lageret
	BANKSEL MODTAGET_DATA
	CLRF MODTAGET_DATA ; Clear vores resultat register, s� nyt data kan blive lageret

	RETURN ; Setup er f�rdig, vend tilbage til hovedprogram

; ******* PROGRAM AFSLUTTET ***************************************************************		


