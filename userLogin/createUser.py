import logging

from sqlHandling import dbConnect, execute

import secrets
import argon2

import uuid
import time

import validate

logging.basicConfig(level=logging.DEBUG)

log = logging.getLogger("createUser")

username = input("New Username: ")

if validate.validateUsername(username):
    exit()

password = input("Password: ")

if validate.validatePassword(password):
    exit()

connect = dbConnect("users.db")
c = connect.cursor()

userTable = """CREATE TABLE IF NOT EXISTS users(
                   uid text PRIMARY KEY,
                   username text,
                   salt blob,
                   hash blob,
                   email text
               );"""

# Create table if not already
execute(c, userTable)

# Check to see if user has been made already
execute(c, "SELECT username FROM users WHERE username=?", (username,))

matchingUsers = c.fetchall()

if len(matchingUsers) > 0:
    print("Username already exists.  Try a new username.")
    exit()

email = input("Email: ")

if not validate.validateEmail(email):
    print("Invalid email address")
    exit()

# UUID specs:
# https://tools.ietf.org/html/rfc4122.html
# https://docs.python.org/3/library/uuid.html#uuid.uuid4
uuid = uuid.uuid4().hex

# https://security.stackexchange.com/questions/41617/do-salts-have-to-be-random-or-just-unique-and-unknown
saltString = "apolloMusicServer:password_salt:" + username + ":" + uuid + ":" + str(time.time())
salt = argon2.argon2_hash(saltString, uuid, t=8, buflen=32)

passAsBytes = password.encode('utf_32')
hash = argon2.argon2_hash(salt + passAsBytes, salt)

protectedFinal = salt + hash

execute(c, "INSERT INTO users(uid, username, salt, hash, email) VALUES(?,?,?,?,?)", (uuid, username, salt, protectedFinal, email))

connect.commit()
connect.close()
