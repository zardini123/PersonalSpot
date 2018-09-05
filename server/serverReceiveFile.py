import socket

s = socket.socket()

s.bind(("localhost", 12345))
s.listen(5)

CHUNK_SIZE = 8 * 1024

while True:
    clientSocket, addr = s.accept()

    newFile = open('receivedFile.mp3', 'wb')

    chunk = clientSocket.recv(CHUNK_SIZE)
    while chunk:
        newFile.write(chunk)
        chunk = clientSocket.recv(CHUNK_SIZE)

    print("Recieved")

    clientSocket.close()
