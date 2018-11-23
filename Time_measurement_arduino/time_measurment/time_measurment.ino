// include the library code:
#include <LiquidCrystal.h>
#include "timer_interrupt.h"

/**********************************************************************************
  config                                                                         */  
#define PIN_LIGHT       8
#define PIN_LED_GREEN   9
#define PIN_LED_YELLOW  10
#define PIN_LED_RED     11
#define PIN_LED_BLUE    12
#define PIN_RACE_MODE   13

#define POST_RACE_DELAY 500 // post delay time 5 sec
/*********************************************************************************/

#define ASCI_OFF 48

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void timer_routine();
void display_update();
void calc_race_state(bool light_sensor);
String calculate_time_string(char * p_char_time, uint8_t * timer);
void print_char_array(char char_array[], uint8_t array_lenght);
void check_fast_time(uint8_t actual_time[], uint8_t fastest_time[]);

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

static enum e_race_state race_state;                // state of race
static uint8_t actual_time[3];                      // actual timer
static uint8_t fastest_time[]={99,59,9};            // fast timer
static bool disp_update_bit;                        // bit for display update request
static uint16_t post_cout;                          // counter for post race state
static bool light_senosr;                           // light sensor input
static bool diplay_time_var;                        // display timer variant input

void setup() {
  timer_init(&timer_routine);                       // init timer interrupt for time measurment
  lcd.begin(16, 2);                                 // initialize the LCD
  race_state = PRE_RACE_STATE;                      // init race_state als PRE_RACE_STATE
  disp_update_bit = TRUE;                           // init display_update as true
  pinMode(PIN_LIGHT, INPUT);                        // config pin off light-sensor as digital input
  pinMode(PIN_RACE_MODE, INPUT_PULLUP);             // config pin off race mode switch as digital input
  pinMode(PIN_LED_GREEN, OUTPUT);                   // config pin off led green as digital output 
  pinMode(PIN_LED_YELLOW, OUTPUT);                  // config pin off led yellow as digital output 
  pinMode(PIN_LED_RED, OUTPUT);                     // config pin off led red as digital output 
  pinMode(PIN_LED_BLUE, OUTPUT);                    // config pin off led blue as digital output 
  post_cout       = 0;                              // init post_count with zero
  diplay_time_var = FALSE;                          // init display_timer_var as false
  light_senosr    = TRUE;                           // init light_sensor as true
  lcd.clear();                                      // clear lcd
  Serial.begin(9600);                               // open the serial port at 9600 bps:
  interrupts();                                     // enable all interrupts
}

void loop() {
  
  light_senosr    = !digitalRead(PIN_LIGHT);        // read status of light sensor
  if(diplay_time_var != digitalRead(PIN_RACE_MODE)){ // if display_timer_var changed
    disp_update_bit = TRUE;                         // set display update request
    diplay_time_var = !diplay_time_var;
  }
  calc_race_state(light_senosr);                    // calculate race state
  calc_info_led(race_state, light_senosr);          // activate info leds
  display_update();                                 // check display update
}

/*
 * interrupt fucion is called every 10ms
 * update timer
 */
void timer_routine(){
  static uint8_t  in_count  = 0;

  if(race_state == IN_RACE_STATE){                  // if IN_RACE_STATE is activ
    post_cout = 0;
    
    if(in_count > 9){                               // if in_count higher 9
      in_count = 0;                                 // reset in_cout
      disp_update_bit = TRUE;                       // set diplay update request
      actual_time[milsec]++;                        // and increment mis_sec
    }
    if(actual_time[milsec] > 9){                    // if mis_sec higher 9
      actual_time[milsec] = 0;                      // reset mis_sec
      actual_time[seconds]++;                       // and increment seconds
    }
    if(actual_time[seconds] > 59){                  // if seconds higher 59
      actual_time[seconds] = 0;                     // reset seconds
      actual_time[minutes]++;                       // and increment minutes
    }
    if(actual_time[minutes] < 99)                   // if minutes 99
    {
      in_count++;                                   // stop counter
    }
  }
  else if(race_state == POST_RACE_STATE){            // if IN_RACE_STATE is activ
    in_count = 0;
    if(post_cout < POST_RACE_DELAY){                 // increment post_delay
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
    race_state = POST_RACE_STATE;                                       // change to POST_RACE_STATE    
  }
  else if(race_state == POST_RACE_STATE && post_cout >= POST_RACE_DELAY)
  {
    race_state = PRE_RACE_STATE;
    disp_update_bit = TRUE;
    calculate_time_string(&last_time_string[0], &actual_time[0]);     // update last time string
    check_fast_time(actual_time, fastest_time);
    actual_time[minutes] = 0;
    actual_time[seconds] = 0;
    actual_time[milsec]  = 0;
  }
}

/*
 * calculate info led
 */
void calc_info_led(enum e_race_state race_state, bool light_senosr){
  digitalWrite(PIN_LED_BLUE, light_senosr);

  if(race_state == PRE_RACE_STATE){                                     // if RPE_RACE_STATE is active
    digitalWrite(PIN_LED_GREEN,  TRUE);                                 // activate only green led
    digitalWrite(PIN_LED_YELLOW, FALSE);
    digitalWrite(PIN_LED_RED,    FALSE);
  }
  else if(race_state == IN_RACE_STATE){                                 // if IN_RACE_STATE is active
    digitalWrite(PIN_LED_GREEN,  FALSE);                                // activate only yellow led
    digitalWrite(PIN_LED_YELLOW, TRUE);
    digitalWrite(PIN_LED_RED,    FALSE);
  }
  else{                                                                 // if POST_RACE_STATE is active
    digitalWrite(PIN_LED_GREEN,  FALSE);                                // activate only red led
    digitalWrite(PIN_LED_YELLOW, FALSE);                        
    digitalWrite(PIN_LED_RED,    TRUE);
  }
}

/*
 * function for manage display update
 */
void display_update(){
  if(disp_update_bit){                                                  // if display updte responsed
    lcd.clear();                                                        // clear lcd
    calculate_time_string(&actual_time_string[0], &actual_time[0]);     // update actal time string
    lcd.setCursor(0,0);                                                 // set cursor in fist line
    print_char_array(actual_time_string, 13);                           // print actual time string
    if(diplay_time_var){                                                // if last time mode active
      lcd.setCursor(0,1);                                               // set cursor in second line
      print_char_array(last_time_string, 13);                           // and print last time
    }
    else{                                                               // if fast time mode active
      lcd.setCursor(0,1);                                               // set cursor in second line
      print_char_array(fastest_time_string, 13);                        // and print fast time 
    }
    disp_update_bit = FALSE;                                            // reset diplay update bit
  }
}

/*
 * function for update timer string
*/
String calculate_time_string(char * p_char_time, uint8_t * timer){
  if(timer[minutes] > 9){                                               // if minutes of timer higher 9
    p_char_time[6] = timer[minutes]/10 + ASCI_OFF;                      // update both positions
    p_char_time[7] = timer[minutes]%10 + ASCI_OFF;
  }
  else{                                                                 // if minutes of timer smaler 10
    p_char_time[6] = (char)((int)('0'));                                // update only one position
    p_char_time[7] = timer[minutes] + ASCI_OFF;
  }
  if(timer[seconds] > 9){                                               // if seconds of timer higher 9
    p_char_time[9]  = timer[seconds]/10 + ASCI_OFF;                     // update both positions
    p_char_time[10] = timer[seconds]%10 + ASCI_OFF;
  }
  else{                                                                 // if seconds of timer smaler 10
    p_char_time[9]  = (char)((int)('0'));                               // update both positions
    p_char_time[10] = timer[seconds] + ASCI_OFF;
  }
  p_char_time[12] = timer[milsec] + ASCI_OFF;                           // update mil second
}

/*
 * function for print a char array to display
 */
void print_char_array(char char_array[], uint8_t array_lenght)
{
  uint8_t count = 0;
  for(count=0; count < array_lenght; count++){                          // for length of char array
    lcd.print(char_array[count]);                                       // print a letter to display
  }
}

/*
 * function check if time is fastest
 */
void check_fast_time(uint8_t actual_time[], uint8_t fastest_time[]){
  bool is_faster = false;
  
  if(    actual_time[minutes] < fastest_time[minutes])
  {
    is_faster = true;
  }
  else if(actual_time[minutes] == fastest_time[minutes])
  {
    if(actual_time[seconds] < fastest_time[seconds])
    {
      is_faster = true;
    }
    else if(actual_time[seconds] == fastest_time[seconds])
    {
      if(actual_time[milsec]  <  fastest_time[milsec])
      {
        is_faster = true;
      }
      else
      {
         is_faster = false;
      }
    }
    else
    {
      is_faster = false;
    }
  }
  else
  {
    is_faster = false;
  }

  Serial.println(is_faster);
  if(is_faster)
  {
    memcpy(fastest_time, actual_time, 3);  
    calculate_time_string(&fastest_time_string[0], &actual_time[0]);     // update fastest time string
  }
}
