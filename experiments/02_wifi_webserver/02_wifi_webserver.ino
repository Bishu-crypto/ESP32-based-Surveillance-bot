/*
 * Experiment 01 — Blink onboard LED
 * Board  : AI Thinker ESP32-CAM
 * LED    : GPIO 33 (red, active LOW) · GPIO 4 (flash, active HIGH)
 * Status : DONE
 */

#define LED_RED    33
#define FLASH_LED   4

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED,   OUTPUT);
  pinMode(FLASH_LED, OUTPUT);
  digitalWrite(FLASH_LED, LOW);
}

void loop() {
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  Serial.println("blink");
}