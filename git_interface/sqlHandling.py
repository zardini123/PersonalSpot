import sqlite3

def dbConnect(db_file):
    try:
        conn = sqlite3.connect(db_file)
        return conn
    except Error as e:
        print("DB Connection Error:\n", "DB: ", db_file, "\n", e)

    return None

def execute(cursor, sql, parameters = None):
    try:
        if parameters == None:
            cursor.execute(sql)
        else:
            cursor.execute(sql, parameters)
    except sqlite3.Error as e:
        print("DB Execute Error:\n", "SQL: ", sql, "\n", e)
        return True

    return False
