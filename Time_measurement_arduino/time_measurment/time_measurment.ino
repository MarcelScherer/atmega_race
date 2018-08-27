#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "timer_interrupt.h"

/**********************************************************************************
  config                                                                         */  
#define PIN_LIGHT       6
#define PIN_LED_GREEN   7
#define PIN_LED_YELLOW  8
#define PIN_LED_RED     9
#define PIN_LED_BLUE    10
#define PIN_RACE_MODE   11

#define POST_RACE_DELAY 500 // post delay time 5 sec
/*********************************************************************************/

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void timer_routine();
void calc_race_state(bool light_sensor);
String calculate_time_string(char * p_char_time, uint8_t * timer);

static char actual_time_string[]  = "ACT:  00:00.0";
static char fastest_time_string[] = "FAST: 00:00.0";
static char last_time_string[]    = "LAST: 00:00.0";

enum e_race_state {
  PRE_RACE_STATE,
  IN_RACE_STATE,
  POST_RACE_STATE
};

#define minutes 0
#define seconds 1
#define milsec  2

static enum e_race_state race_state;  // state of race
static uint8_t actual_time[3];
static uint8_t fastest_time[3];
static bool disp_update_bit;
static uint16_t post_cout;
static bool light_senosr;

void setup() {
  timer_init(&timer_routine);                       // init timer interrupt for time measurment
  lcd.begin();                                      // initialize the LCD
  lcd.backlight();                                  // Turn on the blacklight
  
  race_state = PRE_RACE_STATE;                      // init race_state als PRE_RACE_STATE
  disp_update_bit = FALSE;                          // init display_update as false
  pinMode(PIN_LIGHT, INPUT);                        // config pin off light-sensor as digital input
  pinMode(PIN_RACE_MODE, INPUT);                    // config pin off race mode switch as digital input
  pinMode(PIN_LED_GREEN, OUTPUT);                   // config pin off led green as digital output 
  pinMode(PIN_LED_YELLOW, OUTPUT);                  // config pin off led yellow as digital output 
  pinMode(PIN_LED_RED, OUTPUT);                     // config pin off led red as digital output 
  pinMode(PIN_LED_BLUE, OUTPUT);                    // config pin off led blue as digital output 
  post_cout = 0;                                    // init post_count with zero
}

void loop() {
  light_senosr = digitalRead(PIN_LIGHT);            // read status of light sensor
  calc_race_state(light_senosr);                    // calculate race state
  calc_info_led(race_state, light_senosr);          // activate info leds
  if(disp_update_bit){
    disp_update_bit = FALSE;
  }
}

/*
 * interrupt fucion is called every 10ms
 * update timer
 */
void timer_routine(){
  static uint8_t  in_count  = 0;

  if(race_state == IN_RACE_STATE){
    post_cout = 0;
    
    if(in_count > 9){
      in_count = 0;
      disp_update_bit = TRUE;
      actual_time[milsec]++;
    }
    if(actual_time[milsec] > 9){
      actual_time[milsec] = 0;
      actual_time[seconds]++;
    }
    if(actual_time[seconds] > 59){
      actual_time[seconds] = 0;
      actual_time[minutes]++;
    }
    if(actual_time[minutes] < 99)
    {
      in_count++;
    }
  }
  else if(race_state == POST_RACE_STATE){
    in_count = 0;
    if(post_cout < POST_RACE_DELAY){
      post_cout++;
    }
  }
}

/*
 * function calculate actual race state
 */
void calc_race_state(bool light_sensor){
  if(race_state == PRE_RACE_STATE && light_sensor == FALSE){
    race_state = IN_RACE_STATE;
  }
  else if(race_state == IN_RACE_STATE && light_sensor == FALSE && actual_time[seconds] > 1){
    race_state = POST_RACE_STATE;
  }
  else if(race_state == POST_RACE_STATE && post_cout >= POST_RACE_DELAY)
  {
    race_state = PRE_RACE_STATE;
  }
}

/*
 * calculate info led
 */
void calc_info_led(enum e_race_state race_state, bool light_senosr){
  digitalWrite(PIN_LED_BLUE, !light_senosr);

  if(race_state == PRE_RACE_STATE){
    digitalWrite(PIN_LED_GREEN,  TRUE);
    digitalWrite(PIN_LED_YELLOW, FALSE);
    digitalWrite(PIN_LED_RED,    FALSE);
  }
  else if(race_state == IN_RACE_STATE){
    digitalWrite(PIN_LED_GREEN,  FALSE);
    digitalWrite(PIN_LED_YELLOW, TRUE);
    digitalWrite(PIN_LED_RED,    FALSE);
  }
  else{
    digitalWrite(PIN_LED_GREEN,  FALSE);
    digitalWrite(PIN_LED_YELLOW, FALSE);
    digitalWrite(PIN_LED_RED,    TRUE);
  }
}

void display_update(){
  
}

String calculate_time_string(char * p_char_time, uint8_t * timer){
  if(timer[minutes] > 9){
    p_char_time[6] = timer[minutes]/10;
    p_char_time[7] = timer[minutes]%10;
  }
  else{
    p_char_time[6] = (char)((int)('0'));
    p_char_time[7] = timer[minutes];
  }
}

