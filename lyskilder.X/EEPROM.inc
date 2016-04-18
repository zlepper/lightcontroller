;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		EEPROM.inc
;   Dato:		16.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Dette program sørger for, at der kan blive skrevet til EEPROM fra hovedprogrammet af
	
; ******* HOVEDPROGRAM ********************************************************************

; -------- Skriv data til EEPROM ---------------------------------------------------------
    ; Se side 113 i databladet for mere information om skrvning til EEPROM
EEPROM_WRITE
    BANKSEL NVMCON1
    CHECK_READY BTFSC NVMCON1,WR ; Tjek om EEPROM er klar til at blive skrevet til
	GOTO CHECK_READY
	
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BSF NVMCON1,NVMREGS
    BSF NVMCON1,WREN
	
    ; Data givet i hovedprogram (LYSNR 1)
    BANKSEL NVMDATH ; Gå til bank
    MOVLW B'00000100' ; Flyt de mest betydelige bits i dataen
    MOVWF NVMDATH ; til NVMDATH
    
    BANKSEL NVMDATL ; Gå til bank
    MOVLW B'00000000' ; Flyt de mindst betydelige bits i dataen
    MOVWF NVMDATL ; til NVMDATL
    
    BANKSEL NVMADRH ; Gå til bank
    MOVLW 0xFF ; Flyt de mest betydelige bits i adressen
    MOVWF NVMADRH ; til NVMADRH
    
    BANKSEL NVMADRL ; Gå til bank
     MOVLW 0x70 ; Flyt de mindst betydelige bits i adressen
    MOVWF NVMADRL ; til NVMADRL
    
;   Følgende kode er krævet EEPROM sekvens (Se side 112 i databladet)
    BANKSEL NVMCON1
    BSF NVMCON1,WREN ; Enable write/erase
    MOVLW 0x55 ; Load 0x55
    BCF INTCON,GIE ; Recommended so sequence is not interrupted
    MOVWF NVMCON2 ; Step 1: Load 0x55 into NVMCON2
    MOVLW 0xAA ; Step 2: Load W with 0xAA
    MOVWF NVMCON2 ; Step 3: Load AAh into NVMCON2
    BSF NVMCON1,WR ; Step 4: Set WR bit to begin write/erase
    BSF INTCON,GIE ; Re-enable interrupts
	
    RETURN ; Skrivning til EEPROM er færdig, vend tilbage til hovedprogram

; ******* PROGRAM AFSLUTTET ***************************************************************		






