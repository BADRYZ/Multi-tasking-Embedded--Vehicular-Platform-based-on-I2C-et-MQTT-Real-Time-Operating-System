#include <Arduino_FreeRTOS.h>
#include "Wire.h"

#define I2C_SLAVE_ADDRESS 13
#define len 4 // Adjusted for the LED pins

int ledPins[] = {3, 4, 5, 6}; // LED pins
int buttonPins[] = {12, 13};   // Button pins

int BUTTON;
byte RightButton;
byte LeftButton;

void setup() {
Serial.begin(9600);
pinMode(ledPins[0], OUTPUT); // Setting the first LED pin as the red LED
pinMode(buttonPins[0], INPUT_PULLUP);
pinMode(buttonPins[1], INPUT_PULLUP);

for (int i = 1; i < len; i++) {
pinMode(ledPins[i], OUTPUT);
digitalWrite(ledPins[i], LOW);
}

digitalWrite(buttonPins[0], HIGH);
digitalWrite(buttonPins[1], HIGH);

Wire.begin(I2C_SLAVE_ADDRESS);
Wire.onRequest(i2cCommuniDataTask);

xTaskCreate(CollectDataTask, "Button", 128, NULL, 1, NULL);
xTaskCreate(ComputeDataTask, "Button Pressed", 128, NULL, 2, NULL);
xTaskCreate(NotifyDataTask, "Signal", 128, NULL, 2, NULL);
vTaskStartScheduler();
}

void CollectDataTask(void *pvParameters) {
while (1) {
RightButton = digitalRead(buttonPins[0]);
LeftButton = digitalRead(buttonPins[1]);
vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}

void ComputeDataTask(void *pvParameters) {
while (1) {
if (digitalRead(buttonPins[0]) == LOW) {
Serial.println("Button RIGHT Pressed");
BUTTON = 1;
}
if (digitalRead(buttonPins[1]) == LOW) {
Serial.println("Button LEFT Pressed");
BUTTON = 2;
}
vTaskDelay(10 / portTICK_PERIOD_MS);
}
}

void NotifyDataTask(void *pvParameters) {
while (1) {
if (digitalRead(buttonPins[0]) == LOW) {
for (int i = len - 1; i >= 0; i--) {
digitalWrite(ledPins[i], HIGH);
vTaskDelay(70 / portTICK_PERIOD_MS);
}
for (int i = len - 1; i >= 0; i--) {
digitalWrite(ledPins[i], LOW);
vTaskDelay(100 / portTICK_PERIOD_MS);
}
}
if (digitalRead(buttonPins[1]) == LOW) {
for (int i = 0; i < len; i++) {
digitalWrite(ledPins[i], HIGH);
vTaskDelay(70 / portTICK_PERIOD_MS);
}
for (int i = 0; i < len; i++) {
digitalWrite(ledPins[i], LOW);
vTaskDelay(100 / portTICK_PERIOD_MS);
}
}
}
}

void i2cCommuniDataTask(void *pvParameters) {
int a = BUTTON;
BUTTON = 0;
Wire.write(a);
}

void loop() {}