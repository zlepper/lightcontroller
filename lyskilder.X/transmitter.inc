;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	transmitter.inc
;   Dato:	04.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil sørger for transmitteringen af data til Raspberry Pi'en, som inkluderer at opsætte
;   microcontrolleren til seriel asynkron transmittering og transmittering af fire data pakker:
;   Start, lysnr, data og stop pakke

; ******* HOVEDPROGRAM ********************************************************************

; ------- OPDATÉR RASPBERRY PI MED LYSNR OG LYSSTADIE ------------------------------------
OPDATER_RPI
    ; Opdater lysstadie
    BANKSEL PWM5DCH ; Gå til bank
    MOVF PWM5DCH,W ; Flyt LED lysstadie til arbejdsregistret
    BANKSEL SEND_DATA ; Gå til bank
    MOVWF SEND_DATA ; Flyt det til SEND_DATA registret for at sende dataen til RPI'en
    
    CALL TRANSMIT_TO_RPI ; Kald transmissions koden, som sørger for at 4 pakker bliver sendt afsted: Start, lysnr, data og stop pakken
    
    RETURN ; Returnér
    
TRANSMIT_TO_RPI
; Vores transmittering af data består af 4 pakker: Start, lysnr, data og stop pakke
    CALL TRANSMITTERSETUP ; Sæt lyskildens microcontroller op til at være transmitter
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med at transmittere
    
    BANKSEL START_SERIEL ; Gå til bank
    MOVF START_SERIEL,W ; Flyt start pakke til arbejdsregistret
    BANKSEL TX1REG ; Gå til bank
    MOVWF TX1REG ; Transmittér start pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med at transmittere
    
    BANKSEL LYSNR ; Gå til bank
    MOVF LYSNR,W ; Flyt lysnr pakke til arbejdsregistret
    BANKSEL TX1REG ; Gå til bank
    MOVWF TX1REG ; Transmittér lysnr pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med at transmittere
    
    BANKSEL SEND_DATA ; Gå til bank
    MOVF SEND_DATA,W ; Flyt data pakke til arbejdsregistret (Den data vi gerne vil sende)
    BANKSEL TX1REG ; Gå til bank
    MOVWF TX1REG ; Transmittér data pakke, som sender vores ønsket data afsted
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med at transmittere
    
    BANKSEL STOP_SERIEL ; Gå til bank
    MOVF STOP_SERIEL,W  ; Flyt stop pakke til arbejdsregistret
    BANKSEL TX1REG ; Gå til bank
    MOVWF TX1REG ; Transmittér stop pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med at transmittere
    
    BANKSEL SEND_DATA ; Gå til bank
    CLRF SEND_DATA ; Vi rydder SEND_DATA, da dataen er blevet sendt
    RETURN ; Returnér når den er færdig med at transmittere
    
CHECK_TRANSMIT_STATUS
   BANKSEL PIR1 ; Gå til bank
   BTFSS PIR1,TXIF ; Tjek flag: Hvis den er clearet, så er den igang med at transmittere
	GOTO CHECK_TRANSMIT_STATUS ; Vent på at den er færdig
   RETURN ; Returner når den er klar igen    

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


