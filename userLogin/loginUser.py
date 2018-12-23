from sqlHandling import dbConnect, execute
import validate

import argon2
import secrets

connect = dbConnect("users.db")
c = connect.cursor()

username = input("Username: ")
if validate.validateUsername(username):
    exit()

password = input("Password: ")
if validate.containsControlChars(password):
    print("Invalid password")
    exit()

# Everything past this point should be done server side
#   Hashes or salts should never be sent back to the client via any means

# Seperate grabbing all user credentail data outside of checking username
#   to not waste disk time grabbing that data, and to avoid brute force attacts
execute(c, "SELECT username FROM users WHERE username=?", (username,))

matchedUsers = c.fetchall()
if len(matchedUsers) == 0:
    print("User not found")
    exit()

# If username exists, check credentials
execute(c, "SELECT uid, username, salt, hash FROM users WHERE username=?", (username,))

# Get first element because there should be only one matched user anyway
matchedUser = c.fetchall()[0]

salt = matchedUser[2]
passAsBytes = password.encode('utf_32')
hash = argon2.argon2_hash(salt + passAsBytes, salt)
finalHash = salt + hash

storedHash = matchedUser[3]

match = secrets.compare_digest(finalHash, storedHash)

connect.close()

if match:
    print("User " + matchedUser[0] + " logged in")
else:
    print("Credentials for user do not match")
