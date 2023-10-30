import socket

# Configuration from your .ini file
port = 9000  # Replace with the port from your .ini file
p1 = "fup"  # Replace with the prefix from your .ini file

p2 = "fdown"
# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server (replace with the server address if needed)
server_address = ('localhost', port)
client_socket.connect(server_address)

tmp = 1200
# Message to send with the prefix
message = f"fdown=a test"  

try:
    # Send the message to the server
    client_socket.sendall(message.encode())

    # Receive the response from the server
    data = client_socket.recv(1024)

    print(f"Received: {data.decode()}")

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    # Close the client socket
    client_socket.close()
