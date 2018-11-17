import sqlite3
import argon2

user = input("Username: ")
print(user)

password = input("Password: ")
salt = "abcdefghijk"
hash = argon2.argon2_hash(password=password, salt=salt);

print(hash);
