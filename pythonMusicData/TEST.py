import musicData
import re

import os
import time

musicLibraryURL = "/Users/Tarasik/Music/iTunes/iTunes Media/Music"

totalTime = 0
fileCount = 0

for root, dirs, files in os.walk(musicLibraryURL):
    fileCount += len(files)

currentFile = 0
succFile = 0

for root, dirs, files in os.walk(musicLibraryURL):
   for name in files:
       currentTime = time.time()
       currentFile += 1

       url = os.path.join(root, name)
       out = musicData.getMetadata(url)
       print(out)

       timeTaken = time.time() - currentTime
       print(succFile, "/", currentFile, "/", fileCount, "\t", "%.6f" % timeTaken, "\t", name)
       totalTime += timeTaken

print("Total time:", totalTime)
print("Average time per file: ", totalTime / fileCount)
