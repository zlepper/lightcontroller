;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		reciever.inc
;   Dato:		04.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:

; ******* HOVEDPROGRAM ********************************************************************

RECEIVERSETUP
	; Sæt de forskellige bits, som er associeret med "asynchronous reception"
	BANKSEL PIR1 ; Gå til banken hvor PIR1 ligger
	BCF PIR1,RCIF	; Sætter interuptflaget til 0, 1 betyder byte modtaget
	
	BANKSEL TX1STA ; Gå til banken hvor TX1STA ligger
	BCF TX1STA,SYNC ; Clear SYNC når asynchronous reception skal køre
	
	BANKSEL PIE1 ; Gå til banken hvor PIE1 ligger
	BSF PIE1,RCIE ; Vi ønsker interupts
	
	BANKSEL INTCON ; Gå til banken hvor INTCON ligger
	BSF INTCON,GIE ; Vi ønsker interupts
	BSF INTCON,PEIE ; Vi ønsker interupts
	
	; Sæt baud rate generatoren
	BANKSEL TX1STA ; Gå til bank
	BSF TX1STA,BRGH	; Sættes til høj da det giver den mindste fejl med 9600 baud rate 
	BANKSEL BAUD1CON ; Gå til bank
	BCF BAUD1CON,BRG16 ; Sættes lav, da det giver den mindste fejl med 9600 baud rate
	
	BANKSEL SP1BRG ; Gå til bank
	MOVLW D'207' ; Ved en MHz på 32
	MOVWF SP1BRG ; Flyt W til SPBRG så boud rate generator får en error% på 0.16
	
	BANKSEL RC1STA ; Gå til banken hvor RC1STA ligger
	BSF RC1STA,SPEN	; Sæt SPEN bit for at aktivere den serielle port
	BCF RC1STA,RX9 ; Vi skal ikke bruge den niende bit
	BCF RC1STA,OERR	; Denne bit skal cleares, ellers vil intet data kunne modtages!
	BCF RC1STA,RX9D	; 9ende bit sættes til 0, men anvendes ikke i dette program
	BCF RC1STA,FERR	; Denne bit cleares, men hvis et stop bit er opfanget som clearet, så sættes denne bit
	BSF RC1STA,CREN ; Aktiver reception
	
	CLRW ; Clear W, så nyt data kan blive lageret i W
	BANKSEL RC1REG ; Gå til banken hvor RCREG befinder sig 
	CLRF RC1REG ; Clear registeringsregistret, så nyt data kan blive lageret
	BANKSEL MODTAGET_DATA
	CLRF MODTAGET_DATA ; Clear vores resultat register, så nyt data kan blive lageret

	RETURN ; Setup er færdig, vend tilbage til hovedprogram

; ******* PROGRAM AFSLUTTET ***************************************************************		



