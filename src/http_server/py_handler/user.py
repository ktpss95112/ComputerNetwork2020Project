import sqlite3
import uuid

from init_db import init_db


class User:
    def __init__(self):
        self.conn = sqlite3.connect('data.db', isolation_level=None)
        c = self.conn.cursor()

        init_db(c)


    def register(self, username, password):
        # return True on success; False otherwise
        c = self.conn.cursor()
        c.execute('SELECT username FROM user WHERE username = ?', (username, ))

        if len(c.fetchall()) == 1:
            return False

        c.execute('INSERT INTO user (username, password) VALUES (?, ?)', (username, password, ))
        return True


    def login(self, username, password):
        # return (True, session_id) on success; (False, '') otherwise
        c = self.conn.cursor()
        c.execute('SELECT username FROM user WHERE username = ? AND password = ?', (username, password, ))

        if len(c.fetchall()) == 0:
            return False, ''

        session_id = str(uuid.uuid4())
        c.execute('INSERT INTO session (username, session_id) VALUES (?, ?)', (username, session_id, ))

        return True, session_id


    def get_username_by_session(self, session_id):
        # return (True, username) on success; (False, '') otherwise
        c = self.conn.cursor()
        c.execute('SELECT username FROM session WHERE session_id = ?', (session_id, ))

        username = c.fetchall()

        if len(username) == 0:
            return False, ''

        return True, username[0][0]


    def __del__(self):
        self.conn.close()
