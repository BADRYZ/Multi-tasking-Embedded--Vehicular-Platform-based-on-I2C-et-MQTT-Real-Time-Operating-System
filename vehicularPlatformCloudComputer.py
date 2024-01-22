import socket

# Configuration du client
HOST = '192.168.250.70'  # Adresse IP de la Raspberry Pi
PORT = 12345

def send_request():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        client_socket.sendall(b'Request for data')
        print("Requête envoyée à la Raspberry Pi")

if __name__ == "__main__":
    send_request()
