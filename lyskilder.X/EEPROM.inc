;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		EEPROM.inc
;   Dato:		16.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil sørger for, at der kan blive skrevet til / læst fra EEPROM
	
; ******* HOVEDPROGRAM ********************************************************************

; -------- Skriv data til EEPROM ---------------------------------------------------------
    ; Se side 113 i databladet for mere information om skrvning til EEPROM
EEPROM_WRITE
    
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1
    BSF NVMCON1,NVMREGS
    BSF NVMCON1,WREN
    BCF NVMCON1,LWLO ; Ikke brugt til EEPROM skrivning
    
    ; Sæt adressen til EEPROM
    BANKSEL NVMADRH ; Gå til bank
    MOVLW 0x70 ; Flyt de mest betydelige bits i adressen til EEPROM til dette register
    MOVWF NVMADRH ; til NVMADRH
    
    BANKSEL EEPROM_ADRESSE
    MOVF EEPROM_ADRESSE,W ; Flyt de mindst betydelige bits i adressen til EEPROM til dette register
    BANKSEL NVMADRL ; Gå til bank
    MOVWF NVMADRL ; til NVMADRL
    
    ; HER SÆTTES DATAEN SOM SKAL I EEPROM
    BANKSEL EEPROM_DATA
    MOVF EEPROM_DATA,W ; Hent dataen som skal skrives til EEPROM
    BANKSEL NVMDATL ; Gå til bank
    MOVWF NVMDATL ; Flyt det til registret
    
    ;   Følgende kode er krævet EEPROM sekvens for at åbne for skrivningen til EEPROM (Se side 112 i databladet)
    BANKSEL INTCON
    BCF INTCON,GIE ; Recommended so sequence is not interrupted
    
    BANKSEL NVMCON2 ; Gå til bank
    MOVLW 0x55 ; Load 0x55
    MOVWF NVMCON2 ; Step 1: Load 0x55 into NVMCON2
    MOVLW 0xAA ; Step 2: Load W with 0xAA
    MOVWF NVMCON2 ; Step 3: Load AAh into NVMCON2
    BSF NVMCON1,WR ; Step 4: Set WR bit to begin write
    
    CALL WAIT_ON_EEPROM ; Vent på at den er færdig med at skrive
    
    BANKSEL INTCON ; Gå til bank
    BSF INTCON,GIE ; Re-enable interrupts
    
    BANKSEL NVMCON1 ; Gå til bank
    BCF NVMCON1,WREN ; Slå skrivning fra til EEPROM
    
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    CLRF EEPROM_ADRESSE ; Ryd dataen, da den er blevet anvendt
    
    BANKSEL EEPROM_DATA ; Gå til bank
    CLRF EEPROM_DATA ; Ryd dataen, da den er blevet anvendt
    
    RETURN ; Skrivning til EEPROM er færdig, vend tilbage til hovedprogram
    
EEPROM_READ
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1 ; Gå til bank
    BSF NVMCON1,NVMREGS
    
    ; Sæt adressen til EEPROM
    BANKSEL NVMADRH ; Gå til bank
    MOVLW 0x70 ; Flyt de mest betydelige bits i adressen til EEPROM
    MOVWF NVMADRH ; til NVMADRL
    
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVF EEPROM_ADRESSE,W ; Flyt data til arbejdsregistret
    BANKSEL NVMADRL ; Gå til bank
    MOVWF NVMADRL ; til NVMADRL
    
    ; Start skrivning
    BANKSEL NVMCON1 ; Gå til bank
    BSF	NVMCON1,RD ; Start skrivning
    
    BANKSEL NVMDATL ; Gå til bank
    MOVF NVMDATL,W ; Hent dataen fra EEPROM
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVWF EEPROM_DATA ; Lagrer data fra EEPROM i GPR
    
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    CLRF EEPROM_ADRESSE ; Ryd dette register
    
    RETURN ; Returnér
		
WAIT_ON_EEPROM ; Vent på, at EEPROM er færdig
    BTFSC NVMCON1,WR ; Gå ud af loop når den er færdig med at skrive til EEPROM
	GOTO WAIT_ON_EEPROM ; Vent indtil den er klar
    BCF NVMCON1,WREN ; Ryd WREN for at indikere, at den er færdig med at skrive til EEPROM
    RETURN ; Returnér
    
; ------- HENT DATA FRA EEPROM VED START ---------------------------------------------

GET_DATA_FROM_EEPROM
    ; Hent LYSNR fra EEPROM
    MOVLW 0x01 ; Gå til adressen, hvor LYSNR'eret befinder sig
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    CALL EEPROM_READ ; Hent dataen på overstående adresse
    
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVF EEPROM_DATA,W ; Flyt dataen ud i arbejdsregistret

    BANKSEL LYSNR ; Gå til bank
    MOVWF LYSNR ; Flyt dataen fra EEPRROM til LYSNR register
    
    BANKSEL EEPROM_DATA ; Gå til bank
    CLRF EEPROM_DATA ; Slet den modtagede data, da vi er færdig med at bruge den
    
    ; Hent lysstadie fra EEPROM
    MOVLW 0x02 ; Gå til adressen, hvor LYSNR'eret befinder sig
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    CALL EEPROM_READ ; Hent dataen på overstående adresse
    
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVF EEPROM_DATA,W ; Flyt dataen ud i arbejdsregistret

    BANKSEL PWM5DCH ; Gå til bank
    MOVWF PWM5DCH ; Flyt dataen ud på LED ved hjælp af Pulse-bredde modulation
    BANKSEL EEPROM_DATA ; Gå til bank
    CLRF EEPROM_DATA ; Slet den modtagede data, da vi er færdig med at bruge den
    
    RETURN ; Returnér
    
; ------- OPDATER DATAEN I EEPROM  --------------------------------------------------------
OPDATER_EEPROM
    ; Opdater LYSNR
    MOVLW 0x01 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    BANKSEL LYSNR ; Gå til bank
    MOVF LYSNR,W ; Flyt dataen til arbejdsregistret
    
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen på overstående adresse
    
    ; Opdater lysstadie
    MOVLW 0x02 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    BANKSEL PWM5DCH ; Gå til bank
    MOVF PWM5DCH,W ; Flyt dataen til arbejdsregistret
    
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen på overstående adresse
    
    RETURN ; Returnér
  
; ------- RESET DATAEN I EEPROM  --------------------------------------------------------
RESET_EEPROM
    
    ; Opdater LYSNR
    MOVLW 0x01 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; Gå til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    MOVLW 0x00 ; Ryd dataen, ved blot at sætte det til nul
    BANKSEL EEPROM_DATA ; Gå til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen på overstående adresse
    
    MOVLW RESET_TIME ; Nulstil tælle registret
    BANKSEL TÆLLE_REGISTER2 ; Gå til bank
    MOVWF TÆLLE_REGISTER2 ; Flyt til bank
    
    GOTO init
    
; ******* PROGRAM AFSLUTTET ***************************************************************		


