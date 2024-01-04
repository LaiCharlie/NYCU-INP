import socket
import select

# Define the server address and port
server_address = ('140.113.213.213', 10495)
# server_address = ('127.0.0.1', 10495)

# Define the message to send for the hello command
hello_id = "inplab1"

# Define the challenge ID obtained from the hello response
challenge_id = "temp_str"

# Function to send a message and wait for a response with a timeout
def send_and_receive_with_timeout(socket, message, timeout=5):
    socket.sendall(message.encode())
    ready = select.select([socket], [], [], timeout)
    if ready[0]:
        response = socket.recv(1024).decode()
        return response
    else:
        return None

# Send the hello command and obtain the challenge ID with a timeout
with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
    s.connect(server_address)

    hello_message = f"hello {hello_id}"
    response = send_and_receive_with_timeout(s, hello_message)
    
    if response:
        challenge_id = response.split()[1]
        print("< hello success >")
        # print("< server response >", response)
    else:
        print("< No response received within the timeout. >")

# Send the chals command and receive the response with a timeout
with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
    s.connect(server_address)

    chals_message = f"chals {challenge_id}"
    response = send_and_receive_with_timeout(s, chals_message)
    
    if response:
        print("< chals success >")
        # print("< server response >", response)
    else:
        print("< No response received within the timeout. >")
