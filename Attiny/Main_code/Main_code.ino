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
#define STRIP_PIN                     4                ///< Which pin on the Attiny is connected to the NeoPixels?
#define NUM_PIXELS                    8                ///< How many NeoPixels are attached to the Arduino?
#define FIRST_PIXEL_ADDR              0                ///< For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
#define RIGHT_LED_PIN                 0                ///< Input from right intermitent motorbike light.
#define LEFT_LED_PIN                  1                ///< Input from left intermitent motorbike light.
#define BRAKE_LED_PIN                 2                ///< Input from break motorbike light.
#define DIRECTION_PROGRESSION_DELAY   100              ///< Time between turning on progressive leds.
#define DIRECTION_ON_DELAY            300              ///< Time with all LEDs turned on in progression.
#define INITIAL_LED_RIGHT             NUM_PIXELS/2     ///< First led related to right light with connector in left side. Otherwise: int led_right=0;
#define SECOND_THIRD_FIRST_PIXEL      NUM_PIXELS/3     ///< .
#define THIRD_THIRD_FIRST_PIXEL       2*(NUM-PIXELS/3) ///< .
#define INITIAL_LED_LEFT              0                ///< First led related to left light with connector in left side. Otherwise: int led_left=(NUM_PIXELS/2);
#define TURN_ON_DELAY                 500              ///< Time that direction lights remain on after progression.
#define MIN_TIME_BTWN_DIR             800              ///< .

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   INSTANCES    //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   GLOBAL VARIABLES     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
uint32_t position_color       = strip.Color(50, 0, 0);      ///< Low bright RED color.
uint32_t direction_color      = strip.Color(200, 200, 0);   ///< Moderately bright YELLOW color.
uint32_t brake_color          = strip.Color(200, 0, 0);     ///< Moderately bright RED color.
int led_right                 = INITIAL_LED_RIGHT;          ///< Variable used to store the current pixel in right progression.
int led_left                  = INITIAL_LED_LEFT;           ///< Variable used to store the current pixel in left progression.
bool turn_right_cmd           = false;                      ///< Flag to indicate turn on of the right light.
bool turn_left_cmd            = false;                      ///< Flag to indicate turn on of the left light.
bool brake_cmd                = false;                      ///< Flag to indicate turn on of the brake light.
long last_right_millis        = 0;                          ///< Initialize last time a pixel was turned on in right progression.
long last_right_light_millis  = 0;                          ///< Initialize last time the right light in the motorbike was turned on.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////   SETUP FUNCTION     //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**************************************************************************/
/*!
    @brief    Code executed one time at the beginning.
*/
/**************************************************************************/
void setup() {
  pinMode(RIGHT_LED_PIN,INPUT);          ///< Declare right intermitent led pin as input.
  pinMode(LEFT_LED_PIN,INPUT);           ///< Declare left intermitent led pin as input.
  pinMode(BRAKE_LED_PIN,INPUT);          ///< Declare break led pin as input.
  strip.begin();                         ///< This initializes the NeoPixel library.
  delay(TURN_ON_DELAY);                  ///< Make sure the strip is powered.
  strip.fill(position_color);            ///< Fill the entire strip with position color.
  strip.show();                          ///< This sends the updated pixel color to the hardware.
}

/**************************************************************************/
/*!
    @brief    Right light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void turn_right(){
  if ((turn_right_cmd==true) && (turn_left_cmd==false) && (brake_cmd==false)){          ///< Just the right light on.
    if(led_right<=NUM_PIXELS-1){                                                          ///< .
      unsigned long current_right_millis = millis();                                    ///< .
      if(current_right_millis - last_right_millis >= DIRECTION_PROGRESSION_DELAY){      ///< .
        strip.setPixelColor(led_right,direction_color);                                 ///< .
        strip.show();                                                                   ///< .
        led_right=led_right+1;                                                          ///< .
        last_right_millis=current_right_millis;                                         ///< .
      }
    }else{                                                                              ///< .
      unsigned long current_right_millis = millis();                                    ///< .
      if(current_right_millis - last_right_millis >= direction_on_delay){               ///< .
        led_right=INITIAL_LED_RIGHT;                                                    ///< .
        turn_right_cmd=false;                                                           ///< .
        strip.fill(position_color);                                                     ///< .
        strip.show();                                                                   ///< .
      }
      else{
      ///< poner de posición o freno
      }
   }
 }else if((turn_right_cmd==true) && (turn_left_cmd==false) && (brake_cmd==true))
}

/**************************************************************************/
/*!
    @brief    Left light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void turn_left(){
  if ((turn_left_cmd==true) && (turn_right_cmd==false) && (brake_cmd==false)){  ///< .
    strip.fill(direction_color);                                                ///< .
    strip.show();                                                               ///< .
  }
}

/**************************************************************************/
/*!
    @brief    Break light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void brake(){
  if ((brake_cmd==true) && (turn_left_cmd==false) && (turn_right_cmd==false)){  ///< .
      strip.fill(brake_color);                                                  ///< .
      strip.show();                                                             ///< .
  }
}

/**************************************************************************/
/*!
    @brief    Position light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void brake(){
  if ((brake_cmd==false) && (turn_left_cmd==false) && (turn_right_cmd==false)){ ///< .
      strip.fill(position_color);                                               ///< .
      strip.show();                                                             ///< .
  }
}

/**************************************************************************/
/*!
    @brief    Emergency light is activated in the motorbike
    @return   None.
*/
/**************************************************************************/
void emergency(){
  if ((brake_cmd==false) && (turn_left_cmd==true) && (turn_right_cmd==true)){ ///< .
  }
}

/**************************************************************************/
/*!
    @brief    Read input pins to decide which lights turn on.
    @return   None.
*/
/**************************************************************************/
void check_inputs(){
  if(digitalRead(RIGHT_LED_PIN)){                                               ///< .
    unsigned long current_right_light_millis = millis();                        ///< .
    if(current_right_light_millis-last_right_light_millis>MIN_TIME_BTWN_DIR)    ///< .
      turn_right_cmd=true;                                                      ///< .
  }else                                                                         ///< .
    turn_right_cmd=false;                                                       ///< .
  if(digitalRead(LEFT_LED_PIN))                                                 ///< .
    turn_left_cmd=true;                                                         ///< .
  else                                                                          ///< .
    turn_left_cmd=false;                                                        ///< .
  if(digitalRead(BRAKE_LED_PIN))                                                ///< .
    brake_cmd=true;                                                             ///< .
  else                                                                          ///< .
    brake_cmd=false;                                                            ///< .
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
  check_inputs();                                                               ///< .
  turn_right();                                                                 ///< .
  turn_left();                                                                  ///< .
  position();                                                                   ///< .
  emergency();
  brake();                                                                      ///< .
}
