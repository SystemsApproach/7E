# Socket API calls used by this client:
#   socket.socket(family, type) -> socket
#   socket.connect((host, port))
#   socket.send(data) -> bytes_sent
#   socket.recv(bufsize) -> data
#   socket.close()

import sys
import socket

BUFSIZE = 4096
SERVER_PORT = 5555

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <server-name>", file=sys.stderr)
        sys.exit(1)

    server_name = sys.argv[1]

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((server_name, SERVER_PORT))

    for msg in sys.stdin:
        sock.send(msg.encode())

    sock.close()

if __name__ == "__main__":
    main()
