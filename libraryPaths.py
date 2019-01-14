import musicData
from sqlHandling import dbConnect, execute

import urllib.parse as urlparse
import urllib.request as urllib

import re

import os
import platform

import hashlib
import uuid

illegalNames = ["CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"]
musicLibraryURL = os.path.normpath("/Users/Tarasik/Music/iTunes/iTunes Media/Music")

def makeDir(path):
    if os.path.exists(path):
        return True
    else:
        os.mkdir(path)
        return False

apolloPath = os.path.join(musicLibraryURL, ".apollo")
if not makeDir(apolloPath):
    print("Apollo directory did not exist; created folder")

collectionPath = os.path.join(apolloPath, "Compilations")
collectionExists = os.path.exists(collectionPath)

# Regex to replace characters with underscores:
#   1st: Find 1 leading whitespace char (^\s?)          [Windows]
#   2nd: Find select characters <>:"/\|?*               [Windows and Unix]
#   3rd: Find 1 trailing dot or whitespace char
#       ([\s\.]{1}$)                                    [Windows]
# All is smashed together using an regex OR: |
underscoreReg = re.compile(r"(^\s{1})|[\/\\:*?\"<>|]|([\s\.]{1}$)")
# RegEx to replace characters with nothing:
#   1st: Control character regex for ASCII characters 0 to 1F (31)
#       [\x00-\x1F]                                     [Windows and Unix]
#   2nd: Find all trailing whitespaces if and only if it's after
#       non-whitespace characters, using positive lookbehind
#       (?<=[^\s])[\s]+$                                [Windows]
# All is smashed together using an regex OR: |
removeReg = re.compile(r"[\x00-\x1F]|(?<=[^\s])[\s]+$")

# filenameReg = reg.compile(

def filterString(string):
    string = removeReg.sub("", string)
    string = underscoreReg.sub("_", string)

    if string in illegalNames:
        string = string[:(len(string) - 1)] + "_"

    return string

def path2url(path):
    return urlparse.urljoin('file:', urllib.pathname2url(path))

def sha1Hash(file):
    h  = hashlib.sha1()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(file, 'rb', buffering=0) as f:
        for n in iter(lambda : f.readinto(mv), 0):
            h.update(mv[:n])
    return h.hexdigest()

def creationTime(path_to_file):
    """
    Try to get the date that a file was created, falling back to when it was
    last modified if that isn't possible.
    See http://stackoverflow.com/a/39501288/1709587 for explanation.
    """
    if platform.system() == 'Windows':
        return os.path.getctime(path_to_file)
    else:
        stat = os.stat(path_to_file)
        try:
            return stat.st_birthtime
        except AttributeError:
            # We're probably on Linux. No easy way to get creation dates here,
            # so we'll settle for when its content was last modified.
            return stat.st_mtime

# Improve SQLite speed:
# https://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite?rq=1
dbPath = os.path.join(apolloPath, "apollo.db")
conn = dbConnect(dbPath)
c = conn.cursor()
if conn is not None:
    trackTable = """CREATE TABLE IF NOT EXISTS tracks(
        hash blob PRIMARY KEY,
        location text,
        filename text,
        title text,
        artist text,
        album text,
        niceTitle text,
        niceArtist text,
        niceAlbum text,
        modifiedTime real,
        creationTime real
    );"""
    # idTable = """CREATE TABLE IF NOT EXISTS ids(
    #     uid text PRIMARY KEY,
    #     hash blob,
    #     location text,
    #     FOREIGN KEY (hash) REFERENCES tracks(hash)
    #         ON UPDATE SET NULL
    # );"""
    invalidFiles = """CREATE TABLE IF NOT EXISTS invalidFiles(
        location text PRIMARY KEY,
        mtime real,
        ctime real
    );"""
    if execute(c, trackTable) or execute(c, invalidFiles):
        exit()

    for root, dirs, files in os.walk(musicLibraryURL):
        for name in files:
            fullPath = os.path.join(root, name)
            metadata = musicData.getMetadata(fullPath)

            stats = os.stat(fullPath)
            #print(type(stats.st_ctime))

            #url = path2url(fullPath)
            #print(url)

            if type(metadata) is dict:
                uid = uuid.uuid4().hex

                hash = sha1Hash(fullPath)

                relPath = os.path.relpath(fullPath, musicLibraryURL)
                relPathUrl = urllib.pathname2url(relPath)

                if execute(c, "SELECT hash, location FROM tracks WHERE hash=?", (hash,)):
                    exit()
                matchingHashes = c.fetchall()
                if len(matchingHashes) > 0:
                    ####################
                    # DUPLICATE TRACKS
                    print("Duplicate track", name, "exists")
                    continue;

                # if execute(c, "INSERT INTO ids(uid, hash, location) VALUES(?,?,?)", (uid, hash, relPathUrl)):
                #     exit()

                # TODO:  Metadata is case-insensitive
                niceArtist = artist = ""
                if 'album_artist' in metadata:
                    artist = metadata['album_artist']
                else:
                    if 'artist' in metadata:
                        artist = metadata['artist']

                #print(artist)
                niceArtist = filterString(artist)
                if niceArtist == "":
                    niceArtist = "Unknown Artist"
                #print(artist)

                artistPath = os.path.join(apolloPath, niceArtist)

                if ('compilation' in metadata) and (metadata['compilation'] == '1'):
                    #if (not collectionExists) and (not makeDir(collectionPath)):
                    #    collectionExists = True
                    artistPath = collectionPath
                #else:
                    #makeDir(artistPath)

                niceAlbum = album = ""
                if 'album' in metadata:
                    album = metadata['album']

                niceAlbum = filterString(album)
                if niceAlbum == "":
                    niceAlbum = "Unknown Album"

                albumPath = os.path.join(artistPath, niceAlbum)

                niceTitle = title = ""
                if 'title' in metadata:
                    title = metadata['title']

                filename, fileExt = os.path.splitext(name)

                if title == "":
                    title = filename
                    niceTitle = filterString(filename)
                #if niceTitle

                #makeDir(albumPath)

                # TODO:  Adjust niceTitle to be unique if duplicate

                mTime = os.path.getmtime(fullPath)
                cTime = creationTime(fullPath)

                if execute(c, """INSERT INTO tracks(
                    hash, location, filename, title, artist, album,
                    niceTitle, niceArtist, niceAlbum,
                    modifiedTime, creationTime) VALUES(?,?,?,?,?,?,?,?,?,?,?)""",
                    (hash, relPathUrl, filename, title, artist, album,
                    niceTitle, niceArtist, niceAlbum, mTime, cTime)):
                    exit()

                conn.commit()
                continue;
            else:
                continue;

    conn.commit()
    conn.close()
