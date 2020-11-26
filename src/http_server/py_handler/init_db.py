def init_db(cursor):
    cursor.execute('CREATE TABLE IF NOT EXISTS chat ('
        'id        INTEGER PRIMARY KEY AUTOINCREMENT,'
        "timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),"
        'user      TEXT NOT NULL,'
        'content   TEXT NOT NULL'
        ')'
    )

    cursor.execute('CREATE TABLE IF NOT EXISTS user ('
        "username TEXT PRIMARY KEY,"
        "password TEXT NOT NULL" # TODO: hash the password with random salt
        ')'
    )

    cursor.execute('CREATE TABLE IF NOT EXISTS session ('
        "username   TEXT NOT NULL,"
        "session_id TEXT NOT NULL UNIQUE"
        ')'
    )
