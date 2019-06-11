#include <stdio.h>

extern "C" {
  #include "libavutil/hash.h"
  #include "libavutil/base64.h"
}

void hash_file(struct AVHashContext *hasher, FILE *fp, char *output_hash, int hash_size, uint8_t *buffer, int buffer_size) {
  av_hash_init(hasher);
  if (fp != NULL) {
    int read = 0;
    while((read = fread(buffer, 1, buffer_size, fp)) > 0) {
      av_hash_update(hasher, buffer, read);
    }
  }

  av_hash_final_hex(hasher, (uint8_t *)output_hash, hash_size);
}
