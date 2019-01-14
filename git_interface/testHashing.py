import hashlib
from timeit import default_timer as timer

# https://www.pythoncentral.io/hashing-files-with-python/
def pythonCenteralSha(filename):
    BLOCKSIZE = 65536
    hasher = hashlib.sha1()
    with open(filename, 'rb') as afile:
        buf = afile.read(BLOCKSIZE)
        while len(buf) > 0:
            hasher.update(buf)
            buf = afile.read(BLOCKSIZE)
    return hasher.hexdigest()

# https://stackoverflow.com/questions/22058048/hashing-a-file-in-python
def sha256sum(filename):
    h  = hashlib.sha1()
    b  = bytearray(128*1024)
    mv = memoryview(b)
    with open(filename, 'rb', buffering=0) as f:
        for n in iter(lambda : f.readinto(mv), 0):
            h.update(mv[:n])
    return h.hexdigest()

file = "/Users/Tarasik/Downloads/FLAC-CROSS2/Vol.\ 01\ -\ Super\ Mario/13.\ Ground\ Theme\ \(Super\ Mario\ Bros.\ 2\).flac"
file = file.replace("\\","")

start = timer()
hash = pythonCenteralSha(file)
end = timer()

print("PythonCenteral\t\t", end - start)
print(hash)

start = timer()
hash = sha256sum(file)
end = timer()

print("Sha sum\t\t", end - start)
print(hash)
