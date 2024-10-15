# PAST Python Bindings

## Introduction

This is a Python package for [PAST](https://sourceforge.net/projects/pocc/files/1.6/release/modules/), a tool developed by Louis-Noel Pouchet as part of the [PoCC project](https://sourceforge.net/projects/pocc/files/?source=navbar). The package provides Python bindings using **pybind11**.

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
```

## Publications
Please refer to the [FPGA'24 paper](https://dl.acm.org/doi/10.1145/3626202.3637563) for more details on formal verification of source-to-source HLS transformations. 
```bibtex
@inproceedings{pouchet2024formal,
    author = {Pouchet, Louis-No\"{e}l and Tucker, Emily and Zhang, Niansong and Chen, Hongzheng and Pal, Debjit and Rodr\'{\i}guez, Gabriel and Zhang, Zhiru},
    title = {Formal Verification of Source-to-Source Transformations for HLS},
    year = {2024},
    isbn = {9798400704185},
    publisher = {Association for Computing Machinery},
    address = {New York, NY, USA},
    url = {https://doi.org/10.1145/3626202.3637563},
    doi = {10.1145/3626202.3637563},
    booktitle = {Proceedings of the 2024 ACM/SIGDA International Symposium on Field Programmable Gate Arrays},
    pages = {97–107},
    numpages = {11},
    keywords = {formal verification, high-level synthesis, program equivalence},
    location = {Monterey, CA, USA},
    series = {FPGA '24}
}
```
