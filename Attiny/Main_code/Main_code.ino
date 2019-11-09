/**@file Main_code.ino
* @brief Attiny sketch to control addressable LED strips reading lights signals from motorbike.
* @section intro_sec Introduction
* @subsection Things to have in mind
* - Motorbike lights work with 12V. A DC-DC converter is needed for powering the Attiny85.
* - Motorbike voltage is not as stable as it should due to the alternator properties.
* @subsection Components
* - TSR 2-2450 (DC-DC converter. Output: 5V 2A).
* - Attiny85 (Clock at 8 MHz).
* - CNY74-2 x2 (Optocoupler 2 Channel).
* - 2N2222 x3
* - 4K7 Ohm x6
* - 10 uF capacitor (Between Vcc and GND in the Attiny85)
* - WS2812 LED strip
* @section dependencies Dependencies
* Adafruit_NeoPixel
* @section author Author
* Written by Carlos Manuel Gomez Jimenez. November 2019.
* @section license License
* BSD license, all text here must be included in any redistribution.
*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   DEPENDENCIES    /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   PARAMETERS     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define STRIP_PIN                     4         ///< Which pin on the Attiny is connected to the NeoPixels?
#define NUM_PIXELS                    8         ///< How many NeoPixels are attached to the Arduino?
#define FIRST_PIXEL_ADDR              0         ///< For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
#define RIGHT_LED_PIN                 0
#define LEFT_LED_PIN                  1
#define BRAKE_LED_PIN                 2
#define DIRECTION_PROGRESSION_DELAY   100
#define INITIAL_LED_RIGHT             NUM_PIXELS/2  ///< Connector in left side. Otherwise: int led_right=0;
#define INITIAL_LED_LEFT              0             ///< Connector in left side. Otherwise: int led_left=(NUM_PIXELS/2);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   INSTANCES    //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   GLOBAL VARIABLES     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
uint32_t position_color       = strip.Color(50, 0, 0);      ///< Low bright red color.
uint32_t direction_color      = strip.Color(200, 200, 0);   ///< Moderately bright yellow color.
uint32_t brake_color          = strip.Color(200, 0, 0);     ///< Moderately bright red color.
int led_right                 = INITIAL_LED_RIGHT;
int led_left                  = INITIAL_LED_LEFT;
bool turn_right_cmd           = false;
bool turn_left_cmd            = false;
bool brake_cmd                = false;
long last_right_millis        = 0;
long last_right_light_millis  = 0;
long progression_delay        = 200;
long direction_on_delay       = 300;
long minimum_time_btwn_dir    = 1000;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////   SETUP FUNCTION     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**************************************************************************/
/*!
    @brief    Code executed one time at the beginning.
*/
/**************************************************************************/
void setup() {
  pinMode(RIGHT_LED_PIN,INPUT);
  pinMode(LEFT_LED_PIN,INPUT);
  pinMode(BRAKE_LED_PIN,INPUT);
  strip.begin();                         ///< This initializes the NeoPixel library.
  strip.fill(position_color);            ///< Fill the entire strip with one color.
  strip.show();                          ///< This sends the updated pixel color to the hardware.
}

/**************************************************************************/
/*!
    @brief    Right light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void turn_right(){
  if ((turn_right_cmd) && (turn_left_cmd==false) && (brake_cmd==false)){
    if(led_right<=NUM_PIXELS){
      unsigned long current_right_millis = millis();
      if(current_right_millis - last_right_millis >= progression_delay){
        strip.setPixelColor(led_right,direction_color);
        strip.show();
        led_right=led_right+1;
        last_right_millis=current_right_millis;
      }
    }else{
      unsigned long current_right_millis = millis();
      if(current_right_millis - last_right_millis >= direction_on_delay){
        led_right=INITIAL_LED_RIGHT;
        turn_right_cmd=false;
        strip.fill(position_color);
      }
   }else{
      ///< poner de posición o freno
   }
 }
}

/**************************************************************************/
/*!
    @brief    Left light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void turn_left(){

}

/**************************************************************************/
/*!
    @brief    Break light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void brake(){


}

/**************************************************************************/
/*!
    @brief    Read input pins to decide which lights turn on.
    @return   None.
*/
/**************************************************************************/
void check_inputs(){
  if(digitalRead(RIGHT_LED_PIN))
    unsigned long current_right_light_millis = millis();
    if(current_right_light_millis-last_right_light_millis>minimum_time_btwn_dir)
      turn_right_cmd=true;
  else
    turn_right_cmd=false;
  if(digitalRead(LEFT_LED_PIN))
    turn_left_cmd=true;
  else
    turn_left_cmd=false;
  if(digitalRead(BRAKE_LED_PIN))
    brake_cmd=true;
  else
    brake_cmd=false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////  MAIN LOOP FUNCTION  //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**************************************************************************/
/*!
    @brief    Main loop repeated constantly.
*/
/**************************************************************************/
void loop() {
  check_inputs();
  turn_right();
  turn_left();
  brake();
}
