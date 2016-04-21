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
	
	; CONFIG1: RSTOSC skal være HFINT32 for at få 32 MHz
	__CONFIG _CONFIG1, _FEXTOSC_OFF & _RSTOSC_HFINT32 & _CLKOUTEN_OFF & _CSWEN_ON & _FCMEN_ON
	; CONFIG2 TODO: PRØV AT SÆTTE PWERTE ON
	 __CONFIG _CONFIG2, _MCLRE_ON & _PWRTE_ON & _WDTE_OFF & _LPBOREN_OFF & _BOREN_ON & _BORV_LOW & _PPS1WAY_ON & _STVREN_ON & _DEBUG_OFF
	; CONFIG3
	__CONFIG _CONFIG3, _WRT_OFF & _LVP_OFF
	; CONFIG4: CPD skal være OFF, så vi kan skrive til EEPROM
	__CONFIG _CONFIG4, _CP_OFF & _CPD_OFF
		
; ******* DEFFINITION AF VARIABLE *********************************************************
	LYSNR		EQU 0x20    ; Definer lysnr til lyskilde
	START_SERIEL	EQU 0x21    ; Start register til seriel kommunikation
	STOP_SERIEL	EQU 0x22    ; Stop register til seriel kommunikation
	MODTAGET_DATA	EQU 0x23    ; Definer register til lagring af modtaget data
	SEND_DATA	EQU 0x24    ; Definér regiter til lagring af data der skal sendes til RPI
	EEPROM_ADRESSE	EQU 0x25    ; Definér register til lagring af ønsket EEPROM adresse
	EEPROM_DATA	EQU 0x26    ; Definér register til lagring af ønkset skrevet data til EEPROM
	ADC_DATA	EQU 0x27    ; Definér register til lagring af data fra ADC
	TEMP_ADC_DATA	EQU 0x28    ; Definér register til lagring af tidligere data fra ADC
	FLAG		EQU 0x29    ; Definér register til lagring af knap
        TÆLLE_REGISTER1 EQU 0x30    ; Definér register til at tælle til reset EEPROM
	TÆLLE_REGISTER2 EQU 0x31    ; Definér register til at tælle til reset EEPROM
		
	#DEFINE BUTTON_CLICKED FLAG,0
	#DEFINE DATA_READY FLAG,1
	#DEFINE RESET_TIME D'20'
	
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
    BSF ANSELA,4    ; Sæt TRISA,4 for ADC-konventering TODO ADC

; ******* PERIPHERAL PIN SELECT - OMDEFINÉR RX OG TX TIL NYE PINS **************************
    ; Krævet for at åbne/lukke for PPS
    BANKSEL INTCON
    BCF INTCON,GIE ; Stop interupts, imens vi omdefinerer pins
    
    BANKSEL PPSLOCK ; Set bank
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
    
    BANKSEL RA5PPS
    MOVLW b'00000010' ; PWM til RA5 (Pin 5)
    MOVWF RA5PPS
    
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
    BSF TRISA,2	    ; Sæt op til input fra knap
    BSF TRISA,4     ; Sæt op til ADC-konvetering fra potentiometer TODO ADC
    BSF TRISA,0	    ; Sæt op til RX (Seriel kommunikation)
    BSF TRISA,1	    ; Sæt op til TX (Seriel komunikation)
    
; ******* PULS-BREDDE MODULATION ***********************************************************
    
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
    MOVLW 0x00 ; Lysnr pakke defineres til at være 0, fra start (Ændres senere) TODO
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
    
    ; Sæt EEPROM data registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVWF EEPROM_DATA ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt EEPROM adresse registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Flyt til bank
    CLRW ; Clear arbejdsregistret

    ; Sæt ADC_DATA registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL ADC_DATA ; Gå til bank
    MOVWF ADC_DATA ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    ; Sæt TEMP_ADC_DATA adresse registret til nul
    MOVLW 0x00 ; Sæt egne definerede registre til 0
    BANKSEL TEMP_ADC_DATA ; Gå til bank
    MOVWF TEMP_ADC_DATA ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    MOVLW RESET_TIME
    BANKSEL TÆLLE_REGISTER2 ; Gå til bank
    MOVWF TÆLLE_REGISTER2 ; Flyt til bank
    CLRW ; Clear arbejdsregistret
    
    CALL GET_DATA_FROM_EEPROM ; Ved initialisering henter vi lagret data fra EEPROM TODO EEPROM
   
    CALL OPDATER_RPI ; DEN LÆSER 255 FRA EEPROM
    
    GOTO MAIN ; Gå til MAIN som tager sig af, hvad lysstyringen skal gøre

; ******* HOVEDPROGRAM **********************************************************************
    
MAIN
   ; Hvis lyskildens microcontroller ikke har et LYSNR, så skal den have tildelt et
    BANKSEL LYSNR ; Gå til bank
    MOVF LYSNR,W ; Flyt lyskildens LYSNR til W
    
    BTFSC STATUS,Z ; Hvis dens LYSNR er 0
	GOTO GET_LYSNR ; Så skal den forespørge RPI'en om et lysnr
    
    GOTO LYSSTADIE ; Ellers, så skal den gå til check for data 
    
; ------- SERIEL TRANSMITTERING AF DATA TIL RASPBERRY PI ---------------------------------------
GET_LYSNR

    CALL CHECK_FOR_DATA_FROM_RPI ; Modtag  et lysnr'et fra RPI'en
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVF MODTAGET_DATA,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL LYSNR ; Gå til bank
    MOVWF LYSNR ; Flyt dataen til LYSNR registret
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    CLRF MODTAGET_DATA ; Vi rydder den modtagede data, da det er anvendt
    
    CALL OPDATER_EEPROM ; Opdatér dataen lagret i EEPROM TODO EEPROM
   
    CALL OPDATER_RPI
    
    GOTO LYSSTADIE ; Gå til MAIN
    
; ------- SERIEL RECIPERING AF DATA FRA RASPBERRY PI ---------------------------------------
LYSSTADIE
    
    ; Denne del af koden sørger for modtagelse af det ønskede lysstadie fra Raspberry Pi'en
    ; og tjekker ofte, om knappen er blevet trykket på
    
    CALL CHECK_FOR_DATA_FROM_RPI ; Tjek om RPI'en har sendt et nyt lysstadie som lyskilden skal køre
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    MOVF MODTAGET_DATA,W ; Flyt det modtagede data til arbejdsregistret
    
    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Flyt dataen ud på LED ved hjælp af Pulse-bredde modulation
    
    BANKSEL MODTAGET_DATA ; Gå til bank
    CLRF MODTAGET_DATA ; Vi rydder den modtagede data, da det er sendt ud til lyset
    
    CALL OPDATER_EEPROM ; Opdatér EEPROM med det nyeste lysstadie TODO EEPROM
    
    GOTO LYSSTADIE ; Bliv ved med at tjekke for nyt lysstadie

; ------- TJEK OM DER ER BLEVET TRYKKET PÅ KNAPPEN ---------------------------------------
    
CHECK_BUTTON
    BANKSEL PORTA
    BTFSS PORTA,2 ; Hvis knappen ikke er trykket
	GOTO BUTTON_CLEAR ; Returnér
	
    CALL DELAY
    
    BANKSEL PORTA
    BTFSS PORTA,2 ; Hvis knappen ikke er trykket
	GOTO BUTTON_CLEAR ; Returnér    
    
    CALL CHECK_EEPROM_RESET_1
	
    BANKSEL FLAG
    BTFSC BUTTON_CLICKED
	RETURN

    BSF BUTTON_CLICKED ; Sæt flag, at knappen er trykket	
    
    ; Ellers, så skal lysstadiet ændres til det modsatte, og EEPROM og RPI skal opdateres
    BANKSEL PWM5DCH ; Gå til bank
    MOVF PWM5DCH,W ; Flyt lysstadie til arbejdsregistret
    
    BTFSC STATUS,Z ; Hvis lysstadiet er nul				
	GOTO TOGGLE_LYSSTADIE_ON ; Så skal LED'en tændes			
    GOTO TOGGLE_LYSSTADIE_OFF ; Hvis ikke, så skal den slukkes		    

BUTTON_CLEAR
    BANKSEL TÆLLE_REGISTER2
    MOVLW RESET_TIME
    MOVWF TÆLLE_REGISTER2
    
    BANKSEL FLAG
    BCF BUTTON_CLICKED ; Ryd flag
    RETURN
    
CHECK_EEPROM_RESET_1
    BANKSEL TÆLLE_REGISTER1
    INCF TÆLLE_REGISTER1,F
    
    BTFSC STATUS,Z  
	GOTO CHECK_EEPROM_RESET2
    RETURN
    
CHECK_EEPROM_RESET2
    
    BANKSEL TÆLLE_REGISTER2
    MOVF TÆLLE_REGISTER2,W
    BANKSEL SEND_DATA ; Gå til bank
    MOVWF SEND_DATA ; Flyt det til SEND_DATA registret for at sende dataen til RPI'en
    
    CALL TRANSMIT_TO_RPI ; Kald transmissions koden, som sørger for at 4 pakker bliver sendt afsted: Start, lysnr, data og stop pakken
    
    BANKSEL TÆLLE_REGISTER2
    DECF TÆLLE_REGISTER2,F
    
    BTFSC STATUS,Z
	GOTO RESET_EEPROM
    RETURN
    
TOGGLE_LYSSTADIE_ON
    MOVLW 0xFF ; Tænd LED på fuld styrke
    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Tænd for LED
    
    CALL OPDATER_EEPROM ; Opdatér EEPROM med det nyeste lysstadie TODO EEPROM
    
    CALL OPDATER_RPI ; Opdatér RPI med det nyeste lysstadie TODO RPI
    
    GOTO LYSSTADIE ; Gå til tjek for nyt lysstadie
    
TOGGLE_LYSSTADIE_OFF
    MOVLW 0x00 ; Sluk LED
    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Flyt ud på LED
    
    CALL OPDATER_EEPROM ; Opdatér EEPROM med det nyeste lysstadie TODO EEPROM
    
    CALL OPDATER_RPI ; Opdatér RPI med det nyeste lysstadie TODO RPI
    
    GOTO LYSSTADIE ; Gå til tjek for nyt lysstadie
	
CHECK_POTENTIOMETER
    ; Select ADC input channel
    BANKSEL ADCON0 ; Gå til bank
    CLRF ADCON0 ; Ryd registret
    BSF ADCON0,4 ; Sæt denne bit for at vælge RA4 som input
    
    CALL DELAY ; Et delay er påkrævet
    
    ; Select ADC conversion clock
    BANKSEL ADCON1 ; Gå til bank
    CLRF ADCON1 ; Ryd registret
    BSF ADCON1,5 ; Ved 32 MHz får vi en TAD på 1 pikosekund
    BSF ADCON1,7 ; Højre justeret: Seks mest betydende bits er sat til 0, når den er færdig
    
    BANKSEL ADCON0 ; Gå til bank
    BSF ADCON0,0 ; Turn on ADC module
    
    ; Configure ADC interupt
    BANKSEL PIR1 ; Gå til bank
    BCF PIR1,ADIF ; Clear ADC flag
    
    BANKSEL PIE1 ; Gå til bank
    BSF PIE1,ADIE ; Enable ADC interrupt
    
    BANKSEL INTCON ; Gå til bank
    BSF INTCON,PEIE ; Skal sættes
    BCF INTCON,GIE ; Skal cleares
    
    ; CALL DELAY
    ; CALL WAIT_FOR_ACQUISITION_TIME ; Wait the required time TODO
    
    ; Start conversion
    BANKSEL ADCON0 ; Gå til bank
    BSF ADCON0,1 ; Set GO/DONE bit
    
    BANKSEL PIR1 ; Gå til bank
    CALL WAIT_ON_ADC ; Wait for the ADC conversion to complete 
    
    ; læs ADC: Vi læser kun det lave register, da vi kun har brug for de 8 bits, istedet for 10. Ellers skulle vi også læse high registret
    BANKSEL ADRESL ; Gå til bank
    MOVF ADRESL,W ; Flyt ADC data til arbejdsregister
    MOVWF ADC_DATA ; Gem data i GPR
    MOVF ADC_DATA,W ; Flyt data ud i arbejdsregistret igen
    
    SUBWF TEMP_ADC_DATA,W ; Træk sidste indlæsning fra, hvis den giver nul, så er dataen ikke ændret
    BTFSC STATUS,Z ; Tjek om dataen er forskellig fra sidste indlæsning
	GOTO ADC_CHANGE_LYSSTADIE ; Dataen er forskellig, derfor udlæser vi den nye data til lyskilden
    
    ; Clear the ADC interrupt flag
    BANKSEL PIR1 ; Gå til bank
    BCF PIR1,ADIF ; Skal ryddes i software
    
    RETURN ; Returnér
    
WAIT_ON_ADC
    BTFSS PIR1,ADIF ; Tjek om ADC er færdig, flag sættes
	GOTO WAIT_ON_ADC ; Vent på den er færdig
    RETURN ; Returnér
    
ADC_CHANGE_LYSSTADIE
    BANKSEL ADC_DATA ; Gå til bank
    MOVF ADC_DATA,W ; Flyt dataen ud i arbejdsregistret
    
    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Flyt dataen ud til lyskilden ved puls-bredde modulation
    
    BANKSEL ADC_DATA ; Gå til bank
    MOVF ADC_DATA,W ; Flyt dataen ud i arbejdsregistret
    BANKSEL TEMP_ADC_DATA ; Gå til bank
    MOVWF TEMP_ADC_DATA ; Flyt til GPR for tidligere data
    
    CALL OPDATER_EEPROM ; Opdatér EEPROM med det nye lysstadie
    
    CALL OPDATER_RPI ; Opdatér RPI'en med det nye lysstadie
    
    ; Clear the ADC interrupt flag
    BANKSEL PIR1 ; Gå til bank
    BCF PIR1,ADIF ; Skal ryddes i software
    
    GOTO LYSSTADIE

; ******* PROGRAM AFSLUTTET ***************************************************************		
    END ; Her slutter programmet