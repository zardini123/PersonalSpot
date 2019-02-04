// If problems with mismatching node versions when running this module:
// https://stackoverflow.com/a/52796884/6183001

#include <nan.h>

#include "musicData.h"

// (1/13/19) Libary was having problems linking at runtime
//    due to the interfaces of ffmpeg being pure C, not C++
// https://stackoverflow.com/a/14644974/6183001
extern "C" {
  #include <libavformat/avformat.h>
  #include <libavutil/dict.h>

  #include "sqlite/sqlite3.h"
}

// Currently Libc++, which is the C++ standard library on OS X, has not moved
//    filesystem headers to a sandard location because the specification is
//    "not stable."
// https://stackoverflow.com/questions/42633477/macos-clang-c17-filesystem-header-not-found
#include "boost/filesystem.hpp"
using namespace boost::filesystem;

NAN_METHOD(setup) {
  // Expect exactly 1 argument
  if (info.Length() != 1) {
    return Nan::ThrowError(Nan::New("setup - expected 1 argument").ToLocalChecked());
  }
  // Expect argument to be a string
  if (!info[0]->IsString()) {
    return Nan::ThrowError(Nan::New("setup - expected argument to be a string").ToLocalChecked());
  }
  std::string v8Path = *Nan::Utf8String(info[0]);
  const char *url = v8Path.c_str();

  info.GetReturnValue().Set(Nan::New(databaseSetup(url)));
}

NAN_METHOD(listFiles) {
  // Expect exactly 1 argument
  if (info.Length() != 1) {
    return Nan::ThrowError(Nan::New("getMetadata - expected 1 argument").ToLocalChecked());
  }

  // Expect argument to be a string
  if (!info[0]->IsString()) {
    return Nan::ThrowError(Nan::New("getMetadata - expected argument to be a string").ToLocalChecked());
  }

  std::string v8Path = *Nan::Utf8String(info[0]);
  const char *url = v8Path.c_str();
  /*
  for (auto& p: fs::recursive_directory_iterator(url)) {
    fprintf(stderr, "%s\n", p.path());
  }*/

  for (recursive_directory_iterator iter(url), end;
       iter != end; ++iter) {
    std::string name = iter->path().filename().string();
    fprintf(stderr, "%s\n", iter->path().c_str());
  }
}

NAN_METHOD(getMetadata) {
  // Expect exactly 1 argument
  if (info.Length() != 1) {
    return Nan::ThrowError(Nan::New("getMetadata - expected 1 argument").ToLocalChecked());
  }

  // Expect argument to be a string
  if (!info[0]->IsString()) {
    return Nan::ThrowError(Nan::New("getMetadata - expected argument to be a string").ToLocalChecked());
  }

  std::string v8Path = *Nan::Utf8String(info[0]);
  const char *url = v8Path.c_str();

  AVFormatContext *fmt_ctx = NULL;

  int ret;
  if ((ret = avformat_open_input(&fmt_ctx, url, NULL, NULL))) {
    avformat_close_input(&fmt_ctx);
    info.GetReturnValue().Set(Nan::New(ret));
    // Nan::Error("Cannot open input file")
    return;
  }

  if ((ret = avformat_find_stream_info(fmt_ctx, NULL))) {
    avformat_close_input(&fmt_ctx);
    info.GetReturnValue().Set(Nan::New(ret));
    // Nan::Error("Cannot find stream information")
    return;
  }

  AVCodec *dec;
  ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
  if (ret < 0) {
    avformat_close_input(&fmt_ctx);
    info.GetReturnValue().Set(Nan::New(ret));
    //fprintf(stderr, "Cannot find an audio stream in the input file\n");
    //return PyLong_FromLong(ret);
    return;
  }

  v8::Local<v8::Object> outJson = Nan::New<v8::Object>();
  Nan::Set(outJson, Nan::New("codecID").ToLocalChecked(), Nan::New(dec->id));
  Nan::Set(outJson, Nan::New("codecName").ToLocalChecked(), Nan::New(dec->name).ToLocalChecked());

  AVDictionaryEntry *tag = NULL;
  v8::Local<v8::Object> metadataJson = Nan::New<v8::Object>();

  while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    //printf("%s=%s\n", tag->key, tag->value);
    Nan::Set(metadataJson, Nan::New(tag->key).ToLocalChecked(), Nan::New(tag->value).ToLocalChecked());
  }
  Nan::Set(outJson, Nan::New("metadata").ToLocalChecked(), metadataJson);

  avformat_close_input(&fmt_ctx);
  info.GetReturnValue().Set(outJson);
}

NAN_MODULE_INIT(InitModule) {
  NAN_EXPORT(target, setup);
  NAN_EXPORT(target, getMetadata);
  NAN_EXPORT(target, listFiles);
}

NODE_MODULE(musicData, InitModule);
