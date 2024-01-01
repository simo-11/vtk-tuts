/*******************************************************************************
*  Copyright (C) 2009-2015 Intel Corporation. All Rights Reserved.
*  The information and material ("Material") provided below is owned by Intel
*  Corporation or its suppliers or licensors, and title to such Material remains
*  with Intel Corporation or its suppliers or licensors. The Material contains
*  proprietary information of Intel or its suppliers and licensors. The Material
*  is protected by worldwide copyright laws and treaty provisions. No part of
*  the Material may be copied, reproduced, published, uploaded, posted,
*  transmitted, or distributed in any way without Intel's prior express written
*  permission. No license under any patent, copyright or other intellectual
*  property rights in the Material is granted to or conferred upon you, either
*  expressly, by implication, inducement, estoppel or otherwise. Any license
*  under such intellectual property rights must be express and approved by Intel
*  in writing.
*
********************************************************************************
*/
/*
   LAPACKE_sgesv Example.
   ======================

   The program computes the solution to the system of linear
   equations with a square matrix A and multiple
   right-hand sides B, where A is the coefficient matrix:

     6.80  -6.05  -0.45   8.32  -9.67
    -2.11  -3.30   2.58   2.71  -5.14
     5.66   5.36  -2.70   4.35  -7.26
     5.97  -4.44   0.27  -7.17   6.08
     8.23   1.08   9.04   2.14  -6.87

   and B is the right-hand side matrix:

     4.02  -1.56   9.81
     6.19   4.00  -4.09
    -8.22  -8.67  -4.57
    -7.57   1.75  -8.61
    -3.03   2.86   8.99

   Description.
   ============

   The routine solves for X the system of linear equations A*X = B,
   where A is an n-by-n matrix, the columns of matrix B are individual
   right-hand sides, and the columns of X are the corresponding
   solutions.

   The LU decomposition with partial pivoting and row interchanges is
   used to factor A as A = P*L*U, where P is a permutation matrix, L
   is unit lower triangular, and U is upper triangular. The factored
   form of A is then used to solve the system of equations A*X = B.

   Example Program Results.
   ========================

 LAPACKE_sgesv (row-major, high-level) Example Program Results

 Solution
  -0.80  -0.39   0.96
  -0.70  -0.55   0.22
   0.59   0.84   1.90
   1.32  -0.10   5.36
   0.57   0.11   4.04

 Details of LU factorization
   8.23   1.08   9.04   2.14  -6.87
   0.83  -6.94  -7.92   6.55  -3.99
   0.69  -0.67 -14.18   7.24  -5.19
   0.73   0.75   0.02 -13.82  14.19
  -0.26   0.44  -0.59  -0.34  -3.43

 Pivot indices
      5      5      3      4      5
*/
/*
Modified for vtk-tuts
*/
#include <stdlib.h>
#include <stdio.h>
#define CUBLASAPI
#include "cusolver_utils.h"
#include "MathExample.h"

namespace vtk_tuts {
    /* Auxiliary routine: printing a matrix */
    static void print_float_matrix(char* desc, int m, int n, float* a, int lda) {
        int i, j;
        printf("\n %s\n", desc);
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) printf(" %6.2f", a[i * lda + j]);
            printf("\n");
        }
    }

    /* Auxiliary routine: printing a vector of integers */
    static void print_int_vector(char* desc, int n, int* a) {
        int j;
        printf("\n %s\n", desc);
        for (j = 0; j < n; j++) printf(" %6i", a[j]);
        printf("\n");
    }
    static float* getRandomA(int n) {
        float* a;
        int lda;
        if (n == 5) {
            float src[5 * 5] = {
                6.80f, -6.05f, -0.45f,  8.32f, -9.67f,
               -2.11f, -3.30f,  2.58f,  2.71f, -5.14f,
                5.66f, 5.36f, -2.70f,  4.35f, -7.26f,
                5.97f, -4.44f,  0.27f, -7.17f, 6.08f,
                8.23f, 1.08f,  9.04f,  2.14f, -6.87f
            };
            size_t size = 5 * 5 * sizeof(float);
            a = (float*)malloc(size);
            if (a!=nullptr) {
                memcpy(a, src, size);
            }
        }
        else {
            generate_random_matrix<float>(n, n, &a, &lda);
            make_diag_dominant_matrix<float>(n, n, a, lda);
        }
        return a;
    }
    static float* getRandomB(int n) {
        float* b;
        int ldb;
        if (n == 5) {
            float src[5] = {
                4.02f,
                6.19f,
               -8.22f,
               -7.57f,
               -3.03f,
            };
            size_t size = 5 * 5 * sizeof(float);
            b = (float*)malloc(size);
            memcpy(b, src, size);
        }
        else {
            generate_random_matrix<float>(n, 1, &b, &ldb);
        }
        return b;
    }
}
/*
@return info from solver
*/
int MathSolve(int verbose, SolverImpl impl, int n) {
    float *a= vtk_tuts::getRandomA(n);
    float *b= vtk_tuts::getRandomB(n);
    int* ipiv = (int *)malloc(n * sizeof(float));
    int info = 0;
    /* Solve the equations A*X = B */
    switch (impl) {
    case mkl:
        info = lapack_solve(verbose, n, a, b, ipiv);
        break;
    case cuda:
        info = cuda_solve(verbose, n, a, b, ipiv);
    }
    if (n == 5) {
        float expected[5] = { -0.80,-0.70,0.59,1.32,0.57 };
        for (int i = 0; i < 5; i++) {
            float rd = b[i] / expected[i];
            if (rd < 0.99 || rd>1.01) {
                printf("Solution failed, for %i expected %4.2f but got %4.2f\n",
                    i,expected[i], b[i]);
                info = 99;
                break;
            }
        }
    }
    if (verbose && n<11) {
        /* Print solution */
        vtk_tuts::print_float_matrix("Solution", n, 1, b, 1);
        /* Print details of LU factorization */
        vtk_tuts::print_float_matrix("Details of LU factorization", n, n, a, n);
        /* Print pivot indices */
        vtk_tuts::print_int_vector("Pivot indices", n, ipiv);
    }
    free(a);
    free(b);
    free(ipiv);
    return info;
}
/**
* moved from cusolver_utils.h to avoid LNK2005
*/
template <> void print_matrix(const int& m, const int& n, const float* A, const int& lda) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::printf("%0.2f ", A[j * lda + i]);
        }
        std::printf("\n");
    }
}

template <> void print_matrix(const int& m, const int& n, const double* A, const int& lda) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::printf("%0.2f ", A[j * lda + i]);
        }
        std::printf("\n");
    }
}

template <> void print_matrix(const int& m, const int& n, const cuComplex* A, const int& lda) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::printf("%0.2f + %0.2fj ", A[j * lda + i].x, A[j * lda + i].y);
        }
        std::printf("\n");
    }
}

template <>
void print_matrix(const int& m, const int& n, const cuDoubleComplex* A, const int& lda) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::printf("%0.2f + %0.2fj ", A[j * lda + i].x, A[j * lda + i].y);
        }
        std::printf("\n");
    }
}

// Returns cudaDataType value as defined in library_types.h for the string containing type name
cudaDataType get_cuda_library_type(std::string type_string) {
    if (type_string.compare("CUDA_R_16F") == 0)
        return CUDA_R_16F;
    else if (type_string.compare("CUDA_C_16F") == 0)
        return CUDA_C_16F;
    else if (type_string.compare("CUDA_R_32F") == 0)
        return CUDA_R_32F;
    else if (type_string.compare("CUDA_C_32F") == 0)
        return CUDA_C_32F;
    else if (type_string.compare("CUDA_R_64F") == 0)
        return CUDA_R_64F;
    else if (type_string.compare("CUDA_C_64F") == 0)
        return CUDA_C_64F;
    else if (type_string.compare("CUDA_R_8I") == 0)
        return CUDA_R_8I;
    else if (type_string.compare("CUDA_C_8I") == 0)
        return CUDA_C_8I;
    else if (type_string.compare("CUDA_R_8U") == 0)
        return CUDA_R_8U;
    else if (type_string.compare("CUDA_C_8U") == 0)
        return CUDA_C_8U;
    else if (type_string.compare("CUDA_R_32I") == 0)
        return CUDA_R_32I;
    else if (type_string.compare("CUDA_C_32I") == 0)
        return CUDA_C_32I;
    else if (type_string.compare("CUDA_R_32U") == 0)
        return CUDA_R_32U;
    else if (type_string.compare("CUDA_C_32U") == 0)
        return CUDA_C_32U;
    else
        throw std::runtime_error("Unknown CUDA datatype");
}

// Returns cusolverIRSRefinement_t value as defined in cusolver_common.h for the string containing
// solver name
cusolverIRSRefinement_t get_cusolver_refinement_solver(std::string solver_string) {
    if (solver_string.compare("CUSOLVER_IRS_REFINE_NONE") == 0)
        return CUSOLVER_IRS_REFINE_NONE;
    else if (solver_string.compare("CUSOLVER_IRS_REFINE_CLASSICAL") == 0)
        return CUSOLVER_IRS_REFINE_CLASSICAL;
    else if (solver_string.compare("CUSOLVER_IRS_REFINE_GMRES") == 0)
        return CUSOLVER_IRS_REFINE_GMRES;
    else if (solver_string.compare("CUSOLVER_IRS_REFINE_CLASSICAL_GMRES") == 0)
        return CUSOLVER_IRS_REFINE_CLASSICAL_GMRES;
    else if (solver_string.compare("CUSOLVER_IRS_REFINE_GMRES_GMRES") == 0)
        return CUSOLVER_IRS_REFINE_GMRES_GMRES;
    else
        printf("Unknown solver parameter: \"%s\"\n", solver_string.c_str());

    return CUSOLVER_IRS_REFINE_NOT_SET;
}

