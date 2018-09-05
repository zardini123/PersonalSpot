import socket

s = socket.socket()

s.bind(("localhost", 12345))
s.listen(5)

CHUNK_SIZE = 8 * 1024

while True:
    clientSocket, addr = s.accept()

    chunk = clientSocket.recv(CHUNK_SIZE)
    while chunk:
        chunk = clientSocket.recv(CHUNK_SIZE)
        print(chunk)

    clientSocket.close()
