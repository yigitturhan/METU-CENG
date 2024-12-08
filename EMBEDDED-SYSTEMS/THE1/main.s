PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF       ; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF      ; Disable In-Circuit Debugger

GLOBAL var1
GLOBAL var2
GLOBAL var3
GLOBAL pc
GLOBAL pb
GLOBAL re0
GLOBAL re1
GLOBAL portb_enabled
GLOBAL portc_enabled

PSECT udata_acs
var1:
    DS 1
var2:
    DS 1 
var3:
    DS 1 
pb:
    DS 1
pc:
    DS 1
re0:
    DS 1
re1:  
    DS 1
portb_enabled:
    DS 1
portc_enabled:
    DS 1


PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE
main:
    clrf var1	
    clrf pb
    clrf pc
    clrf portb_enabled
    clrf portc_enabled
    clrf var2
    clrf var3
    movlw 7
    movwf pc; pc = 7
    clrf re0
    clrf re1
    
    ; PORTB
    ; LATB
    ; TRISB determines whether the port is input/output
    ; set output ports
    clrf TRISB
    clrf TRISC
    clrf TRISD
    setf TRISE ; PORTE is input
    
    movlw 00001111B
    movwf TRISA
    
    setf PORTB
    setf LATC ; light up all pins in PORTC
    setf LATD
    
    movlw 6
    movwf var3
    loop:
	call busy_wait
	decf var3
	bnz loop
	
    clrf var1
    clrf var2
    clrf var3
    
    clrf PORTB
    clrf LATC ; light up all pins in PORTC
    clrf LATD
    
main_loop:
    call check_buttons
    call blink_off
    call update_portb
    call update_portc
    call blink_on
    call update_portb
    call update_portc
    goto main_loop
    
blink_on:
    movlw 3
    movwf var3
    bsf PORTD, 0
    loop2:
	call check_buttons
	call busy_wait2
	decf var3
	bnz loop2
    bcf PORTD, 0
    return
    
blink_off:
    movlw 3
    movwf var3
    loop3:
	call check_buttons
	call busy_wait2
	decf var3
	bnz loop3
    bsf PORTD, 0
    return

update_portb:
    movlw 0
    cpfsgt portb_enabled
    bra reset_portb
    movlw 8
    cpfslt pb
    bra reset_portb
    movlw 7
    cpfslt pb
    bsf PORTB, 7
    movlw 6
    cpfslt pb
    bsf PORTB, 6
    movlw 5
    cpfslt pb
    bsf PORTB, 5
    movlw 4
    cpfslt pb
    bsf PORTB, 4
    movlw 3
    cpfslt pb
    bsf PORTB, 3
    movlw 2
    cpfslt pb
    bsf PORTB, 2
    movlw 1
    cpfslt pb
    bsf PORTB, 1
    movlw 0
    cpfslt pb
    bsf PORTB, 0
    incf pb
    return
    reset_portb:
	clrf PORTB
	movlw 0
	movwf pb
    return

update_portc:
    movlw 0
    cpfsgt portc_enabled
    bra reset_portc
    movlw -1
    cpfslt pc
    bra reset_portc
    movlw 0
    cpfsgt pc
    bsf PORTC, 0
    movlw 1
    cpfsgt pc
    bsf PORTC,1
    movlw 2
    cpfsgt pc
    bsf PORTC,2
    movlw 3
    cpfsgt pc
    bsf PORTC,3
    movlw 4
    cpfsgt pc
    bsf PORTC,4
    movlw 5
    cpfsgt pc
    bsf PORTC,5
    movlw 6
    cpfsgt pc
    bsf PORTC,6
    movlw 7
    cpfsgt pc
    bsf PORTC,7
    decf pc
    return
    reset_portc:
	clrf PORTC
	movlw 7
	movwf pc
    return
    
busy_wait:
    movlw 39
    movwf var2
    outer_loop_start:
	setf var1
	loop_start:
	    decf var1
	    bnz loop_start
	incfsz var2
	bra outer_loop_start
    return
    
busy_wait2:
    movlw 2
    movwf var2
    outer_loop_start2:
	movlw 26
	movwf var1
	loop_start2:
	    call check_buttons
	    decf var1
	    bnz loop_start2
	incfsz var2
	bra outer_loop_start2
    return

re0_pressed:
    movlw 1
    movwf re0
    return
re1_pressed:
    movlw 1
    movwf re1
    return
re0_released:
    movlw 0
    cpfsgt re0
    return
    movlw 1
    xorwf portc_enabled
    movlw 0
    movwf re0
    return
re1_released:
    movlw 0
    cpfsgt re1
    return
    movlw 1
    xorwf portb_enabled
    movlw 0
    movwf re1
    return
check_buttons:
    btfsc PORTE, 0; check RE0, skip if 0
    rcall re0_pressed
    btfsc PORTE, 1
    rcall re1_pressed
    btfss PORTE, 0
    rcall re0_released
    btfss PORTE, 1
    rcall re1_released
    return
    
    
    
end resetVec