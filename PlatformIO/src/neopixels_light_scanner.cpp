/* 
   Knight Rider KITT LED Light Scanner with neopixels or neopixels plus button or neopixels plus button plus potentiometers/trimmers
   or neopixels plus potentiometers/trimmers, and a uC board.
   
      Board: Pro Micro (Sparkfun Pro Micro clone) or equivalent.
              Should work with any board that has an AVR microcontroller and it works with the necessary
              libraries and it has enough memory space. Maybe also work with ESP32, ESP8266, RP2040,...

      Necessary libraries: FastLED by Daniel Garcia and FireTimer by PowerBroker2.
      
      Version: v1.0
      Author: @ElectroZeusTIC (https://github.com/electrozeustic)
*/

#include <Arduino.h>
#include <FastLED.h>
#include <FireTimer.h>

/////////////////////////////////////////////////////////////// TYPES ///////////////////////////////////////////////////////////////

typedef enum ledsSyst { NOT_STARTED,
                        STOPPED,
                        STARTING_FADEOUT,
                        STARTING_FADEIN,
                        STARTED,
                        PAUSED         } tSystState; // System states

//////////////////////////////////////////////////////// DEFINES & CONSTANTS ////////////////////////////////////////////////////////

#define LEDS_BUTTON_POTENTIOMETERS      // Select assembly type: ONLY_LEDS, LEDS_BUTTON, LEDS_POTENTIOMETERS or LEDS_BUTTON_POTENTIOMETERS

const int PIN_DATAIN_STRIPLINE  =    9, // Pin data in of the LED stripline
          NUM_LEDS_STRIPLINE    =    8; // Total number of LEDs on the LED stripline. Be careful with this number! (1 <= NUMBER_SNAKE_LEDS <= NUM_LEDS_STRIPLINE)

const uint8_t NUMBER_SNAKE_LEDS =    7, // Subset of LEDs on the stripline that move. Be careful with this number! (1 <= NUMBER_SNAKE_LEDS <= NUM_LEDS_STRIPLINE)
              MIN_BRIGHTNESS    =    1, // Minimum brightness: 1 ~ 255.
              MAX_BRIGHTNESS    =  255; // Max brigthness: 1 ~ 255. MAX_BRIGHTNESS >= MIN_BRIGHTNESS

#if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
    const int  PIN_ANALOG_IN_BRIGHTNESS =   A0, // Analog pin of brightness potentiometer
               PIN_ANALOG_IN_COLOR      =   A6, // Analog pin of color (tone) potentiometer
               PIN_ANALOG_IN_SPEED      =   A1; // Analog pin of speed potentiometer
    const long MAX_DELAY_LED            = 2000, // Maximum delay LED period, in ms. This is the lowest speed. MAX_DELAY_LED > MIN_DELAY_LED
               MIN_DELAY_LED            =    2, // Minimum delay LED period, in ms. This is the highest speed. MAX_DELAY_LED > MIN_DELAY_LED
               LOW_BOUND_ANALOG_SPEED_INPUT =   48; // This value obtained empirically for high speed with my circuit: 0 ~ 1023. Increase this value a little to increase the maximum speed.
#endif

#if defined(LEDS_BUTTON)  || defined(LEDS_BUTTON_POTENTIOMETERS)
    const int  PIN_INPUT_BUTTON         =   21; // Button digital pin
    const unsigned long TEMP_DEB        =   15, // Button debounce time in ms
                        T_LONG_PRESS    = 1000; // Long press activated button time > 2 * TEMP_DEB, in ms
#endif          

    const unsigned long T_FADE_IN       =       850, // Fade-in duration, in ms
                        T_FADE_OUT      = T_FADE_IN; // Fade-out duration, in ms. T_FADE_IN if you want it to last the same as the fade-in

//////////////////////////////////////////////////////////// VARIABLES ////////////////////////////////////////////////////////////

unsigned long tDELAY_LED               = 90; // Next changing LEDs period, in ms

CRGB leds_color                        = CRGB(255, 0, 0), // Values (red, green, blue) of the initial color: 0 ~ 255
     ledsStripline[NUM_LEDS_STRIPLINE] =             {0},
     barLEDs_snake[NUMBER_SNAKE_LEDS]  =             {0};

#if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
    int pot1Value    = 0, // Analog values for brightness, color and speed of LEDs if potentiometers exit
        pot2Value    = 0,
        pot3Value    = 0,
        outputValue1 = 0,
        outputValue2 = 0,
        outputValue3 = 0;
#endif

#if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_BUTTON)
    bool activatedButton          = false,  // To know if the button has been activated
         lastActivatedButtonState = false,  // The previous reading from the input pin (button)
         finishedDebounce         = true,   // To know that the debounce has finished
         longPressEnded           = true;   // To indicate that the long press period has ended
#endif

tSystState state = NOT_STARTED, lastState = NOT_STARTED; // Initial system state, last system state

//////////////////////////////////////////////////////////// INSTANCES ////////////////////////////////////////////////////////////

FireTimer ledMovTimer, debounceTimer, longPressTimer;

/////////////////////////////////////////////////////// FUNCTION DECLARATIONS /////////////////////////////////////////////////////

void initializeSnake(void);
void nextLEDs(tSystState);

/////////////////////////////////////////////////////////////  SETUP  /////////////////////////////////////////////////////////////

void setup() {  
  FastLED.addLeds<NEOPIXEL, PIN_DATAIN_STRIPLINE>(ledsStripline, NUM_LEDS_STRIPLINE).setCorrection(TypicalLEDStrip);
  FastLED.clear(true);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_BUTTON)
      // Next state for LEDS_BUTTON or LEDS_BUTTON_POTENTIOMETERS assembly type.
      state = STOPPED;
  #else
      // Next state for ONLY_LEDS or LEDS_POTENTIOMETERS assembly type. 
      state = STARTED; // or STARTING_FADEIN 
  #endif
  initializeSnake();
  ledMovTimer.begin(tDELAY_LED);  
  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_BUTTON)
      pinMode(PIN_INPUT_BUTTON, INPUT);
      debounceTimer.begin(TEMP_DEB);
      longPressTimer.begin(T_LONG_PRESS);
  #endif   
} // End void setup()

/////////////////////////////////////////////////////////////  LOOP  //////////////////////////////////////////////////////////////

void loop() {

  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
      // Values read from the potentiometers and their re-mappings and limits

      pot1Value    = analogRead(PIN_ANALOG_IN_BRIGHTNESS);
      pot2Value    = analogRead(PIN_ANALOG_IN_COLOR);  
      pot3Value    = analogRead(PIN_ANALOG_IN_SPEED);  
      outputValue1 = map(pot1Value, 0, 1023, MIN_BRIGHTNESS, MAX_BRIGHTNESS); 
      outputValue1 = constrain(outputValue1, MIN_BRIGHTNESS, MAX_BRIGHTNESS);  
      outputValue2 = map(pot2Value, 0, 1023, 0, 255); 
      outputValue2 = constrain(outputValue2, 0, 255);
      leds_color   = CHSV(outputValue2, 255, outputValue1); // Use FastLED automatic HSV->RGB conversion  
      outputValue3 = map(pot3Value, LOW_BOUND_ANALOG_SPEED_INPUT, 1023, MIN_DELAY_LED, MAX_DELAY_LED);
      tDELAY_LED   = (unsigned long)constrain(outputValue3, MIN_DELAY_LED, MAX_DELAY_LED);
  #endif  
  
  if (ledMovTimer.fire(false) && state != STOPPED) nextLEDs(state); // ledMovTimer timeout => Update LEDs
  
  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_BUTTON)
      activatedButton = digitalRead(PIN_INPUT_BUTTON) == LOW ? true : false;
      if (activatedButton != lastActivatedButtonState) {
        debounceTimer.reset();  // It's reactivated the debounce period        
        finishedDebounce = false;           
      }

      if (debounceTimer.fire(false) && finishedDebounce == false) { // Debounce time timeout
        if (activatedButton == true) {
          longPressTimer.start(); // It's reactivated the long press period
          longPressEnded = false;    
          lastState      = state;
        } else if (activatedButton == false && lastActivatedButtonState == true) {
          lastState = state;
          if (longPressTimer.fire(false) && longPressEnded == false) {
            switch (state) {      
              case STOPPED: state = STARTING_FADEIN;
                            break; 
              case STARTED: state = STARTING_FADEOUT;
                            break;
              case PAUSED:  state = STARTING_FADEOUT;
                            break;            
              default:      break;       
            } // End switch (state)
            longPressEnded = true;          
          } else if (longPressEnded == false) {
              switch (state) {              
                case STARTED: state = PAUSED;
                              break;
                case PAUSED:  state = STARTED;
                              break;            
                default:      break;   
              } // End switch (state)        
          } // End else if (longPressEnded == false)              
        } // End else if (activatedButton == false && lastActivatedButtonState == true)
        finishedDebounce = true;    
      } // End if (debounceTimer.fire(false) && finishedDebounce == false)
      lastActivatedButtonState = activatedButton;  
  #endif
} // End void loop()

////////////////////////////////////////////////////////////// FUNCTIONS //////////////////////////////////////////////////////////////

/// void initializeSnake(): initializes the barLEDs_snake array with a fading sequence

void initializeSnake() {
  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
    FastLED.setBrightness(255);
    const uint8_t ALT_BRIGHTNESS = 145; // This value should be adjusted according to the LED strip used and our experience
  #else
    const uint8_t ALT_BRIGHTNESS = 160; // This value should be adjusted according to the LED strip used and our experience
  #endif

  barLEDs_snake[NUMBER_SNAKE_LEDS - 1] = leds_color;
  if (NUMBER_SNAKE_LEDS > 1) {
    for (byte i = 1; i < NUMBER_SNAKE_LEDS; i++) {      
      uint8_t corr_brightness = brighten8_video((uint8_t)round((255.0f * (i + 1.0f) / NUMBER_SNAKE_LEDS)));
      if (corr_brightness > ALT_BRIGHTNESS) corr_brightness = ALT_BRIGHTNESS;
      barLEDs_snake[NUMBER_SNAKE_LEDS - i - 1] = leds_color.fadeLightBy(corr_brightness);      
    } // End for (byte i = 1; i < NUMBER_SNAKE_LEDS; i++)
  } // End if (NUMBER_SNAKE_LEDS > 1)
} // End void initializeSnake()

/// void nextLEDs(tSystState newState): it executes the relevant actions for each state and updates it when they are finished, if applicable.

void nextLEDs(tSystState newState) {  
  static int dot = 0;
  int temp_dot;
  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
    int maxBrightnessLevel = outputValue1;
  #else
    int maxBrightnessLevel = MAX_BRIGHTNESS;
  #endif 

  fill_solid(ledsStripline, NUM_LEDS_STRIPLINE, CRGB::Black);

  #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_POTENTIOMETERS)
      initializeSnake();
  #endif

  switch (newState) {
    #if defined(LEDS_BUTTON_POTENTIOMETERS) || defined(LEDS_BUTTON)        
        case STARTING_FADEOUT: {
                                  for (int j = 0; j < NUMBER_SNAKE_LEDS; j++) {
                                    temp_dot = dot + j < NUM_LEDS_STRIPLINE ? dot + j : 2 * (NUM_LEDS_STRIPLINE - 1) - dot - j;
                                    if (temp_dot < 0) temp_dot = -temp_dot;
                                    ledsStripline[temp_dot] = barLEDs_snake[j];
                                  }   
                                  for (int brightnessLevel = maxBrightnessLevel; brightnessLevel >= 0 ; brightnessLevel--) {
                                    FastLED.setBrightness(brightnessLevel);                             
                                    FastLED.delay(T_FADE_OUT / maxBrightnessLevel);
                                  }                            
                                  state = STOPPED;
                                  break;
                              }

        case PAUSED: {                        
                        for (int j = 0; j < NUMBER_SNAKE_LEDS; j++) {
                          temp_dot = dot + j < NUM_LEDS_STRIPLINE ? dot + j : 2 * (NUM_LEDS_STRIPLINE - 1) - dot - j;
                          if (temp_dot < 0) temp_dot = -temp_dot;
                          ledsStripline[temp_dot] = barLEDs_snake[j];
                        }
                        FastLED.show(); // 30 us per pixel => 240 us the stripline per show()
                        lastState = PAUSED;                        
                        break;                    
                    }
    #endif

    case STARTING_FADEIN: {                            
                            ledMovTimer.start();                            
                            for (int j = 0; j < NUMBER_SNAKE_LEDS; j++) {
                              temp_dot = dot + j < NUM_LEDS_STRIPLINE ? dot + j : 2 * (NUM_LEDS_STRIPLINE - 1) - dot - j;
                              if (temp_dot < 0) temp_dot = -temp_dot;
                              ledsStripline[temp_dot] = barLEDs_snake[j];
                            }                
                            for (int brightnessLevel = 1 ; brightnessLevel <= maxBrightnessLevel ; brightnessLevel++) {
                              FastLED.setBrightness(brightnessLevel);                                                                                          
                              FastLED.delay (T_FADE_IN / maxBrightnessLevel);
                            }                            
                            state = STARTED;                                                                 
                            break;
                          }

    case STARTED:  {
                      int temp_dot;
                      for (int j = 0; j < NUMBER_SNAKE_LEDS; j++) {
                        temp_dot = dot + j < NUM_LEDS_STRIPLINE ? dot + j : 2 * (NUM_LEDS_STRIPLINE - 1) - dot - j;
                        if (temp_dot < 0) temp_dot = -temp_dot;
                        ledsStripline[temp_dot] = barLEDs_snake[j];
                      }
                      FastLED.show(); // 30 us per pixel => 240 us the stripline per show()
                      ledMovTimer.update(tDELAY_LED);     
                      dot += 1;
                      if (dot > 2 * (NUM_LEDS_STRIPLINE - 1)) dot = 1;
                      lastState = STARTED;                                              
                      break;
                   }

    default:  {
                break;  // In other cases nothing is done.        
              }
  } // End switch (newState)
} // End void nextLEDs(tSystState newState)