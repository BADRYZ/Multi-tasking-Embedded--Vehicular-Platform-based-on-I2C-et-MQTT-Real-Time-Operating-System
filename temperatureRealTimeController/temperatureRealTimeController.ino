#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPin 2

#define I2C_SLAVE_ADDRESS 0x30

DHT dht(DHTPin, DHTTYPE);
int tmax = 25;
int tmin = 19;
float t;
float tm;
float temp;
float hum;
int ti;

int BLED = 3;
int GLED = 4;
int RLED = 5;

void setup()
{
  dht.begin();
  Serial.begin(9600);
  pinMode(BLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(RLED, OUTPUT);
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(communiDataTask);

  xTaskCreate(collectDataTask, "Temperature", 400, NULL, 1, NULL);
  xTaskCreate(computeDataTak, "ComputeData", 128, NULL, 1, NULL);
  xTaskCreate(notifyDataTask, "Notify", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void collectDataTask(void *pvParameters)
{
  (void)pvParameters;
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 500;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    if (isnan(temp) || isnan(hum) )
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void computeDataTak(void *pvParameters)
{
  while (1)
  {
    t = temp;
    Serial.print("Temperature :");
    Serial.print(t);
    Serial.println("^C");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void notifyDataTask(void *pvParameters)
{
  while (1)
  {
    tm = temp;

    if (tm <= tmin)
    {
      digitalWrite(BLED, HIGH);
      digitalWrite(GLED, LOW);
      digitalWrite(RLED, LOW);
      vTaskDelay(1);
    }

    else if (tm > tmin && tm < tmax)
    {
      digitalWrite(BLED, LOW);
      digitalWrite(GLED, HIGH);
      digitalWrite(RLED, LOW);
      vTaskDelay(1);
    }

    else
    {
      digitalWrite(BLED, LOW);
      digitalWrite(GLED, LOW);
      digitalWrite(RLED, HIGH);
      vTaskDelay(1);
    }
  }
}

void communiDataTask()
{
  char DHT_values[14];
  String distanceStr = String(temp, 2);
  String speedStr = String(hum, 2);
  snprintf(DHT_values, 14, "%s;%s;", distanceStr.c_str(), speedStr.c_str());
  Wire.write(DHT_values);
}

void loop() {}
