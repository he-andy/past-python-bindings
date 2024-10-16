import pytest
import past



def test_gemm():
    prog_a = "gemm/vanilla.c"
    prog_b = "gemm/acc-inter.c"
    out_var = "C"
    ret = past.verify(prog_a, prog_b, out_var)
    print("return value: ", ret)

if __name__ == "__main__":
    # pytest.main([__file__])
    # print("All tests passed!")
    test_gemm()