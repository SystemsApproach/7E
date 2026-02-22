# Socket API calls used by this server:
#   socket.socket(family, type) -> socket
#   socket.setsockopt(level, optname, value)
#   socket.bind((host, port))
#   socket.listen(backlog)
#   socket.accept() -> (conn, addr)
#   socket.recv(bufsize) -> data
#   socket.close()

import socket

BUFSIZE = 4096
SERVER_PORT = 5555
MAX_PENDING = 5

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", SERVER_PORT))
    sock.listen(MAX_PENDING)

    print(f"Server listening on port {SERVER_PORT}")

    while True:
        conn, addr = sock.accept()
        print(f"Connection from {addr}")

        while True:
            data = conn.recv(BUFSIZE)
            if not data:
                break
            print(data.decode())

        conn.close()

if __name__ == "__main__":
    main()
