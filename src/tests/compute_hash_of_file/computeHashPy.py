import hashlib

def sha1Hash(file):
    h  = hashlib.sha1()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(file, 'rb', buffering=0) as f:
        for n in iter(lambda : f.readinto(mv), 0):
            h.update(mv[:n])
    return h.hexdigest()

print(sha1Hash("/Users/Tarasik/Music/iTunes/iTunes Media/Music/RADWIMPS/Kimi no Na wa. Original Soundtrack/01 Yumetourou.mp3"))
