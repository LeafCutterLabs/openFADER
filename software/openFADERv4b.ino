/*
Direct connect, no shift register
TODO

*/

#include "MIDIUSB.h"
#include <FastLED.h>

#define N_FADERS 8

const uint8_t MIDICC = 0x0B;

const byte faderPIN[] = {A10, A0, A3, A2, A1, A6, A7, A8}; //fader pins

/*
gaderPIN arrangement for the slim/no-led version, you can also comment out all the led stuff if you are using the slim version
const byte faderPIN[] = {A6, A7, A3, A2, A1, A0, A8, A9}; //fader pins
*/

#define MIDI_CHANNEL 0
const uint8_t faderCN[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x011};  // MIDI control values

int faderVALUE[N_FADERS];
int faderVALUEnext;

unsigned long timePRINT=0;

//LED
#define LED_PIN     5
#define NUM_LEDS    8
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

//Debug baud rate
#define SERIAL_RATE 9600

void setup(){
	Serial.begin(SERIAL_RATE);
	delay(3000); // power-up safety delay
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
	FastLED.setBrightness(BRIGHTNESS);
}

void loop(){
	for(int i=0; i<N_FADERS; i++) {
		faderVALUEnext = (analogRead(faderPIN[i])/8 + faderVALUE[i])/2;
		if(faderVALUE[i] != faderVALUEnext){
			controlChange(0, faderCN[i], faderVALUEnext);
			faderVALUE[i] = faderVALUEnext;
		}
	}
	MidiUSB.flush();
	colorDISPLAY();

	//displays raw state every 1 second
	if ((millis() - timePRINT) > 1000){
		timePRINT = millis();
		displayRAW();
	}
}
  
//shows the current state of leds/color
void colorDISPLAY(){
   for (int i=0; i<NUM_LEDS; i++){
      leds[i] = CHSV(map(faderVALUE[i], 0, 127, 0, 213), 255, 255); //maps to max value of 171 blue 213 purple
   }
   FastLED.show();
}


//pushes midi event
void controlChange(byte channel, byte control, byte value){
	midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
	MidiUSB.sendMIDI(event);
}

void displayRAW(){
	for (byte i = 0; i < N_FADERS; i++){
		Serial.print(analogRead(faderPIN[i]));
		Serial.print(' ');
    }
    Serial.println();
}
