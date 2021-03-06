
/*
 This is an example of how to drive a Neopixel string without ever turn off interrupts.
 It is Written for an Arduino Uno, but could easily be changed for any Arduino or AVR.
 More info at http://wp.josh.com/2014/05/11/ws2812-neopixels-made-easy/
*/


// We will be using Output B of Timer2 becuase we must have the clock bits and the force compare bits in a single
// register that we can write to atomically for our tricks to work.

// OC2B is pin #5 on the ATTMEGA chip which maps to Digital Pin 3 on an Arduino Uno. Connect this pin
// to the Data In on your NeoPixel string. 

// Change this to be at least as long as your pixel string (too long will work fine, just be a little slower)

#define PIXELS 96*11  // Number of pixels in the string

// These are the timing constraints taken mostly from the WS2812 datasheets 
// These are chosen to be conservative and avoid problems rather than for maximum throughput 

#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives

#define CYCLES_PER_SEC (F_CPU)

#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )

#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )


void setupTimer2(void) {
  
    // Assume pin 11 is low & input mode on reset
    // Assume compare output mode is Normal port operation, OC0A disconnected
    // Assume TCCR2B = 0 on reset
    // Assume OC2B = 0 on reset
    
    
    // Pull pin low with resistor, then leave pin in input mode and set OC2B to high, then send waveform to off, then enable output- which will show 0 on port, then enable WGN at clock at same write.
    // Hopefully OC2B will not drive the pullup when high?The general I/O port function is overridden by the Output Compare (OC2x) from the Waveform Generator if


    /*
      either of the COM2x1:0 bits are set. However, the OC2x pin direction (input or output) is still controlled by the
      Data Direction Register (DDR) for the port pin. The Data Direction Register bit for the OC2x pin (DDR_OC2x)
      must be set as output before the OC2x value is visible on the pin. The port override function is independent of
      the Waveform Generation mode
    */
        
    pinMode(3,OUTPUT);      // Pin 3 is the Timer2 output B, but until we enable waveform generation, 
                            // this will drive pin 11 low based on the current 0 in PORT
    
  
  
  
}


void sendBit( bool x ) {
  
    // Assume pin 11 is setup low & output mode 
    // Assume compare output mode is Normal port operation, OC0A disconnected
    // Assume TCCR2B = 0, 
    // Assume TCNT2 = 0
    // Assume AOC2B = 0    
    
    
    // First we need to get the OC2B output bit to be a 1    
    // The DDR is set to input, so nothing we do to OC2B will actually show up on the output pin
    // Note that we might need pull down resistor attached to the pin to keep it low when the pin is set to input mode
    
    // Next match will set the OC2B bit
    
    TCCR2A = _BV( COM2B1 ) | _BV( COM2B0) |         // Set OC2B on Compare Match, normal Waveform  generation
    
   
    TCNT2 = 0;      // Start counting at zero
    OCR2B = 0;      // Make a match
    
    TCCR2B = _BV( FOC2B );                // FOC2B: Force Output Compare B. This will set the OC2B output bit to             
                                          // Note that this would output the 1 on the pin becuase of the COM2 bits, except the pin is currently in input mode 

    TCCR2A = 0;                            // Normal port operation, OC0A disconnected.
    
    pinMode( 2 , OUTPUT );                // Pin will now be driven to 0. Ok to switch to output mode becuase COM2B bits are now zero so the OC2B bit is disconnected from the pin. 
    
    OCR2B = 10;                           // New match
    
    TCCR
    
    
    
    if (x) {      // We are sending a 1 bit
      
      
      
    } else       // We are sending a 0 bit
    
    }
    
    
  

// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.

void  sendBit(bool) __attribute__ ((optimize(0)));

void sendBit( bool bitVal ) {
  
    if (  bitVal ) {
      
      bitSet( PIXEL_PORT , PIXEL_BIT );
          
      DELAY_CYCLES( NS_TO_CYCLES( T1H ) - 2 );       // 1-bit width less  overhead  for the actual bit setting
                                                     // Note that this delay could be longer and everything would still work
      bitClear( PIXEL_PORT , PIXEL_BIT );
      
      DELAY_CYCLES( NS_TO_CYCLES( T1L ) - 10 );       // 1-bit gap less the overhead of the loop
                                  
    } else {

      bitSet( PIXEL_PORT , PIXEL_BIT );
      
      DELAY_CYCLES( NS_TO_CYCLES( T0H ) - 2 );      // 0-bit width less overhead 
                                                    // **************************************************************************
                                                    // This line is really the only tight goldilocks timing in the whole program!
                                                    // **************************************************************************
      bitClear( PIXEL_PORT , PIXEL_BIT );
      
      DELAY_CYCLES( NS_TO_CYCLES( T0L ) - 10  );      // 0-bit gap less overhead of the loop
      
    }
    
    // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
    // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
    // This has thenice side effect of avoid glitches on very long strings becuase 

    
}  

  
void sendByte( unsigned char byte ) {
    
    for( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
      
      sendBit( bitRead( byte , 7 ) );                // Neopixel wants bit in highest-to-lowest order
                                                     // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
      byte <<= 1;                                    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
      
    }           
} 

/*

  The following three functions are the public API:
  
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.  
  sendPixel( r g , b ) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame. 
  
*/


// Set the specified pin up as digital out

void ledsetup() {
  
  bitSet( PIXEL_DDR , PIXEL_BIT );
  
}

void sendPixel( unsigned char r, unsigned char g , unsigned char b )  {  
  
  sendByte(g);          // Neopixel wants colors in green then red then blue order
  sendByte(r);
  sendByte(b);
  
}


// Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame

void show() {
    DELAY_CYCLES( NS_TO_CYCLES(RES) );               
}


/*

  That is the whole API. What follows are some demo functions rewriten from the AdaFruit strandtest code...
  
  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
  
  Note that we always turn off interrupts while we are sending pixels becuase an interupt
  could happen just when we were in the middle of somehting time sensitive.
  
  If we wanted to minimize the time interrupts were off, we could instead 
  could get away with only turning off interrupts just for the very brief moment 
  when we are actually sending a 0 bit (~1us), as long as we were sure that the total time 
  taken by any interrupts + the time in our pixel generation code never exceeded the reset time (5us).
  
*/


// Display a single color on the whole string

void showColor( unsigned char r , unsigned char g , unsigned char b ) {
  
  cli();  
  for( int p=0; p<PIXELS; p++ ) {
    sendPixel( r , g , b );
  }
  sei();
  show();
  
}

// Fill the dots one after the other with a color
// rewrite to lift the compare out of the loop
void colorWipe(unsigned char r , unsigned char g, unsigned char b, unsigned  char wait ) {
  for(unsigned int i=0; i<PIXELS; i+= (PIXELS/60) ) {
    
    cli();
    unsigned int p=0;
    
    while (p++<=i) {
        sendPixel(r,g,b);
    } 
     
    while (p++<=PIXELS) {
        sendPixel(0,0,0);  
      
    }
    
    sei();
    show();
    delay(wait);
  }
}

// Theatre-style crawling lights.
// Changes spacing to be dynmaic based on string size

#define THEATER_SPACING (PIXELS/20)

void theaterChase( unsigned char r , unsigned char g, unsigned char b, unsigned char wait ) {
  
  for (int j=0; j< 3 ; j++) {  
  
    for (int q=0; q < THEATER_SPACING ; q++) {
      
      unsigned int step=0;
      
      cli();
      
      for (int i=0; i < PIXELS ; i++) {
        
        if (step==q) {
          
          sendPixel( r , g , b );
          
        } else {
          
          sendPixel( 0 , 0 , 0 );
          
        }
        
        step++;
        
        if (step==THEATER_SPACING) step =0;
        
      }
      
      sei();
      
      show();
      delay(wait);
      
    }
    
  }
  
}
        


// I rewrite this one from scrtach to use high resolution for the color wheel to look nicer on a *much* bigger string
                                                                            
void rainbowCycle(unsigned char frames , unsigned int frameAdvance, unsigned int pixelAdvance ) {
  
  // Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
  // hue of 0 = Full red
  // hue of 128 = 1/2 red and 1/2 green
  // hue of 256 = Full Green
  // hue of 384 = 1/2 green and 1/2 blue
  // ...
  
  unsigned int firstPixelHue = 0;     // Color for the first pixel in the string
  
  for(unsigned int j=0; j<frames; j++) {                                  
    
    unsigned int currentPixelHue = firstPixelHue;
       
    cli();    
        
    for(unsigned int i=0; i< PIXELS; i++) {
      
      if (currentPixelHue>=(3*256)) {                  // Normalize back down incase we incremented and overflowed
        currentPixelHue -= (3*256);
      }
            
      unsigned char phase = currentPixelHue >> 8;
      unsigned char step = currentPixelHue & 0xff;
                 
      switch (phase) {
        
        case 0: 
          sendPixel( ~step , step ,  0 );
          break;
          
        case 1: 
          sendPixel( 0 , ~step , step );
          break;

        case 2: 
          sendPixel(  step ,0 , ~step );
          break;
          
      }
      
      currentPixelHue+=pixelAdvance;                                      
      
                          
    } 
    
    sei();
    
    show();
    
    firstPixelHue += frameAdvance;
           
  }
}

  
// I added this one just to demonstrate how quickly you can flash the string.
// Flashes get faster and faster until *boom* and fade to black.

void detonate( unsigned char r , unsigned char g , unsigned char b , unsigned int startdelayms) {
  while (startdelayms) {
    
    showColor( r , g , b );      // Flash the color 
    showColor( 0 , 0 , 0 );
    
    delay( startdelayms );      
    
    startdelayms =  ( startdelayms * 4 ) / 5 ;           // delay between flashes is halved each time until zero
    
  }
  
  // Then we fade to black....
  
  for( int fade=256; fade>0; fade-- ) {
    
    showColor( (r * fade) / 256 ,(g*fade) /256 , (b*fade)/256 );
        
  }
  
  showColor( 0 , 0 , 0 );
  
    
}

void setup() {
    
  ledsetup();
  
}


void loop() {
  
  // Some example procedures showing how to display to the pixels:
  colorWipe(255, 0, 0, 0); // Red
  colorWipe(0, 255, 0, 0); // Green
  colorWipe(0, 0, 255, 0); // Blue
  
  // Send a theater pixel chase in...
  theaterChase(127, 127, 127, 0); // White
  theaterChase(127,   0,   0, 0); // Red
  theaterChase(  0,   0, 127, 0); // Blue
  
  rainbowCycle(1000 , 20 , 5 );
  detonate( 255 , 255 , 255 , 1000);
  
  return;
  
}





