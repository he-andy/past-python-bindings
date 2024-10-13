float A[32][32];
float B[32][32];
float C[32][32];
float constant = 0.000000;
#pragma pocc-region-start
{
  // If I put constant inside region in both vanilla and transformed
  // past reports that P1 and P2 are not equivalent!
//   float constant = 0.000000;
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      C[i][j] = constant;
    }
  }
  float buf_C[32];
  for (int i = 0; i < 32; i++) {
    // 1) initialization
    l_j_init: for (int j = 0; j < 32; j++) {
      buf_C[j] = constant;
    }
    // 2) computation
    for (int k = 0; k < 32; k++) { // loop reordering
      for (int j = 0; j < 32; j++) {
        buf_C[j] += A[i][k] * B[k][j];
      }
    }
    // 3) write-back
    for (int j = 0; j < 32; j++) {
      C[i][j] = buf_C[j];
    }
  }
}
#pragma pocc-region-end