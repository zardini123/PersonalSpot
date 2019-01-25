import subprocess
import re

import os
import time

out = subprocess.run(["ffmpeg", "-codecs"], capture_output=True)
stdout = out.stdout.decode("utf-8")

# RegEx split finds a 2 or more consecutive dashes with a following newline
formats = str.split(re.split('--+\n?', stdout)[1], "\n")

extensions = []

for format in formats:
    format = re.sub(' +', ' ', format).strip()
    info = str.split(format, " ")

    if len(info) > 1 and info[0][2] == "A":
        extensions.append("." + info[1])

musicLibraryURL = "/Users/Tarasik/Music/iTunes/iTunes Media/Music"

extensionBlacklist = [".asd"]

totalTime = 0
fileCount = 0

for root, dirs, files in os.walk(musicLibraryURL):
    fileCount += len(files)

currentFile = 0
succFile = 0

extensionDict = dict()

for root, dirs, files in os.walk(musicLibraryURL):
   for name in files:
       currentTime = time.time()
       currentFile += 1

       fileExt = os.path.splitext(name)[1]
       if fileExt != "":
           if fileExt in extensionDict:
               extensionDict[fileExt] += 1
           else:
               extensionDict.update({fileExt:1})

       print(extensionDict)

       out = subprocess.run(["ffmpeg", "-i", os.path.join(root, name)], capture_output=True)
       stderr = out.stderr.decode("utf-8")
       if "Invalid data found when processing input" not in stderr and not name.startswith("."):
           succFile += 1
           metadata = stderr.partition("Metadata:")[2].split("\n")
           metadata = [re.sub(' +', ' ', data).strip() for data in metadata]
           print(metadata)

       timeTaken = time.time() - currentTime
       print(succFile, "/", currentFile, "/", fileCount, "\t", "%.6f" % timeTaken, "\t", name)
       totalTime += timeTaken

print("Total time:", totalTime)
print("Average time per file: ", totalTime / fileCount)

print(extensionDict)
