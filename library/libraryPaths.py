import musicData
from sqlHandling import dbConnect, execute

import urllib.parse as urlparse
import urllib.request as urllib

import os
import re

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

dbPath = os.path.join(apolloPath, "apollo.db")
conn = dbConnect(dbPath)
c = conn.cursor()
#if conn is not None:
if False:
    trackTable = """CREATE TABLE IF NOT EXISTS track(
                       uid INTEGER PRIMARY KEY,
                       location text,
                       mtime real,
                       ctime real,
                       atime real,
                       hash blob,
                       artist text,
                       album text,
                       name text,
                       fileArtist text,
                       fileAlbum text,
                       fileName text
                   );"""

    execute(c, musicTable)


for root, dirs, files in os.walk(musicLibraryURL):
    for name in files:
        fullPath = os.path.join(root, name)
        metadata = musicData.getMetadata(fullPath)

        stats = os.stat(fullPath)
        #print(type(stats.st_ctime))

        url = path2url(fullPath)
        print(url)

        if type(metadata) is dict:
            relPath = os.path.relpath(fullPath, musicLibraryURL)

            artistFile = artist = ""
            if 'album_artist' in metadata:
                artist = metadata['album_artist']
            else:
                if 'artist' in metadata:
                    artist = metadata['artist']

            #print(artist)
            artistFile = filterString(artist)
            if artistFile == "":
                artistFile = "Unknown Artist"
            #print(artist)

            artistPath = os.path.join(apolloPath, artistFile)

            if ('compilation' in metadata) and (metadata['compilation'] == '1'):
                #if (not collectionExists) and (not makeDir(collectionPath)):
                #    collectionExists = True
                artistPath = collectionPath
            #else:
                #makeDir(artistPath)

            albumFile = album = ""
            if 'album' in metadata:
                album = metadata['album']

            albumFile = filterString(album)
            if albumFile == "":
                albumFile = "Unknown Album"

            albumPath = os.path.join(artistPath, albumFile)

            #makeDir(albumPath)

            filename, fileExt = os.path.splitext(fullPath)

            continue;

conn.commit()
conn.close()
