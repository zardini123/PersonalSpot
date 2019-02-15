// g++ -o musicData musicData.cpp -I ../../libraries/include -L ../../libraries/lib -l sqlite3 -l boost_filesystem -l avformat -l avutil

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

// (1/13/19) Libary was having problems linking at runtime
//    due to the interfaces of ffmpeg being pure C, not C++
// https://stackoverflow.com/a/14644974/6183001
extern "C" {
  #include "sqlite/sqlite3.h"

  #include "libavformat/avformat.h"
  #include "libavutil/hash.h"
  #include "libavutil/base64.h"
}

// Currently Libc++, which is the C++ standard library on OS X, has not moved
//    filesystem headers to a sandard location because the specification is
//    "not stable."
// https://stackoverflow.com/questions/42633477/macos-clang-c17-filesystem-header-not-found
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#include "musicData.h"

sqlite3 *db;
char* dbUrl;

int databaseSetup(const char* rootUrl) {
  int ret;

  fs::path baseUrl = rootUrl;
  baseUrl /= "Apollo";

  fs::path dbUrlPath = baseUrl / "apollo.db";

  /*
   * dbUrlPath gets GC'ed at the end of this function,
   *    so create a global copy
   */
  dbUrl = strdup(dbUrlPath.c_str());

  fprintf(stderr, "%s\n", dbUrl);

  if (!fs::exists(dbUrlPath)) {
    ret = fs::create_directories(baseUrl);
  }

  // if ((ret = sqlite3_open(dbUrl, &db))) {
  //   fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  // }
  //
  // sqlite3_close(db);

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

int get_codec_and_stream_index(AVFormatContext **input_format_context,
                                  enum AVMediaType stream_type,
                                  AVCodec **input_codec) {

  /* Find a audio decoder for the audio stream. */
  int audio_stream_index = av_find_best_stream(*input_format_context,
        stream_type, -1, -1, input_codec, 0);
  int error = audio_stream_index;
  if (error < 0) {
    if (error == AVERROR_STREAM_NOT_FOUND)
      fprintf(stderr, "Could not find stream in file\n");
    if (error == AVERROR_DECODER_NOT_FOUND)
      fprintf(stderr, "Decoder cannot be found for stream type\n");

    avformat_close_input(input_format_context);
    return error;
  }

  return audio_stream_index;
}

void init_packet(AVPacket *packet)
{
    av_init_packet(packet);
    /* Set the packet data and size so that it is recognized as being empty. */
    packet->data = NULL;
    packet->size = 0;
}

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}

void text_free(void *p) {
  free(p);
}

int linkPersistent(const char* url) {
  char *zErrMsg = 0;
  const char *sql;

  struct AVHashContext *hash;
  const char *hash_name = "sha160";

  int bufferSize = 65535;
  unsigned char buffer[bufferSize + 1];

  int ret;
  int parentID = 0;
  sqlite3_stmt* ppSmt;

  sqlite3_stmt* ppSmtSelect;
  sqlite3_stmt* ppSmtArtSelect;
  sqlite3_stmt* ppSmtInsert;
  sqlite3_stmt* ppSmtArtInsert;

  fprintf(stderr, "dbUrl %s\n", dbUrl);

  /* Open main sql database */
  if ((ret = sqlite3_open(dbUrl, &db))) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  /*
   * Create table for persistent links
   *  This table includes all previously linked libraries
   */
  //sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

  sql = "CREATE TABLE IF NOT EXISTS persistent_links(" \
    "id INTEGER PRIMARY KEY," \
    "url TEXT," \
    "UNIQUE(url)" \
    ");";

  ret = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    goto cleanup;
  }

  sql = "SELECT 1 FROM persistent_links WHERE url=? LIMIT 1;";

  ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmt, NULL);
  if (ret != SQLITE_OK) {
    printf("error: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  ret = sqlite3_bind_text(ppSmt, 1, url, strlen(url), NULL);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Problem with bind: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  fprintf(stderr, "%s\n", sqlite3_expanded_sql(ppSmt));

  while ((ret = sqlite3_step(ppSmt)) == SQLITE_ROW) {
    parentID = sqlite3_column_int(ppSmt, 0);
    printf("%d\n", parentID);
  }
  if (ret != SQLITE_DONE) {
    printf("error: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_finalize(ppSmt);

  if (!parentID) {
    sql = "INSERT INTO persistent_links(url)" \
      "VALUES(?);";
    fprintf(stderr, "\n%s\n", sql);

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmt, NULL);
    if (ret != SQLITE_OK) {
      fprintf(stderr, "Problem with prepare: %s (code %d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
      goto cleanup;
    }

    ret = sqlite3_bind_text(ppSmt, 1, url, strlen(url), NULL);
    if (ret != SQLITE_OK) {
      fprintf(stderr, "Problem with bind: %s\n", sqlite3_errmsg(db));
      goto cleanup;
    }

    while ((ret = sqlite3_step(ppSmt)) == SQLITE_ROW);
    if (ret != SQLITE_DONE) {
      printf("error: %s\n", sqlite3_errmsg(db));
      goto cleanup;
    }

    parentID = sqlite3_last_insert_rowid(db);

    sqlite3_finalize(ppSmt);
  }

  printf("Final Link ID: %d\n", parentID);

  //sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

  // sqlite3_close(db);
  //
  // return 0;

  /* Create table for all media */
  sql = "CREATE TABLE IF NOT EXISTS media(" \
    "hash TEXT PRIMARY KEY," \
    "persistent_link INTEGER," \
    "path TEXT," \
    "codec_ID INTEGER," \
    "audio_stream_index INTEGER," \
    "art_index INTEGER," \
    "art_hash TEXT" \
    ");";

  ret = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    goto cleanup;
  }

  /* Create table for album art */
  sql = "CREATE TABLE IF NOT EXISTS album_art(" \
    "hash TEXT PRIMARY KEY," \
    "base64 TEXT," \
    "codec_ID INTEGER" \
    ");";

  ret = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    goto cleanup;
  }

  sql = "SELECT hash, persistent_link FROM media WHERE hash=?;";
  ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmtSelect, NULL);
  if (ret != SQLITE_OK) {
    printf("error: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  sql = "SELECT hash FROM album_art WHERE hash=?;";
  ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmtArtSelect, NULL);
  if (ret != SQLITE_OK) {
    printf("error: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  sql = "INSERT INTO media(" \
    "hash, persistent_link, path, codec_ID, audio_stream_index, art_index, art_hash)" \
    " VALUES(?,?,?,?,?,?,?);";
  fprintf(stderr, "\n%s\n", sql);
  ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmtInsert, NULL);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Problem with prepare: %s (code %d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
    goto cleanup;
  }

  sql = "INSERT INTO album_art(" \
    "hash, base64, codec_ID)" \
    " VALUES(?,?,?);";
  fprintf(stderr, "\n%s\n", sql);
  ret = sqlite3_prepare_v2(db, sql, strlen(sql), &ppSmtArtInsert, NULL);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "Problem with prepare: %s (code %d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
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

      AVFormatContext *input_format_context = NULL;

      int audio_stream_index = -1;
      int art_index = -1;
      AVCodec *input_codec;
      AVCodec *art_codec;

      char *prevHash = NULL;
      const char *relativePath;

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

      ret = sqlite3_bind_text(ppSmtSelect, 1, res, strlen(res), NULL);
      if (ret != SQLITE_OK) {
        fprintf(stderr, "Problem with bind: %s\n", sqlite3_errmsg(db));
        goto fileCleanup;
      }

      //fprintf(stderr, "\n%s\n", sqlite3_expanded_sql(ppSmtSelect));

      while ((ret = sqlite3_step(ppSmtSelect)) == SQLITE_ROW) {
        /*
         * Create copy of column text due to
         * being GC'd by the end of while loop
         */
        prevHash = strdup((const char *)sqlite3_column_text(ppSmtSelect, 0));
      }
      if (ret != SQLITE_DONE) {
        printf("error: %s\n", sqlite3_errmsg(db));
        goto fileCleanup;
      }

      if (prevHash != NULL) {
        printf("Grabbed Hash: %s\n", prevHash);
        free(prevHash);

        /*
         * TODO: Link Library edge case: Previous hash EXISTS
         *  Potentially, if a previous file exists in the database,
         *  prompt the user if they want to link the existing file to
         *  the linking database.  This prompt can be done in bulk
         *  (some files...).
         */
      } else {
        free(prevHash);

        /* Check to see if file is a valid audio file */
        if (open_input_file(path, &input_format_context))
          goto fileCleanup;

        if ((audio_stream_index = get_codec_and_stream_index(&input_format_context,
                                        AVMEDIA_TYPE_AUDIO, &input_codec)) < 0)
          goto fileCleanup;

        art_index = get_codec_and_stream_index(&input_format_context,
                                          AVMEDIA_TYPE_VIDEO, &art_codec);

        fs::path childPath(url);
        relativePath = fs::relative(iter->path(), childPath).c_str();
        printf("Relative path: %s\n", relativePath);

        ret = sqlite3_bind_text(ppSmtInsert, 1, res, strlen(res), SQLITE_TRANSIENT);
        if (ret != SQLITE_OK) {
          fprintf(stderr, "Problem with bind res: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }
        ret = sqlite3_bind_int(ppSmtInsert, 2, parentID);
        if (ret != SQLITE_OK) {
          fprintf(stderr, "Problem with bind parentID: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }
        ret = sqlite3_bind_text(ppSmtInsert, 3, relativePath, strlen(relativePath), SQLITE_TRANSIENT);
        if (ret != SQLITE_OK) {
          fprintf(stderr, "Problem with bind relativePath: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }
        ret = sqlite3_bind_int(ppSmtInsert, 4, input_codec->id);
        if (ret != SQLITE_OK) {
          fprintf(stderr, "Problem with bind input_codec: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }
        ret = sqlite3_bind_int(ppSmtInsert, 5, audio_stream_index);
        if (ret != SQLITE_OK) {
          fprintf(stderr, "Problem with bind audio_stream_index: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }
        if (art_index >= 0) {
          ret = sqlite3_bind_int(ppSmtInsert, 6, art_index);
          if (ret != SQLITE_OK) {
            fprintf(stderr, "Problem with bind audio_stream_index: %s\n", sqlite3_errmsg(db));
            goto fileCleanup;
          }
        }

        if (art_index >= 0) {
          AVCodecContext *avctx;

          /* Allocate a new decoding context. */
          avctx = avcodec_alloc_context3(art_codec);
          if (!avctx) {
            fprintf(stderr, "Could not allocate a decoding context\n");
            goto fileCleanup;
          }

          /* Initialize the stream parameters with demuxer information. */
          ret = avcodec_parameters_to_context(avctx,
                    input_format_context->streams[art_index]->codecpar);
          if (ret < 0) {
            fprintf(stderr, "Error setting parameters to context\n");
            avcodec_free_context(&avctx);
            goto fileCleanup;
          }

          /* Open the decoder for the audio stream to use it later. */
          if ((ret = avcodec_open2(avctx, art_codec, NULL)) < 0) {
              fprintf(stderr, "Could not open input codec (error '%s')\n",
                      av_err2str(ret));
              avcodec_free_context(&avctx);
              goto fileCleanup;
          }

          AVPacket packet;
          init_packet(&packet);

          do {
            if ((ret = av_read_frame(input_format_context, &packet)) < 0) {
              fprintf(stderr, "Error reading frame\n");
              avcodec_free_context(&avctx);
              goto fileCleanup;
            }
            //printf("Packet index, %d, art index, %d\n", packet.stream_index, art_index);
          } while (packet.stream_index != art_index);

          if ((ret = avcodec_send_packet(avctx, &packet)) < 0) {
            fprintf(stderr, "Error while sending a packet to the decoder: %s\n", av_err2str(ret));
            avcodec_free_context(&avctx);
            goto fileCleanup;
          }

          av_hash_init(hash);
          av_hash_update(hash, packet.data, packet.size);
          av_hash_final_hex(hash, (uint8_t *)(&res), sizeof(res));

          ret = sqlite3_bind_text(ppSmtArtSelect, 1, res, strlen(res), NULL);
          if (ret != SQLITE_OK) {
            fprintf(stderr, "Problem with bind: %s\n", sqlite3_errmsg(db));
            goto fileCleanup;
          }

          while ((ret = sqlite3_step(ppSmtArtSelect)) == SQLITE_ROW) {
            prevHash = strdup((const char *)sqlite3_column_text(ppSmtArtSelect, 0));
          }
          if (ret != SQLITE_DONE) {
            printf("error: %s\n", sqlite3_errmsg(db));
            goto fileCleanup;
          }

          sqlite3_clear_bindings(ppSmtArtSelect);
          sqlite3_reset(ppSmtArtSelect);

          if (prevHash != NULL) {
            // If same hashes...
          } else {
            int baseSize = AV_BASE64_SIZE(packet.size);
            char *base64 = (char *)malloc(baseSize);
            av_base64_encode(base64, baseSize, packet.data, packet.size);

            //printf("YAY!\n");
            ret = sqlite3_bind_text(ppSmtArtInsert, 1, res, strlen(res), SQLITE_TRANSIENT);
            if (ret != SQLITE_OK) {
              fprintf(stderr, "Problem with bind res: %s\n", sqlite3_errmsg(db));
              goto fileCleanup;
            }
            ret = sqlite3_bind_text(ppSmtArtInsert, 2, base64, strlen(base64), NULL);
            if (ret != SQLITE_OK) {
              fprintf(stderr, "Problem with bind parentID: %s\n", sqlite3_errmsg(db));
              goto fileCleanup;
            }
            ret = sqlite3_bind_int(ppSmtArtInsert, 3, art_codec->id);
            if (ret != SQLITE_OK) {
              fprintf(stderr, "Problem with bind relativePath: %s\n", sqlite3_errmsg(db));
              goto fileCleanup;
            }

            while ((ret = sqlite3_step(ppSmtArtInsert)) == SQLITE_ROW);
            if (ret != SQLITE_DONE) {
              fprintf(stderr, "error: %s\n", sqlite3_errmsg(db));
              goto fileCleanup;
            }

            sqlite3_clear_bindings(ppSmtArtInsert);
            sqlite3_reset(ppSmtArtInsert);

            free(base64);
          }

          ret = sqlite3_bind_text(ppSmtInsert, 7, res, strlen(res), NULL);
          if (ret != SQLITE_OK) {
            fprintf(stderr, "Problem with bind res: %s\n", sqlite3_errmsg(db));
            goto fileCleanup;
          }
          if (prevHash)
            free(prevHash);

          av_packet_unref(&packet);

          // FILE *img = fopen(argv[2], "wb");
          // fwrite(packet.data, 1, packet.size, img);
          // fclose(img);
        }

        while ((ret = sqlite3_step(ppSmtInsert)) == SQLITE_ROW);
        if (ret != SQLITE_DONE) {
          fprintf(stderr, "error: %s\n", sqlite3_errmsg(db));
          goto fileCleanup;
        }

        //fprintf(stderr, "%s\n", sqlite3_expanded_sql(ppSmtInsert));
      }

    fileCleanup:
      if (input_format_context)
        avformat_close_input(&input_format_context);
      if (fp)
        fclose(fp);
      sqlite3_clear_bindings(ppSmtInsert);
      sqlite3_clear_bindings(ppSmtSelect);
      sqlite3_reset(ppSmtInsert);
      sqlite3_reset(ppSmtSelect);

      printf("\n\n");
    }
  }

cleanup:
  if (hash)
    av_hash_freep(&hash);
  sqlite3_close(db);

  sqlite3_finalize(ppSmtInsert);
  sqlite3_finalize(ppSmtArtInsert);
  sqlite3_finalize(ppSmtSelect);
  sqlite3_finalize(ppSmtArtSelect);

  return ret;
}

int main() {
  databaseSetup("/Users/Tarasik/Music");
  linkPersistent("/Users/Tarasik/Music/iTunes/iTunes Media/Music");

  free(dbUrl);
  return 0;
}
