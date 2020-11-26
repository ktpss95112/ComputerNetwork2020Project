#/usr/bin/env python3

import sys
import traceback

from handler import Request, die, route


def parse_input() -> Request:
    len_ = int(sys.stdin.buffer.readline())
    data = sys.stdin.buffer.read(len_)

    return Request.from_raw(data)


def main():
    try:
        req = parse_input()
        route(req)

    except Exception as e:
        traceback.print_exc()

    die()


if __name__ == "__main__":
    main()
