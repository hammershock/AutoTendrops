from setuptools import setup, Extension
import pybind11

setup(
    name="waterdrop",
    version="1.0",
    ext_modules=[
        Extension(
            "waterdrop",
            ["./src/waterdrop.cpp"],
            include_dirs=[pybind11.get_include()],
            language='c++'
        ),
    ],
    zip_safe=False,
)

# python setup.py build_ext --inplace
