/**@file Main_code.ino
* @page Programming
* @brief Attiny sketch to control addressable LED strips reading lights signals from motorbike.
* @section intro_sec Introduction
*
*
* @ref Main_code.ino "Arduino code"
*
* @ref arduino_diagrams.md
*
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
#define NUM_PIXELS                    60               ///< How many NeoPixels are attached to the Arduino?
#define FIRST_PIXEL_ADDR              0                ///< For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
#define RIGHT_LED_PIN                 0                ///< Input from right intermitent motorbike light.
#define LEFT_LED_PIN                  1                ///< Input from left intermitent motorbike light.
#define BRAKE_LED_PIN                 2                ///< Input from break motorbike light.
#define DIRECTION_PROGRESSION_DELAY   20               ///< Time between turning on progressive leds.
#define DIRECTION_ON_DELAY            300              ///< Time with all LEDs turned on in progression.
#define INITIAL_LED_RIGHT             35               ///< First led related to right light with connector in left side. Otherwise: int led_right=0;
#define SECOND_THIRD_FIRST_PIXEL      25               ///< aaaa.
#define INITIAL_LED_LEFT              24               ///< First led related to left light with connector in left side. Otherwise: int led_left=(NUM_PIXELS/2);
#define TURN_ON_DELAY                 500              ///< Time that direction lights remain on after progression.
#define MIN_TIME_BTWN_DIR             800              ///< bbb.
#define MAX_INTENSITY                 200              ///< ccc.
#define LOW_INTENSITY                 20               ///< ddd.
#define MAX_INTENSITY_TEST            30               ///< eeee.
#define LOW_INTENSITY_TEST            5                ///< ffff.
#define DEBOUNCE_DELAY                50               ///< gggg.

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
  uint32_t position_color               = strip.Color(LOW_INTENSITY, 0, 0);                         ///< Low bright RED color.
uint32_t direction_color                = strip.Color(MAX_INTENSITY, MAX_INTENSITY, 0);             ///< Moderately bright YELLOW color.
uint32_t brake_color                    = strip.Color(MAX_INTENSITY, 0, 0);                         ///< Moderately bright RED color.
int led_right                           = INITIAL_LED_RIGHT;                                        ///< Variable used to store the current pixel in right progression.
int led_left                            = INITIAL_LED_LEFT;                                         ///< Variable used to store the current pixel in left progression.
bool turn_right_cmd                     = false;                                                    ///< Flag to indicate turn on of the right light.
bool turn_left_cmd                      = false;                                                    ///< Flag to indicate turn on of the left light.
bool brake_cmd                          = false;                                                    ///< Flag to indicate turn on of the brake light.
unsigned long last_right_millis         = 0;                                                        ///< Initialize last time a pixel was turned on in right progression.
unsigned long last_right_light_debounce = TURN_ON_DELAY;                                            ///< Initialize last time the right light in the motorbike was turned on.
bool right_light_reading                = false;                                                    ///< hhhh.
bool last_right_light_state             = false;                                                    ///< iiii.
bool right_light_state                  = false;                                                    ///< jjjjj.
unsigned long last_left_millis          = 0;                                                        ///< Initialize last time a pixel was turned on in left progression.
unsigned long last_left_light_debounce  = TURN_ON_DELAY;                                            ///< Initialize last time the left light in the motorbike was turned on.
bool left_light_reading                 = false;                                                    ///< kkkkk.
bool last_left_light_state              = false;                                                    ///< lllll.
bool left_light_state                   = false;                                                    ///< mmmmm.
unsigned long last_brake_millis         = 0;                                                        ///< Initialize last time a pixel was turned on in left progression.
unsigned long last_brake_light_debounce = TURN_ON_DELAY;                                            ///< Initialize last time the left light in the motorbike was turned on.
bool brake_light_reading                = false;                                                    ///< nnnn.
bool last_brake_light_state             = false;                                                    ///< oooo.
bool brake_light_state                  = false;                                                    ///< ppppp.

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
  pinMode(RIGHT_LED_PIN,INPUT);                                                       // Declare right intermitent led pin as input.
  pinMode(LEFT_LED_PIN,INPUT);                                                        // Declare left intermitent led pin as input.
  pinMode(BRAKE_LED_PIN,INPUT);                                                       // Declare break led pin as input.
  strip.begin();                                                                      // This initializes the NeoPixel library.
  delay(TURN_ON_DELAY);                                                               // Make sure the strip is powered.
  for(int i=SECOND_THIRD_FIRST_PIXEL;i<INITIAL_LED_RIGHT;i++){                        // Fill the entire strip with position color.
        strip.setPixelColor(i,position_color);
      }
  strip.show();                                                                       // This sends the updated pixel color to the hardware.
}

/*!
    @brief    Right light is activated in the motorbike
    @return   None.
*/
void turn_right(){
  unsigned long current_right_millis;
  if (turn_right_cmd==true) {                                                           // Just the right light on.
    if(led_right<NUM_PIXELS){                                                           // qqq.
      unsigned long current_right_millis = millis();                                    // rrrr.
      if(current_right_millis - last_right_millis >= DIRECTION_PROGRESSION_DELAY){      // sssss.
        strip.setPixelColor(led_right,direction_color);                                 // ttttt.
        led_right=led_right+1;                                                          // uuuuu.
        last_right_millis=current_right_millis;                                         // vvvvvv.
      }
    }else{                                                                              // wwww.
      current_right_millis = millis();                                                  // xxxx.
      if(current_right_millis - last_right_millis >= DIRECTION_ON_DELAY){               // yyyy.
        led_right=INITIAL_LED_RIGHT;                                                    // zzzzz.
        turn_right_cmd=false;                                                           // abbb.
        for(int i=INITIAL_LED_RIGHT;i<NUM_PIXELS;i++){                                  // Fill the entire strip with position color.
          strip.setPixelColor(i,0,0,0);
        }
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
  if (turn_left_cmd==true){                                                             // Just the right light on.
    if(led_left>=FIRST_PIXEL_ADDR){                                                     // accccc.
      unsigned long current_left_millis = millis();                                     // adddd.
      if(current_left_millis - last_left_millis >= DIRECTION_PROGRESSION_DELAY){        // aeeee.
        strip.setPixelColor(led_left,direction_color);                                  // afffff.
        led_left=led_left-1;                                                            // aggggg.
        last_left_millis=current_left_millis;                                           // ahhhh.
      }
    }else{                                                                              // aiiiii.
      current_left_millis = millis();                                                   // ajjjjj.
      if(current_left_millis - last_left_millis >= DIRECTION_ON_DELAY){                 // akkkk.
        led_left=INITIAL_LED_LEFT;                                                      // alllll.
        turn_left_cmd=false;                                                            // ammmm.
        for(int i=INITIAL_LED_LEFT;i>=FIRST_PIXEL_ADDR;i--){                             // Fill the entire strip with position color.
          strip.setPixelColor(i,0,0,0);
        }      
      }
   }
  }
}

/*!
    @brief    Break light is activated in the motorbike
    @return   None.
*/
void brake(){
  if (brake_cmd==true){                                                                // annnn.
    for(int i=SECOND_THIRD_FIRST_PIXEL;i<INITIAL_LED_RIGHT;i++){                       // Fill the entire strip with position color.
        strip.setPixelColor(i,brake_color);
      }
  }else{
    for(int i=SECOND_THIRD_FIRST_PIXEL;i<INITIAL_LED_RIGHT;i++){                       // Fill the entire strip with position color.
        strip.setPixelColor(i,position_color);
      }
  }
}

/*!
    @brief    Read right light state.
    @return   None.
*/
void check_right(){
  int right_light_reading=digitalRead(RIGHT_LED_PIN);
  if(right_light_reading!=last_right_light_state){                                    // aoooo.
    last_right_light_debounce = millis();                                             // apppp.
  }
  if(millis()-last_right_light_debounce>DEBOUNCE_DELAY){                              // aqqqq.
      if(right_light_reading != right_light_state){                                   // arrrr.
        right_light_state=right_light_reading;                                        // assss.
        if (right_light_state==true){                                                 // attt.
          turn_right_cmd=true;                                                        // auuu.
        }
      }
  }
  last_right_light_state=right_light_reading;                                         // avvvv.
}

/*!
    @brief    Read left light state.
    @return   None.
*/
void check_left(){
  int left_light_reading=digitalRead(LEFT_LED_PIN);
  if(left_light_reading!=last_left_light_state){                                      // aww.
    last_left_light_debounce = millis();                                              // axxxx.
  }
  if(millis()-last_left_light_debounce>DEBOUNCE_DELAY){                               // ayyyy.
      if(left_light_reading != left_light_state){                                     // azzzz.
        left_light_state=left_light_reading;                                          // baaaa.
        if (left_light_state==true){                                                  // bcccc.
          turn_left_cmd=true;                                                         // bdddddd.
        }
      }
  }
  last_left_light_state=left_light_reading;                                           // beeee.
}

/*!
    @brief    Read brake light state.
    @return   None.
*/
void check_brake(){
  int brake_light_reading=digitalRead(BRAKE_LED_PIN);
  if(brake_light_reading!=last_brake_light_state){                                    // bffff.
    last_brake_light_debounce = millis();                                             // bgggg.
  }
  if(millis()-last_brake_light_debounce>DEBOUNCE_DELAY){                              // bhhhh.
      if(brake_light_reading != brake_light_state){                                   // biiii.
        brake_light_state=brake_light_reading;                                        // bjjjjj.
        if (brake_light_state==true){                                                 // bkkkkk.
          brake_cmd=true;                                                             // blllll.
        }else
          brake_cmd=false;
      }
  }
  last_brake_light_state=brake_light_reading;                                         // bmmmmm.
}

/*---------------------------------------------
 -----------------------------------------------
 MAIN LOOP FUNCTION
 -----------------------------------------------        else
 ----------------------------------------------*/

/*!
 @brief    Main loop repeated constantly.  
 @return   None.
 */
void loop() {
  check_right();                                                                      // boooo.
  check_left();
  check_brake();
  turn_right();                                                                       // bpppp.
  turn_left();                                                                        // bqqqq.
  emergency();
  brake();                                                                            // brrrrr.
  strip.show();
}
