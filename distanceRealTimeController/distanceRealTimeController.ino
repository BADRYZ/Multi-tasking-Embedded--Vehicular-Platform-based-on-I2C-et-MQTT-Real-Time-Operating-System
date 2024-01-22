#include <Wire.h>
#include <Arduino_FreeRTOS.h>

// Pin Definitions
#define ULTRASONIC_TRIG_PIN 11
#define ULTRASONIC_ECHO_PIN 12
#define RGB_RED_PIN 6
#define RGB_GREEN_PIN 5
#define RGB_BLUE_PIN 4
#define I2C_SLAVE_ADDRESS 9

// Task handles
TaskHandle_t collectDataTaskHandle;
TaskHandle_t computeDataTaskHandle;
TaskHandle_t notifyDataTaskHandle;
TaskHandle_t i2cCommuniDataTaskHandle;

// Variables
int distance = 0;
int color = 0;

// Task prototypes
void collectDataTask(void* pvParameters);
void computeDataTask(void* pvParameters);
void notifyDataTask(void* pvParameters);
void i2cCommuniDataTask(void* pvParameters);
void requestEvent();

void setup() {
  Wire.onRequest(requestEvent); // register request event
  Serial.begin(9600); // Initialize serial monitor
  pinMode(2,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(11,OUTPUT);
  digitalWrite(2,1);
  digitalWrite(5,0);
  digitalWrite(11,0);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);

  xTaskCreate(collectDataTask, "CollectData", 128, NULL, 1, &collectDataTaskHandle);
  xTaskCreate(computeDataTask, "ComputeData", 128, NULL, 2, &computeDataTaskHandle);
  xTaskCreate(notifyDataTask, "NotifyData", 128, NULL, 3, &notifyDataTaskHandle);
  xTaskCreate(i2cCommuniDataTask, "I2CCommunication", 128, NULL, 4, &i2cCommuniDataTaskHandle);

  vTaskStartScheduler();
}

void loop() {
  // Empty loop, tasks will be scheduled by FreeRTOS
}

void collectDataTask(void* pvParameters) {
  while (1) {
    // Read data from the ultrasonic sensor
    long duration;
    
    // Trigger ultrasonic sensor to start measurement
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

    // Measure the duration of the pulse from the ultrasonic sensor
    duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);

    // Calculate distance based on the duration of the pulse
    distance = duration * 0.034 / 2;

    // Print the distance value to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    vTaskDelay(pdMS_TO_TICKS(20)); // Adjust the delay based on the required data collection rate
  }
}

void computeDataTask(void* pvParameters) {
  while (1) {
    // Compute color based on distance value
    if (distance <= 10) {
      color = 1; // Red
    } else if (distance <= 20) {
      color = 2; // Green
    } else {
      color = 3; // Blue
    }

    vTaskDelay(pdMS_TO_TICKS(20)); // Adjust the delay based on the required computation rate
  }
}

void notifyDataTask(void* pvParameters) {
  while (1) {
    // Light up RGB LED based on the assigned color
    switch (color) {
      case 1: // Red
        digitalWrite(RGB_RED_PIN, HIGH);
        digitalWrite(RGB_GREEN_PIN, LOW);
        digitalWrite(RGB_BLUE_PIN, LOW);
        break;
      case 2: // Green
        digitalWrite(RGB_RED_PIN, LOW);
        digitalWrite(RGB_GREEN_PIN, HIGH);
        digitalWrite(RGB_BLUE_PIN, LOW);
        break;
      case 3: // Blue
        digitalWrite(RGB_RED_PIN, LOW);
        digitalWrite(RGB_GREEN_PIN, LOW);
        digitalWrite(RGB_BLUE_PIN, HIGH);
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(20)); // Adjust the delay based on the required notification rate
  }
}

void i2cCommuniDataTask(void *pvParameters) {
  while (1) {
    // Empty task for I2C communication
    vTaskDelay(100); // Adjust the delay as needed
  }
}

void requestEvent() {
  Wire.write(distance); // send data to master device
}
