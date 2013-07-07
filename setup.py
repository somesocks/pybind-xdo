from distutils.core import setup, Extension

ext = Extension('xdo', sources = ['xdo_bindings.c'], libraries = ['xdo'])

setup(name = 'xdo',
       version = '2.0',
       author = 'James Eric Larsen',
       author_email = 'somesocks@gmail.com',
       license = 'Apache 2',
       description = 'Python bindings for libxdo2',
       ext_modules = [ext])
