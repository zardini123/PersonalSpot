// python3 setup.py build_ext --inplace -l avformat

#include <Python.h>

//#include "metadata.h"
#include "error.h"

#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>

PyObject *test(PyObject* self) {
  return Py_BuildValue("i", 0);
}

PyObject *getMetadata(PyObject* self, PyObject *args) {
  const char *url;

  if (!PyArg_ParseTuple(args, "s", &url))
    return NULL;

  AVFormatContext *fmt_ctx = NULL;
  //AVDictionaryEntry *tag = NULL;

  int ret;

  // Get AVFormatContext from an input file
  if ((ret = avformat_open_input(&fmt_ctx, url, NULL, NULL))) {
      fprintf(stderr, "Cannot open input file\n");
      return PyLong_FromLong(ret);
  }

  // Find the stream info from format context
  //    if not, abandon.
  if ((ret = avformat_find_stream_info(fmt_ctx, NULL))) {
      fprintf(stderr, "Cannot find stream information\n");
      avformat_close_input(&fmt_ctx);
      return PyLong_FromLong(ret);
  }

  // Get audio that best fits the stream
  // Use this to find the codec
  /*
  AVCodec *dec;
  ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
  if (ret < 0) {
    fprintf(stderr, "Cannot find an audio stream in the input file\n");
    return PyLong_FromLong(ret);
  }
  */
  //int audioStreamIndex = ret;

  AVDictionaryEntry *tag = NULL;

  // https://stackoverflow.com/questions/36050713/using-py-buildvalue-to-create-a-list-of-tuples-in-c
  // https://docs.python.org/3.7/c-api/concrete.html
  PyObject *metaDict = PyDict_New();
  if (metaDict == NULL) {
    avformat_close_input(&fmt_ctx);
    return PyLong_FromLong(-1);
  }

  while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    //printf("%s=%s\n", tag->key, tag->value);

    ret = PyDict_SetItem(metaDict, Py_BuildValue("s", tag->key), Py_BuildValue("s", tag->value));
    if (ret < 0) {
      fprintf(stderr, "Error setting dict items\n");
      avformat_close_input(&fmt_ctx);
      return PyLong_FromLong(ret);
    }
  }

  avformat_close_input(&fmt_ctx);

  return metaDict;
}
