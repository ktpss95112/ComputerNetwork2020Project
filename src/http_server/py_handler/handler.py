import sys
import json
from dataclasses import dataclass

from chat import Chat


def send_and_exit(data):
    if isinstance(data, str):
        data = data.encode()
    sys.stdout.write(f'{len(data)}\n')
    sys.stdout.flush()
    sys.stdout.buffer.write(data)
    sys.stdout.flush()
    exit(0)


def die():
    send_and_exit('HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nInternal Server Error')


def die_ok():
    send_and_exit('HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 2\r\n\r\nOK')


def die_send_json(data):
    if isinstance(data, str):
        data = data.encode()
    send_and_exit(b'HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: ' + str(len(data)).encode() + b'\r\n\r\n' + data)


@dataclass
class Request:
    method: bytes
    path: bytes
    http_version: bytes
    headers: dict
    body: bytes


    @classmethod
    def from_raw(cls, data):
        if isinstance(data, str):
            data = data.encode()

        method, path, version = data.split(b'\r\n')[0].split()
        if method == b'GET':
            head, body = data.strip(), b''
        elif method == b'POST':
            head, _, body = data.partition(b'\r\n\r\n')

        lines = head.split(b'\r\n')

        headers = {
            key.strip() : value.strip()
            for line in lines[1:]
            for key, _, value in [line.partition(b':')]
        }

        return cls(method, path, version, headers, body)


def route(req: Request):
    if req.path.startswith(b'/api/chat'):
        if req.method == b'GET':
            if req.path == b'/api/chat/size':
                get_chat_size()

            else:
                if req.path == b'/api/chat': _id = 0
                else                       : _id = int(req.path[10:])
                get_chat(_id)

        elif req.method == b'POST' and req.path == b'/api/chat':
            post_chat(req.body)


def get_chat_size():
    chat = Chat()
    data = chat.get_size()

    data = json.dumps(data)
    die_send_json(data)


def get_chat(_id):
    chat = Chat()

    if _id == 0: data = chat.get_all()
    else       : data = chat.get_one_by_id(_id)

    data = json.dumps(data)
    die_send_json(data)


def post_chat(data):
    data = json.loads(data)
    chat = Chat()

    _id = chat.create(data['user'], data['content'])

    data = chat.get_one_by_id(_id)

    data = json.dumps(data)
    die_send_json(data)
