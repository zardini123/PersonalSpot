// gcc -o manageLibraryGit manageLibraryGit.c -l git2 -l avformat

// When running, use 2> /dev/null to suppress stderr output

#include <git2.h>
#include <libavformat/avformat.h>

#include <stdio.h>
#include <sys/stat.h>

#define false 0
#define true 1

const char *musicPath = "/Users/Tarasik/Music/iTunes/iTunes Media/Music";

git_index_time convertTime(struct timespec time) {
  git_index_time out;

  out.seconds = time.tv_sec;
  out.nanoseconds = time.tv_nsec;

  return out;
}

int main(int argc, char** argv) {
  git_libgit2_init();

  git_repository *repo = NULL;
  int error;

  /* Pass NULL for the output parameter to check for but not open the repo */
  if (git_repository_open_ext(NULL, musicPath, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) != 0) {
    fprintf(stdout, "Path is not an repo.  Initalizing repo...\n");

    error = git_repository_init(&repo, musicPath, false);
    if (error < 0) {
      fprintf(stderr, "Cannot initalize the repository: %s\n", giterr_last()->message);
      return error;
    }
    printf("%d\n", error);
  }

  error = git_repository_open(&repo, musicPath);
  if (error < 0) {
    fprintf(stderr, "Could not open repository: %s\n", giterr_last()->message);
    return error;
  }

  git_index *idx = NULL;
  error = git_repository_index(&idx, repo);
  if (error < 0) {
    fprintf(stderr, "Cannot get repository index: %s\n", giterr_last()->message);
    return error;
  }

  git_status_options opts = GIT_STATUS_OPTIONS_INIT;
  git_status_list *status;

  opts.show  = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
  opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
    GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
    GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH |
    GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
    GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;

  error = git_status_list_new(&status, repo, &opts);
  if (error < 0) {
    fprintf(stderr, "Cannot compile a status list: %s\n", giterr_last()->message);
    return error;
  }

  AVFormatContext *fmt_ctx = NULL;
  size_t pathLen = strlen(musicPath);

  size_t count = git_status_list_entrycount(status);
  for (size_t i = 0; i < count; ++i) {
    const git_status_entry *s = git_status_byindex(status, i);
    const char *entryPath = s->index_to_workdir->old_file.path;

    size_t entryPathLen = strlen(entryPath);

    // TODO:  Have platform-dependent path combination
    //    (e.g. / vs \\ for Mac and Windows respectively)
    char finalPath[pathLen + entryPathLen + 1];
    strcpy(finalPath, musicPath);
    strcat(finalPath, "/");
    strcat(finalPath, entryPath);

    //printf("%s\n", finalPath);

    //if (s->status == GIT_STATUS_WT_NEW) {
    //printf("%d\n", (*s).status);
    //printf("#\t%s\n", s->index_to_workdir->old_file.path);

    if ((error = avformat_open_input(&fmt_ctx, finalPath, NULL, NULL))) {
      fprintf(stderr, "Cannot open input file\n");
      goto clean;
    }

    if ((error = avformat_find_stream_info(fmt_ctx, NULL))) {
      fprintf(stderr, "Cannot find stream information\n");
      goto clean;
    }

    unsigned int fileType = 0x7;
    AVCodec *dec;
    error = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (error < 0) {
      fprintf(stderr, "Cannot find an audio stream in the input file\n");
      fileType &= ~1;
    }

    AVCodec *attachDec;
    error = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_ATTACHMENT, -1, -1, &attachDec, 0);
    if (error < 0) {
      fprintf(stderr, "Cannot find an attachment stream in the input file\n");
      fileType &= ~2;
    }

    AVCodec *videoDec;
    error = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDec, 0);
    if (error < 0) {
      fprintf(stderr, "Cannot find an video stream in the input file\n");
      fileType &= ~3;
    }

    if (fileType == 0x0) {
      fprintf(stderr, "File does not contain an audio, attachment, or video stream");
      goto clean;
    }

    printf("%s\n", finalPath);
    // Good files come here

    /*
    error = git_index_add_bypath(idx, entryPath);
    if (error < 0) {
      fprintf(stderr, "Cannot get repository index: %s\n", giterr_last()->message);
    }
    goto ee;
    */

    git_oid oid;
    error = git_repository_hashfile(&oid, repo, entryPath, GIT_OBJ_BLOB, NULL);
    if (error < 0) {
      fprintf(stderr, "Cannot hash file: %s\n", giterr_last()->message);
      return error;
    }

    char out[41];
    out[40] = '\0';
    git_oid_fmt(out, &oid);
    printf("%s\n", out);

    /*
    git_index *fakeIndex;
    git_index_new(&fakeIndex);

    error = git_index_add_bypath(fakeIndex, entryPath);
    if (error < 0) {
      fprintf(stderr, "Cannot get repository index: %s\n", giterr_last()->message);
      return error;
    }
    git_index_free(fakeIndex);
    const git_index_entry *entry = git_index_get_bypath(fakeIndex, entryPath, GIT_INDEX_STAGE_NORMAL);
    */

    // https://github.com/git/git/blob/master/Documentation/technical/index-format.txt
    // https://mincong-h.github.io/2018/04/28/git-index/
    git_index_entry entry;

    struct stat info;
    stat(finalPath, &info);

    entry.ctime = convertTime(info.st_ctimespec);
    entry.mtime = convertTime(info.st_mtimespec);

    entry.dev = info.st_dev;
    entry.ino = info.st_ino;
    entry.mode = info.st_mode;
    entry.uid = info.st_uid;
    entry.gid = info.st_gid;
    entry.file_size = info.st_size;

    entry.id = oid;

    //entry.flags
    //entry.flags_extended

    entry.path = entryPath;

    //git_blob_create_frombuffer(&oid, repo, finalPath, strlen(finalPath));
    error = git_index_add_frombuffer(idx, &entry, out, strlen(out));
    if (error < 0) {
      fprintf(stderr, "Cannot add buffer: %s\n", giterr_last()->message);
      return error;
    }

    ee:
    error = git_index_write(idx);
    if (error < 0) {
      fprintf(stderr, "Cannot write index: %s\n", giterr_last()->message);
      return error;
    }

    //printf("%s\n", finalPath);
    clean:
    avformat_close_input(&fmt_ctx);
    continue;
  }

  git_index_free(idx);
  git_repository_free(repo);

  git_libgit2_shutdown();

  return error;
}
