#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "mipslab.h"

static int exit = 0;
static int timeoutcount = 0;
static int seconds = 0;
static int distance = 0;
static int timeoutcount;
volatile int timervar = 100;
volatile int measure1;
volatile int measure2;
volatile int buttons;
volatile int button;
volatile int marker = 0;

int hSCounter = 0;
int mainloop = 1;
int innerloop = 0;
int gameloop = 1;
char * letters;
char * lettersArr [] = {"A", "B", "C", "D", "E", "F", "G",
                        "H", "I", "J", "K", "L", "M", "N",
                        "O", "P", "Q", "R", "S", "T", "U",
                        "V", "W", "X", "Y", "Z", };

int highscoreint1 = 0;
int highscoreint2 = 0;
int highscoreint3 = 0;
int highscoreint1t2 = 0;
int highscoreint2t2 = 0;
int highscoreint3t2 = 0;

char highscore1 [3];
char highscore2 [3];
char highscore3 [3];
char highscore1t2 [3];
char highscore2t2 [3];
char highscore3t2 [3];

int getbtns (void) {
    
    return (PORTD >> 5) & 0x7;
}

void timerdelay_ms (int duration) {
 
    while (duration > 0) {
       
        if (((IFS(0) >> 8) & 0x1) == 1) {
        
        duration-- ; // decrement
            
        IFS(0) &= 0xfffffeff;
            
        }
    }
}

int main(void) {

    char buf[32], *s, *t;

	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
    TRISDSET = (1 << 11); // switch
	TRISFSET = (1 << 1);  // buttons
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	
	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
        SPI2CON |= 0x60;
	
	/* Turn on SPI */
	SPI2CONSET = 0x8000;
	
    display_init();

    /* Timer init*/
    T2CON |= 0x00008000;
    PR2 = 0x9C40;   //   (40000) period reg for 1 ms interval
    
    
    // i2c init
    i2c_initialize();
    initsensor();
    single_measure_init();
    
    /* Main loop*/

    while(mainloop) {
        timerdelay_ms(200);
        measure1 = 0;
        measure2 = 0;
        distance = 0;
        timeoutcount = 0;
        gameloop = 1;

        while(1) {
            display_string_0(0, "");
            display_string_0(1, "");
            display_string_0(2, "B2: Set/B3:Play");
            display_string_0(3, "B4: Highscores");
            display_update();
            display_image_ni (32, nail_it);
            buttons = getbtns ();
            timerdelay_ms(100);

            if(buttons == 1) {
                buttons = 0;
                timerdelay_ms(200);

                while(1) {
                    display_string_0(0, "Btn 2 for 10 sec. ");
                    display_string_0(1, "Btn 3 for 20 sec. ");
                    display_string_0(2, "Btn 4 to go back");
                    display_string_0(3, "");
                    display_update();
                    
                    buttons = getbtns ();
                    timerdelay_ms(20);
                    
                    if(buttons == 1) {
                        timervar = 100;
                        buttons = 0;
                        display_string_0(0, "Timer is set");
                        display_string_0(1, "to 10 sec.");
                        display_string_0(2, "");
                        display_string_0(3, "");
                        display_update();
                        timerdelay_ms (2000);
                        break;
                    }
                    if(buttons == 2 ) {
                        timervar = 200;
                        buttons = 0;
                        display_string_0(0, "Timer is set");
                        display_string_0(1, "to 20 sec.");
                        display_string_0(2, "");
                        display_string_0(3, "");
                        display_update();
                        timerdelay_ms (2000);
                        break;
                    }
                    if(buttons == 4) {
                        buttons = 0;
                        timerdelay_ms (1000);
                        break;
                    }
                }
            }

            if (buttons == 2){
                timerdelay_ms(200);
                buttons = 0;
                innerloop = 1;
                marker = 0;
                break;
            }
            
            if(buttons == 4) {
                timerdelay_ms(200);
                buttons = 0;

                while(1) {
                buttons = getbtns ();
                timerdelay_ms(20);
                
                display_string_0(0, "B2:Highscore 10sec");
                display_string_0(1, "B3:Highscore 20sec");
                display_string_0(2, "");
                display_string_0(3, "B4:Back");
                display_update();
                
                    if(buttons == 1) {
                        timerdelay_ms(200);
                        buttons = 0;
                        
                        while (1){
                            display_string_0(0, highscore1);
                            display_string_0(1, highscore2);
                            display_string_0(2, highscore3);
                            display_string_0(3, "B4:Back (10s)");
                            display_string_4(0,itoaconv(highscoreint1));
                            display_string_4(1,itoaconv(highscoreint2));
                            display_string_4(2,itoaconv(highscoreint3));
                            display_string_8(0, "cm");
                            display_string_8(1, "cm");
                            display_string_8(2, "cm");
                            display_update();
                            buttons = getbtns ();
                            timerdelay_ms (20);

                            if(buttons == 4) {
                                buttons = 0;
                                timerdelay_ms (200);
                                break;
                            }
                        }
                    }
                    if(buttons == 2) {
                        timerdelay_ms(200);
                        buttons = 0;
                
                        while (1) {
                            display_string_0(0, highscore1t2);
                            display_string_0(1, highscore2t2);
                            display_string_0(2, highscore3t2);
                            display_string_0(3, "B4:Back(20s)");
                            display_string_4(0,itoaconv(highscoreint1t2));
                            display_string_4(1,itoaconv(highscoreint2t2));
                            display_string_4(2,itoaconv(highscoreint3t2));
                            display_string_8(0, "cm");
                            display_string_8(1, "cm");
                            display_string_8(2, "cm");
                            display_update();
                            buttons = getbtns ();
                            timerdelay_ms (20);

                            if(buttons == 4) {
                                buttons = 0;
                                timerdelay_ms (200);
                                break;
                            }
                        }
                    }
                    
                    if (buttons == 4) {
                        buttons = 0;
                        timerdelay_ms (200);
                        break;
                    }
                }
            }
        }
        
        while(innerloop) {
            while(gameloop) {
                
                display_string_4 (0, "GET READY!");
                display_string_0(1, "");
                display_string_0(2, "");
                display_string_0(3, "");
                display_update();

                timerdelay_ms (1000);
                display_string_4 (0, " **** ");
                display_string_4 (1, "   *  ");
                display_string_4 (2, "    * ");
                display_string_4 (3, " **** ");
                display_update();

                timerdelay_ms (1000);
                
                display_string_4 (0, "  **  ");
                display_string_4 (1, " *  * ");
                display_string_4 (2, "   *  ");
                display_string_4 (3, " **** ");
                display_update();

                timerdelay_ms (1000);
                
                display_string_4 (0, "   *  ");
                display_string_4 (1, "  **  ");
                display_string_4 (2, "   *  ");
                display_string_4 (3, "  *** ");
                display_update();
                
                timerdelay_ms (1000);

                display_string_0(0, "");
                display_string_0(1, "");
                display_string_0(2, "");
                display_string_0(3, "");
                display_update();
                
                int countdown = 10;
                while(countdown > 0){
                    display_image_ni (32, nail_it);
                    timerdelay_ms(50);
                    display_image_niR (32, nail_it);
                    timerdelay_ms(50);
                    countdown--;
                }
                gameloop = 0;
            }
                measure1 = single_measure();
                if((measure1 < measure2) && (measure2 - measure1) < 1000 && (measure2-measure1) > 100) {

                    distance += (measure2-measure1);
                }
           
                display_image_clr ((distance/700) - 2, clear);
                display_image_nail (distance/700, icon);
                display_image_64 (64, wood);

                timerdelay_ms (100);
            
                measure2 = measure1;
            
            
                    while (((PORTD & 0x800) >> 11) == 1){
                        
                    cameraoff();
                        
                        if(((PORTD & 0x400) >> 10) == 1){
                            exit = 1;
                            break;
                        }
                    }
                    if (exit == 1){
                        break;
                    }
            
            timeoutcount++;
            
                if(timeoutcount == timervar){
                    break;
                }
        }
                display_string_0(0, "");
                display_string_0(1, "");
                display_string_0(2, "");
                display_string_0(3, "");
                display_update();
        
                display_string_0(3, "NAILED");
                display_string_8(3, itoaconv((distance/100)));
                display_string_12(3, "CM");
                display_update();
                display_image_go (32,gameover);
                cameraoff();
                timerdelay_ms(3000);
        
        //highscore1 t:10s
            if((distance/100 > highscoreint1) && (marker == 0) && (timervar == 100))  {
                
                highscoreint3 = highscoreint2;
                highscoreint2 = highscoreint1;
                highscoreint1 = distance/100;
                
                int i;
                for(i = 0; i < 3; i++){

                    highscore3[i] = highscore2[i];
                    highscore2[i] = highscore1[i];
                    highscore1[i] = ' ';
                    
                }
                display_string_0(0, "NEW HIGH SCORE!!");
                display_string_0(1, "");
                display_string_0(2, "");
                display_string_0(3, "");
                display_update();
                timerdelay_ms(1000);
                
                while(1) {
                    
                    button += getbtns();
                    timerdelay_ms(200);

                    display_string_0 (0, "Write your name");
                    display_string_0 (1, lettersArr [button % 26]);

                    while (((PORTD & 0x800) >> 11) == 1){
                        
                    highscore1 [hSCounter] = * lettersArr [button % 26];
                        
                        timerdelay_ms (100);
                        
                        if(((PORTD & 0x800) >> 11) == 0){
                            hSCounter++;
                        }
                    }
      
                    display_string_0 (2, highscore1);
                    display_string_0 (3, itoaconv((highscoreint1)));
                    display_update ();
                    
                        if (hSCounter == 3) {
                            hSCounter = 0;
                            marker = 1;
                            timerdelay_ms (1000);
                            break;
                        }
                    }
                }
        
        //highscore2 timer 10s:
            if((distance/100 > highscoreint2) && (marker == 0) && (timervar == 100)) {
                
                    highscoreint3 = highscoreint2;
                    highscoreint2 = distance/100;
                
                int i;
                for (i = 0; i < 3; i++){
                    
                    highscore3[i] = highscore2[i];
                    highscore2[i] = ' ';
                    
                }
                
                    display_string_0(0, "NEW HIGH SCORE");
                    display_string_0(1, "");
                    display_string_0(2, "");
                    display_string_0(3, "");
                    display_update();
                    timerdelay_ms(1000);

                    while (1) {
                        
                        button += getbtns();
                        timerdelay_ms(200);
                        
                        display_string_0 (0, "Write your name");
                        display_string_0 (1, lettersArr [button % 26]);
                        
                        while (((PORTD & 0x800) >> 11) == 1){
                            
                            highscore2 [hSCounter] = * lettersArr [button % 26];
                            
                            timerdelay_ms (100);
                            
                            if(((PORTD & 0x800) >> 11) == 0){
                                hSCounter++;
                            }
                        }
                        
                        display_string_0 (2, highscore2);
                        display_string_0 (3, itoaconv((highscoreint2)));
                        display_update ();
                        
                        if (hSCounter == 3) {
                            hSCounter = 0;
                            marker = 1;
                            timerdelay_ms (1000);
                            break;
                        }
                    }
                }
        
            //highscore3 timer 10s
                if((distance/100 > highscoreint3) && (marker == 0) && (timervar == 100)) {
                    
                    highscoreint3 = distance/100;
                    int i;
                    for (i = 0; i < 3; i++){
                        
                        highscore3[i] = ' ';
                        
                    }
                    
                    display_string_0(0, "NEW HIGH SCORE");
                    display_string_0(1, "");
                    display_string_0(2, "");
                    display_string_0(3, "");
                    display_update();
                    timerdelay_ms(1000);

                    while (1) {
                        
                        button += getbtns();
                        timerdelay_ms(200);
                        
                        display_string_0 (0, "Write your name");
                        display_string_0 (1, lettersArr [button % 26]);
                        
                        while (((PORTD & 0x800) >> 11) == 1){
                            
                            highscore3 [hSCounter] = * lettersArr [button % 26];
                            
                            timerdelay_ms (100);
                            
                            if(((PORTD & 0x800) >> 11) == 0){
                                hSCounter++;
                            }
                        }
                        display_string_0 (2, highscore3);
                        display_string_0 (3, itoaconv((highscoreint3)));
                        display_update ();
                        
                        if (hSCounter == 3) {
                            hSCounter = 0;
                            marker = 1;
                            timerdelay_ms (1000);
                            break;
                        }
                    }
                }
        
        //highscore1 t:20s
        if((distance/100 > highscoreint1t2) && (marker == 0) && (timervar == 200))  {
            
            highscoreint3t2 = highscoreint2t2;
            highscoreint2t2 = highscoreint1t2;
            highscoreint1t2 = distance/100;
            
            int i;
            for (i = 0; i < 3; i++){
                
                highscore3t2[i] = highscore2t2[i];
                highscore2t2[i] = highscore1t2[i];
                highscore1t2[i] = ' ';
                
            }
            display_string_0(0, "NEW HIGH SCORE!!");
            display_string_0(1, "");
            display_string_0(2, "");
            display_string_0(3, "");
            display_update();
            timerdelay_ms(1000);
            
            while(1) {
                
                button += getbtns();
                timerdelay_ms(200);
                
                display_string_0 (0, "Write your name");
                display_string_0 (1, lettersArr [button % 26]);
                
                while (((PORTD & 0x800) >> 11) == 1){
                    
                    highscore1t2 [hSCounter] = * lettersArr [button % 26];
                    
                    timerdelay_ms (100);
                    
                    if(((PORTD & 0x800) >> 11) == 0){
                        hSCounter++;
                    }
                }
                
                display_string_0 (2, highscore1t2);
                display_string_0 (3, itoaconv((highscoreint1t2)));
                display_update ();
                
                if (hSCounter == 3) {
                    hSCounter = 0;
                    marker = 1;
                    timerdelay_ms (1000);
                    break;
                }
            }
        }
        
        //highscore2 timer 20s:
        if((distance/100 > highscoreint2t2) && (marker == 0) && (timervar == 200)) {
            
            highscoreint3t2 = highscoreint2t2;
            highscoreint2t2 = distance/100;
            
            int i;
            for (i = 0; i < 3; i++){
                
                highscore3t2[i] = highscore2t2[i];
                highscore2t2[i] = ' ';
                
            }
            
            display_string_0(0, "NEW HIGH SCORE");
            display_string_0(1, "");
            display_string_0(2, "");
            display_string_0(3, "");
            display_update();
            timerdelay_ms(1000);
            
            while (1) {
                
                button += getbtns();
                timerdelay_ms(200);
                
                display_string_0 (0, "Write your name");
                display_string_0 (1, lettersArr [button % 26]);
                
                while (((PORTD & 0x800) >> 11) == 1){
                    
                    highscore2t2 [hSCounter] = * lettersArr [button % 26];
                    
                    timerdelay_ms (100);
                    
                    if(((PORTD & 0x800) >> 11) == 0){
                        hSCounter++;
                    }
                }
                
                display_string_0 (2, highscore2t2);
                display_string_0 (3, itoaconv((highscoreint2t2)));
                display_update ();
                
                if (hSCounter == 3) {
                    hSCounter = 0;
                    marker = 1;
                    timerdelay_ms (1000);
                    break;
                }
            }
        }
        
        //highscore3 timer 20s
        if((distance/100 > highscoreint3t2) && (marker == 0) && (timervar == 200)) {
            
            highscoreint3t2 = distance/100;
            int i;
            for (i = 0; i < 3; i++){
                
                highscore3t2[i] = ' ';
                
            }
            
            display_string_0(0, "NEW HIGH SCORE");
            display_string_0(1, "");
            display_string_0(2, "");
            display_string_0(3, "");
            display_update();
            timerdelay_ms(1000);
            
            while (1) {
                
                button += getbtns();
                timerdelay_ms(200);
                
                display_string_0 (0, "Write your name");
                display_string_0 (1, lettersArr [button % 26]);
                
                while (((PORTD & 0x800) >> 11) == 1){
                    
                    highscore3t2 [hSCounter] = * lettersArr [button % 26];
                    
                    timerdelay_ms (100);
                    
                    if(((PORTD & 0x800) >> 11) == 0){
                        hSCounter++;
                    }
                }
                display_string_0 (2, highscore3t2);
                display_string_0 (3, itoaconv((highscoreint3t2)));
                display_update ();
                
                if (hSCounter == 3) {
                    hSCounter = 0;
                    marker = 1;
                    timerdelay_ms (1000);
                    break;
                }
            }
        }
        
        while(1) {
            
            buttons = getbtns ();
            timerdelay_ms(100);
            display_string_0(0, "");
            display_string_0(1, "");
            display_string_0(2, "B2:Rplay");
            display_string_8(2, "B3:EXIT");
            display_string_0(3, "Score:");
            display_string_8(3, itoaconv (distance/100));
            display_update();
            display_image_go (32, gameover);

            if (buttons == 1) {
                timerdelay_ms (100);
                buttons = 0;
                break;
            }
            
            if (buttons == 2 ) {
                buttons = 0;
                mainloop = 0;
                break;
                
            }
        }
    }
    display_string_0(0, "");
    display_string_0(1, "BYE FOREVER");
    display_string_0(2, "");
    display_string_0(3, "");
    cameraoff();
    display_update();
    return 0;
    
}
