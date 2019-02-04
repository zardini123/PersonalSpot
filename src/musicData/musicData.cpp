// g++ -o musicData musicData.cpp -I ../../libraries/include -L ../../libraries/lib -l sqlite3 -l boost_filesystem

// (1/13/19) Libary was having problems linking at runtime
//    due to the interfaces of ffmpeg being pure C, not C++
// https://stackoverflow.com/a/14644974/6183001
extern "C" {
  #include "sqlite/sqlite3.h"
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

  int ret;

  if ((ret = sqlite3_open(dbUrl, &db))) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    goto cleanup;
  }

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

  // for (fs::recursive_directory_iterator iter(url), end;
  //      iter != end; ++iter) {
  //   std::string name = iter->path().filename().string();
  //   fprintf(stderr, "%s\n", iter->path().c_str());
  // }

cleanup:
  sqlite3_close(db);

  return ret;
}
