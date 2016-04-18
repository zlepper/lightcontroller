;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		lyskilder.asm
;   Dato:		04.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:

; ******* PROCESSOR DEFINITIONER **********************************************************
	processor	16f18313				;Sets processor
	#include 	p16f18313.inc
	errorlevel -302						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -305						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -307
	errorlevel -207
; ******* Configure the PIC *****************************************************
	
	; CONFIG1
	; __config 0xFFFF
	__CONFIG _CONFIG1, _FEXTOSC_OFF & _RSTOSC_HFINT32 & _CLKOUTEN_OFF & _CSWEN_ON & _FCMEN_ON
	; CONFIG2
	; __config 0xFFFF
	 __CONFIG _CONFIG2, _MCLRE_ON & _PWRTE_OFF & _WDTE_OFF & _LPBOREN_OFF & _BOREN_ON & _BORV_LOW & _PPS1WAY_ON & _STVREN_ON & _DEBUG_OFF
	; CONFIG3
	; __config 0x2003
	__CONFIG _CONFIG3, _WRT_OFF & _LVP_OFF
	; CONFIG4
	; __config 0x3
	__CONFIG _CONFIG4, _CP_OFF & _CPD_OFF
		
; ******* DEFFINITION AF VARIABLE *********************************************************
	LYSNR		EQU 0x20    ; Definer lysnr til lyskilde
	START_SERIEL	EQU 0x21    ; Start register til seriel kommunikation
	STOP_SERIEL	EQU 0x22    ; Stop register til seriel kommunikation
	MODTAGET_DATA	EQU 0x23    ; Definer register til lagring af modtaget data
		
; ******* OPSÆTNING AF PROGRAM POINTERE ***************************************************
    org		0x0000				; Programstart efter et reset
    GOTO	init				; Gå til opsætning
    org		0x0005				; Første position efter interrupt-vektor

; ******* INCLUDEREDE FILER ***************************************************************
    #Include	"delay.inc"			; Tilføjer delay filen
    #Include	"transmitter.inc"		; Tilføjer Transmitter filen
    #Include	"receiver.inc"			; Tilføjer Reciever filen
    #Include	"EEPROM.inc"			; Tilføjer EEPROM filen
	
; ******* INITIALISERING AF CONTROLLER *****************************************************
init
    BANKSEL LATA    ; Sæt bank til hvor LATA befinder sig
    CLRF LATA	    ; Data latch
    BANKSEL ANSELA  ; Sæt bank til hvor ANSELA befinder sig
    CLRF ANSELA	    ; Digital I/O

; ******* PERIPHERAL PIN SELECT - OMDEFINÉR RX OG TX TIL NYE PINS **************************
    BANKSEL INTCON
    BCF INTCON,GIE ; Stop interupts, imens vi omdefinerer pins
    
    BANKSEL PPSLOCK ; Set bank
    ; Krævet for at åbne/lukke for PPS
    MOVLW 0x55 
    MOVWF PPSLOCK
    MOVLW 0xAA
    MOVWF PPSLOCK
    BCF PPSLOCK,PPSLOCKED ; Åben for PPS
    
    BANKSEL RXPPS ; Set bank
    MOVLW 0x00 ; RX til RA0 (PIN 7)
    MOVWF RXPPS ; RX omdefineres
    
    BANKSEL RA1PPS ; Set bank
    MOVLW 0x14 ; TX til RA1 (PIN 6)
    MOVWF RA1PPS
    
    BANKSEL RA4PPS
    MOVLW b'00000010' ; PWM til RA4 (Pin 4)
    MOVWF RA4PPS
    
    ; Krævet for at åbne/lukke for PPS
    BANKSEL PPSLOCK ; Set bank
    MOVLW 0x55 
    MOVWF PPSLOCK
    MOVLW 0xAA
    MOVWF PPSLOCK
    BSF PPSLOCK,PPSLOCKED ; Luk PPS
    
    BANKSEL INTCON
    BSF INTCON,GIE ; Genopret interupts
    
; ******* PINS DEFINERING TIL BLA. SERIEL KOMMUNIKATION ************************************
    BANKSEL TRISA   ; Sæt bank til hvor TRISA befinder sig
    BCF TRISA,4	    ; Sæt TRISA til output
    BSF TRISA,0	    ; Sæt op til RX (Seriel kommunikation)
    BSF TRISA,1	    ; Sæt op til TX (Seriel komunikation)
    
; ******* PULS-BREDDE MODULATION ***********************************************************
    
    ; Sæt oscillatoren
    ; Vi vælger HFINTOSC (High-Frequency Internal  Oscillator), for at få 32 MHz i configuration 
    ;BANKSEL OSCFRQ ; HFINTOSC FREQUENCY SELECTION REGISTER
    ;MOVLW B'00000100'
    ;MOVWF OSCFRQ ; Vi omdanner MHz fra 32 til 8 MHz
    
    BANKSEL TRISA ; Set bank
    BSF TRISA,5 ; Disable the PWMx pin output driver(s) by setting the associated TRIS bit(s)
    
    BANKSEL PWM5CON ; Set bank
    BCF PWM5CON,PWM5POL ; Configure the PWM output polarity by configuring the PWMxPOL bit of the PWMxCON register.
    
    ; Load the PR2 register with the PWM period value
    BANKSEL PR2 ; Set bank
    MOVLW 0x20 ; Beregnet ved hjælp af formel 18-1 side 168
    MOVWF PR2 ; Så får vi en PWM Frequenzy på 19.23 kHz, ved en MHz på 32 (se side 171 - tabel 18-1)
    
    ; Load the PWMxDCH register and bits <7:6> of the PWMxDCL register with the PWM duty cycle value
    BANKSEL PWM5DCH ; Set bank
    MOVLW B'00000000'  ; Beregnet ved hjælp af formel 18-2 side 168
    MOVWF PWM5DCH
    
    BANKSEL PWM5DCL ; Set bank
    MOVLW B'00000000' ; Beregnet ved hjælp af formel 18-2 side 168
    MOVWF PWM5DCL
    
    ; Configure and start Timer2
    BANKSEL PIR1 ; Set bank
    BCF PIR1,TMR2IF
    
    ; Sæt en timer prescale value til 1
    BANKSEL T2CON ; Set bank
    BCF T2CON,0
    BCF T2CON,1
    
    ; Enable Timer2
    BSF T2CON,TMR2ON
    
    BANKSEL PIR1 ; Set bank
    WAIT_ON_TIMER BTFSS PIR1,TMR2IF
	GOTO WAIT_ON_TIMER
	
    ; When the TMR2IF flag bit is set: 
    ; Clear the associated TRIS bit(s) to enable the output driver.
    BANKSEL TRISA ; Set bank
    BCF TRISA,5
    
    BANKSEL PWM5CON ; Set bank
    BSF PWM5CON,PWM5EN ; Enable the PWMx module
    
; ******* INDSTILLING AF REGISTRE **********************************************************
    ; Sæt start værdi i register
   
    MOVLW 0xFF
    BANKSEL START_SERIEL
    MOVWF START_SERIEL
    CLRW
    
    ; Sæt lysnr til lyskilde
    MOVLW 0x01
    BANKSEL LYSNR
    MOVWF LYSNR
    CLRW
    
    ; Sæt stop værdi i register
    MOVLW 0x00
    BANKSEL STOP_SERIEL
    MOVWF STOP_SERIEL
    CLRW
    
    GOTO MAIN ; Gå til MAIN som tager sig af, hvad lysstyringen skal gøre

; ******* HOVEDPROGRAM **********************************************************************
    
MAIN
   GOTO INITIALIZE_SERIEL_RECEIVER
   ;GOTO INITIALIZE_SERIEL_TRANSMIT

; ------- SERIEL RECIPERING AF DATA FRA RASPBERRY PI ---------------------------------------
INITIALIZE_SERIEL_RECEIVER ; Initialiser seriel recipering
    CALL RECEIVERSETUP ; Opsæt til at modtage data fra Rasperry Pi'en
    GOTO CHECK_SERIEL_START ; Gå til check for data 
    
CHECK_SERIEL_START ; Har vi modtaget noget nyt data siden sidst?
    CALL DATA_CHECK ; Tjek flag for nyt data
    
    BANKSEL RC1REG ; Hvis nyt data modtaget, flyt det til arbejdsregistret
    MOVF RC1REG,W


    ; Check for start på modtaget data
    BANKSEL START_SERIEL
    SUBWF START_SERIEL,W
    
    BTFSC STATUS,Z
	GOTO CHECK_FOR_LYSNR

    CLRW
    
    GOTO CHECK_SERIEL_START
    
CHECK_FOR_LYSNR
    
    CALL DATA_CHECK
    
    BANKSEL RC1REG
    MOVF RC1REG,W

    ;Check for LYSNR
    BANKSEL LYSNR
    SUBWF LYSNR,W
    
    BTFSC STATUS,Z
	GOTO CHECK_FOR_DATA
    
    CLRW
    GOTO MAIN

CHECK_FOR_DATA

    ; Check for data
    CALL DATA_CHECK
    
    BANKSEL RC1REG
    MOVF RC1REG,W
    
    ; Flyt dataen ud på LED ved hjælp af Pulse-bredde modulation
    BANKSEL PWM5DCH
    MOVWF PWM5DCH
    
    CLRW
    GOTO CHECK_FOR_STOP

CHECK_FOR_STOP
    ; Check for stop på modtagning af data
    CALL DATA_CHECK
    BANKSEL RC1REG
    MOVF RC1REG,W

    ; Check for start på modtaget data
    BANKSEL STOP_SERIEL
    SUBWF STOP_SERIEL,W
    
    BTFSC STATUS,Z
	GOTO MAIN
	
    CLRW
    GOTO CHECK_FOR_STOP

DATA_CHECK
    ; Tjek om nyt data er klar til at blive modtaget fra Rasperry Pi
    BANKSEL PIR1 ; Gå til bank
    BTFSS PIR1,RCIF ; Tjek flag
    	GOTO DATA_CHECK
    RETURN

; ------- SERIEL TRANSMITTERING AF DATA TIL RASPBERRY PI ---------------------------------------    

INITIALIZE_SERIEL_TRANSMIT ; Initialiser seriel transmittering
   CALL TRANSMITTERSETUP ; Opsæt til at transmittere data til Rasperry Pi'en
   GOTO SEND_STATUS_TO_PI ; Send lysets stadie til Raspberry Pi
   
SEND_STATUS_TO_PI
; Send information om lys
   CALL CHECK_TRANSMIT_STATUS ; Tjek om den er igang med at transmittere andet data, vent hvis ja
   
   BANKSEL LYSNR
   MOVF LYSNR,W ; Send LYSNR

   BANKSEL TX1REG
   MOVWF TX1REG ; til Raspberry Pi

   CALL CHECK_TRANSMIT_STATUS ; Tjek om den er færdig med den sidste transmittering, vent hvis nej
   
   BANKSEL PWM5DCH ; Gå til bank
   MOVF PWM5DCH,W ; Flyt lysets stadie ud i arbejdsregistret
   
   BANKSEL TX1REG ; Gå til bank
   MOVWF TX1REG ; Transmitter lysets stadie til Raspberry Pi
   
   CALL CHECK_TRANSMIT_STATUS ; Vent på at den er færdig med at transmittere
   
   GOTO MAIN ; Gå tilbage til MAIN
   
CHECK_TRANSMIT_STATUS
   BANKSEL PIR1 ; Gå til bank
   BTFSS PIR1,TXIF ; Tjek flag: Hvis den er clearet, så er den igang med at transmittere
	GOTO CHECK_TRANSMIT_STATUS ; Vent på at den er færdig
   RETURN ; Returner når den er klar igen
   
; ------- TEST TEST TEST TEST TEST TEST TEST TEST TEST ------------------------------------
TEST_DEAD
    BANKSEL PWM5DCH ; Set bank
    MOVLW B'11111111'  ; Beregnet ved hjælp af formel 18-2 side 168
    MOVWF PWM5DCH
    CALL DELAY
    MOVLW B'00000000'
    MOVWF PWM5DCH
    GOTO TEST_DEAD
    
; ******* PROGRAM AFSLUTTET ***************************************************************		
    END ; Her slutter programmet