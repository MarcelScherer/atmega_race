;
; time_measurement.asm
;
; Created: 06.08.2018 19:09:13
; Author : Marcel
;


.def temp1 = r16
.def temp2 = r17
.def temp3 = r18

.def flag    = r19
.def mil_sec = r20
.def second  = r21
.def minute  = r22

.org 0x0000
		rjmp main						; Reset Handler jump to main
.org OC1Aaddr
		rjmp timer1_compare				; timer compare handler

.include "lcd-routines.asm"				; LCD-Routinen werden hier eingefügt

main:
	; init stack pointer
	ldi temp1, LOW(RAMEND)				; LOW-Byte der obersten RAM-Adresse
	out SPL, temp1
	ldi temp1, HIGH(RAMEND)				; HIGH-Byte der obersten RAM-Adresse
	out SPH, temp1
	
	; init Prot D for Display actuation
	ldi temp1, 0xFF						; Port D = Ausgang
	out DDRD, temp1

	; init and clear display
	rcall lcd_init						; Display initialisieren
	rcall lcd_clear						; Display löschen
	
	; timer initialisieren
	ldi temp1, high( 40000 - 1 )
	out OCR1AH, temp1
	ldi temp1, low( 40000 - 1 ) 
	ldi temp1, (1 << WGM12) | (1 << CS12) | (1 << CS10) ; ctc modus einschalten, teiler auf 1
	out TCCR1B, temp1 
	ldi temp1, 1 << OCIE1A				; OCIE1A: Interrupt bei Timer compare
	out TIMSK, temp1

	;initialisiere variable
	clr mil_sec
	clr second
	clr minute
	ldi flag,1

	sei									; schalte interrupts scharf

loop:
	cpi flag,0							; teste ob flag gleich 0
	breq loop							; wenn 0 springe zu 0
	ldi flag,0							; wenn nicht setzte flag wieder auf 0 und ...

	rcall lcd_clear						; display anzeige löschen
	mov temp1,minute
	rcall lcd_number					; die minuten im display angeben
	ldi temp1, ':'
	rcall lcd_data
	mov temp1,second
	rcall lcd_number					; die sekunde im display angeben
	ldi temp1, ':'
	rcall lcd_data
	mov temp1,mil_sec
	rcall lcd_number					; die milisecond im display angeben

	rjmp loop

; function calculate new timer value
timer1_compare:
	push temp1							; save temp1 to stack
	in temp1,sreg						; save SREG in temp1

	inc mil_sec							; increment milisecond value
	cpi mil_sec, 100					; Wenn dies nicht der 100 interrup ist ...
	brne end_isr						; springe zu end_isr

	clr	mil_sec							; setzte milisecond zu 0
	inc second							; increment second value
	cpi second, 60						; Wenn dies nicht der 60 interrup ist ...
	brne end_isr						; springe zu ausgabe

	clr	second							; setzte second zu 0
	inc minute							; increment minute value

ausgabe:
	ldi flag, 1							; setze flag das Display aktualisiert werden muss

end_isr:					
	out sreg,temp1						; zurück kopieren von sreg
	pop temp1							; zurückholen des temp1 values vom stack
	reti

