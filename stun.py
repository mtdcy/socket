import socket


local_port = 8082


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM);
sock.bind(socket.INADDR_ANY, local_port);



