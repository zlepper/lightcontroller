;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	transmitter.inc
;   Dato:	04.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil s�rger for transmitteringen af data til Raspberry Pi'en, som inkluderer at ops�tte
;   microcontrolleren til seriel asynkron transmittering og transmittering af fire data pakker:
;   Start, lysnr, data og stop pakke

; ******* HOVEDPROGRAM ********************************************************************

; ------- OPDAT�R RASPBERRY PI MED LYSNR OG LYSSTADIE ------------------------------------
OPDATER_RPI
    ; Opdater lysstadie
    BANKSEL PWM5DCH ; G� til bank
    MOVF PWM5DCH,W ; Flyt LED lysstadie til arbejdsregistret
    BANKSEL SEND_DATA ; G� til bank
    MOVWF SEND_DATA ; Flyt det til SEND_DATA registret for at sende dataen til RPI'en
    
    CALL TRANSMIT_TO_RPI ; Kald transmissions koden, som s�rger for at 4 pakker bliver sendt afsted: Start, lysnr, data og stop pakken
    
    RETURN ; Return�r

; ------- SERIEL TRANSMITTERING AF DATA TIL RASPBERRY PI ---------------------------------------
TRANSMIT_TO_RPI
; Vores transmittering af data best�r af 4 pakker: Start, lysnr, data og stop pakke
    CALL TRANSMITTERSETUP ; S�t lyskildens microcontroller op til at v�re transmitter
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er f�rdig med at transmittere
    
    BANKSEL START_SERIEL ; G� til bank
    MOVF START_SERIEL,W ; Flyt start pakke til arbejdsregistret
    BANKSEL TX1REG ; G� til bank
    MOVWF TX1REG ; Transmitt�r start pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er f�rdig med at transmittere
    
    BANKSEL LYSNR ; G� til bank
    MOVF LYSNR,W ; Flyt lysnr pakke til arbejdsregistret
    BANKSEL TX1REG ; G� til bank
    MOVWF TX1REG ; Transmitt�r lysnr pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er f�rdig med at transmittere
    
    BANKSEL SEND_DATA ; G� til bank
    MOVF SEND_DATA,W ; Flyt data pakke til arbejdsregistret (Den data vi gerne vil sende)
    BANKSEL TX1REG ; G� til bank
    MOVWF TX1REG ; Transmitt�r data pakke, som sender vores �nsket data afsted
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er f�rdig med at transmittere
    
    BANKSEL STOP_SERIEL ; G� til bank
    MOVF STOP_SERIEL,W  ; Flyt stop pakke til arbejdsregistret
    BANKSEL TX1REG ; G� til bank
    MOVWF TX1REG ; Transmitt�r stop pakke
    
    CALL CHECK_TRANSMIT_STATUS ; Tjek om den er f�rdig med at transmittere
    
    BANKSEL SEND_DATA ; G� til bank
    CLRF SEND_DATA ; Vi rydder SEND_DATA, da dataen er blevet sendt
    RETURN ; Return�r n�r den er f�rdig med at transmittere
    
CHECK_TRANSMIT_STATUS
   BANKSEL PIR1 ; G� til bank
   BTFSS PIR1,TXIF ; Tjek flag: Hvis den er clearet, s� er den igang med at transmittere
	GOTO CHECK_TRANSMIT_STATUS ; Vent p� at den er f�rdig
   RETURN ; Returner n�r den er klar igen    

; ------- OPS�TNING AF MICROCONTROLLER TIL TRANSMITTER ---------------------------------------
   
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


