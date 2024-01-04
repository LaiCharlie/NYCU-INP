import socket
import select

# Define the server address and port
server_address = ('140.113.213.213', 10495)
# server_address = ('127.0.0.1', 10495)

# Function to send a message and wait for a response with a timeout
def send_and_receive_with_timeout(socket, message, timeout=5):
    socket.sendall(message.encode())
    ready = select.select([socket], [], [], timeout)
    if ready[0]:
        response = socket.recv(1024).decode()
        return response
    else:
        return None

path = 'flag.txt'
f = open(path, 'r')
flag = f.read()
f.close()

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
    s.connect(server_address)

    verfy_message = flag
    response = send_and_receive_with_timeout(s, verfy_message)
    
    if response:
        print(response)
    else:
        print("No response received within the timeout.")
