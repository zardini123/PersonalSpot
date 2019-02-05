#include <stdio.h>
#include <stdlib.h>

#include <libavutil/hash.h>
#include <libavutil/error.h>
#include <string.h>

int main() {
  struct AVHashContext *hash;

  const char *hash_name = "sha160";
  int ret;
  if ((ret = av_hash_alloc(&hash, hash_name)) < 0) {
    switch(ret) {
    case AVERROR(EINVAL):
        printf("Invalid hash type: %s\n", hash_name);
        break;
    case AVERROR(ENOMEM):
        printf("%s\n", strerror(errno));
        break;
    }
    return 1;
  }

  const char *url = "/Users/Tarasik/Music/iTunes/iTunes Media/Music/RADWIMPS/Kimi no Na wa. Original Soundtrack/01 Yumetourou.mp3";
  FILE *fp = fopen(url, "rb");

  int bufferSize = 65535;
  unsigned char buffer[bufferSize + 1];

  av_hash_init(hash);
  if (fp != NULL) {
    int read = 0;
    while((read = fread(buffer, 1, bufferSize, fp)) > 0) {
      av_hash_update(hash, buffer, read);
    }
    fclose(fp);
  }

  char res[2 * AV_HASH_MAX_SIZE + 4];
  av_hash_final_hex(hash, (uint8_t *)(&res), sizeof(res));
  printf("%s\n", res);

  av_hash_freep(&hash);

  return 0;
}
