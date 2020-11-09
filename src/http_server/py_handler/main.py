#/usr/bin/env python3

import sys

from handler import Request


def parse_input() -> Request:
    len_ = int(sys.stdin.buffer.readline())
    data = sys.stdin.buffer.read(len_)

    return Request()


def main():

    req = parse_input()

    data = b'HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 5\r\n\r\nHello'
    sys.stdout.write(f'{len(data)}\n')
    sys.stdout.flush()
    sys.stdout.buffer.write(data)
    sys.stdout.flush()


if __name__ == "__main__":
    main()
