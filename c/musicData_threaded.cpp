// g++ musicData_threaded.cpp util.cpp fileReadWrite.cpp -l sqlite3 -l boost_filesystem -l avformat -l avutil -l pthread -o musicData_threaded

#include <stdio.h>
#include <stdlib.h>

extern "C" {
  // #include "sqlite/sqlite3.h"
  #include "sqlite3.h"

  #include "libavformat/avformat.h"
  #include "libavutil/hash.h"
}

#define NUM_THREADS 4
#define BUFFER_SIZE 65536

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#include <pthread.h>

#include "util.h"
#include "fileReadWrite.h"

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  int working;
  int queued;
  int finish;
  char* path;
} thread_data_t;

/* Shared Data */
struct AVHashContext *hasher;
unsigned char buffer[BUFFER_SIZE];
const char *hash_name = "sha160";

pthread_mutex_t lock_hasher;

void *thr_func(void *arg) {
  while (1) {
    thread_data_t *data = (thread_data_t *)arg;

    while (!data->queued) {
      data = (thread_data_t *)arg;

      if (data->finish)
        pthread_exit(NULL);
    }
    data->queued = 0;
    data->working = 1;

    // printf("hello from thr_func, thread id: %d\n", data->tid);
    fprintf(stderr, "Thread %d: Url: %s\n", data->tid, data->path);

    FILE *fp = fopen(data->path, "rb");
    int size = 2 * AV_HASH_MAX_SIZE + 4;
    char res[size];

    /* get mutex before modifying and printing shared_x */
    pthread_mutex_lock(&lock_hasher);
    /* BEGIN CRITICAL SECTION */
    // Make the critical section as short as possible
    // Don't clog with unnessicary things

    /* Hash file using SHA-160, therefore being a 40 char output */
    hash_file(hasher, fp, res, size, buffer, BUFFER_SIZE);


    fprintf(stderr, "Thread %d: Exit\n", data->tid);
    fprintf(stderr, "Thread %d: %s\n", data->tid, res);

    /* END CRITICAL SECTION */
    pthread_mutex_unlock(&lock_hasher);

    // AVFormatContext *input_format_context = NULL;

    // printf("\nThread aeowdihjasdoifh %d: %s", data->tid, data->path);
    //
    // if (open_input_file(data->path, &input_format_context))
    //   goto cleanup;

    // sleep(rand() % 3);

cleanup:
    // if (input_format_context)
    //   avformat_close_input(&input_format_context);

    fclose(fp);
    free(data->path);

    data->working = 0;
  }
}

int main(int argc, char const *argv[]) {
  const char* url = "/Users/Tarasik/Music/iTunes/iTunes Media/Music";
  int ret;

  pthread_mutex_init(&lock_hasher, NULL);

  /* Allocate hash instance to be used beween threads */
  if ((ret = av_hash_alloc(&hasher, hash_name)) < 0) {
    switch(ret) {
    case AVERROR(EINVAL):
        fprintf(stderr, "Invalid hash type: %s\n", hash_name);
        break;
    case AVERROR(ENOMEM):
        fprintf(stderr, "%s\n", strerror(errno));
        break;
    }
    return ret;
  }

  int i, workers_booked;
  pthread_t thr[NUM_THREADS];
  thread_data_t thr_data[NUM_THREADS];

  for (i = 0; i < NUM_THREADS; ++i) {
    thr_data[i].tid = i;
    thr_data[i].working = 0;
    thr_data[i].queued = 0;
    thr_data[i].finish = 0;
    if ((ret = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
      fprintf(stderr, "error: pthread_create, ret: %d\n", ret);
      return EXIT_FAILURE;
    }
  }

  int count;
  for (fs::recursive_directory_iterator iter(url), end;
       iter != end; ++iter) {
    const char *name = iter->path().filename().string().c_str();

    const char *path = iter->path().c_str();
    //printf("%s\n", name);
    if (fs::is_regular_file(iter->path())) {
      // count++;
      workers_booked = 1;
      while(workers_booked) {
        // printf(" \33[2K");
        // for (i = 0; i < NUM_THREADS; ++i) {
        //   printf("%d: %d,%d ", i, thr_data[i].working, thr_data[i].queued);
        // }
        // printf("\r");

        for (i = 0; i < NUM_THREADS; ++i) {
          if (!thr_data[i].working && !thr_data[i].queued) {
            //printf("Worker %d open\n", i);
            workers_booked = 0;
            break;
          }
        }
        // fflush(stdout);
      }
      thr_data[i].path = strdup(path);
      thr_data[i].queued = 1;
    }
  }

  /* block until all threads complete */
  for (i = 0; i < NUM_THREADS; ++i) {
    fprintf(stderr, "Waiting to close thread %d...\n", i);
    thr_data[i].finish = 1;
    pthread_join(thr[i], NULL);
    fprintf(stderr, "Closed thread %d\n", i);
  }

  if (hasher)
    av_hash_freep(&hasher);

  return EXIT_SUCCESS;
}
