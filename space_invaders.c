/*  CAB202 ASSIGNMENT TWO
*
*
*	D.Huffer N9493671, May 2016
*	Queensland University of Technology
*/
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "graphics.h"
#include "cpu_speed.h"
#include "sprite.h"
#include <stdbool.h>
#include <stdlib.h>
/*
* Defines
*/
#define NUM_BUTTONS 6
#define BTN_DPAD_LEFT 0
#define BTN_DPAD_RIGHT 1
#define BTN_DPAD_UP 2
#define BTN_DPAD_DOWN 3
#define BTN_LEFT 4
#define BTN_RIGHT 5
#define BTN_STATE_UP 0
#define BTN_STATE_DOWN 1
#define NUM_OF_ALIENS 5
#define FREQUENCY 8000000.0
#define PRESCALER 256.0
#define LTHRES 500
#define RTHRES 500

Sprite bullets[10];
Sprite alienbullets[10];
Sprite alien_row1[NUM_OF_ALIENS];
Sprite alien_row2[NUM_OF_ALIENS];
Sprite alien_row3[NUM_OF_ALIENS];
Sprite player;
Sprite lives[3];
Sprite score;
Sprite lostLives[3];
Sprite levelimg;

unsigned char levelbmap[] = {
  0b10001100,
  0b10001100,
  0b10000000,
  0b10001100,
  0b11101100
};
unsigned char bang[] = {
  0b0000100,
};
unsigned char playerShipBitmap[] = {
  0b00000000,
  0b00001000,
  0b00101010,
  0b01111111,
  0b01111111
};
unsigned char scoreBitmap[] = {
   0b01110011, 0b10011100, 0b11110011, 0b11011000,
   0b10000100, 0b00100010, 0b10001010, 0b00011000,
   0b01100100, 0b00100010, 0b11110011, 0b11000000,
   0b00010100, 0b00100010, 0b10010010, 0b00011000,
   0b11100011, 0b10011100, 0b10001011, 0b11011000
};
unsigned char alienBitmap[] = {
  0b11000011,
  0b11011011,
  0b11111111,
  0b11011011,
  0b11000011
};
unsigned char fullHeartBitmap[] = {
	0b01101100,
	0b11111110,
	0b01111100,
	0b00111000,
	0b00010000
};
unsigned char emptyHeartBitmap[] = {
	0b01101100,
	0b10010010,
	0b01000100,
	0b00101000,
	0b00010000
};

bool drawRow1[NUM_OF_ALIENS];
bool drawRow2[NUM_OF_ALIENS];
bool drawRow3[NUM_OF_ALIENS];
volatile bool shoot = false;
volatile Sprite btn_sprites[NUM_BUTTONS];
unsigned int livesRemaining = 3;
volatile unsigned char btn_hists[NUM_BUTTONS];
volatile unsigned char btn_states[NUM_BUTTONS];
int heroY = LCD_Y-5; //Movement of player y
volatile unsigned int press_count = 0;
volatile unsigned int lvlcount = 1;
volatile int direction[5] = {1,1,1,1,1};
volatile int direction2[5] = {1,1,1,1,1};
volatile int direction3[5] = {1,1,1,1,1};
volatile int alienBulletCounter;
volatile int rand1;
volatile int rand2;
double timerSeed;
int scoreNum;
int alienRow1_xlocations[5];
int alienRow1_ylocations[5];
double currtime;
bool gameover = false;
uint16_t adc_result0, adc_result1;
unsigned char data[18];
unsigned char data2[18];
unsigned char data3[18];
bool headbackup;
volatile int timer_overflows;
unsigned int potenDifference;
unsigned int newPos;
volatile bool lcddraw = false;
volatile bool movequick = false;
/*
* Function declarations
*/
void init_hardware(void);
void init_sprites(void);
void init_aliens(void);
void check_buttons(void);
void draw_aliens(void);
void draw_lives(void);
void draw_all(void);
void game_menu(void);
void process_aliens(void);
double get_system_time(double timer_count);
void seed_timer(void);
void collision(void);
void init_alien_bullets(void);
void draw_alien_bullets(void);
void draw_score(void);
void end_game_screen(void);
void adc_init(void);
uint16_t adc_read(uint8_t ch);
void adcresults(void);
void configureLCD(void);
void initalise_block_dmg(void);
void updating_blocks(void);
void aliensLevelThree(void);
/*
* Main
*/
int main() {
    set_clock_speed(CPU_8MHz);
    init_hardware();
    adc_init();
    initalise_block_dmg();
    game_menu();
    init_sprites();
    seed_timer();
    timer_overflows = 0;
    _delay_ms(100);
    while (1 && !gameover) {
        clear_screen();
        _delay_ms(19);
        draw_sprite(&player);
        draw_score();
        check_buttons();
        process_aliens();
        collision();
        init_alien_bullets();
        aliensLevelThree();
        if(lcddraw == true && (lvlcount == 2 || lvlcount == 3)){
          updating_blocks();
          configureLCD();
        }
        draw_all();
        show_screen();
        if(gameover == true){
          break;
        }
    }
    end_game_screen();
    return 0;
}

/*
* Function implementations
*/
void aliensLevelThree(){

  if(lvlcount == 3 ){
    for( int i = 0 ; i < 5 ; i++){
  if(timer_overflows == 1 && alien_row3[i].y < 24 ){
    alien_row3[i].y = alien_row3[i].y + 1;
    alien_row3[i].y = alien_row3[i].y + 1;
  }
  if(timer_overflows == 2 && alien_row2[i].y < 19   ){
    alien_row2[i].y = alien_row2[i].y + 1;
  }
  if(timer_overflows == 3 && alien_row1[i].y < 14  ){
    alien_row1[i].y = alien_row1[i].y + 1;
  }
  if(timer_overflows == 4 && alien_row1[i].y > 9 && alien_row1[i].y < 40 ){
    alien_row1[i].y =   alien_row1[i].y -1;
    alien_row1[i].y =   alien_row1[i].y -1;
    alien_row1[i].y =   alien_row1[i].y -1;
  }
  if(timer_overflows == 5 && alien_row2[i].y > 13  && alien_row2[i].y < 40 ){
    alien_row2[i].y =   alien_row2[i].y -1;
  }
  if(timer_overflows == 6 && alien_row3[i].y > 18 && alien_row3[i].y < 40 ){
    alien_row3[i].y =   alien_row3[i].y -1;
  }
  }
  }
}

void adcresults(){
  adc_result0 = adc_read(0);      // read adc value at PA0
  adc_result1 = adc_read(1);      // read adc value at PA1
}
// initialize adc
void adc_init()
{
    // AREF = AVcc
    ADMUX = (1<<REFS0);
    // ADC Enable and pre-scaler of 128
    // 8000000/128 = 62500
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
// read adc value
uint16_t adc_read(uint8_t ch)
{
    // select the corresponding channel 0~7
    // ANDing with '7' will always keep the value
    // of 'ch' between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    return (ADC);
}

void game_menu(){
	while(1){
		_delay_ms(75);
		clear_screen();
    draw_string(0,10,"Please choose a");
    draw_string(0,20,"level:");
      char buff[30];
    draw_string(30,20, buff);
    sprintf(buff, "%4d", lvlcount);
    draw_string(0,30,"R BTN to cycle");
    draw_string(0,40,"L BTN to choose");
    if (!bit_is_clear(PINF, 5)) {
       _delay_ms(100);
       while((PINF >> PF5) & 1);
       lvlcount++;
}
if(lvlcount >3 ){
  lvlcount = 1;
}
		 if((PINF>>PINF6)&1){
			break;
		}
		show_screen();
	}
}

void check_buttons(){
  if(shoot == true && press_count%2 == 0){
    init_sprite((Sprite*)&bullets[press_count],player.x-3,player.y,8,1,bang);
  }
  if(shoot == true && press_count%2 != 0){
    init_sprite((Sprite*)&bullets[press_count],player.x+1,player.y,8,1,bang);
  }
	if (lvlcount == 1){
    if(player.x > 79){
      player.x--;
    }
    if(player.x<0){
      player.x++;
    }
		if((btn_states[BTN_LEFT] == BTN_STATE_DOWN)){
			player.x = player.x - 1;
		} else if((btn_states[BTN_RIGHT] == BTN_STATE_DOWN)){
			player.x = player.x + 1;
		}
	}
	if(lvlcount==2){
		adcresults();
    if(player.x > 79){
      player.x--;
    }
    if(player.x<0){
      player.x++;
    }
		if (adc_result1 >= 450 ){
			player.x = player.x +1;
		} else {
			player.x = player.x;
		}
		if(adc_result1 <= 50){
			player.x = player.x -1;
		}else{
			player.x = player.x;
		}
	}
	if(lvlcount == 3){
    if(player.x > 79){
      player.x--;
    }
    if(player.x<0){
      player.x++;
    }
		float max_adc = 1023.0;
		adc_result0 = adc_read(0);
		long max_lcd_bullet = (adc_result0*(long)(LCD_X-7))/ max_adc;
		if(btn_states[BTN_DPAD_RIGHT] == BTN_STATE_DOWN){
			player.x = player.x+1;
		}
		else if(btn_states[BTN_DPAD_LEFT] == BTN_STATE_DOWN){
			player.x = player.x-1;
		}
		newPos = bullets[press_count-1].x + (max_lcd_bullet - potenDifference);
		if (newPos > 0 && newPos < 80 && max_lcd_bullet != potenDifference){
			bullets[press_count-1].x = newPos;
		}
		potenDifference = max_lcd_bullet;
    if(press_count == 10){
      press_count = 0;
    }
	}
}

void seed_timer(){
  if((PINF>>PIN6)&1){
      timerSeed = get_system_time(currtime);
    }
    srand(timerSeed);
}

void process_aliens(){

  if(alien_row1[0].x >= 35){
    direction[0] = - 1;
  }
  if(alien_row1[0].x <= 1){
    direction[0] = 1;
  }
  if(alien_row1[1].x >= 45){
    direction[1] = -1;
  }
  if(alien_row1[1].x <= 11){
    direction[1] = 1;
  }
  if(alien_row1[2].x >= 55){
    direction[2] = -1;
  }
  if(alien_row1[2].x <= 21){
    direction[2] = 1;
  }
  if(alien_row1[3].x >= 65){
    direction[3] = -1;
  }
  if(alien_row1[3].x <= 31){
    direction[3] = 1;
  }
  if(alien_row1[4].x >= 75){
    direction[4] = -1;
  }
  if(alien_row1[4].x <= 41){
    direction[4] = 1;
  }

  alien_row1[0].x = alien_row1[0].x + direction[0];
  alien_row1[1].x = alien_row1[1].x + direction[1];
  alien_row1[2].x = alien_row1[2].x + direction[2];
  alien_row1[3].x = alien_row1[3].x + direction[3];
  alien_row1[4].x = alien_row1[4].x + direction[4];

  if(alien_row2[0].x >= 35){
    direction2[0] = - 1;
  }
  if(alien_row2[0].x <= 1){
    direction2[0] = 1;
  }
  if(alien_row2[1].x >= 45){
    direction2[1] = -1;
  }
  if(alien_row2[1].x <= 11){
    direction2[1] = 1;
  }
  if(alien_row2[2].x >= 55){
    direction2[2] = -1;
  }
  if(alien_row2[2].x <= 21){
    direction2[2] = 1;
  }
  if(alien_row2[3].x >= 65){
    direction2[3] = -1;
  }
  if(alien_row2[3].x <= 31){
    direction2[3] = 1;
  }
  if(alien_row2[4].x >= 75){
    direction2[4] = -1;
  }
  if(alien_row2[4].x <= 41){
    direction2[4] = 1;
  }
  alien_row2[0].x = alien_row2[0].x + direction2[0];
  alien_row2[1].x = alien_row2[1].x + direction2[1];
  alien_row2[2].x = alien_row2[2].x + direction2[2];
  alien_row2[3].x = alien_row2[3].x + direction2[3];
  alien_row2[4].x = alien_row2[4].x + direction2[4];
  if(lvlcount == 2 || lvlcount == 3  ){
  alien_row2[0].x = alien_row2[0].x + direction2[0];
  alien_row2[1].x = alien_row2[1].x + direction2[1];
  alien_row2[2].x = alien_row2[2].x + direction2[2];
  alien_row2[3].x = alien_row2[3].x + direction2[3];
  alien_row2[4].x = alien_row2[4].x + direction2[4];
    if(movequick == true){
  alien_row3[0].x = alien_row3[0].x + direction3[0];
  alien_row3[1].x = alien_row3[1].x + direction3[1];
  alien_row3[2].x = alien_row3[2].x + direction3[2];
  alien_row3[3].x = alien_row3[3].x + direction3[3];
  alien_row3[4].x = alien_row3[4].x + direction3[4];
    }
  }
  if(alien_row3[0].x >= 35){
    direction3[0] = - 1;
  }
  if(alien_row3[0].x <= 1){
    direction3[0] = 1;
  }
  if(alien_row3[1].x >= 45){
    direction3[1] = -1;
  }
  if(alien_row3[1].x <= 11){
    direction3[1] = 1;
  }
  if(alien_row3[2].x >= 55){
    direction3[2] = -1;
  }
  if(alien_row3[2].x <= 21){
    direction3[2] = 1;
  }
  if(alien_row3[3].x >= 65){
    direction3[3] = -1;
  }
  if(alien_row3[3].x <= 31){
    direction3[3] = 1;
  }
  if(alien_row3[4].x >= 75){
    direction3[4] = -1;
  }
  if(alien_row3[4].x <= 41){
    direction3[4] = 1;
  }
  alien_row3[0].x = alien_row3[0].x + direction3[0];
  alien_row3[1].x = alien_row3[1].x + direction3[1];
  alien_row3[2].x = alien_row3[2].x + direction3[2];
  alien_row3[3].x = alien_row3[3].x + direction3[3];
  alien_row3[4].x = alien_row3[4].x + direction3[4];
  if(lvlcount == 2){
    for( int i = 0 ; i < 5 ; i++){
    if(alien_row3[i].y == 19){
      headbackup = false;
    } if(alien_row3[i].y == 24){
      headbackup= true;
    }
    if(headbackup== true)
    {
      alien_row3[i].y = alien_row3[i].y - 1;
    }
    if(headbackup == false){
        alien_row3[i].y = alien_row3[i].y + 1;
    }
    if(alien_row2[i].y == 14){
      headbackup = false;
    } if(alien_row2[i].y == 19){
      headbackup = true;
    }
    if(headbackup == true)
    {
      alien_row2[i].y = alien_row2[i].y - 1;
    }
    if(headbackup == false){
        alien_row2[i].y = alien_row2[i].y + 1;
    }
    if(alien_row1[i].y == 9){
      headbackup = false;
    } if(alien_row1[i].y == 14){
      headbackup = true;
    }
    if(headbackup == true)
    {
      alien_row1[i].y = alien_row1[i].y - 1;
    }
    if(headbackup == false){
        alien_row1[i].y = alien_row1[i].y + 1;
    }
  }
}
}

void initalise_block_dmg(){
  for(int i = 0 ; i < 18 ; i++){
    data[i] = 0b0000111;
    data2[i] = 0b00000111;
    data3[i] = 0b00000111;
  }
}

void configureLCD(){
  if(lvlcount == 3 || lvlcount == 2){
    lcd_write(LCD_C,0x21 );
    lcd_write(LCD_C, 0x20);
    lcd_write(LCD_C, 0xC);
    for(int i = 0 ; i < 18; i++){
    lcd_position(5+i,5);
    lcd_write(LCD_D, data[i]);
    lcd_position(33+i,5);
    lcd_write(LCD_D, data2[i]);
    lcd_position(61+i,5);
    lcd_write(LCD_D, data3[i]);
  }
  }
}

  void updating_blocks(){
    for(int k = 0; k <= 18 ; k++){
      for(int i = 0 ; i < 10 ; i++){
          if(alienbullets[i].y == 40 && alienbullets[i].x == k && (data[k] & (1 << 0)) != 0){
            data[k] ^= (1 << 0);
            alienbullets[i].y = 49;
          }
          if(alienbullets[i].y == 41 && alienbullets[i].x == k && (data[k] & (1 << 1)) != 0){
            data[k] ^= (1 << 1);
            alienbullets[i].y = 49;
          }
          if(alienbullets[i].y == 42 && alienbullets[i].x == k && (data[k] & (1 << 2)) != 0){
            data[k] ^= (1 << 2);
            alienbullets[i].y = 49;
          }
          if(bullets[i].y == 40 && bullets[i].x == k && (data[k] & (1 << 0)) != 0){
            data[k] ^= (1 << 0);
            bullets[i].y = -1;
          }
          if(bullets[i].y == 41 && bullets[i].x == k && (data[k] & (1 << 1)) != 0){
            data[k] ^= (1 << 1);
            bullets[i].y = -1;
          }
          if(bullets[i].y == 42 && bullets[i].x == k && (data[k] & (1 << 2)) != 0){
            data[k] ^= (1 << 2);
            bullets[i].y = -1;
          }
          lcd_position(k+33,5);
          if(alienbullets[i].y == 40 && alienbullets[i].x == k+28 && (data2[k] & (1 << 0)) != 0){
            data2[k] ^= (1 << 0);
            alienbullets[i].y = 49;
          }
          if(alienbullets[i].y == 41 && alienbullets[i].x == k+28  && (data2[k] & (1 << 1)) != 0){
            data2[k] ^= (1 << 1);
            alienbullets[i].y = 49;
          }
          if(alienbullets[i].y == 42 && alienbullets[i].x == k+28  && (data2[k] & (1 << 2)) != 0){
            data2[k] ^= (1 << 2);
            alienbullets[i].y = 49;
          }
          if(bullets[i].y == 40 && bullets[i].x == k+28 && (data2[k] & (1 << 0)) != 0){
            data2[k] ^= (1 << 0);
            bullets[i].y = -1;
          }
          if(bullets[i].y == 41 && bullets[i].x == k+28 && (data2[k] & (1 << 1)) != 0){
            data2[k] ^= (1 << 1);
            bullets[i].y = -1;
          }
          if(bullets[i].y == 42 && bullets[i].x == k+28 && (data2[k] & (1 << 2)) != 0){
            data2[k] ^= (1 << 2);
            bullets[i].y = -1;
          }
          lcd_position(k+61,5);
             if(alienbullets[i].y == 40 && alienbullets[i].x == k+56 && (data3[k] & (1 << 0)) != 0){
               data3[k] ^= (1 << 0);
               alienbullets[i].y = 49;
             }

             if(alienbullets[i].y == 41 && alienbullets[i].x == k+56 && (data3[k] & (1 << 1)) != 0){
               data3[k] ^= (1 << 1);
               alienbullets[i].y = 49;
             }
             if(alienbullets[i].y == 42 && alienbullets[i].x == k+56 && (data3[k] & (1 << 2)) != 0){
               data3[k] ^= (1 << 2);
               alienbullets[i].y = 49;
             }
             if(bullets[i].y == 40 && bullets[i].x == k+56 && (data3[k] & (1 << 0)) != 0){
               data3[k] ^= (1 << 0);
               bullets[i].y = -1;
             }
             if(bullets[i].y == 41 && bullets[i].x == k+56 && (data3[k] & (1 << 1)) != 0){
               data3[k] ^= (1 << 1);
               bullets[i].y = -1;
             }
             if(bullets[i].y == 42 && bullets[i].x == k+56 && (data3[k] & (1 << 2)) != 0){
               data3[k] ^= (1 << 2);
               bullets[i].y = -1;
             }
           }
         }
  }

void end_game_screen(){
  {
    while(1){
      clear_screen();
      draw_string(0,20,"GAME OVER");
      draw_string(0,30,"reboot to play");
      draw_string(0,40,"AGAIN");
      show_screen();
    }
  }
}

void draw_lives(){
	if (livesRemaining == 3){
		for (unsigned int i = 0; i < 3; i++){
			draw_sprite(&lives[i]);
		}
	} else if (livesRemaining == 2){
		draw_sprite(&lostLives[0]);
		draw_sprite(&lives[1]);
		draw_sprite(&lives[2]);
	} else if (livesRemaining == 1){
		draw_sprite(&lostLives[0]);
		draw_sprite(&lostLives[1]);
		draw_sprite(&lives[2]);
	} else if (livesRemaining == 0){
		draw_sprite(&lostLives[0]);
		draw_sprite(&lostLives[1]);
		draw_sprite(&lostLives[2]);
	}
  if(livesRemaining == 0 ){
    gameover = true;
  }
}

void draw_bullets(){
  for(int i = 0 ; i < 10; i ++ ){
    draw_sprite((Sprite*)&bullets[i]);
    if(  bullets[i].y > 7  ) {
      bullets[i].y = bullets[i].y - 1;
      }
    }
  }

void draw_aliens(){
  for(int i = 0; i < 5 ; i++){
    if(alien_row1[i].x != 90){
      draw_sprite(&alien_row1[i]);
    }
    if(alien_row2[i].x != 90){
      draw_sprite(&alien_row2[i]);
    }
    if(alien_row3[i].x != 90){
      draw_sprite(&alien_row3[i]);
      }
    }
  }

  void collision(){
  for(int i = 0; i < 10 ; i++){
    for(int k = 0; k < 5 ; k ++){
	if ((bullets[i].y - 1) <= (alien_row1[k].y + 3) && (bullets[i].y >= alien_row1[k].y) && bullets[i].x >= (alien_row1[k].x - 3) && bullets[i].x <= (alien_row1[k].x + 3)){
    bullets[i].x = 90;
    alien_row1[k].x = 50;
    alien_row1[k].y = 60;
    scoreNum++;
  }
	if ((bullets[i].y - 1) <= (alien_row2[k].y + 3) && (bullets[i].y >= alien_row2[k].y) && bullets[i].x >= (alien_row2[k].x - 3) && bullets[i].x <= (alien_row2[k].x + 3)){
    bullets[i].x = 90;
    alien_row2[k].x = 50;
    alien_row2[k].y = 60;
    scoreNum++;
  }
	if ((bullets[i].y - 1) <= (alien_row3[k].y + 3) && (bullets[i].y >= alien_row3[k].y) && bullets[i].x >= (alien_row3[k].x - 3) && bullets[i].x <= (alien_row3[k].x + 3)){
    bullets[i].x = 90;
    alien_row3[k].x = 50;
    alien_row3[k].y = 60;
    scoreNum++;
      }
    }
  }
  if(scoreNum == 15){
    gameover = true;
  }
  for(int p = 0 ; p < 10 ; p++){
    if(alienbullets[p].y == player.y && player.x+2 <= alienbullets[p].x+6 && player.x + player.width >= alienbullets[p].x+6){
      livesRemaining--;
    }
  }
  }

void draw_all(){
draw_sprite(&score);
draw_sprite(&levelimg);
draw_score();
draw_lives();
draw_alien_bullets();
draw_bullets();
draw_aliens();
draw_sprite(&player);
if(lvlcount == 1){
  draw_string(76,0 , "1");
 }
if(lvlcount == 2){
  draw_string(76,0 , "2");
}
if(lvlcount == 3){
  draw_string(76,0 , "3");
}
draw_line(0, 7, 83, 7);
}

void draw_score(){
  char* array[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
  for(int i = 0 ; i < 16 ; i ++ ){
    if(scoreNum == i){
        draw_string(32,0 , array[i]);
    }
  }
}

void init_alien_bullets(){
    if(alien_row1[alienBulletCounter].y < 90 && rand1 == 0){
        init_sprite((Sprite*)&alienbullets[alienBulletCounter],alien_row1[rand2].x,alien_row1[rand2].y,8,1,bang);
    }
    if(alien_row2[alienBulletCounter].y < 90 && rand1 == 1){
        init_sprite((Sprite*)&alienbullets[alienBulletCounter],alien_row2[rand2].x,alien_row2[rand2].y,8,1,bang);
    }
    if(alien_row1[alienBulletCounter].y < 90 && rand1 == 2){
        init_sprite((Sprite*)&alienbullets[alienBulletCounter],alien_row3[rand2].x,alien_row3[rand2].y,8,1,bang);
    }
  if(alienBulletCounter == 10){
    alienBulletCounter = 0;
  }
}

void draw_alien_bullets(){
for(int i = 0; i < 10 ; i++){
  if(alienbullets[i].y > 48){
    alienbullets[i].y = -5;
  }
  if(alienbullets[i].y > 0){
      alienbullets[i].y = alienbullets[i].y + 1;
  }
  draw_sprite((Sprite*)&alienbullets[i]);
}
}

void init_hardware(void) {
    // Initialising the LCD screen
    lcd_init(LCD_HIGH_CONTRAST);
    // Initalising the buttons as inputs//
    DDRD |= (1<<PIND6);//|(1<<PINB3);	//Teensy LEDLED0 and LED1 as outputs
    DDRB |= (1<<PIN3);
	   DDRF &= ~((1<<PINF5)|(1<<PINF6));	//Switch2 and Switch3 as input
  //Set to Normal Timer Mode using TCCR0B
    TCCR0B &= ~((1<<WGM02));
    //Set to Normal Timer Mode using TCCR0B
    TCCR0B &= ~((1<<WGM02));
    //Set timer overflow to 256
    TCCR0B |= (1<<CS02);
    //enable timer overflow
    TIMSK0 |= (1 << TOIE0);
    TCCR1B &= ~((1<<WGM12));  //Set to Normal Timer Mode using TCCR1B
    // Set the prescaler for TIMER1 so that the clock overflows every ~2.1 seconds
      TCCR1B |= ((1<<CS02));
     // enable interrupt overflow
      TIMSK1 |= (1 << TOIE1);
    sei(); //Global interrupts
}

void init_aliens(){
  int xcor[] = {1,11,21,31,41};
  int xcor2[] = {1,11,21,31,41};
  int xcor3[] = {1,11,21,31,41};
  if(lvlcount == 2 || lvlcount == 3){
    for(int i = 0 ; i < 5 ; i ++){
      xcor2[i] = xcor2[i] + 10;
      xcor3[i] = xcor3[i] + 30;
      init_sprite((Sprite*)&alien_row1[i],xcor2[i],9,8,5,alienBitmap);
      init_sprite((Sprite*)&alien_row2[i],xcor[i],14,8,5,alienBitmap);
      init_sprite((Sprite*)&alien_row3[i],xcor3[i],19,8,5,alienBitmap);
    }
  }
  for(int i = 0 ; i < 5 ; i ++ ){
  init_sprite((Sprite*)&alien_row1[i],xcor2[i],9,8,5,alienBitmap);
  init_sprite((Sprite*)&alien_row2[i],xcor[i],14,8,5,alienBitmap);
  init_sprite((Sprite*)&alien_row3[i],xcor3[i],19,8,5,alienBitmap);
  }
}

double get_system_time(double timer_count) {
    timer_count = TCNT1/(FREQUENCY/PRESCALER);
    return timer_count;
}

void init_lives(){
	for(unsigned int i = 0; i < 3; i++){
		init_sprite(&lives[i], (i * 7) + 44, 1, 8, 5, fullHeartBitmap);
		init_sprite(&lostLives[i], (i * 7) + 44, 1, 8, 5, emptyHeartBitmap);
	}
}

void init_sprites(){
  // PLAYER
init_sprite((Sprite*) &player, 0, heroY, 8, 5, playerShipBitmap); //Player Sprite
// UI DISPLAY
init_sprite(&score,0,0,32,5,scoreBitmap); //Score Sprite
init_sprite(&levelimg,68,0,8,5,levelbmap); //Level Sprite
// Aliens
init_aliens();
// Lives Remaining
init_lives();
}

ISR(TIMER0_OVF_vect){
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if(btn_hists[i]==0b11111111){
      btn_states[i] = BTN_STATE_DOWN;
    }else{btn_states[i] = BTN_STATE_UP;}

      btn_hists[i] = (btn_hists[i]<<1);//shifts each byte left 1 placed
    }
    if(btn_hists[BTN_DPAD_UP]==0b10000000){

      press_count++;
    }
    if(press_count == 10){
      press_count = 0;
    }
if((PINF>>PIN5)&1){
btn_hists[BTN_RIGHT] ^= (1<<0);
}
  if((PINF>>PIN6)&1){
    btn_hists[BTN_LEFT] ^= (1<<0);
  }
  if((PINB>>PIN7)&1){
    btn_hists[BTN_DPAD_DOWN] ^= (1<<0);
  }
  if((PINB>>PIN1)&1){
    btn_hists[BTN_DPAD_LEFT] ^= (1<<0);
  }
  if((PIND>>PIN1)&1){
    btn_hists[BTN_DPAD_UP] ^= (1<<0);
  }
  if((PIND>>PIN0)&1){
    btn_hists[BTN_DPAD_RIGHT] ^= (1<<0);
  }
  lcddraw = true;
  if(btn_hists[BTN_DPAD_UP] == BTN_STATE_DOWN){
    shoot = true;
  }
}

ISR(TIMER1_OVF_vect) {
  timer_overflows++;
  if(timer_overflows%2 == 0 ){
    movequick = true;
  }else{
    movequick = false;
  }

if(timer_overflows ==7){
  timer_overflows =0;
}
    alienBulletCounter++;
     rand1 = rand()%2;
    rand2 = rand()%4;
}
