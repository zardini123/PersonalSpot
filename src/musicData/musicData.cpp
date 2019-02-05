// g++ -o musicData musicData.cpp -I ../../libraries/include -L ../../libraries/lib -l sqlite3 -l boost_filesystem

#include <stdio.h>
#include <stdlib.h>

// (1/13/19) Libary was having problems linking at runtime
//    due to the interfaces of ffmpeg being pure C, not C++
// https://stackoverflow.com/a/14644974/6183001
extern "C" {
  #include "sqlite/sqlite3.h"

  #include "libavformat/avformat.h"
  #include "libavutil/hash.h"
}

// Currently Libc++, which is the C++ standard library on OS X, has not moved
//    filesystem headers to a sandard location because the specification is
//    "not stable."
// https://stackoverflow.com/questions/42633477/macos-clang-c17-filesystem-header-not-found
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#include "musicData.h"

sqlite3 *db;
const char* dbUrl;

int main() {
  databaseSetup("/Users/Tarasik/Music");
  linkPersistent("/Users/Tarasik/Music/iTunes/iTunes Media/Music");
  return 0;
}

int databaseSetup(const char* rootUrl) {
  int ret;

  fs::path baseUrl = rootUrl;
  baseUrl /= "Apollo";

  fs::path dbUrlPath = baseUrl / "apollo.db";
  dbUrl = dbUrlPath.c_str();

  fprintf(stderr, "%s\n", dbUrl);

  if (!fs::exists(dbUrlPath)) {
    ret = fs::create_directories(baseUrl);
    if (ret == 0) {
      fprintf(stderr, "Error creating directory: %s\n", baseUrl.c_str());
    }
  }

  if ((ret = sqlite3_open(dbUrl, &db))) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  }

  sqlite3_close(db);

  return ret;
}

int open_input_file(const char *filename,
                           AVFormatContext **input_format_context)
{
    AVCodecContext *avctx;
    int error;

    /* Open the input file to read from it. */
    if ((error = avformat_open_input(input_format_context, filename, NULL,
                                     NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
                filename, av_err2str(error));
        *input_format_context = NULL;
        return error;
    }

    /* Get information on the input file (number of streams etc.). */
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not open find stream info (error '%s')\n",
                av_err2str(error));
        avformat_close_input(input_format_context);
        return error;
    }

    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int linkPersistent(const char* url) {
  char *zErrMsg = 0;
  const char *sql;

  AVFormatContext *input_format_context = NULL;

  struct AVHashContext *hash;
  const char *hash_name = "sha160";

  int bufferSize = 65535;
  unsigned char buffer[bufferSize + 1];

  int ret;

  /* Open main sql database */
  if ((ret = sqlite3_open(dbUrl, &db))) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  /*
   * Create table for persistent links
   *  This table includes all previously linked libraries
   */
  sql = "CREATE TABLE IF NOT EXISTS persistentLinks(" \
    "id INTEGER PRIMARY KEY," \
    "url TEXT" \
    ");";

  ret = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    goto cleanup;
  }

  /* Allocate hashing instance for later */
  if ((ret = av_hash_alloc(&hash, hash_name)) < 0) {
    switch(ret) {
    case AVERROR(EINVAL):
        printf("Invalid hash type: %s\n", hash_name);
        break;
    case AVERROR(ENOMEM):
        printf("%s\n", strerror(errno));
        break;
    }
    return ret;
  }

  for (fs::recursive_directory_iterator iter(url), end;
       iter != end; ++iter) {
    //std::string name = iter->path().filename().string();

    const char *path = iter->path().c_str();
    if (fs::is_regular_file(iter->path())) {
      fprintf(stderr, "%s\n", path);

      FILE *fp = fopen(path, "rb");

      /* Check to see if file is a valid audio file */
      if (open_input_file(path, &input_format_context))
        goto fileCleanup;

      /*
       * If a valid audio file:
       *  Calculate the sha160 byte hash of the entire file
       */
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

    fileCleanup:
      if (input_format_context)
        avformat_close_input(&input_format_context);
      if (fp)
        fclose(fp);
    }
  }

cleanup:
  if (hash)
    av_hash_freep(&hash);
  sqlite3_close(db);

  return ret;
}
