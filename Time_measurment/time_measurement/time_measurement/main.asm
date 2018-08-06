;
; time_measurement.asm
;
; Created: 06.08.2018 19:09:13
; Author : Marcel
;


.def temp1 = r16
.def temp2 = r17
.def temp3 = r18

	ldi temp1, LOW(RAMEND)				; LOW-Byte der obersten RAM-Adresse
	out SPL, temp1
	ldi temp1, HIGH(RAMEND)				; HIGH-Byte der obersten RAM-Adresse
	out SPH, temp1
	ldi temp1, 0xFF						; Port D = Ausgang
	out DDRD, temp1
	rcall lcd_init						; Display initialisieren
	rcall lcd_clear						; Display löschen
	ldi temp1, 'T'						; Zeichen anzeigen
	rcall lcd_data
	ldi temp1, 'e'						; Zeichen anzeigen
	rcall lcd_data
	ldi temp1, 's'						; Zeichen anzeigen
	rcall lcd_data
	ldi temp1, 't'						; Zeichen anzeigen
	rcall lcd_data
loop:
	rjmp loop

.include "lcd-routines.asm"				; LCD-Routinen werden hier eingefügt

