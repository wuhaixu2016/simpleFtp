import socket

size = 8192

try:
    for i in range(50):
        msg = str(i)
        msg = msg.encode(encoding = 'utf-8')
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(msg, ('127.0.0.1', 9876))
        print(sock.recv(size).decode('utf-8'))
        sock.close()
except:
    print("cannot reach the server")