import sqlite3

from init_db import init_db


class Chat:
    def __init__(self):
        self.conn = sqlite3.connect('data.db', isolation_level=None)
        c = self.conn.cursor()

        init_db(c)


    def get_size(self):
        c = self.conn.cursor()
        c.execute('SELECT COUNT(id) FROM chat')

        return { 'size': c.fetchone()[0] }


    def get_one_by_id(self, _id):
        c = self.conn.cursor()

        c.execute('SELECT timestamp, user, content FROM chat WHERE id = ?', (_id, ))
        timestamp, user, content = c.fetchone()

        return {
            'id': _id,
            'timestamp': timestamp,
            'user': user,
            'content': content,
        }


    def get_all(self):
        c = self.conn.cursor()

        c.execute('SELECT id, timestamp, user, content FROM chat')
        data = c.fetchall()

        return [
            {
                'id': d[0],
                'timestamp': d[1],
                'user': d[2],
                'content': d[3],
            }
            for d in data
        ]


    def create(self, user, content):
        c = self.conn.cursor()

        c.execute('INSERT INTO chat (user, content) VALUES(?, ?)', (user, content, ))
        return c.lastrowid


    def __del__(self):
        self.conn.close()
