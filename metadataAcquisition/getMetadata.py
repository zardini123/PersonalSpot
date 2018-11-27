import subprocess
import re

out = subprocess.run(["ffmpeg", "-codecs"], capture_output=True)
stdout = out.stdout.decode("utf-8")

formats = str.split(re.split('--+\n?', stdout)[1], "\n")

for format in formats:
    format = re.sub(' +', ' ', format).strip()
    info = str.split(format, " ")
    #if info[0] == "DE":
    print(info)

musicLibraryURL = "/Users/Tarasik/Music/iTunes/iTunes Media/Music"
