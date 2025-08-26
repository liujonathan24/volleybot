import os
import sys
import shutil
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# pybind11 header files are required
try:
    import pybind11
except ImportError:
    print("Error: pybind11 is required to build this package.")
    print("Please run: pip install pybind11")
    sys.exit(1)

# A custom build_ext command to copy the compiled .dylib and fix paths
class CustomBuildExt(build_ext):
    def build_extension(self, ext):
        super().build_extension(ext)
        
        lib_name = 'libvolleybot_physics.dylib'
        if sys.platform == 'linux':
            lib_name = 'libvolleybot_physics.so'
        elif sys.platform == 'win32':
            lib_name = 'volleybot_physics.dll'

        lib_source = os.path.join("physics_engine/build", lib_name)
        lib_dest_dir = os.path.dirname(self.get_ext_fullpath(ext.name))
        
        if os.path.exists(lib_source):
            print(f"Copying {lib_source} to {lib_dest_dir}")
            shutil.copy(lib_source, lib_dest_dir)

            if sys.platform == 'darwin':
                ext_path = self.get_ext_fullpath(ext.name)
                copied_lib_path = os.path.join(lib_dest_dir, lib_name)
                
                print(f"Patching '{ext.name}' to find its library.")
                subprocess.run([
                    'install_name_tool',
                    '-change',
                    f'@rpath/{lib_name}',
                    f'@loader_path/{lib_name}',
                    ext_path
                ], check=True)

                print(f"Setting library ID for '{lib_name}'.")
                subprocess.run([
                    'install_name_tool',
                    '-id',
                    f'@loader_path/{lib_name}',
                    copied_lib_path
                ], check=True)
        else:
            print(f"Warning: {lib_source} not found. The package may not run.")

cpp_args = ['-std=c++17']

# Linker arguments
link_args = [
    f'-L{os.path.abspath("physics_engine/build")}',
    '-lvolleybot_physics'
]
if sys.platform == 'darwin': # macOS
    # This is still good practice, even with the manual patching.
    link_args.append('-Wl,-rpath,@loader_path')
elif sys.platform == 'linux':
    link_args.append('-Wl,-rpath,$ORIGIN')


ext_modules = [
    Extension(
        'volleybot_physics',
        ['physics_engine/bindings/python_bindings.cpp'],
        include_dirs=[
            pybind11.get_include(),
            'physics_engine/include'
        ],
        language='c++',
        extra_compile_args=cpp_args,
        extra_link_args=link_args
    ),
]

setup(
    name='volleybot_physics',
    version='0.0.1',
    author='Fritz',
    description='Physics engine for volleybot',
    ext_modules=ext_modules,
    cmdclass={'build_ext': CustomBuildExt},
)