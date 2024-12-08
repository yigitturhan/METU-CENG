#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "pragmas.h"

#define _XTAL_FREQ 40000000
#define BAUD 115200
#define MY_UBRR ((_XTAL_FREQ / (16UL * BAUD)) - 1)

volatile unsigned int ad_period = 0, ad_period_counter = 0, adc_flag = 0, remainingDistance = 0,totalDistance = 0, altitude = 0;
volatile unsigned char character,  counter = 0, switch_state = 0, send_serial = 0, manual_control_on = 0;
volatile unsigned char rb4_pressed = 0, rb5_pressed = 0, rb6_pressed = 0, rb7_pressed = 0, send_prs4 = 0, send_prs5 = 0, send_prs6 = 0, send_prs7 = 0, active_led = 0, message_send = 0;
char message_buffer[10], command_buffer[10];

//initializer for ADC port
void initADC(void) {
    TRISH = 0x10;
    ADCON0 = 0x31;
    ADCON1 = 0x00;
    ADCON2 = 0xAA;
    ADRESH = 0x00;
    ADRESL = 0x00;
}

//initializer for Timer0
void initTimer(void) {
    T0CON = 0x85;
    TMR0H = 0xC2;
    TMR0L = 0xF7;
    INTCON = 0x00;
    INTCON2 = 0x00;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    RCONbits.IPEN = 1;
    INTCON2bits.TMR0IP = 1;
}

//initializer for USART port
void initUSART(void) {
    TXSTA1bits.TX9 = 0;
    TXSTA1bits.TXEN = 0;
    TXSTA1bits.SYNC = 0;
    TXSTA1bits.BRGH = 0;
    RCSTA1bits.SPEN = 1;
    RCSTA1bits.RX9 = 0;
    RCSTA1bits.CREN = 1;
    BAUDCON1bits.BRG16 = 1;
    SPBRGH1 = (21 >> 8) & 0xff;
    SPBRG1 = 21 & 0xff;
    PIR1 = 0;
}
//making the portb interrupt on change enabled
void init_portB(){
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 1;
    LATB = 0x00;
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    INTCON2bits.RBIP = 0;
    unsigned char x = PORTB;
}
//make the PORTB interrupt on change disabled
void reset_portB(){
    TRISB = 0x00;
    ADCON1 = 0x00;
    INTCONbits.RBIE = 0;
    INTCON2bits.RBIP = 0;
}
//just a basic initializer for leds
void init_leds(){
    TRISAbits.RA0 = 0;
    TRISBbits.RB0 = 0;
    TRISCbits.RC0 = 0;
    TRISDbits.RD0 = 0;
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
}

//a function the reset timer in order to interrupt again
void reset_timer(){
    TMR0IF = 0;
    TMR0H = 0xC2;
    TMR0L = 0xF7;
}

// a function to reset global variables which are related with ADC
void reset_adc(){
    ad_period_counter = 0;
    adc_flag = 0;
}
//function to update the leds LD0, LC0, LB0, LA0
void set_leds(unsigned char val){
    switch(val){
        case 0:
            LATAbits.LA0 = 0; //making all the leds turned off
            LATBbits.LB0 = 0;
            LATCbits.LC0 = 0;
            LATDbits.LD0 = 0;
            break;
        case 1:
            LATDbits.LD0 = 1; //lighting the ld0 up and lines under there is at the same purpose
            break;
        case 2:
            LATCbits.LC0 = 1;
            break;
        case 3:
            LATBbits.LB0 = 1;
            break;
        case 4:
            LATAbits.LA0 = 1;
            break;
    }
}
//function to delete message buffer before refilling it
void deleteMessage(){
    for (unsigned char i = 0; i < 9; i++) message_buffer[i] = 0; //loop through buffer and set every value to 0
}

//this function writes the given message to TXREG. Shortly it is for sending message.
void sendUSART(char *data) {
    for (int i = 0; i < strlen(data); i++) {
        if(data[i] == 0) continue; //if there is a null value because of an error ignore it
        while (!PIR1bits.TX1IF); //wait until buffer is empty
        TXREG = data[i]; //write again
    }
    deleteMessage();
}

//a function to clear command buffer before refill it
void deleteCommand(){
    for (unsigned char i = 0; i < 9; i++) command_buffer[i] = 0;
}

//this function gets the ADC level as parameter and sets the altitude based on the table at homework pdf
void handle_alt(unsigned int level){
    altitude = 9000 + (level/256) * 1000; //this line calculates the value received from ad pin and sets the altitude
}

//this function sends distance message to simulator. It is called at every TMR0 interrupt if another message was not sent at the same period.
void send_distance(){
    sprintf(message_buffer, "$DST%04X#", remainingDistance); //write to message to message_buffer
    sendUSART(message_buffer); //function call to send the distance message
}

//like the function above, it sends altitude message to simulator. It is called when the given period on the received ALT message is met.
void send_altitude(unsigned int val){
    sprintf(message_buffer, "$ALT%04X#", altitude); //write the message to message_buffer
    sendUSART(message_buffer); //function call to send the altitude message
}
void send_prs(){ //with an unknown reason, when this function is called the flag at PIR1bits.RC1IF does not be 1 again
    if(send_prs4){ //with the variable which is set at interrupt send the corresponding message
        sprintf(message_buffer, "$PRS04#");
        sendUSART(message_buffer);
    }
    else if(send_prs5){
        sprintf(message_buffer, "$PRS05#");
        sendUSART(message_buffer);
    } 
    else if(send_prs6){
        sprintf(message_buffer, "$PRS06#");
        sendUSART(message_buffer);
    }
    else if(send_prs7){
        sprintf(message_buffer, "$PRS07#");
        sendUSART(message_buffer);
    }
    active_led = 0; //set this to zero in order to not send the same message again
    reset_portB(); //make the PORTB interrupt on change disabled
}

//this function reads the ADC value on the board and calls send_altitude to send the altitude
void read_and_sent_adc(){
    unsigned int result = (ADRESH << 8) + ADRESL; //read value from the pin
    handle_alt(result); //function call to set the altitude with the given value
    send_altitude(result); //it basically sends the altitude message
}

//this function gets the command from main method and based on it's type it processes it and sets the related global variables.
void handleCommand(char *command) {
    if (strncmp(command, "GOO", 3) == 0) {
        send_serial = 1; //send serial messages until the end message is received
        initTimer(); //when goo command is received start the timer
        TXSTA1bits.TXEN = 1; //make the TX enabled
        sscanf(command + 3, "%04X", &totalDistance);
        remainingDistance = totalDistance; //set the remainingDistance with the received value
    }
    else if (strncmp(command, "SPD", 3) == 0) {
        unsigned int dist;
        sscanf(command + 3, "%04X", &dist); //read the distance at SPD command
        remainingDistance -= dist; //and subtract it from the remainingDistance
    }
    else if (strncmp(command, "MAN", 3) == 0) {
        unsigned char state;
        sscanf(command + 3, "%02X", &state);
        manual_control_on = state; //if state is 01 it is on and if state is 00 it is off
        if(state){
            init_leds(); //calling the initialization function for leds
            //init_portB(); //making the RB interrupt on change enabled
        }
        else initADC(); //if manual control is on in order to correct the ADCON1 we are calling the initADC again
    }
    else if (strncmp(command, "ALT", 3) == 0){
        unsigned int period;
        sscanf(command+3, "%04X", &period);
        ad_period = period/100; //we are dividing the period to 100 in order to use it on counter at ISR
        ad_period_counter = 0;
    }
    else if (strncmp(command, "LED", 3) == 0) {
        unsigned char led;
        sscanf(command + 3, "%02X", &led); //reading the led
        active_led = led; //setting active led to check at ISR
        if(led != 0) init_portB(); //we are making RB Interrupt on change disabled at led00 and enabled at other values
        set_leds(led); //function to light the corresponding led up
    }
    else if (strncmp(command, "END", 3) == 0) send_serial = 0; //end message. we set this variable to zero and we are checking it at ISR
}

//main function. reads the received commands and calls handleCommand function in order to process it
void main(void) {
    initADC();
    initUSART();
    deleteCommand();
    while (1) {
        if (PIR1bits.RC1IF) { //if a byte is received
            character = RCREG; //reading the character from RCREG
            switch(switch_state){
                case 0:
                    if(character == '$') switch_state = 1; //state where we are expecting the char '$' as start of the message
                    break;
                case 1:
                    if(counter > 9){
                        switch_state = 0; //a check for not to corrupt memory when a problematic message is received
                        counter = 0; //at this case reset everything and start to wait the new message
                        deleteCommand(); //clear the command buffer
                        break;
                    }
                    if(character != '#'){ //wait for characters until '#' received
                        command_buffer[counter] = character; //set the character on the buffer and continue with next one
                        counter++; //increment the counter to reach the correct index
                    }
                    else{ //when the char '#' received (which means end of the message) reset everything and handle to command
                        counter = 0;
                        switch_state = 0;
                        handleCommand(command_buffer);
                        deleteCommand();
                    }
                    break;
            }
        }
    }
}

//interrupt service routine. It is triggered at every timer0 overflow. Used for sending messages periodically
__interrupt(high_priority)
void handle_interrupt() {
    message_send = 0;
    if(manual_control_on && INTCONbits.RBIF){
    	unsigned char chr = PORTB;
        INTCONbits.RBIF = 0; //check for button press and active line to set related variables
        if(PORTBbits.RB4 && active_led == 1) send_prs4 = 1;
        else if(PORTBbits.RB5 && active_led == 2) send_prs5 = 1;
        else if(PORTBbits.RB6 && active_led == 3) send_prs6 = 1;
        else if(PORTBbits.RB7 && active_led == 4) send_prs7 = 1;
    }
    else if(INTCONbits.RBIF){ //if manual control is off ignore the press and clean the flag
    	unsigned char c = PORTB;
        INTCONbits.RBIF = 0;
    }
    if (TMR0IF) {
        reset_timer(); //remove the flag and reload the preload values
        if(!send_serial) return; //if the simulator does not wait messages from us this line will stop sending it
        if(ad_period != 0) ad_period_counter++; //this counter is used for alt message period calculation. Instead of using another timer we have used a counter and timer0.
        if(adc_flag){
            reset_adc(); //reset adc to read again.
            read_and_sent_adc(); //this function reads the adc value and send the alt message
            message_send = 1;
        }
        else if(ad_period != 0 && ad_period_counter == ad_period-1){
            GODONE = 1;
            if(ADCON0bits.GODONE) adc_flag = 1;
        }
        else if(send_prs4 || send_prs5 || send_prs6 || send_prs7){
            send_prs(); 
            send_prs4 = 0;
            send_prs5 = 0;
            send_prs6 = 0;
            send_prs7 = 0;
        }
        if (!message_send) send_distance();
    }
    
    return;
}
