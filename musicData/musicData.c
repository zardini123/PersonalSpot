#include <Python.h>
#include "metadata.h"
#include "error.h"

/*
  Module definitions
*/

static char musicDataDocs[] = "Using ffmpeg to aquire music "
                              "metadata and data for Python\n";

static PyMethodDef musicDataPythonMethods[] = {
  {"test", (PyCFunction)test, METH_NOARGS,
    "test if working"},
  {"getMetadata", (PyCFunction)getMetadata, METH_VARARGS,
    "Get metadata from audio file"},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef musicDataModule = {
  PyModuleDef_HEAD_INIT,
  "musicData",
  musicDataDocs,
  -1,
  musicDataPythonMethods
};

// Initalize musicData python module, called on 'import musicData'
PyMODINIT_FUNC PyInit_musicData(void) {
  PyObject *m = PyModule_Create(&musicDataModule);

  if (m == NULL)
    return NULL;

  MuseError = PyErr_NewException("musicData.error", NULL, NULL);
  Py_INCREF(MuseError);
  PyModule_AddObject(m, "error", MuseError);
  return m;
}
