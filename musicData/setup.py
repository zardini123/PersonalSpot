# Build command:
#   python3 setup.py build_ext --inplace -lavformat

from distutils.core import setup, Extension

setup(name = 'musicData', version = '0.1', \
    ext_modules = [Extension('musicData', ['musicData.c', 'metadata.c', 'error.c'])])
