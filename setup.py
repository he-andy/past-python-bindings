from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import pybind11
import sysconfig
import os
import shutil

include_dirs = [
    sysconfig.get_path('include'),
    pybind11.get_include(),
    "past/include",
    "past/include/past",
]

library_dirs = [
  "past/.libs",
  "src/"
]

libraries = [
    "past",
]


class CustomBuildExt(build_ext):
    def run(self):
        subprocess.check_call(["./bootstrap.sh"])
        subprocess.check_call(["./configure"])
        subprocess.check_call(["make", "-j4"])
        
        # Proceed with the usual builid_ext command
        super().run()

        # Copy the shared library to the package directory
        build_lib_dir = self.get_ext_fullpath("past")
        build_lib_dir = os.path.dirname(build_lib_dir)

        for lib_pth in library_dirs:
            # copy all .so, .so.** files
            for file in os.listdir(lib_pth):
                if ".so" in file:
                    shutil.copy(os.path.join(lib_pth, file), build_lib_dir)


ext_modules = [
    Extension(
        'past',  # Name of the Python module
        ['src/bindings.cpp'],    # Source files
        include_dirs=include_dirs,  # pybind11 include directory
        library_dirs=library_dirs,
        libraries=libraries,
        extra_compile_args=['-fPIC'],
        extra_link_args=['-Wl,-rpath,$ORIGIN'],  # Add RPATH to search the directory of the .so
        language='c++',             # Using C++
    ),
]

setup(
    name='past',
    version='0.7.2',
    author='Louis-Noel Pouchet, Niansong Zhang',
    ext_modules=ext_modules,
    cmdclass={'build_ext': CustomBuildExt},
    include_package_data=True,
)
