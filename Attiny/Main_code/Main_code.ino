/**@file Main_code.ino
* @page Programming
* @brief Attiny sketch to control addressable LED strips reading lights signals from motorbike.
* @section intro_sec Introduction
* Write Intro
* @subsection min_sec Things to have in mind
* - Motorbike lights work with 12V. A DC-DC converter is needed for powering the Attiny85.
* - Motorbike voltage is not as stable as it should due to the alternator properties.
* @	section Components
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
* @section Code
* @copydoc
* Main_code.ino
*/

/*---------------------------------------------
 -----------------------------------------------
 DEPENDENCIES 
 -----------------------------------------------                   
 ----------------------------------------------*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

/*---------------------------------------------
 -----------------------------------------------
 PARAMETERS 
 -----------------------------------------------                   
 ----------------------------------------------*/

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
#define THIRD_THIRD_FIRST_PIXEL       2*(NUM_PIXELS/3) ///< .
#define INITIAL_LED_LEFT              NUM_PIXELS/2-1   ///< First led related to left light with connector in left side. Otherwise: int led_left=(NUM_PIXELS/2);
#define TURN_ON_DELAY                 500              ///< Time that direction lights remain on after progression.
#define MIN_TIME_BTWN_DIR             800              ///< .
#define MAX_INTENSITY                 200              ///< .
#define LOW_INTENSITY                 50               ///< .
#define MAX_INTENSITY_TEST            30               ///< .
#define LOW_INTENSITY_TEST            5                ///< .
#define DEBOUNCE_DELAY                50               ///< .

/*---------------------------------------------
 -----------------------------------------------
 OBJECT INSTANCES  
 -----------------------------------------------               
 ----------------------------------------------*/
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

/*---------------------------------------------
 -----------------------------------------------
 GLOBAL VARIABLES
 -----------------------------------------------               
 ----------------------------------------------*/
uint32_t position_color                 = strip.Color(LOW_INTENSITY_TEST, 0, 0);                    ///< Low bright RED color.
uint32_t direction_color                = strip.Color(MAX_INTENSITY_TEST, MAX_INTENSITY_TEST, 0);   ///< Moderately bright YELLOW color.
uint32_t brake_color                    = strip.Color(MAX_INTENSITY_TEST, 0, 0);                    ///< Moderately bright RED color.
int led_right                           = INITIAL_LED_RIGHT;                                        ///< Variable used to store the current pixel in right progression.
int led_left                            = INITIAL_LED_LEFT;                                         ///< Variable used to store the current pixel in left progression.
bool turn_right_cmd                     = false;                                                    ///< Flag to indicate turn on of the right light.
bool turn_left_cmd                      = false;                                                    ///< Flag to indicate turn on of the left light.
bool brake_cmd                          = false;                                                    ///< Flag to indicate turn on of the brake light.
unsigned long last_right_millis         = 0;                                                        ///< Initialize last time a pixel was turned on in right progression.
unsigned long last_right_light_debounce = TURN_ON_DELAY;                                            ///< Initialize last time the right light in the motorbike was turned on.
bool right_light_reading                = false;                                                    ///< .
bool last_right_light_state             = false;                                                    ///< .
bool right_light_state                  = false;                                                    ///< .
unsigned long last_left_millis          = 0;                                                        ///< Initialize last time a pixel was turned on in left progression.
unsigned long last_left_light_debounce  = TURN_ON_DELAY;                                            ///< Initialize last time the left light in the motorbike was turned on.
bool left_light_reading                 = false;                                                    ///< .
bool last_left_light_state              = false;                                                    ///< .
bool left_light_state                   = false;                                                    ///< .
unsigned long last_brake_millis         = 0;                                                        ///< Initialize last time a pixel was turned on in left progression.
unsigned long last_brake_light_debounce = TURN_ON_DELAY;                                            ///< Initialize last time the left light in the motorbike was turned on.
bool brake_light_reading                = false;                                                    ///< .
bool last_brake_light_state             = false;                                                    ///< .
bool brake_light_state                  = false;                                                    ///< .

/*---------------------------------------------
 -----------------------------------------------
 SETUP FUNCTION
 -----------------------------------------------        
 ----------------------------------------------*/


/*!
 @brief    Code executed one time at the beginning.  
 @return   None.
 */
void setup() {
  pinMode(RIGHT_LED_PIN,INPUT);                  // Declare right intermitent led pin as input.
  pinMode(LEFT_LED_PIN,INPUT);                   // Declare left intermitent led pin as input.
  pinMode(BRAKE_LED_PIN,INPUT);                  // Declare break led pin as input.
  strip.begin();                                 // This initializes the NeoPixel library.
  delay(TURN_ON_DELAY);                          // Make sure the strip is powered.
  for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){  // Fill the entire strip with position color.
    strip.setPixelColor(i,position_color);
  }
  strip.show();                                  // This sends the updated pixel color to the hardware.
}

/*!
    @brief    Right light is activated in the motorbike
    @return   None.
*/
void turn_right(){
  unsigned long current_right_millis;
  if ((turn_right_cmd==true) && (brake_cmd==false)){                                    // Just the right light on.
    if(led_right<=NUM_PIXELS-1){                                                        // .
      unsigned long current_right_millis = millis();                                    // .
      if(current_right_millis - last_right_millis >= DIRECTION_PROGRESSION_DELAY){      // .
        strip.setPixelColor(led_right,direction_color);                                 // .
        strip.show();                                                                   // .
        led_right=led_right+1;                                                          // .
        last_right_millis=current_right_millis;                                         // .
      }
    }else{                                                                              // .
      unsigned long current_right_millis = millis();                                    // .
      if(current_right_millis - last_right_millis >= DIRECTION_ON_DELAY){               // .
        led_right=INITIAL_LED_RIGHT;                                                    // .
        turn_right_cmd=false;                                                           // .
        for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                                   // Fill the entire strip with position color.
          strip.setPixelColor(i,position_color);
        }
        strip.show();                                                                   // .
      }
   }
 }else if((turn_right_cmd==true) && (brake_cmd==true)){                                 // .
   for(int i=FIRST_PIXEL_ADDR;i<5;i++){                                     // Fill the entire strip with position color.
      strip.setPixelColor(i,30,0,0);
   }
   strip.show();                                                                      // .   
   if(led_right<=NUM_PIXELS-1){// .
    if(led_right>=5){
      current_right_millis = millis();                                                // .
      if(current_right_millis - last_right_millis >= DIRECTION_PROGRESSION_DELAY){    // .
        strip.setPixelColor(led_right,direction_color);                               // .
        strip.show();                                                                 // .
        led_right=led_right+1;                                                        // .
        last_right_millis=current_right_millis;                                       // .
      }
    }else if(led_right<5){
      led_right=5;
      strip.setPixelColor(led_right,direction_color);                                 // .
      strip.show();                                                                   // .
      led_right=led_right+1;                                                          // .
      last_right_millis=current_right_millis;                                         // .
    }
  }else{                                                                              // .
    unsigned long current_right_millis = millis();                                    // .
    if(current_right_millis - last_right_millis >= DIRECTION_ON_DELAY){               // .
      led_right=5;                                                                    // .
      turn_right_cmd=false;                                                           // .
      for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                                   // Fill the entire strip with position color.
        strip.setPixelColor(i,30,0,0);
      }
      strip.show();                                                                   // .
    }   
  }  
 }
}

/*!
    @brief    Left light is activated in the motorbike
    @return   None.
*/
void turn_left(){
  unsigned long current_left_millis;
  if ((turn_left_cmd==true) && (brake_cmd==false)){                                   // Just the right light on.
    if(led_left>=FIRST_PIXEL_ADDR){                                                   // .
      unsigned long current_left_millis = millis();                                   // .
      if(current_left_millis - last_left_millis >= DIRECTION_PROGRESSION_DELAY){      // .
        strip.setPixelColor(led_left,direction_color);                                // .
        strip.show();                                                                 // .
        led_left=led_left-1;                                                          // .
        last_left_millis=current_left_millis;                                         // .
      }
    }else{                                                                            // .
      current_left_millis = millis();                                   // .
      if(current_left_millis - last_left_millis >= DIRECTION_ON_DELAY){               // .
        led_left=INITIAL_LED_LEFT;                                                    // .
        turn_left_cmd=false;                                                          // .
        for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                                 // Fill the entire strip with position color.
          strip.setPixelColor(i,position_color);
        }
        strip.show();                                                                 // .
      }
      else{
      ///< poner de posición o freno
      }
   }
  }else if((turn_left_cmd==true) && (brake_cmd==true)){                                 // .
   for(int i=3;i<NUM_PIXELS;i++){                                     // Fill the entire strip with position color.
      strip.setPixelColor(i,30,0,0);
   }
   strip.show();                                                                     // .   
   if(led_left>=FIRST_PIXEL_ADDR){// .
    if(led_left<=2){
      current_left_millis = millis();                                    // .
      if(current_left_millis - last_left_millis >= DIRECTION_PROGRESSION_DELAY){      // .
        strip.setPixelColor(led_left,direction_color);                                 // .
        strip.show();                                                                   // .
        led_left=led_left-1;                                                          // .
        last_left_millis=current_left_millis;                                         // .
      }
    }else if(led_left>2){
      led_left=2;
      strip.setPixelColor(led_left,direction_color);                                 // .
      strip.show();                                                                   // .
      led_left=led_left-1;                                                          // .
      last_left_millis=current_left_millis;                                         // .
    }
  }else{                                                                              // .
    unsigned long current_left_millis = millis();                                    // .
    if(current_left_millis - last_left_millis >= DIRECTION_ON_DELAY){               // .
      led_left=2;                                                    // .
      turn_left_cmd=false;                                                           // .
      for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                                     // Fill the entire strip with position color.
        strip.setPixelColor(i,30,0,0);
      }
      strip.show();                                                                   // .
    }   
  }  
 }
}

/*!
    @brief    Break light is activated in the motorbike
    @return   None.
*/
void brake(){
  if ((brake_cmd==true) && (turn_left_cmd==false) && (turn_right_cmd==false)){  // .
    for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                               // Fill the entire strip with position color.
        strip.setPixelColor(i,brake_color);
      }
      strip.show();                                                             // .
  }
}

/*!
    @brief    Position light is activated in the motorbike
    @return   None.
*/
void position_f(){
  if ((brake_cmd==false) && (turn_left_cmd==false) && (turn_right_cmd==false)){ // .
   for(int i=FIRST_PIXEL_ADDR;i<NUM_PIXELS;i++){                                // Fill the entire strip with position color.
    strip.setPixelColor(i,position_color);
  }
      strip.show();                                                             // .
  }
}

/*!
    @brief    Emergency light is activated in the motorbike
    @return   None.
*/
void emergency(){
  if ((brake_cmd==false) && (turn_left_cmd==true) && (turn_right_cmd==true)){   // .
  }
}


/*!
    @brief    Read right light state.
    @return   None.
*/
void check_right(){
  int right_light_reading=digitalRead(RIGHT_LED_PIN);
  if(right_light_reading!=last_right_light_state){                              // .
    last_right_light_debounce = millis();                                       // .
  }
  if(millis()-last_right_light_debounce>DEBOUNCE_DELAY){                        // .
      if(right_light_reading != right_light_state){                             // .
        right_light_state=right_light_reading;                                  // .
        if (right_light_state==true){                                           // .
          turn_right_cmd=true;                                                  // .
        }
      }
  }
  last_right_light_state=right_light_reading;                                   // .
}

/*!
    @brief    Read left light state.
    @return   None.
*/
void check_left(){
  int left_light_reading=digitalRead(LEFT_LED_PIN);
  if(left_light_reading!=last_left_light_state){                                // .
    last_left_light_debounce = millis();                                        // .
  }
  if(millis()-last_left_light_debounce>DEBOUNCE_DELAY){                         // .
      if(left_light_reading != left_light_state){                               // .
        left_light_state=left_light_reading;                                    // .
        if (left_light_state==true){                                            // .
          turn_left_cmd=true;                                                   // .
        }
      }
  }
  last_left_light_state=left_light_reading;                                     // .
}

/*!
    @brief    Read brake light state.
    @return   None.
*/
void check_brake(){
  int brake_light_reading=digitalRead(BRAKE_LED_PIN);
  if(brake_light_reading!=last_brake_light_state){                              // .
    last_brake_light_debounce = millis();                                       // .
  }
  if(millis()-last_brake_light_debounce>DEBOUNCE_DELAY){                        // .
      if(brake_light_reading != brake_light_state){                             // .
        brake_light_state=brake_light_reading;                                  // .
        if (brake_light_state==true){                                           // .
          brake_cmd=true;                                                       // .
        }else
          brake_cmd=false;
      }
  }
  last_brake_light_state=brake_light_reading;                                   // .

}
/*---------------------------------------------
 -----------------------------------------------
 MAIN LOOP FUNCTION
 -----------------------------------------------        
 ----------------------------------------------*/

/*!
 @brief    Main loop repeated constantly.  
 @return   None.
 */
void loop() {
  check_right();                                                                // .
  check_left();
  check_brake();
  turn_right();                                                                 // .
  turn_left();                                                                  // .
  position_f();                                                                 // .
  emergency();
  brake();                                                                      // .
}
