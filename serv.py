import socket
import threading

# List to keep track of connected clients
clients = []

def handle_client(client_socket, addr):
    print(f"[NEW] Connection from {addr}")
    clients.append(client_socket)

    while True:
        try:
            message = client_socket.recv(1024).decode('utf-8')
            if message:
                print(f"[{addr}] {message}")
                broadcast(message, client_socket)
            else:
                break
        except:
            break

    print(f"[DISCONNECT] {addr}")
    clients.remove(client_socket)
    client_socket.close()

def broadcast(message, client_socket):
    for client in clients:
        if client != client_socket:
            try:
                client.send(message.encode('utf-8'))
            except:
                client.close()
                clients.remove(client)

def start_server(port):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('0.0.0.0', port))
    server.listen(5)
    print(f"[LISTENING] Server is listening on port {port}")

    while True:
        client_socket, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(client_socket, addr))
        thread.start()

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python3 main.py <port>")
        sys.exit(1)

    port = int(sys.argv[1])
    start_server(port)

