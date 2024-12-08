// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

#include <xc.h>
#define T_PRELOAD_LOW 0xC1
#define T_PRELOAD_HIGH 0x34

struct Piece{
    char shape; //L,D,S
    unsigned char positionX;
    unsigned char positionY;
    unsigned char direction[4];
};
struct Piece active_piece;

unsigned char board[8][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
unsigned char tempBoard[8][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
unsigned char score = 0, pieceVisible = 1;
unsigned char mr = 0, ml = 0, mu = 0, md = 0;
unsigned char two_second_counter = 0;
unsigned char rb5 = 0, rb6 = 0;


unsigned char boundaryChecker(char direction){
    if(active_piece.shape=='L' || active_piece.shape=='S'){
        if(direction=='l'){
            if(active_piece.positionX-1<0) return 0;
            return 1;
        }
        if(direction=='r'){
            if(active_piece.positionX+2>3) return 0;
            return 1;
        }
        if(direction=='u'){
            if(active_piece.positionY-1<0) return 0;
            return 1;
        }
        if(direction=='d'){
            if(active_piece.positionY+2>7) return 0;
            return 1;
        }
    }
    else{   
        if(direction=='l'){
            if(active_piece.positionX-1<0) return 0;
            else return 1;
        }
        if(direction=='r'){
            if(active_piece.positionX+1>3) return 0;
            else return 1;
        }
        if(direction=='u'){
            if(active_piece.positionY-1<0) return 0;
            else return 1;
        }
        if(direction=='d'){
            if(active_piece.positionY+1>7) return 0;
            else return 1;
        }
    }
    return 1;
}
void copyBoardToTempboard(){
    for(unsigned char i = 0; i < 8; i++){
        for(unsigned char j = 0; j < 4; j++){
            tempBoard[i][j] = board[i][j];
        }
    }
}
void copyTempBoardToBoard(){
    unsigned char x = active_piece.positionX, y = active_piece.positionY;
    if(active_piece.direction[0]) tempBoard[y][x] = 1;
    if(active_piece.direction[1]) tempBoard[y][x+1] = 1;
    if(active_piece.direction[2]) tempBoard[y+1][x+1] = 1;
    if(active_piece.direction[3]) tempBoard[y+1][x] = 1;
    for(unsigned char i = 0; i < 8; i++){
        for(unsigned char j = 0; j < 4; j++){
            board[i][j] = tempBoard[i][j];
        }
    }
}
void moveDown(){
    if(boundaryChecker('d')){
        copyBoardToTempboard();
        active_piece.positionY++;
    }
}

void moveR(){
    if(boundaryChecker('r')){
        copyBoardToTempboard();
        active_piece.positionX++;
    }
}

void moveL(){
    if(boundaryChecker('l')){
        copyBoardToTempboard();
        active_piece.positionX--;
    }
}

void moveUp(){
    if(boundaryChecker('u')){
        copyBoardToTempboard();
        active_piece.positionY--;
    }
}

void initializeTimer()
{
    T0CON = 0x00;
    T0CONbits.TMR0ON = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 1;
    TMR0H = T_PRELOAD_HIGH;
    TMR0L = T_PRELOAD_LOW;
    RCONbits.IPEN = 1;
    INTCON = 0x00;
    INTCONbits.INT0IE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCON2bits.RBIP = 0;
}

void configureSevenSegmentLeds(unsigned char digit){
    switch (digit){
        case 0:
            LATJ = 0x3F;
            break;
        case 1:
            LATJ = 0x06;
            break;
        case 2:
            LATJ = 0x5B;
            break;
        case 3:
            LATJ = 0x4F;
            break;
        case 4:
            LATJ = 0x66;
            break;
        case 5:
            LATJ = 0x6D;
            break;
        case 6:
            LATJ = 0x7D;
            break;
        case 7:
            LATJ = 0x07;
            break;
        case 8:
            LATJ = 0x7F;
            break;
        case 9:
            LATJ = 0x6F;
            break;
    }
}
void sevenSegmentUpdate(){
    unsigned char firstDigit = score%10;
    unsigned char secondDigit = (score - firstDigit) / 10;
    configureSevenSegmentLeds(firstDigit);
    LATHbits.LATH3 = 1;
    __delay_ms(5);
    LATHbits.LATH3 = 0;
    configureSevenSegmentLeds(secondDigit);
    LATHbits.LATH2 = 1;
    __delay_ms(5);
    LATHbits.LATH2 = 0;
    configureSevenSegmentLeds(0);
    LATHbits.LATH0 = 1;
    LATHbits.LATH1 = 1;
    __delay_ms(5);
    LATHbits.LATH0 = 0;
    LATHbits.LATH1 = 0;
}

void getNext(){
    switch (active_piece.shape){
        case 'D':
            active_piece.shape = 'S';
            active_piece.direction[0] = 1;
            active_piece.direction[1] = 1;
            active_piece.direction[2] = 1;
            active_piece.direction[3] = 1;
            break;
        case 'S':
            active_piece.shape = 'L';
            active_piece.direction[0] = 1;
            active_piece.direction[1] = 0;
            active_piece.direction[2] = 1;
            active_piece.direction[3] = 1;
            break;
        case 'L':
            active_piece.shape = 'D';
            active_piece.direction[0] = 1;
            active_piece.direction[1] = 0;
            active_piece.direction[2] = 0;
            active_piece.direction[3] = 0;
            break;
    }
    active_piece.positionX = 0;
    active_piece.positionY = 0;
}
void reset(){
    for(unsigned char i = 0; i < 8; i++){
        for (unsigned char j = 0; j < 4; j++){
            board[i][j] = 0;
            tempBoard[i][j] = 0;
        }
    }
    score = 0;
    active_piece.shape = 'D';
    active_piece.positionX = 0;
    active_piece.positionY = 0;
    active_piece.direction[0] = 1;
    active_piece.direction[1] = 0;
    active_piece.direction[2] = 0;
    active_piece.direction[3] = 0;
}
void scoreUpdate(){
    switch (active_piece.shape){
        case 'D':
            score += 1;
            break;
        case 'L':
            score += 3;
            break;
        case 'S':
            score += 4;
            break;
    }
    if (score >= 32){
        reset();
        return;
    }
    getNext();
}

void render(){
    unsigned char m = 1;
    unsigned char temp1 = 0, temp2 =0, temp3 =0, temp4 =0;
    for (unsigned char i = 0; i < 8; i++, m*= 2){
        temp1 += tempBoard[i][0]*m;
        temp2 += tempBoard[i][1]*m;
        temp3 += tempBoard[i][2]*m;
        temp4 += tempBoard[i][3]*m;
    }
    LATC = temp1;
    LATD = temp2;
    LATE = temp3;
    LATF = temp4;
}

void renderPiece(){
    unsigned char x = active_piece.positionX, y = active_piece.positionY;
    unsigned char v = pieceVisible;
    if(active_piece.direction[0]) tempBoard[y][x] = v;
    if(active_piece.direction[1]) tempBoard[y][x+1] = v;
    if(active_piece.direction[2]) tempBoard[y+1][x+1] = v;
    if(active_piece.direction[3]) tempBoard[y+1][x] = v;
}

void initPorts() {
    ADCON1 = 0x0F;
    TRISG = 0x1D;
    TRISB = 0x60;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    TRISF = 0x00;
    TRISH = 0x00;
    TRISJ = 0x00;
    LATA = 0X00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    LATF = 0x00;
    LATH = 0x00;
    LATJ = 0x00;
    LATG = 0X00;
    __delay_ms(1000);
}

void rotate(){
    if(active_piece.shape == 'L'){
        if(active_piece.direction[0] == 0){
            active_piece.direction[0] = 1;
            active_piece.direction[1] = 0;
        }
        else if(active_piece.direction[1] == 0){
            active_piece.direction[1] = 1;
            active_piece.direction[2] = 0;
        }
        else if(active_piece.direction[2] == 0){
            active_piece.direction[2] = 1;
            active_piece.direction[3] = 0;
        }
        else if(active_piece.direction[3] == 0){
            active_piece.direction[3] = 1;
            active_piece.direction[0] = 0;
        }
        copyBoardToTempboard();
    }
}

void submit() {
    unsigned char x = active_piece.positionX;
    unsigned char y = active_piece.positionY;
    unsigned char type = active_piece.shape;
    unsigned char stat = 0;
    if(type == 'D' && !board[y][x]) stat = 1; 
    else if(type == 'S' && !board[y][x] && !board[y+1][x] && !board[y][x+1] && !board[y+1][x+1]) stat = 1;
    else if(type == 'L' && !active_piece.direction[2] && !board[y][x] && !board[y+1][x] && !board[y][x+1]) stat = 1;
    else if(type == 'L' && !active_piece.direction[3] && !board[y][x] && !board[y][x+1] && !board[y+1][x+1]) stat = 1;
    else if(type == 'L' && !active_piece.direction[0] && !board[y][x+1] && !board[y+1][x+1] && !board[y+1][x]) stat = 1;
    else if(type == 'L' && !active_piece.direction[1] && !board[y][x] && !board[y+1][x+1] && !board[y+1][x]) stat = 1;
    if(stat){
        two_second_counter = 0;
        copyTempBoardToBoard();
        scoreUpdate();
    }
}

void checkMovement(){
    if (PORTGbits.RG0 == 1 && (mr != 1)){
        moveR();
        mr = 1;
    }
    if (!PORTGbits.RG0) mr = 0;
    if (PORTGbits.RG2 && (mu != 1)){
        mu = 1;
        moveUp();
    }
    if (!PORTGbits.RG2) mu = 0;
    if (PORTGbits.RG3 && (md != 1)){
        md = 1;
        moveDown();
    }
    if (!PORTGbits.RG3) md = 0;
    if (PORTGbits.RG4 && (ml != 1)){
        ml = 1;
        moveL();
    }
    if (!PORTGbits.RG4) ml = 0;
}

__interrupt(high_priority)
void HandleInterrupt()
{
    if(INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        initializeTimer();
        pieceVisible = 1-pieceVisible;
        two_second_counter++;
        
    }
    return;
}

__interrupt(low_priority)
void HandleInterrupt2()
{
    unsigned char savedWREG = WREG;
    unsigned char savedSTATUS = STATUS;
    if (PORTBbits.RB6 && (rb5 != 1)){
        rb5 = 1;
        submit();
        __delay_ms(7);
    }
    if(!PORTBbits.RB6){
        rb5 = 0;
        __delay_ms(7);
    }
    if (PORTBbits.RB5 && (rb6 != 1)){
        rb6 = 1;
        rotate();
        __delay_ms(7);
    }
    if (!PORTBbits.RB5) {
        rb6 = 0;
        __delay_ms(7);
    }
    WREG = savedWREG;
    STATUS = savedSTATUS;
    return;
}

void main()
{
    initPorts();
    initializeTimer();
    reset();
    while(1){
        sevenSegmentUpdate();
        checkMovement();
        render();
        renderPiece();
        if(two_second_counter >= 8){
            moveDown();
            two_second_counter = 0;
        }
    }  
}