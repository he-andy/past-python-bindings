# PAST Python Bindings

## Introduction

This is a Python package for PAST, a tool developed by [Louis-Noel Pouchet](https://sourceforge.net/projects/pocc/files/?source=navbar) as part of the PoCC project. The package provides Python bindings using **pybind11**.

Pre-built wheels are automatically released, making installation easy!

## Build Requirements

To build the package from source, ensure that you have the following:

- **GCC** version 8 or higher
- **pybind11** (used for the Python bindings)

## How It Works

Refer to `.github/workflows/python-publish.yml` or `tests/test_local_build.sh` for more details. The general steps are:

1. Download the official PAST release from [PoCC](https://sourceforge.net/projects/pocc/files/?source=navbar).
2. Copy the binding C++ files and setup script.
3. Build the Python wheel.

## Usage Example

Here’s a basic example of how to use the `past` package for verifying two programs:

```python
def test_gemm():
    prog_a = "gemm/vanilla.c"
    prog_b = "gemm/acc-inter.c"
    out_var = "C"
    past.verify(prog_a, prog_b, out_var, debug=False, verbose=True)
