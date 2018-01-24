from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    name="rnnoise",
    version="0.0.1",
    description="Python bindings for rnnoise",
    author="Andrew Wan",
    ext_modules = cythonize([
        Extension("rnnoise",
            ["rnnoise.pyx"],
            extra_link_args=['-lrnnoise']
        )
    ])
)
