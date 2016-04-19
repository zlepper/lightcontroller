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
    
    BANKSEL NVMCON1 ; Gå til bank
    CALL WAIT_ON_EEPROM ; Vent på, at den er klar til at skrive til EEPROM
    
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1
    BSF NVMCON1,NVMREGS
    BSF NVMCON1,WREN
    
    BANKSEL NVMDATH ; Gå til bank
    MOVLW B'00000' ; Flyt de mest betydelige bits i dataen
    MOVWF NVMDATH ; til NVMDATL
    
    BANKSEL EEPROM_DATA
    MOVF EEPROM_DATA,W
    BANKSEL NVMDATL ; Gå til bank
    MOVWF NVMDATL ; til NVMDATH
    
    BANKSEL EEPROM_DATA ; Gå til bank
    CLRF EEPROM_DATA ; Ryd dataen, da den er blevet anvendt
    
    BANKSEL NVMADRH ; Gå til bank
    MOVLW B'11100' ; Flyt de mest betydelige bits i adressen (Over 7000)
    MOVWF NVMADRH ; til NVMADRH
    
    BANKSEL EEPROM_ADRESSE
    MOVF EEPROM_ADRESSE,W ; Flyt de mest betydelige bits i adressen
    
    BANKSEL NVMADRL ; Gå til bank
    MOVWF NVMADRL ; til NVMADRL
    
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    CLRF EEPROM_ADRESSE ; Ryd dataen, da den er blevet anvendt
    
;   Følgende kode er krævet EEPROM sekvens (Se side 112 i databladet)
    BANKSEL NVMCON1
    BSF NVMCON1,WREN ; Enable write/erase
    MOVLW 0x55 ; Load 0x55
    BCF INTCON,GIE ; Recommended so sequence is not interrupted
    MOVWF NVMCON2 ; Step 1: Load 0x55 into NVMCON2
    MOVLW 0xAA ; Step 2: Load W with 0xAA
    MOVWF NVMCON2 ; Step 3: Load AAh into NVMCON2
    BSF NVMCON1,WR ; Step 4: Set WR bit to begin write
    BSF INTCON,GIE ; Re-enable interrupts
    
    RETURN ; Skrivning til EEPROM er færdig, vend tilbage til hovedprogram
    
EEPROM_READ
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1
    BSF NVMCON1,NVMREGS
    BSF NVMCON1,WREN
    
    BANKSEL NVMADRH ; Gå til bank
    MOVLW B'11100' ; Flyt de mest betydelige bits i adressen (Over 7000)
    MOVWF NVMADRH ; til NVMADRL
    
    BANKSEL EEPROM_ADRESSE
    MOVF EEPROM_ADRESSE,W
 
    BANKSEL NVMADRL ; Gå til bank
    MOVWF NVMADRL ; til NVMADRH
    
    BANKSEL NVMCON1
    BSF	NVMCON1,RD
    
    BANKSEL NVMDATL
    MOVF NVMDATL,W
    BANKSEL EEPROM_DATA
    MOVWF EEPROM_DATA
    
    CLRF EEPROM_ADRESSE
    
    RETURN
		
WAIT_ON_EEPROM
    BTFSC NVMCON1,WR ; Gå ud af loop når den er klar til at skrive til EEPROM
	GOTO WAIT_ON_EEPROM ; Vent indtil den er klar
    BCF NVMCON1,WREN ; Ryd WREN for at indikere, at vi den er klar til at skrive til EEPROM
    RETURN ; Returnér
; ******* PROGRAM AFSLUTTET ***************************************************************		


