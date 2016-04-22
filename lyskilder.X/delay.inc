;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	delay.inc
;   Dato:	16.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Dette program giver et delay på 1 millisekund til brug i hovedprogrammet.
    
; ****** DEFINERING AF REGISTRE ***********************************************************
    Delay1 EQU 0xFF
    Delay2 EQU 0xFE
    Delay3 EQU 0xFD
; ******* HOVEDPROGRAM ********************************************************************
DELAY ; Delay funktion
 
 ; -------  Start på ét millisekund delay rutine ------------------------------------------
    MOVLW	h'95' ; Tidsforsinkelse beregnet ved fosc på 32 MHz			
    MOVWF	Delay1 ; Sættes til den mindst betydende byte
    MOVLW	h'F5' ; Tidsforsinkelse beregnet ved fosc på 32 MHz		
    MOVWF	Delay2 ; Sættes til den mest betydende byte
    
; Udfør delay'et   
    BANKSEL Delay1
LOOP1	INCFSZ	Delay1,F 
	GOTO	LOOP1
	INCF	Delay1,F
	INCFSZ	Delay2,F
	GOTO	LOOP1
; -------  Slut på 1 millisekund delay rutine ---------------------------------------------
	
    RETURN

; ******* PROGRAM AFSLUTTET ***************************************************************


