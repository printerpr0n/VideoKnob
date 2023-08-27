#include <Arduino.h>
#include <USBHIDKeyboard.h>
#include <Encoder.h>
#include <FastLED.h>

#define ENCODER_PIN_A 2   // Pin A of the rotary encoder
#define ENCODER_PIN_B 3   // Pin B of the rotary encoder
#define ENCODER_BUTTON 4  // Button pin of the rotary encoder

#define LED_DATA_PIN 12    // Data pin of the addressable LEDs
#define NUM_LEDS 31       // Number of LEDs in the strip

CRGB leds[NUM_LEDS];
CRGB previousColor = CRGB::Red;  // Initial color

Encoder myEncoder(ENCODER_PIN_A, ENCODER_PIN_B);
int lastEncoderValue = 0;
boolean buttonState = false;
boolean lastButtonState = false;
boolean encoderActive = false;

USBHIDKeyboard Keyboard;

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  Keyboard.begin();
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  fill_solid(leds, NUM_LEDS, previousColor); // Initialize LEDs to previous color
  FastLED.show();
}

void loop() {
  int encoderValue = myEncoder.read();
  buttonState = digitalRead(ENCODER_BUTTON);

  static unsigned long lastButtonPressTime = 0;
  static bool encoderTurned = false;

  if (encoderValue != lastEncoderValue) {
    encoderTurned = true;
  }

  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPressTime) > 400) {
    if (encoderTurned) {
      Keyboard.write((char)0x20);
      delay(05);
      Keyboard.releaseAll();
      changeLEDColor(CRGB::Green);
      encoderTurned = false; // Reset encoder flag
    }
    lastButtonPressTime = millis();
  }

  if (encoderValue > lastEncoderValue) {
    Keyboard.press(KEY_RIGHT_ARROW);
    Keyboard.release(KEY_RIGHT_ARROW);
    chaseLEDs(CRGB::Yellow, true);
  } else if (encoderValue < lastEncoderValue) {
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
    chaseLEDs(CRGB::Blue, false);
  } else {
    encoderActive = false;
    changeLEDColor(previousColor);
  }

  lastEncoderValue = encoderValue;
  lastButtonState = buttonState;

  // Rest of the loop...
}





void changeLEDColor(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
  previousColor = color;
  FastLED.show();
  delay(100);
}

void chaseLEDs(CRGB color, bool direction) {
  if (direction) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color;
      FastLED.show();
      delay(50);
      leds[i] = CRGB::Black;
    }
  } else {
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      leds[i] = color;
      FastLED.show();
      delay(50);
      leds[i] = CRGB::Black;
    }
  }
  if (!encoderActive) {
    changeLEDColor(previousColor); // Revert to previous color when not active
  }
}
