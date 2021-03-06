;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	reciever.inc
;   Dato:	04.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil s�rger for reciperingen af data fra Raspberry Pi'en, som inkluderer at ops�tte
;   microcontrolleren til seriel asynkron recipering, tolke p� den modtagede data 
;   og lagre den brugbare data.

; ******* HOVEDPROGRAM ********************************************************************
    
CHECK_FOR_DATA_FROM_RPI
    ; Har vi modtaget noget nyt data siden sidst?
    CALL RECEIVERSETUP ; S�t lyskildens microcontroller op til at v�re receiver
    
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; G� til bank
    MOVF RC1REG,W ; Hvis nyt data modtaget, flyt det til arbejdsregistret

    ; Check for start p� modtaget data
    BANKSEL START_SERIEL ; G� til bank
    SUBWF START_SERIEL,W ; Tjek om et start er sendt
    
    BTFSC STATUS,Z ; Hvis et start er sendt
	GOTO CHECK_FOR_LYSNR ; S� g� til tjek for LYSNR

    GOTO CHECK_FOR_DATA_FROM_RPI ; Hvis ikke, tjek indtil et start er modtaget
    
CHECK_FOR_LYSNR
      
    ; Tjek om det er denne lyskilde, som RPI'en vil i kontakt med
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; G� til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret

    ;Check for LYSNR
    BANKSEL LYSNR ; G� til bank
    SUBWF LYSNR,W ; Tjek om det er denne lyskilde som RPI'en pr�ver at kommunikere med
    
    BTFSC STATUS,Z ; Hvis det er denne lyskilde
	GOTO CHECK_FOR_DATA ; S� g� til tjek for data pakke
    
    GOTO CHECK_FOR_DATA_FROM_RPI ; Hvis ikke, g� tilbage til MAIN, da det s� ikke er denne lyskilde den vil i kontakt med

CHECK_FOR_DATA
    
    ; Tjek for data pakke
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; G� til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL MODTAGET_DATA ; G� til bank
    MOVWF MODTAGET_DATA ; Flyt dataen til lagring i MODTAGET_DATA registret
    
    GOTO CHECK_FOR_STOP ; G� til tjek for stop pakke

CHECK_FOR_STOP
    ; Tjek for stop pakke
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    BANKSEL RC1REG ; G� til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret
   
    ; Check for start p� modtaget data
    BANKSEL STOP_SERIEL ; G� til bank
    SUBWF STOP_SERIEL,W ; Tjek om et stop er sendt
   
    BTFSC STATUS,Z ; Hvis et stop er sendts
	RETURN ; G� til at bruge det modtagede data
    
    GOTO CHECK_FOR_STOP ; Hvis ikke, tjek indtil et stop pakke er modtaget
	
CHECK_RECEIVE_STATUS
    ; Tjek om nyt data er klar til at blive modtaget fra Rasperry Pi
    BANKSEL PIR1 ; G� til bank
    BTFSS PIR1,RCIF ; Tjek flag
    	GOTO CHECK_OTHER ; G� tilbage til lysstadie
    RETURN ; Return�r n�r nyt data er klar til at blive modtaget

; ------- TJEK OM KNAPPEN ER BLEVET TRYKKET ---------------------------------------
CHECK_OTHER
    CALL CHECK_BUTTON ; Tjek om knappen er blevet trykket
    
    ;CALL CHECK_POTENTIOMETER ; Tjek om potentiometeret er blevet �ndret p� (Dette n�ede vi ikke)
    
    GOTO CHECK_RECEIVE_STATUS ; Forts�t med at tjekke for nyt data fra webserveren
  
; ------- OPS�TNING AF MICROCONTROLLER TIL RECEIVER ---------------------------------------
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



