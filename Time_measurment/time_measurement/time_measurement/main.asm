;
; time_measurement.asm
;
; Created: 06.08.2018 19:09:13
; Author : Marcel
;

.equ STA_START     = 0
.equ STA_ROUND_1   = 1
.equ DELAY_TIME    = 2

.def temp1 = r16
.def temp2 = r17
.def temp3 = r18
.def Flag = r19
.def SubCount    = r21
.def mil_sec     = r22
.def second      = r23
.def minute      = r24
.def timer_state = r25
#define delay_timer XL ;  use register r26

.org 0x0000
	rjmp main							; reset hanlder -> jump to main
.org OC1Aaddr
	rjmp timer1_compare					; Timer Compare Handler -> jump to timer1_compare

.include "lcd-routines.asm"				; include lcd library

main:
	; initialise stack pointer
	ldi temp1, LOW(RAMEND)				
	out SPL, temp1
	ldi temp1, HIGH(RAMEND)
	out SPH, temp1

	; init and clear display
	rcall lcd_init
	rcall lcd_clear

	; init in- and output pints
	ldi temp1,0b00000000
	out DDRB,temp1						; config all PBx -Pins as input

	; init timer for 100 interrupts per second
	ldi temp1, high( 40000 - 1 )
	out OCR1AH, temp1
	ldi temp1, low( 40000 - 1 )
	out OCR1AL, temp1
	ldi temp1, ( 1 << WGM12 ) | ( 1 << CS10 )
	out TCCR1B, temp1
	ldi temp1, 1 << OCIE1A				; OCIE1A: Interrupt bei Timer Compare
	out TIMSK, temp1

	; clear all variable
	clr minute 
	clr second
	clr mil_sec
	clr SubCount
	ldi Flag,1							; update display
	ldi timer_state,STA_START
	clr delay_timer;
	sei

loop:
	sbic PINB, 0						; scip jmp loop_1 if bit 0 is GND
	rjmp loop_1							; jump to loop_1
	cpi delay_timer,0					; compare delay timer with zero
	brne loop_1							; if not zero branch to loop_1 else ...
	ldi delay_timer,DELAY_TIME			; set timer to DELAY_TIME
	inc timer_state						; increment to next state
loop_1:
	cpi flag,0
	breq loop							; check if flag for update display is active
	ldi flag,0							; reset flag

	rcall lcd_clear						; claer lcd display
	ldi ZL, LOW(text*2)					; load text flash address to z-pointer
	ldi ZH, HIGH(text*2)	
	rcall lcd_flash_string				; get string to display

	mov temp1, minute					; write minutes on display
	rcall lcd_number
	ldi temp1, ':'						; write ':' on display
	rcall lcd_data
	mov temp1, second					; write seconds on display
	rcall lcd_number
	ldi temp1, '.'						; write ':' on display
	rcall lcd_data
	ldi temp1, '0'
	add temp1, mil_sec
	rcall lcd_data

	rjmp loop

; Timer 1 Output Compare Handler
timer1_compare: 
	push temp1							; store temp1 on stack
	in temp1,sreg						; sotre SREG in reg temp1

	cpi timer_state,STA_ROUND_1			; check if state is STA_ROUND_1
	brne end_isr						; if state is not STA_ROUND_1 jump to end_isr

	inc SubCount						; increment subCount
	cpi SubCount, 10					; compare Subcount with 10
	brne end_isr						; if not equal 10 go to end_isr else ...
	clr SubCount						; reset Subcount
	inc mil_sec							; increment mil_sec (0.1 second)
	cpi mil_sec, 10						; compare mil_sec (0.1 second) with 10
	brne Ausgabe						; if not equal 10 go to end_isr else ...
	clr mil_sec							; reset mil_sec (0.1 second)
	inc second							; increment seconds
	cpi delay_timer,0					; compare delay timer, if not zero ...
	breq timer1_compare_1
	dec delay_timer						; ... decrement delay timer
timer1_compare_1: 
	cpi second, 60						; compare second witch 60
	brne Ausgabe						; if net equal 60 go to Ausgabe else...
	clr second		  					; reset second
	inc minute							; increment minute
	cpi minute, 99						; compare minute with 99
	brne Ausgabe						; if not equal 99 go to Ausgabe else ...
	clr minute							; clear minutes
Ausgabe:
	ldi flag,1							; set flag for update display
end_isr:
	out sreg,temp1						; restore reg sreg form reg temp1
	pop temp1							; restore temp1 from stack
	reti

text:
	.db "Time: ",0,0					; string array for displaying -> second 0 for alignment