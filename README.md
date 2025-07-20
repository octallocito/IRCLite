# IRCLite

**IRCLite** is a lightweight IRC server written in Python & C.  
It focuses on simplicity, readability, and basic compliance with the IRC protocol (RFC 1459). Perfect for learning, experimenting, or building your own custom server.

## Features

- Basic support for the IRC protocol (RFC 1459)
- Multi-client handling via sockets
- Channel support and private messaging
- Written in pure Python, C — no external libraries required
- Easy to modify and extend

## Getting Started

### Requirements

- Python 3.8 or later
- C lang 

### Run

```bash
python3 irclite.py <port>
gcc -o serv serv.c -lpthread
gcc -o cli cli.c -lpthread
./serv <port>
./cli 127.0.0.1 <port>
```
