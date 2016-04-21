;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	reciever.inc
;   Dato:	04.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil sørger for reciperingen af data fra Raspberry Pi'en, som inkluderer at opsætte
;   microcontrolleren til seriel asynkron recipering, tolke på den modtagede data 
;   og lagre den brugbare data.

; ******* HOVEDPROGRAM ********************************************************************
CHECK_FOR_DATA_FROM_RPI
    ; Har vi modtaget noget nyt data siden sidst?
    CALL RECEIVERSETUP ; Sæt lyskildens microcontroller op til at være receiver
    
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; Gå til bank
    MOVF RC1REG,W ; Hvis nyt data modtaget, flyt det til arbejdsregistret

    ; Check for start på modtaget data
    BANKSEL START_SERIEL ; Gå til bank
    SUBWF START_SERIEL,W ; Tjek om et start er sendt
    
    BTFSC STATUS,Z ; Hvis et start er sendt
	GOTO CHECK_FOR_LYSNR ; Så gå til tjek for LYSNR

    GOTO CHECK_FOR_DATA_FROM_RPI ; Hvis ikke, tjek indtil et start er modtaget
    
CHECK_FOR_LYSNR
      
    ; Tjek om det er denne lyskilde, som RPI'en vil i kontakt med
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; Gå til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret

    ;Check for LYSNR
    BANKSEL LYSNR ; Gå til bank
    SUBWF LYSNR,W ; Tjek om det er denne lyskilde som RPI'en prøver at kommunikere med
    
    BTFSC STATUS,Z ; Hvis det er denne lyskilde
	GOTO CHECK_FOR_DATA ; Så gå til tjek for data pakke
    
    GOTO CHECK_FOR_DATA_FROM_RPI ; Hvis ikke, gå tilbage til MAIN, da det så ikke er denne lyskilde den vil i kontakt med

CHECK_FOR_DATA
    
    ; Tjek for data pakke
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    
    BANKSEL RC1REG ; Gå til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVWF MODTAGET_DATA ; Flyt dataen til lagring i MODTAGET_DATA registret
    
    GOTO CHECK_FOR_STOP ; Gå til tjek for stop pakke

CHECK_FOR_STOP
    ; Tjek for stop pakke
    CALL CHECK_RECEIVE_STATUS ; Tjek om vi har modtaget nyt data fra RPI
    BANKSEL RC1REG ; Gå til bank
    MOVF RC1REG,W ; Flyt det modtagede data til arbejdsregistret
   
    ; Check for start på modtaget data
    BANKSEL STOP_SERIEL ; Gå til bank
    SUBWF STOP_SERIEL,W ; Tjek om et stop er sendt
   
    BTFSC STATUS,Z ; Hvis et stop er sendts
	RETURN ; Gå til at bruge det modtagede data
    
    GOTO CHECK_FOR_STOP ; Hvis ikke, tjek indtil et stop pakke er modtaget
	
CHECK_RECEIVE_STATUS
    ; Tjek om nyt data er klar til at blive modtaget fra Rasperry Pi
    BANKSEL PIR1 ; Gå til bank
    BTFSS PIR1,RCIF ; Tjek flag
    	GOTO CHECK_OTHER ; Gå tilbage til lysstadie
    RETURN ; Returnér når nyt data er klar til at blive modtaget
    
CHECK_OTHER
    CALL CHECK_BUTTON ; Tjek om knappen er blevet trykket TODO BUTTON
    
    ;CALL CHECK_POTENTIOMETER ; Tjek om potentiometeret er blevet ændret på TODO POTENTIOMETER
    
    GOTO CHECK_RECEIVE_STATUS
    
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



