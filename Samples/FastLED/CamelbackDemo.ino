//  _      ___________   _____                      _ _           _    
// | |    |  ___|  _  \ /  __ \                    | | |         | |   
// | |    | |__ | | | | | /  \/ __ _ _ __ ___   ___| | |__   __ _| | __
// | |    |  __|| | | | | |    / _` | '_ ` _ \ / _ \ | '_ \ / _` | |/ /
// | |____| |___| |/ /  | \__/\ (_| | | | | | |  __/ | |_) | (_| |   < 
// \_____/\____/|___/    \____/\__,_|_| |_| |_|\___|_|_.__/ \__,_|_|\_\

// Festival LED Camelbak
// Big ups to Andrew Tuline, Mark Kreigsman and the FastLED community

#include <FastLED.h> // Include FastLED Library
#include <Button.h>  // Include Button Library.  Includes press, long press, double press detection.

FASTLED_USING_NAMESPACE

//Warn me if FASTLED != 3.1 or later
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

//LED Setup
#define LED_PIN 11        //Data pin
#define LED_TYPE WS2811   //LED Type (Note: WS2811 driver used for WS2812B)
#define COLOR_ORDER GRB   //LED Color Order
#define NUM_LEDS 60       //Number of LEDs
#define SECONDS_PER_PALETTE 10
CRGBArray<NUM_LEDS> leds; //Name of LED Array, allows for mirror

//LED Color Palette & Blending
CRGBPalette16 currentPalette; //Color Palette
CRGBPalette16 targetPalette;
TBlendType    currentBlending; //Color Blending
extern CRGBPalette16 warriors;
extern const TProgmemPalette16 warriors_p PROGMEM;
extern CRGBPalette16 usa;
extern const TProgmemPalette16 usa_p PROGMEM;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Button Setup
const int buttonPin = 6;  // Digital pin used for debounced pushbutton
Button myBtn(buttonPin, true, true, 50);  // Declare the button
int val1;
int minmap = 1;
int maxmap = 100;
int pot1;

// Mode Variables
int ledMode = 15; // First active mode
int maxMode;
int animationSpeed; //Pot will adjust anywhere animationSpeed is used
unsigned int dimmer = 1;
uint8_t gCurrentPaletteNumber = 0;  // Current palette number from the 'playlist' of color palettes

// Setup
void setup() {
  delay(1000); // Three second power-up safety delay
  
  //FASTLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  currentBlending = LINEARBLEND;
  set_max_power_in_volts_and_milliamps(5, 500);
  change_mode(ledMode, 1);
  }

// ___  ___      _         _                       
// |  \/  |     (_)       | |                      
// | .  . | __ _ _ _ __   | |     ___   ___  _ __  
// | |\/| |/ _` | | '_ \  | |    / _ \ / _ \| '_ \ 
// | |  | | (_| | | | | | | |___| (_) | (_) | |_) |
// \_|  |_/\__,_|_|_| |_| \_____/\___/ \___/| .__/ 
//                                          | |    
//                                          |_|    

void loop() {
  buttonRead();
  potRead();
  change_mode(ledMode, 0);
  show_at_max_brightness_for_power();
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }
}

//   ___        _                 _   _                 
//  / _ \      (_)               | | (_)                
// / /_\ \_ __  _ _ __ ___   __ _| |_ _  ___  _ __  ___ 
// |  _  | '_ \| | '_ ` _ \ / _` | __| |/ _ \| '_ \/ __|
// | | | | | | | | | | | | | (_| | |_| | (_) | | | \__ \
// \_| |_/_| |_|_|_| |_| |_|\__,_|\__|_|\___/|_| |_|___/

void change_mode(int newMode, int mc){                        // mc stands for 'Mode Change', where mc = 0 is strobe the routine, while mc = 1 is change the routine

   maxMode = 14;
   if(mc) fill_solid(leds,NUM_LEDS,CRGB(0,0,0));              // Clean up the array for the first time through. Don't show display though, so you may have a smooth transition.

  switch (newMode) {                                          // First time through a new mode, so let's initialize the variables for a given display.

    case 0: if(mc) {} rainbow(); break; //Rainbow Colors
    case 1: if(mc) {} currentBlending = LINEARBLEND; rainbowStripe(); break; //Rainbow Stripe
    case 2: if(mc) {} rainbowStripeMirror(); break; //Rainbow stripe mirrored
    case 3: if(mc) {maxmap = 500;} rainbowWithGlitter(); break; //Rainbow with Glitter
    case 4: if(mc) {minmap = 1; maxmap = 50;} confetti(); break; //Confetti
    case 5: if(mc) {minmap = 25; maxmap = 150;} sinelon(); break; //Colored dot
    case 6: if(mc) {} sinelonMirror(); break; //Colored dot mirrored
    case 7: if(mc) {minmap = 10;} bpm(); break; //BPM
    case 8: if(mc) {maxmap = 100;} juggle(); break; //Eight colored dots
    case 9: if(mc) {} juggleMirror(); break; //Eight colored dots
    case 10: if(mc) {} lightning(); break; //Crack bang
    case 11: if(mc) {minmap = 1; maxmap = 100;} partyColors(); break; //Party Colors
    case 12: if(mc) {} warriorsColors(); break;  //GO WARRIORS
    case 13: if(mc) {} usaFlag(); break; //USA Flag
    case 14: if(mc) {} EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
                gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
                targetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
               }

               EVERY_N_MILLISECONDS(10) {
                nblendPaletteTowardPalette(currentPalette, targetPalette, 60);
                }
                paletteCycle( leds, NUM_LEDS, currentPalette);
                break;

    // Demo mode
    case 15: demo_mode(); break;

  } // switch newMode

  ledMode = newMode;

}

void demo_mode(){
  uint8_t smode = 0;
  uint8_t secondHand = (millis() / 1000) % 220;                 // Change '60' to a different value to change length of the loop.
  static uint8_t lastSecond = 99;                                // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                               // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    
    switch(secondHand) {
      case 0: smode = 0; break;
      case 10: smode = 1; break;
      case 20: smode = 2; break;
      case 30: smode = 3; break;
      case 40: smode = 4; break;
      case 50: smode = 5; break;
      case 60: smode = 6; break;
      case 70: smode = 7; break;
      case 80: smode = 8; break;
      case 90: smode = 9; break;
      case 100: smode = 10; break;
      case 110: smode = 11; break;
      case 120: smode = 12; break;
      case 130: smode = 13; break;
    }
  } // if lastSecond

    switch(smode) {
      case 0: rainbow(); break; //Rainbow Colors
      case 1: rainbowStripe(); break; //Rainbow Stripe
      case 2: rainbowStripeMirror(); break; //Rainbow stripe mirrored
      case 3: rainbowWithGlitter(); break; //Rainbow with Glitter
      case 4: confetti(); break; //Confetti
      case 5: sinelon(); break; //Colored dot
      case 6: sinelonMirror(); break; //Colored dot mirrored
      case 7: bpm(); break; //BPM
      case 8: juggle(); break; //Eight colored dots
      case 9: juggleMirror(); break; //Eight colored dots
      case 10: lightning(); break; //Crack bang
      case 11: partyColors(); break; //Party Colors
      case 12: warriorsColors(); break;  //GO WARRIORS
      case 13: usaFlag(); break; //USA Flag
    }
} // demo_mode

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, -5);
  gHue = gHue + 0.8;
}

void rainbowStripe() {
  // Rainbow Stripe effect
  currentPalette = RainbowStripeColors_p;
 
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
 
  FillLEDsFromPaletteColors(startIndex);
  
  delay_at_max_brightness_for_power(animationSpeed);
}

void rainbowStripeMirror() {
  // Rainbow Stripe effect mirrored on both sides
  currentPalette = RainbowStripeColors_p;
 
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
 
  FillLEDsFromPaletteColorsMirror(startIndex);

  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
  
  delay_at_max_brightness_for_power(animationSpeed);
}

void rainbowWithGlitter() {
  // Built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(32);
  delay_at_max_brightness_for_power(50);
}

void addGlitter(fract8 chanceOfGlitter) {
  if( random8(-animationSpeed) < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  // Random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, animationSpeed);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(1), 200, 255);
  delay_at_max_brightness_for_power(500/animationSpeed);
}

void sinelon()
{
  // A colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 3);
  int pos = beatsin16(animationSpeed/12,0,NUM_LEDS);
  static int prevpos = 0;
  if( pos < prevpos ) { 
    fill_solid(leds+pos, (prevpos-pos)+1, CHSV(gHue,220,255));
  } else { 
    fill_solid(leds+prevpos, (pos-prevpos)+1, CHSV( gHue,220,255));
  }
  prevpos = pos;
}

void sinelonMirror()
{
  // A colored dot sweeping back and forth, with fading trails simultaneously up both sides
  fadeToBlackBy(leds, NUM_LEDS, 3);
  int pos = beatsin16(animationSpeed/8,0,NUM_LEDS/2);
  static int prevpos = 0;
  if( pos < prevpos ) { 
    fill_solid(leds+pos, (prevpos-pos)+1, CHSV(gHue,220,255));
  } else { 
    fill_solid(leds+prevpos, (pos-prevpos)+1, CHSV( gHue,220,255));
  }
  prevpos = pos;
  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
}


void bpm() {
  // Colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 25;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { 
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*(animationSpeed*0.35)));
  }
  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
}

void juggle() {
  // Eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+animationSpeed/10,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void juggleMirror() {
  // Eight colored dots, weaving in and out of sync with each other, mirrored
  fadeToBlackBy( leds, NUM_LEDS, 25);
  byte dothue = 0;
  for( int i = 0; i < 6; i++) {
    leds[beatsin16(i+animationSpeed/10,0,NUM_LEDS/2)] |= CHSV(dothue, 225, 255);
    dothue += 42;
  }
  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
}

void lightning() {

  FastLED.clear();
  
  static const uint8_t FLASHES = 8;
  static unsigned long flashNext;
  static uint8_t flashCounter = 0, flashMax = 0;
  static uint8_t ledlen, ledstart;
  static uint8_t flashchance = 10;
  
  // (re)init data
  if (flashCounter == flashMax) {
    ledlen = random8(3, NUM_LEDS-3); // length of flash, at least 3 LEDs
    ledstart = 1+random8(NUM_LEDS-ledlen); // start of flash
    flashNext = millis()+1000*random8(flashchance); // random start of flash sequence
    flashCounter = 0;
    flashMax = random8(3, FLASHES);
  }
 
  // show next flash, if it's time has come
  if (millis() >= flashNext) {
    uint8_t dimmer;
 
    if(flashCounter == 0) {
      // the brightness of the leader is scaled down by a factor of 5
      dimmer = 5;
    } else {
      // return strokes are brighter than the leader
      dimmer = random8(1, 3);
    }
 
    // Show a section of LED's
    fill_solid(leds+ledstart, ledlen, CHSV(255, 0, 255/dimmer));
 
    // each flash only lasts 4-10 milliseconds
    FastLED.delay(random8(4, 10));
 
    // Clear the section of LED's
    fill_solid(leds+ledstart, ledlen, CRGB::Black);
    FastLED.show();
 
    flashNext = millis()+50+random8(100);
    if (flashCounter == 0) {
      flashNext += 150;
    }
    flashCounter++;
  }
}

void partyColors() {
  // Party colors
  currentPalette = PartyColors_p;
 
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
 
  FillLEDsFromPaletteColors( startIndex);
   
  delay_at_max_brightness_for_power(animationSpeed);  
}

void warriorsColors() {
  //Golden State Warriors
  currentPalette = warriors_p;
  currentBlending = NOBLEND;
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 2;
 
  FillLEDsFromPaletteColorsMirror(startIndex);

  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);

  delay_at_max_brightness_for_power(animationSpeed);
}

void usaFlag() {
  // 'murca
  currentPalette = usa_p;
  currentBlending = NOBLEND;
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 2;
 
  FillLEDsFromPaletteColorsMirror(startIndex);

  leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);

  delay_at_max_brightness_for_power(animationSpeed);
}

void paletteCycle( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& currentPalette)
{
  // Cycle through the palette playlist
  static uint8_t startindex = 0;
  startindex++;
  fill_palette(ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, currentPalette, 255, LINEARBLEND);
  delay_at_max_brightness_for_power(animationSpeed);
}

//Fill colors from palette
void FillLEDsFromPaletteColors( uint8_t colorIndex) {
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, 255, currentBlending);
    colorIndex += 3;
  }
}

//Fill colors from palette for mirrored animations
void FillLEDsFromPaletteColorsMirror( uint8_t colorIndex) {
  for( int i = 0; i < NUM_LEDS/2; i++) {
    leds(NUM_LEDS/2 - 1 ,0)[i] = ColorFromPalette( currentPalette, colorIndex, 255, currentBlending);
    colorIndex += 2;
  }
}

// ______       _   _              
// | ___ \     | | | |             
// | |_/ /_   _| |_| |_ ___  _ __  
// | ___ \ | | | __| __/ _ \| '_ \ 
// | |_/ / |_| | |_| || (_) | | | |
// \____/ \__,_|\__|\__\___/|_| |_|
                                
void buttonRead() {
  myBtn.read();
  if(myBtn.wasPressed()) {
    ledMode = ledMode > maxMode ? 0 : ledMode+1;              // Reset to 0 only during a mode change
    change_mode(ledMode, 1);
  }
  if(myBtn.pressedFor(1000)) {
  ledMode = 15;
  //change_mode(ledMode, 1);
  }
}

void potRead() {
  val1 = analogRead(5);
  pot1 = map(val1, 0, 1023, minmap, maxmap);  //Remap pot value range
  animationSpeed = pot1;
}

// ______     _      _   _            
// | ___ \   | |    | | | |           
// | |_/ /_ _| | ___| |_| |_ ___  ___ 
// |  __/ _` | |/ _ \ __| __/ _ \/ __|
// | | | (_| | |  __/ |_| ||  __/\__ \
// \_|  \__,_|_|\___|\__|\__\___||___/

const TProgmemPalette16 warriors_p PROGMEM =
{
    CRGB::Gold, CRGB::Blue, CRGB::Gold, CRGB::Blue,
    CRGB::Gold, CRGB::Gold, CRGB::Blue, CRGB::Blue,
    CRGB::Gold, CRGB::Blue, CRGB::Gold, CRGB::Blue,
    CRGB::Gold, CRGB::Gold, CRGB::Blue, CRGB::Blue};

const TProgmemPalette16 usa_p PROGMEM =
{
    CRGB::Navy, CRGB::Navy, CRGB::Navy, CRGB::Navy, 
    CRGB::Red, CRGB::White, CRGB::Red, CRGB::White, 
    CRGB::Red, CRGB::White, CRGB::Red, CRGB::White, 
    CRGB::Red, CRGB::White, CRGB::Red, CRGB::White,
  };

DEFINE_GRADIENT_PALETTE( ib_jul01_gp ) {
    0, 194,  1,  1,
   94,   1, 29, 18,
  132,  57,131, 28,
  255, 113,  1,  1};

DEFINE_GRADIENT_PALETTE( es_vintage_57_gp ) {
    0,   2,  1,  1,
   53,  18,  1,  0,
  104,  69, 29,  1,
  153, 167,135, 10,
  255,  46, 56,  4};

DEFINE_GRADIENT_PALETTE( es_vintage_01_gp ) {
    0,   4,  1,  1,
   51,  16,  0,  1,
   76,  97,104,  3,
  101, 255,131, 19,
  127,  67,  9,  4,
  153,  16,  0,  1,
  229,   4,  1,  1,
  255,   4,  1,  1};

DEFINE_GRADIENT_PALETTE( es_rivendell_15_gp ) {
    0,   1, 14,  5,
  101,  16, 36, 14,
  165,  56, 68, 30,
  242, 150,156, 99,
  255, 150,156, 99};

DEFINE_GRADIENT_PALETTE( rgi_15_gp ) {
    0,   4,  1, 31,
   31,  55,  1, 16,
   63, 197,  3,  7,
   95,  59,  2, 17,
  127,   6,  2, 34,
  159,  39,  6, 33,
  191, 112, 13, 32,
  223,  56,  9, 35,
  255,  22,  6, 38};

DEFINE_GRADIENT_PALETTE( retro2_16_gp ) {
    0, 188,135,  1,
  255,  46,  7,  1};

DEFINE_GRADIENT_PALETTE( Analogous_1_gp ) {
    0,   3,  0,255,
   63,  23,  0,255,
  127,  67,  0,255,
  191, 142,  0, 45,
  255, 255,  0,  0};

DEFINE_GRADIENT_PALETTE( es_pinksplash_08_gp ) {
    0, 126, 11,255,
  127, 197,  1, 22,
  175, 210,157,172,
  221, 157,  3,112,
  255, 157,  3,112};

DEFINE_GRADIENT_PALETTE( es_pinksplash_07_gp ) {
    0, 229,  1,  1,
   61, 242,  4, 63,
  101, 255, 12,255,
  127, 249, 81,252,
  153, 255, 11,235,
  193, 244,  5, 68,
  255, 232,  1,  5};

DEFINE_GRADIENT_PALETTE( Coral_reef_gp ) {
    0,  40,199,197,
   50,  10,152,155,
   96,   1,111,120,
   96,  43,127,162,
  139,  10, 73,111,
  255,   1, 34, 71};

DEFINE_GRADIENT_PALETTE( es_ocean_breeze_068_gp ) {
    0, 100,156,153,
   51,   1, 99,137,
  101,   1, 68, 84,
  104,  35,142,168,
  178,   0, 63,117,
  255,   1, 10, 10};

DEFINE_GRADIENT_PALETTE( es_ocean_breeze_036_gp ) {
    0,   1,  6,  7,
   89,   1, 99,111,
  153, 144,209,255,
  255,   0, 73, 82};

DEFINE_GRADIENT_PALETTE( departure_gp ) {
    0,   8,  3,  0,
   42,  23,  7,  0,
   63,  75, 38,  6,
   84, 169, 99, 38,
  106, 213,169,119,
  116, 255,255,255,
  138, 135,255,138,
  148,  22,255, 24,
  170,   0,255,  0,
  191,   0,136,  0,
  212,   0, 55,  0,
  255,   0, 55,  0};

DEFINE_GRADIENT_PALETTE( es_landscape_64_gp ) {
    0,   0,  0,  0,
   37,   2, 25,  1,
   76,  15,115,  5,
  127,  79,213,  1,
  128, 126,211, 47,
  130, 188,209,247,
  153, 144,182,205,
  204,  59,117,250,
  255,   1, 37,192};

DEFINE_GRADIENT_PALETTE( es_landscape_33_gp ) {
    0,   1,  5,  0,
   19,  32, 23,  1,
   38, 161, 55,  1,
   63, 229,144,  1,
   66,  39,142, 74,
  255,   1,  4,  1};

DEFINE_GRADIENT_PALETTE( rainbowsherbet_gp ) {
    0, 255, 33,  4,
   43, 255, 68, 25,
   86, 255,  7, 25,
  127, 255, 82,103,
  170, 255,255,242,
  209,  42,255, 22,
  255,  87,255, 65};

DEFINE_GRADIENT_PALETTE( gr65_hult_gp ) {
    0, 247,176,247,
   48, 255,136,255,
   89, 220, 29,226,
  160,   7, 82,178,
  216,   1,124,109,
  255,   1,124,109};

DEFINE_GRADIENT_PALETTE( gr64_hult_gp ) {
    0,   1,124,109,
   66,   1, 93, 79,
  104,  52, 65,  1,
  130, 115,127,  1,
  150,  52, 65,  1,
  201,   1, 86, 72,
  239,   0, 55, 45,
  255,   0, 55, 45};

DEFINE_GRADIENT_PALETTE( GMT_drywet_gp ) {
    0,  47, 30,  2,
   42, 213,147, 24,
   84, 103,219, 52,
  127,   3,219,207,
  170,   1, 48,214,
  212,   1,  1,111,
  255,   1,  7, 33};

DEFINE_GRADIENT_PALETTE( ib15_gp ) {
    0, 113, 91,147,
   72, 157, 88, 78,
   89, 208, 85, 33,
  107, 255, 29, 11,
  141, 137, 31, 39,
  255,  59, 33, 89};

DEFINE_GRADIENT_PALETTE( Fuschia_7_gp ) {
    0,  43,  3,153,
   63, 100,  4,103,
  127, 188,  5, 66,
  191, 161, 11,115,
  255, 135, 20,182};

DEFINE_GRADIENT_PALETTE( es_emerald_dragon_08_gp ) {
    0,  97,255,  1,
  101,  47,133,  1,
  178,  13, 43,  1,
  255,   2, 10,  1};

DEFINE_GRADIENT_PALETTE( lava_gp ) {
    0,   0,  0,  0,
   46,  18,  0,  0,
   96, 113,  0,  0,
  108, 142,  3,  1,
  119, 175, 17,  1,
  146, 213, 44,  2,
  174, 255, 82,  4,
  188, 255,115,  4,
  202, 255,156,  4,
  218, 255,203,  4,
  234, 255,255,  4,
  244, 255,255, 71,
  255, 255,255,255};

DEFINE_GRADIENT_PALETTE( fire_gp ) {
    0,   1,  1,  0,
   76,  32,  5,  0,
  146, 192, 24,  0,
  197, 220,105,  5,
  240, 252,255, 31,
  250, 252,255,111,
  255, 255,255,255};

DEFINE_GRADIENT_PALETTE( Colorfull_gp ) {
    0,  10, 85,  5,
   25,  29,109, 18,
   60,  59,138, 42,
   93,  83, 99, 52,
  106, 110, 66, 64,
  109, 123, 49, 65,
  113, 139, 35, 66,
  116, 192,117, 98,
  124, 255,255,137,
  168, 100,180,155,
  255,  22,121,174};

DEFINE_GRADIENT_PALETTE( Magenta_Evening_gp ) {
    0,  71, 27, 39,
   31, 130, 11, 51,
   63, 213,  2, 64,
   70, 232,  1, 66,
   76, 252,  1, 69,
  108, 123,  2, 51,
  255,  46,  9, 35};

DEFINE_GRADIENT_PALETTE( Pink_Purple_gp ) {
    0,  19,  2, 39,
   25,  26,  4, 45,
   51,  33,  6, 52,
   76,  68, 62,125,
  102, 118,187,240,
  109, 163,215,247,
  114, 217,244,255,
  122, 159,149,221,
  149, 113, 78,188,
  183, 128, 57,155,
  255, 146, 40,123};

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};

DEFINE_GRADIENT_PALETTE( es_autumn_19_gp ) {
    0,  26,  1,  1,
   51,  67,  4,  1,
   84, 118, 14,  1,
  104, 137,152, 52,
  112, 113, 65,  1,
  122, 133,149, 59,
  124, 137,152, 52,
  135, 113, 65,  1,
  142, 139,154, 46,
  163, 113, 13,  1,
  204,  55,  3,  1,
  249,  17,  1,  1,
  255,  17,  1,  1};

DEFINE_GRADIENT_PALETTE( BlacK_Blue_Magenta_White_gp ) {
    0,   0,  0,  0,
   42,   0,  0, 45,
   84,   0,  0,255,
  127,  42,  0,255,
  170, 255,  0,255,
  212, 255, 55,255,
  255, 255,255,255};

DEFINE_GRADIENT_PALETTE( BlacK_Magenta_Red_gp ) {
    0,   0,  0,  0,
   63,  42,  0, 45,
  127, 255,  0,255,
  191, 255,  0, 45,
  255, 255,  0,  0};

DEFINE_GRADIENT_PALETTE( BlacK_Red_Magenta_Yellow_gp ) {
    0,   0,  0,  0,
   42,  42,  0,  0,
   84, 255,  0,  0,
  127, 255,  0, 45,
  170, 255,  0,255,
  212, 255, 55, 45,
  255, 255,255,  0};

DEFINE_GRADIENT_PALETTE( Blue_Cyan_Yellow_gp ) {
    0,   0,  0,255,
   63,   0, 55,255,
  127,   0,255,255,
  191,  42,255, 45,
  255, 255,255,  0};

DEFINE_GRADIENT_PALETTE( Miami_gp ) {
    0,   1,221, 53,
  255,  73,  3,178};

// Single array of defined cpt-city color palettes.
// This will let us programmatically choose one based on
// a number, rather than having to activate each explicitly 
// by name every time.
// Since it is const, this array could also be moved 
// into PROGMEM to save SRAM, but for simplicity of illustration
// we'll keep it in a regular SRAM array.
//
// This list of color palettes acts as a "playlist"; you can
// add or delete, or re-arrange as you wish.

const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
  Miami_gp,
  Sunset_Real_gp,
  es_rivendell_15_gp,
  es_ocean_breeze_036_gp,
  rgi_15_gp,
  retro2_16_gp,
  Analogous_1_gp,
  es_pinksplash_08_gp,
  Coral_reef_gp,
  es_ocean_breeze_068_gp,
  es_pinksplash_07_gp,
  es_vintage_01_gp,
  departure_gp,
  es_landscape_64_gp,
  es_landscape_33_gp,
  rainbowsherbet_gp,
  gr65_hult_gp,
  gr64_hult_gp,
  GMT_drywet_gp,
  ib_jul01_gp,
  es_vintage_57_gp,
  ib15_gp,
  Fuschia_7_gp,
  es_emerald_dragon_08_gp,
  lava_gp,
  fire_gp,
  Colorfull_gp,
  Magenta_Evening_gp,
  Pink_Purple_gp,
  es_autumn_19_gp,
  BlacK_Blue_Magenta_White_gp,
  BlacK_Magenta_Red_gp,
  BlacK_Red_Magenta_Yellow_gp,
  Blue_Cyan_Yellow_gp };

// Count of how many cpt-city gradients are defined:
const uint8_t gGradientPaletteCount = 
  sizeof( gGradientPalettes) / sizeof( TProgmemRGBGradientPalettePtr );

// End
