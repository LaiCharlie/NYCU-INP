import socket
import select

def send_and_receive_with_timeout(socket, message, timeout=5):
    socket.sendall(message.encode())
    ready = select.select([socket], [], [], timeout)
    if ready[0]:
        response = socket.recv(1024).decode()
        return response
    else:
        return None

server_address = ('140.113.213.213', 10301)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)     # TCP
# s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)      # UDP
s.connect(server_address)

# ---------- Method 1 ------------
hello_id = "inplab1"
hello_message = f"hello {hello_id}"
response = send_and_receive_with_timeout(s, hello_message)
if response:
    print("< server response >", response)
else:
    print("< No response received within the timeout. >")

# ---------- Method 2 ------------
command = "example"
msg = f"verfy {command}".encode('utf-8')
s.sendto(msg, server_address)
data, addr = s.recvfrom(1024)
result = data.decode()
print(result)

# ------- close socket -------
s.close()