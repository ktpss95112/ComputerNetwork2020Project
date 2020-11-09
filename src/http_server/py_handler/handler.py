class Request:
    def __init__(self, method=None, path=None, query=None, http_version=None, headers=None, body=None):
        self.method = method
        self.path = path
        self.query = query
        self.http_version = http_version
        self.headers = headers
        self.body = body
