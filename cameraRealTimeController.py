import socket
import cv2
import time
import datetime

HOST = '127.0.0.1'
PORT = 1900
FILENAME = "received_photo.jpg"

def capture_and_send_photo():
    camera = cv2.VideoCapture(0)

    if not camera.isOpened():
        print("Error: Unable to open the camera.")
        return

    ret, frame = camera.read()
    if ret:
        filename = FILENAME
        cv2.imwrite(filename, frame)
        print(f"Photo captured successfully and saved as {filename}")
    else:
        print("Error: Unable to capture the photo")


    connected = False
    while not connected:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
                client_socket.connect((HOST, PORT))
                connected = True
                with open(filename, "rb") as file:
                    while True:
                        data = file.read(512)
                        if not data:
                            break
                        client_socket.send(data)
                print(f"Photo {filename} sent successfully to the same machine.")
        except ConnectionRefusedError:
            print("La connexion a été refusée, réessaie dans quelques secondes...")
            time.sleep(5)

if __name__ == '__main__':
    while True:
        capture_and_send_photo()