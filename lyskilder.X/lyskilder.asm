;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	lyskilder.asm
;   Dato:	04.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil er hovedfilen til lyskildernes microcontroller. Den sørger for at lyskilderne
;   har et LYSNR, den serielle kommunikation til og fra Raspberry Pi'en, omdannelse af data
;   til en spænding ved puls-bredde modulation, omdefinering af pins på microcontrolleren
;   så den kan bruges så effektiv som muligt i et overskueligt kredsløb.

; ******* PROCESSOR DEFINITIONER **********************************************************
	processor	16f18313				;Sets processor
	#include 	p16f18313.inc
	errorlevel -302						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -305						;fjerner meddelser om forkerte banker fra fejl listen
	errorlevel -307
	errorlevel -207
; ******* Configure the PIC *****************************************************
	
	; CONFIG1
	__CONFIG _CONFIG1, _FEXTOSC_OFF & _RSTOSC_HFINT32 & _CLKOUTEN_OFF & _CSWEN_ON & _FCMEN_ON
	; CONFIG2
	 __CONFIG _CONFIG2, _MCLRE_ON & _PWRTE_OFF & _WDTE_OFF & _LPBOREN_OFF & _BOREN_ON & _BORV_LOW & _PPS1WAY_ON & _STVREN_ON & _DEBUG_OFF
	; CONFIG3
	__CONFIG _CONFIG3, _WRT_OFF & _LVP_OFF
	; CONFIG4
	__CONFIG _CONFIG4, _CP_OFF & _CPD_OFF
		
; ******* DEFFINITION AF VARIABLE *********************************************************
	LYSNR		EQU 0x20    ; Definer lysnr til lyskilde
	START_SERIEL	EQU 0x21    ; Start register til seriel kommunikation
	STOP_SERIEL	EQU 0x22    ; Stop register til seriel kommunikation
	MODTAGET_DATA	EQU 0x23    ; Definer register til lagring af modtaget data
	SEND_DATA	EQU 0x24    ; Definér regiter til lagring af data der skal sendes til RPI
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
    
    BANKSEL RA4PPS ; TODO ændre til RA5
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
    BCF TRISA,4 ; TODO SLET
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
    ; Sæt start værdier i de forskellige egne definerede registre
    MOVLW 0xFF ; Start pakke defineres til at være 255
    BANKSEL START_SERIEL ; Gå til bank
    MOVWF START_SERIEL ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt lysnr til lyskilde
    MOVLW 0x01 ; Lysnr pakke defineres til at være 0, fra start (Ændres senere) TODO ændre til 0 
    BANKSEL LYSNR ; Gå til bank
    MOVWF LYSNR ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt stop værdi i register
    MOVLW 0x00 ; Stop pakke defineres til at være 0
    BANKSEL STOP_SERIEL ; Gå til bank
    MOVWF STOP_SERIEL ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt modtaget data registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVWF MODTAGET_DATA ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt sendt data registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL SEND_DATA ; Gå til bank
    MOVWF SEND_DATA ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    GOTO MAIN ; Gå til MAIN som tager sig af, hvad lysstyringen skal gøre

; ******* HOVEDPROGRAM **********************************************************************
    
MAIN
   ; Hvis lyskildens microcontroller ikke har et LYSNR, så skal den have tildelt et
    BANKSEL LYSNR ; Gå til bank
    MOVF LYSNR,W ; Flyt lyskildens LYSNR til W
    
    BTFSC STATUS,Z ; Hvis dens LYSNR er 0
	GOTO GET_LYSNR ; Så skal den forespørge RPI'en om et lysnr
  
    GOTO LYSSTADIE ; Ellers, så skal den gå til check for data 
   
; ------- SERIEL RECIPERING AF DATA FRA RASPBERRY PI ---------------------------------------
    
GET_LYSNR
    MOVLW D'0' ; Hvis vi sender nul til RPI'en, så skal den sende lyskilden et lysnr
    BANKSEL SEND_DATA ; Gå til bank
    MOVWF SEND_DATA ; Flyt det til SEND_DATA registret for at sende dataen til RPI'en
    
    CALL TRANSMIT_TO_RPI ; Kald transmissions koden, som sørger for at 4 pakker bliver sendt afsted: Start, lysnr, data og stop pakken
    CALL CHECK_FOR_DATA_FROM_RPI ; Modtag  et lysnr'et fra RPI'en
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVF MODTAGET_DATA,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL LYSNR ; Gå til bank
    MOVWF LYSNR ; Flyt dataen til LYSNR registret
    BANKSEL MODTAGET_DATA ; Gå til bank
    CLRF MODTAGET_DATA ; Vi rydder den modtagede data, da det er anvendt
    
    GOTO MAIN ; Gå til MAIN
    
LYSSTADIE
    ; Denne del af koden sørger for modtagelse af det ønskede lysstadie fra Raspberry Pi'en
    CALL CHECK_FOR_DATA_FROM_RPI ; Tjek om RPI'en har sendt et nyt lysstadie som lyskilden skal køre
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVF MODTAGET_DATA,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Flyt dataen ud på LED ved hjælp af Pulse-bredde modulation
    BANKSEL MODTAGET_DATA ; Gå til bank
    CLRF MODTAGET_DATA ; Vi rydder den modtagede data, da det er sendt ud til lyset
    
    GOTO LYSSTADIE ; Bliv ved med at tjekke for nyt lysstadie
  
; ------- TEST TEST TEST TEST TEST TEST TEST TEST TEST ------------------------------------
TEST_DEAD
    BANKSEL PWM5DCH ; Set bank
    MOVLW B'11111111'  ; Beregnet ved hjælp af formel 18-2 side 168
    MOVWF PWM5DCH
    CALL DELAY2
    CALL DELAY2
     BANKSEL PWM5DCH ; Set bank
    MOVLW B'00000000'  ; Beregnet ved hjælp af formel 18-2 side 168
    MOVWF PWM5DCH   
    RETURN
    
; ******* PROGRAM AFSLUTTET ***************************************************************		
    END ; Her slutter programmet