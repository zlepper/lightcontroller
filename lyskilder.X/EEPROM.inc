;**** FIL OPLYSNINGER ******************************************************************
;	Fil:		EEPROM.inc
;   Dato:		16.04.2016
;	forfatter:	Mathias Bejlegaard Madsen & Rasmus Hjelmberg Duemose Hansen

; ****** BESKRIVELSE **********************************************************************
;   beskrivelse:
;   Denne fil s�rger for, at der kan blive skrevet til / l�st fra EEPROM
	
; ******* HOVEDPROGRAM ********************************************************************

; -------- Skriv data til EEPROM ---------------------------------------------------------
    ; Se side 113 i databladet for mere information om skrvning til EEPROM
EEPROM_WRITE
    
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1
    BSF NVMCON1,NVMREGS
    BSF NVMCON1,WREN
    BCF NVMCON1,LWLO ; Ikke brugt til EEPROM skrivning
    
    ; S�t adressen til EEPROM
    BANKSEL NVMADRH ; G� til bank
    MOVLW 0x70 ; Flyt de mest betydelige bits i adressen til EEPROM til dette register
    MOVWF NVMADRH ; til NVMADRH
    
    BANKSEL EEPROM_ADRESSE
    MOVF EEPROM_ADRESSE,W ; Flyt de mindst betydelige bits i adressen til EEPROM til dette register
    BANKSEL NVMADRL ; G� til bank
    MOVWF NVMADRL ; til NVMADRL
    
    ; HER S�TTES DATAEN SOM SKAL I EEPROM
    BANKSEL EEPROM_DATA
    MOVF EEPROM_DATA,W ; Hent dataen som skal skrives til EEPROM
    BANKSEL NVMDATL ; G� til bank
    MOVWF NVMDATL ; Flyt det til registret
    
    ;   F�lgende kode er kr�vet EEPROM sekvens for at �bne for skrivningen til EEPROM (Se side 112 i databladet)
    BANKSEL INTCON
    BCF INTCON,GIE ; Recommended so sequence is not interrupted
    
    BANKSEL NVMCON2 ; G� til bank
    MOVLW 0x55 ; Load 0x55
    MOVWF NVMCON2 ; Step 1: Load 0x55 into NVMCON2
    MOVLW 0xAA ; Step 2: Load W with 0xAA
    MOVWF NVMCON2 ; Step 3: Load AAh into NVMCON2
    BSF NVMCON1,WR ; Step 4: Set WR bit to begin write
    
    CALL WAIT_ON_EEPROM ; Vent p� at den er f�rdig med at skrive
    
    BANKSEL INTCON ; G� til bank
    BSF INTCON,GIE ; Re-enable interrupts
    
    BANKSEL NVMCON1 ; G� til bank
    BCF NVMCON1,WREN ; Sl� skrivning fra til EEPROM
    
    BANKSEL EEPROM_ADRESSE ; G� til bank
    CLRF EEPROM_ADRESSE ; Ryd dataen, da den er blevet anvendt
    
    BANKSEL EEPROM_DATA ; G� til bank
    CLRF EEPROM_DATA ; Ryd dataen, da den er blevet anvendt
    
    RETURN ; Skrivning til EEPROM er f�rdig, vend tilbage til hovedprogram
    
EEPROM_READ
    ; Set the NVMREGS and WREN bits of the NVMCON1 register
    BANKSEL NVMCON1 ; G� til bank
    BSF NVMCON1,NVMREGS
    
    ; S�t adressen til EEPROM
    BANKSEL NVMADRH ; G� til bank
    MOVLW 0x70 ; Flyt de mest betydelige bits i adressen til EEPROM
    MOVWF NVMADRH ; til NVMADRL
    
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVF EEPROM_ADRESSE,W ; Flyt data til arbejdsregistret
    BANKSEL NVMADRL ; G� til bank
    MOVWF NVMADRL ; til NVMADRL
    
    ; Start skrivning
    BANKSEL NVMCON1 ; G� til bank
    BSF	NVMCON1,RD ; Start skrivning
    
    BANKSEL NVMDATL ; G� til bank
    MOVF NVMDATL,W ; Hent dataen fra EEPROM
    BANKSEL EEPROM_DATA ; G� til bank
    MOVWF EEPROM_DATA ; Lagrer data fra EEPROM i GPR
    
    BANKSEL EEPROM_ADRESSE ; G� til bank
    CLRF EEPROM_ADRESSE ; Ryd dette register
    
    RETURN ; Return�r
		
WAIT_ON_EEPROM ; Vent p�, at EEPROM er f�rdig
    BTFSC NVMCON1,WR ; G� ud af loop n�r den er f�rdig med at skrive til EEPROM
	GOTO WAIT_ON_EEPROM ; Vent indtil den er klar
    BCF NVMCON1,WREN ; Ryd WREN for at indikere, at den er f�rdig med at skrive til EEPROM
    RETURN ; Return�r
    
; ------- HENT DATA FRA EEPROM VED START ---------------------------------------------

GET_DATA_FROM_EEPROM
    ; Hent LYSNR fra EEPROM
    MOVLW 0x01 ; G� til adressen, hvor LYSNR'eret befinder sig
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    CALL EEPROM_READ ; Hent dataen p� overst�ende adresse
    
    BANKSEL EEPROM_DATA ; G� til bank
    MOVF EEPROM_DATA,W ; Flyt dataen ud i arbejdsregistret

    BANKSEL LYSNR ; G� til bank
    MOVWF LYSNR ; Flyt dataen fra EEPRROM til LYSNR register
    
    BANKSEL EEPROM_DATA ; G� til bank
    CLRF EEPROM_DATA ; Slet den modtagede data, da vi er f�rdig med at bruge den
    
    ; Hent lysstadie fra EEPROM
    MOVLW 0x02 ; G� til adressen, hvor LYSNR'eret befinder sig
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    CALL EEPROM_READ ; Hent dataen p� overst�ende adresse
    
    BANKSEL EEPROM_DATA ; G� til bank
    MOVF EEPROM_DATA,W ; Flyt dataen ud i arbejdsregistret

    BANKSEL PWM5DCH ; G� til bank
    MOVWF PWM5DCH ; Flyt dataen ud p� LED ved hj�lp af Pulse-bredde modulation
    BANKSEL EEPROM_DATA ; G� til bank
    CLRF EEPROM_DATA ; Slet den modtagede data, da vi er f�rdig med at bruge den
    
    RETURN ; Return�r
    
; ------- OPDATER DATAEN I EEPROM  --------------------------------------------------------
OPDATER_EEPROM
    ; Opdater LYSNR
    MOVLW 0x01 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    BANKSEL LYSNR ; G� til bank
    MOVF LYSNR,W ; Flyt dataen til arbejdsregistret
    
    BANKSEL EEPROM_DATA ; G� til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen p� overst�ende adresse
    
    ; Opdater lysstadie
    MOVLW 0x02 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    BANKSEL PWM5DCH ; G� til bank
    MOVF PWM5DCH,W ; Flyt dataen til arbejdsregistret
    
    BANKSEL EEPROM_DATA ; G� til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen p� overst�ende adresse
    
    RETURN ; Return�r
  
; ------- RESET DATAEN I EEPROM  --------------------------------------------------------
RESET_EEPROM
    
    ; Opdater LYSNR
    MOVLW 0x01 ; Flyt dataen til arbejdsregistret
    BANKSEL EEPROM_ADRESSE ; G� til bank
    MOVWF EEPROM_ADRESSE ; Lagrer dataen i registret
    
    MOVLW 0x00 ; Ryd dataen, ved blot at s�tte det til nul
    BANKSEL EEPROM_DATA ; G� til bank
    MOVWF EEPROM_DATA ; Lagrer dataen i registret
    
    CALL EEPROM_WRITE ; Hent dataen p� overst�ende adresse
    
    MOVLW RESET_TIME ; Nulstil t�lle registret
    BANKSEL T�LLE_REGISTER2 ; G� til bank
    MOVWF T�LLE_REGISTER2 ; Flyt til bank
    
    GOTO init
    
; ******* PROGRAM AFSLUTTET ***************************************************************		


