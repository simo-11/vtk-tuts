/*
Proxy layer methods for 
generating and solving equations using intel mkl and cuda.
*/
#include <stdlib.h>
#include <stdio.h>
#define CUBLASAPI
#include "cusolver_utils.h"
#include "MathExample.h"

void print_matrix(const char* desc,
    const int& m, const int& n,
    const float* A, const int& lda) {
    printf("\n%s=[\n", desc);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::printf("%s% 11.7g",(j==0?"":", "),A[j * lda + i]);
        }
        std::printf("\n");
    }
    printf("]\n");
}

float* getRandomA(int n) {
    float* a;
    int lda;
    float* fixed=nullptr;
    float src1[1 * 1] = { 3.f };
    float src5[5 * 5] = { // stored columnwise
         6.80f, -2.11f, 5.66f, 5.97f, 8.23f,
        -6.05f, -3.30f, 5.36f,-4.44f, 1.08f,
        -0.45f,  2.58f,-2.70f, 0.27f, 9.04f,
         8.32f,  2.71f, 4.35f,-7.17f, 2.14f,
        -9.67f, -5.14f,-7.26f, 6.08f,-6.87f
    };
    switch (n) {
    case 1:
    {
        fixed = src1;
    }
    break;
    case 5:
        fixed = src5;
        break;
    default:
        generate_random_matrix<float>(n, n, &a, &lda);
        make_diag_dominant_matrix<float>(n, n, a, lda);
        break;
    }
    if (fixed != nullptr) {
        size_t size = n * n * sizeof(float);
        a = (float*)malloc(size);
        if (a != nullptr) {
            memcpy(a, fixed, size);
        }
    }
    return a;
}
float* getRandomB(int n) {
    float* b;
    int ldb;
    float* fixed = nullptr;
    float src1[1] = { 0.9 };
    float src5[5] = {
        4.02f,
        6.19f,
       -8.22f,
       -7.57f,
       -3.03f,
    };
    switch (n) {
    case 1:
        fixed = src1;
        break;
    case 5:
        fixed = src5;
        break;
    default:
        generate_random_matrix<float>(n, 1, &b, &ldb);
    }
    if (fixed != nullptr) {
        size_t size = n *  sizeof(float);
        b = (float*)malloc(size);
        if (b != nullptr) {
            memcpy(b, fixed, size);
        }
    }
    return b;
}
/*
@return info from solver
*/
int MathSolve(int verbose, SolverImpl impl, int n, float* a, float* b, Workspaces* ws) {
    int info = 0;
    /* Solve the equations A*X = B */
    if (verbose>1) {
        /* Print Problem */
        print_matrix("A", n, n, a, n);
        print_matrix("B", n, 1, b, n);
    }
    switch (impl) {
    case mkl:
        info = lapack_solve(verbose, n, a, b, ws);
        break;
    case cuda:
        info = cuda_solve(verbose, n, a, b, ws);
    }
    float* expected = nullptr;
    float result1[1] = { 0.3 };
    float result5[5] = { -0.80,-0.70,0.59,1.32,0.57 };
    switch (n) {
    case 1:
        expected = result1;
        break;
    case 5:
        expected = result5;
        break;
    }
    if (expected!=nullptr) {
        for (int i = 0; i < n; i++) {
            float rd = b[i] / expected[i];
            if (rd < 0.99 || rd>1.01) {
                printf("Solution failed, for x(%i) expected %4.2f but got %4.2f\n",
                    (i+1),expected[i], b[i]);
                info = 99;
                break;
            }
        }
    }
    if (verbose>1) {
        /* Print solution */
        print_matrix("X", n, 1, b, 1);
    }
    return info;
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

