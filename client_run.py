import argparse
import socket
import sys
from client.recieve import Recieve

def main():
    parser = argparse.ArgumentParser(description='input ip and port')
    parser.add_argument('-i', '--ip', metavar='ip', type=str, default="127.0.0.1", help='ip adress for connection')
    parser.add_argument('-p', '--port', metavar='port', type=int, default=1100, help='port adress for connection')
    args = parser.parse_args()

    recieve = Recieve(args.ip, args.port)

if __name__ == "__main__":
    main()
