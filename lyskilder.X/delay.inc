;**** FIL OPLYSNINGER ******************************************************************
;   Fil:	delay.inc
;   Dato:	16.04.2016
;   Forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Dette program giver et delay p� 1 millisekund til brug i hovedprogrammet.
    
; ****** DEFINERING AF REGISTRE ***********************************************************
    Delay1 EQU 0xFF
    Delay2 EQU 0xFE
    Delay3 EQU 0xFD
; ******* HOVEDPROGRAM ********************************************************************
DELAY ; Delay funktion
 
 ; -------  Start p� �t millisekund delay rutine ------------------------------------------
    MOVLW	h'95' ; Tidsforsinkelse beregnet ved fosc p� 32 MHz			
    MOVWF	Delay1 ; S�ttes til den mindst betydende byte
    MOVLW	h'F5' ; Tidsforsinkelse beregnet ved fosc p� 32 MHz		
    MOVWF	Delay2 ; S�ttes til den mest betydende byte
    
; Udf�r delay'et   
    BANKSEL Delay1
LOOP1	INCFSZ	Delay1,F 
	GOTO	LOOP1
	INCF	Delay1,F
	INCFSZ	Delay2,F
	GOTO	LOOP1
; -------  Slut p� 1 millisekund delay rutine ---------------------------------------------
	
    RETURN

; ******* PROGRAM AFSLUTTET ***************************************************************


