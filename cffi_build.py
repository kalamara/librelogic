# cffi_build.py
from cffi import FFI
import pathlib

""" Build the CFFI Python bindings """
print("Building CFFI Module")
ffibuilder = FFI()

this_dir = pathlib.Path().absolute()
lib_dir = this_dir / "build-lib"

h_file_name = this_dir / "include/plc_iface.h"

with open(h_file_name) as h_file:
    ffibuilder.cdef(h_file.read())

ffibuilder.set_source(
    "_librelogic",
    '#include "include/plc_iface.h"', 
    libraries=["logic"],
    library_dirs=[lib_dir.as_posix()],
    extra_compile_args=["-DGPIOD"], 
    extra_link_args=["-Wl,-rpath,./build-lib"],
)

print("Looking for Logic in " ,lib_dir)

if __name__ == "__main__":

    ffibuilder.compile(verbose=True)


 
