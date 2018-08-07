;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; LCD-Routinen								   ;;
;; ============								   ;;
;; (c)andreas-s@web.de						   ;;
;;											   ;;
;; 4bit-Interface							   ;;
;; DB4-DB7: PD0-PD3							   ;;
;; RS: PD4									   ;;
;; E: PD5									   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;sendet ein Datenbyte an das LCD
lcd_data:
	mov temp2, temp1					;"Sicherungskopie" f�r
										; die �bertragung des 2.Nibbles
	swap temp1							; Vertauschen
	andi temp1, 0b00001111				; oberes Nibble auf Null setzen
	sbr temp1, 1<<4						; entspricht 0b00010000 (Anm.1)
	out PORTD, temp1					; ausgeben
	rcall lcd_enable					; Enable-Routine aufrufen
										; 2. Nibble, kein swap da es schon
										; an der richtigen stelle ist
	andi temp2, 0b00001111				; obere H�lfte auf Null setzen
	sbr temp2, 1<<4						; entspricht 0b00010000
	out PORTD, temp2					; ausgeben
	rcall lcd_enable					; Enable-Routine aufrufen
	rcall delay50us						; Delay-Routine aufrufen
	ret									; zur�ck zum Hauptprogramm

; Eine Zahl aus dem Register temp1 ausgeben
lcd_number:
	push temp2							; register sichern,
										; wird f�r Zwsichenergebnisse gebraucht
	ldi temp2, '0'
lcd_number_10:
	subi temp1, 10						; abz�hlen wieviele Zehner in
	brcs lcd_number_1					; der Zahl enthalten sind
	inc temp2
	rjmp lcd_number_10
lcd_number_1:
	rcall lcd_data						; die Zehnerstelle ausgeben
	subi temp1, -10						; 10 wieder dazuz�hlen, da die
										; vorhergehende Schleife 10 zuviel
										; abgezogen hat
										; das Subtrahieren von -10
										; = Addition von +10 ist ein Trick
										; da kein addi Befehl existiert
	ldi temp2, '0'						; die �brig gebliebenen Einer
	add temp1, temp2					; noch ausgeben
	rcall lcd_data
	pop temp2							; Register wieder herstellen
	ret

; sendet einen Befehl an das LCD
lcd_command:							; wie lcd_data, nur RS=0
	mov temp2, temp1
	swap temp1
	andi temp1, 0b00001111
	out PORTD, temp1
	rcall lcd_enable
	andi temp2, 0b00001111
	out PORTD, temp2
	rcall lcd_enable
	rcall delay50us
	ret

; erzeugt den Enable-Puls
;
; Bei h�herem Takt (>= 8 MHz) kann es notwendig sein, vor dem Enable High
; 1-2 Wartetakte (nop) einzuf�gen.
lcd_enable:
	sbi PORTD, 5						; Enable high
	nop									; 3 Taktzyklen warten
	nop
	nop
	cbi PORTD, 5						; Enable wieder low
	ret									; Und wieder zur�ck

; Pause nach jeder �bertragung
delay50us:								; 50us Pause
	ldi temp1, $42
delay50us_:	dec temp1
	brne delay50us_
	ret									; wieder zur�ck

; L�ngere Pause f�r manche Befehle
delay5ms:								; 5ms Pause
	ldi temp1, $21
WGLOOP0: ldi temp2, $C9
WGLOOP1: dec temp2
	brne WGLOOP1
	dec temp1
	brne WGLOOP0
	ret									; wieder zur�ck

; Initialisierung: muss ganz am Anfang des Programms aufgerufen werden
lcd_init:
	ldi temp3,50
powerupwait:
	rcall delay5ms
	dec temp3
	brne powerupwait
	ldi temp1, 0b00000011				; muss 3mal hintereinander gesendet
	out PORTD, temp1					; werden zur Initialisierung
	rcall lcd_enable					; 1
	rcall delay5ms
	rcall lcd_enable					; 2
	rcall delay5ms
	rcall lcd_enable					; und 3!
	rcall delay5ms
	ldi temp1, 0b00000010				; 4bit-Modus einstellen
	out PORTD, temp1
	rcall lcd_enable
	rcall delay5ms
	ldi temp1, 0b00101000				; 4Bit / 2 Zeilen / 5x8
	rcall lcd_command
	ldi temp1, 0b00001100				; Display ein / Cursor aus / kein
										; Blinken
	rcall lcd_command
	ldi temp1, 0b00000100				; inkrement / kein Scrollen
	rcall lcd_command
	ret

; Sendet den Befehl zur L�schung des Displays
lcd_clear:
	ldi temp1, 0b00000001				; Display l�schen
	rcall lcd_command
	rcall delay5ms
	ret

; Sendet den Befehl: Cursor Home
lcd_home:
	ldi temp1, 0b00000010				; Cursor Home
	rcall lcd_command
	rcall delay5ms
	ret