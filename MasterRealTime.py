# -*- coding: utf-8 -*-
import time
import random
import smbus
import I2C_LCD_driver as driver
import paho.mqtt.client as mqtt

# Define constants
SLAVE_FLASH = 0x13
SLAVE_SPEED = 0x9
SLAVE_DHT = 0x30

# Initialize global variables
LCD = driver.lcd()
BUS = smbus.SMBus(1)
DELAY = 3

BROKER = "192.168.5.70"
PORT = 1883
TEMP_TOPIC = "Temperature"
FLASH_TOPIC = "Flash"
DIRECT_TOPIC = "direction"
SPEED_TOPIC = "speed"

client_id = '{random.randint(0, 1000)}'
username = 'emqx'
password = 'public'

def initApp():
    global LCD  # DÃ©clarez LCD comme une variable globale
    LCD = driver.lcd()
    LCD.lcd_clear()
    print("LCD initialized successfully.")


def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
            client.subscribe(TEMP_TOPIC)
            print("\nSubscribed to topic:", TEMP_TOPIC)
        else:
            print(f"Failed to connect, return code {rc}\n")

    client = mqtt.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect

    try:
        print(f"Attempting to connect to {BROKER}:{PORT}")
        client.connect(BROKER, PORT)
    except Exception as e:
        print(f"Connection error: {e}")

    return client


def mqttCommunicateDataTask(client, Temp_value, direct_value, Speed_value):
    client.publish(TEMP_TOPIC, str(Temp_value))
    client.publish(SPEED_TOPIC, str(Speed_value))
    client.publish(DIRECT_TOPIC, str(direct_value))

def loopApp():
    while True:
        data = colletDataTask()
        displayDataTask(data)
        direct_value = display_STATE()
        Speed_value = display_Speed()  # Corrected function call

        mqttCommunicateDataTask(client, data, direct_value, Speed_value)
        timeSleepTask(DELAY)


def destroyApp():
        LCD.lcd_clear()

def display_STATE():
    try:
        block = BUS.read_i2c_block_data(SLAVE_FLASH, 0, 1)
        n = "".join(map(chr, block))
        n = n.split(";")
        LCD.lcd_display_string("direction: ", 1, 1)
        LCD.lcd_display_string(n[0], 1, 14)
        print("direction ", n[0])
        time.sleep(1)
        direct_value = n[0]
        return direct_value
    except OSError as e:
        print(f"I2C Error: {e}")
        return None  # Vous pouvez adapter cela en fonction de la logique de votre application


def displayDataTask(data):
    print("DISPLAY DATA TASK: LOOK AT THE I2C DISPLAY")

    temperature = str(data[0]) if len(data) >= 1 else "N/A"
    humidity = str(data[1]) if len(data) >= 2 else "N/A"

    LCD.lcd_display_string("Temperature: ", 1, 1)
    LCD.lcd_display_string(temperature, 1, 14)
    LCD.lcd_display_string("Humidity: ", 2, 1)
    LCD.lcd_display_string(humidity, 2, 14)

    time.sleep(1)


def display_Speed():
    block3 = BUS.read_i2c_block_data(SLAVE_SPEED, 0, 14)
    n = "".join(map(chr, block3))
    n = n.split(";")
    LCD.lcd_display_string("Speed: ", 4, 1)
    LCD.lcd_display_string("", 4, 10)
    LCD.lcd_display_string(str(block3[:1]), 4, 10)
    speed_value = str(block3)
    time.sleep(1)
    print("Speed:", block3[:1])
    return speed_value


def colletDataTask():
    print("\nI2C COLLECT DATA TASK:")
    try:
        data = BUS.read_i2c_block_data(SLAVE_DHT, 0, 14)
        print("Data collected successfully:", data)

        # Assuming data is formatted like "Temperature;Humidity"
        data_str = "".join(map(chr, data))
        values = data_str.split(";")

        if len(values) >= 2:
            temperature, humidity = values[0], values[1]
            print("Temperature:", temperature, "C  &   Humidity:", humidity)
            return temperature
        else:
            print("Invalid data format:", data_str)
            return (0.0, 0.0)  # Return default values or handle the error as needed
    except Exception as e:
        print("Error collecting data:", e)
        return (0.0, 0.0)  # Return default values or handle the error as needed




def timeSleepTask(DELAY):
    print("TIME DELAY TASK ......")
    time.sleep(DELAY)



if __name__ == '__main__':
        initApp()
        client = connect_mqtt()
        client.loop_start()
        try:
                loopApp()
        except KeyboardInterrupt:
            destroyApp()
