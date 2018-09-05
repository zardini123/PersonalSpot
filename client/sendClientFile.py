import socket

s = socket.socket()

s.connect(("localhost", 12345))

CHUNK_SIZE = 8 * 1024

with open('haloTheme.mp3', 'rb') as f:
    data = f.read(CHUNK_SIZE)
    while data:
        s.send(data)
        data = f.read(CHUNK_SIZE)

s.close()
