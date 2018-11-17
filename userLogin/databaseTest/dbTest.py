import sqlite3
from sqlite3 import Error
import secrets

def dbConnect(db_file):
    """ create a database connection to the SQLite database
        specified by db_file
    :param db_file: database file
    :return: Connection object or None
    """
    try:
        conn = sqlite3.connect(db_file)
        return conn
    except Error as e:
        print(e)

    return None

def create_table(conn, create_table_sql):
    """ create a table from the create_table_sql statement
    :param conn: Connection object
    :param create_table_sql: a CREATE TABLE statement
    :return:
    """
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)

database = "userDbTest.db"

# create a database connection
conn = dbConnect(database)
if conn is not None:
    # create user db
    userTable = """CREATE TABLE IF NOT EXISTS users(
                       uid INTEGER PRIMARY KEY,
                       username text,
                       hash text,
                       salt text,
                       realFirstName text,
                       realLastName text,
                       email text
                   );"""
    create_table(conn, userTable)

    sql = '''INSERT INTO users DEFAULT VALUES'''
    cur = conn.cursor()
    cur.execute(sql)

    sql = """INSERT INTO users(uid, username) VALUES(?,?)"""
    cur.execute(sql, (secrets.randbits(32), "zarweenie"))

    cur.execute("SELECT username FROM users WHERE username=?", ("zarweenie",))

    rows = cur.fetchall()

    for row in rows:
        print(row)
else:
    print("Error! cannot create the database connection.")

conn.commit()
conn.close()
