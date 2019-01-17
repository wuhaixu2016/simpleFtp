import socket

size = 8192

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 9876))

try:
    count = 0
    while True:
        data, address = sock.recvfrom(size)
        count += 1
        data = str(count) + ' ' + data.decode('utf-8')
        sock.sendto(data.encode(encoding='utf-8'), address)
finally:
    sock.close()