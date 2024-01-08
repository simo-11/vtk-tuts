#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include <cuda_runtime.h>
#include <cusolverDn.h>

#include "cusolver_utils.h"
#include "MathExample.h"
enum errorCode { HX_ALLOCATE = 1 };
int cuda_allocate(Workspaces* ws) {
    int n = ws->n;
    typedef float T;
    ws->hx= (T*)malloc(n * n * sizeof(T));
    if (ws->hx == nullptr) {
        return HX_ALLOCATE;
    }
    const int nrhs = 1;
    const cusolverPrecType_t matrix_precision = CUSOLVER_R_32F;
    // make sure that you specify matrix precision that matches to the data type
    assert(traits<T>::cusolver_precision_type == matrix_precision);
    const cusolverPrecType_t compute_lower_precision = CUSOLVER_R_16F;
    // Use GMRES refinement solver
    const cusolverIRSRefinement_t refinement_solver = CUSOLVER_IRS_REFINE_GMRES;
    CUDA_CHECK(cudaStreamCreateWithFlags(&ws->stream, cudaStreamNonBlocking)); // 112 ms
    CUSOLVER_CHECK(cusolverDnCreate(&ws->handle)); // 62 ms
    CUSOLVER_CHECK(cusolverDnSetStream(ws->handle, ws->stream));
    // create solver parameters
    CUSOLVER_CHECK(cusolverDnIRSParamsCreate(&ws->gesv_params));
    // set matrix precision and factorization precision
    CUSOLVER_CHECK(cusolverDnIRSParamsSetSolverPrecisions
        (ws->gesv_params, matrix_precision,
        compute_lower_precision));
    // set refinement solver
    CUSOLVER_CHECK(cusolverDnIRSParamsSetRefinementSolver
        (ws->gesv_params, refinement_solver));
    // create solve info structure
    CUSOLVER_CHECK(cusolverDnIRSInfosCreate(&ws->gesv_info));
    ws->ldda = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    ws->lddb = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    ws->lddx = ALIGN_TO(n * sizeof(T), device_alignment) / sizeof(T);
    CUDA_CHECK(cudaMalloc(&ws->dA, ws->ldda * n * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&ws->dB, ws->lddb * nrhs * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&ws->dX, ws->lddx * nrhs * sizeof(T)));
    CUDA_CHECK(cudaMalloc(&ws->dinfo, sizeof(cusolver_int_t)));
    // get required device work buffer size
    CUSOLVER_CHECK(cusolverDnIRSXgesv_bufferSize(ws->handle, ws->gesv_params, 
        n, nrhs, &ws->dwork_size));
    CUDA_CHECK(cudaMalloc(&ws->dwork, ws->dwork_size));
    return 0;
}
int cuda_free(Workspaces* ws) {
    if (ws->hx != nullptr) {
        free(ws->hx);
    }
    ws->hx = nullptr;
    CUDA_CHECK(cudaFree(ws->dwork));
    CUDA_CHECK(cudaFree(ws->dinfo));
    CUDA_CHECK(cudaFree(ws->dX));
    CUDA_CHECK(cudaFree(ws->dB));
    CUDA_CHECK(cudaFree(ws->dA));
    CUSOLVER_CHECK(cusolverDnDestroy(ws->handle));
    CUDA_CHECK(cudaStreamDestroy(ws->stream));
    return 0;
}
namespace {
    char eb[120];
}
const char* get_cuda_error_reason(int n) {
    switch (n) {
    case HX_ALLOCATE: return "HX_ALLOCATE";
    default:
        snprintf(eb, sizeof(eb), "Uknown code %d, add to %s", n, __FILE__);
        return eb;
    }
}


int cuda_solve(int verbose, int n, float* a, float* b, Workspaces* ws) {
    // Numer of right hand sides
    const int nrhs = 1;
    typedef float T;
    cusolver_int_t lda=n;
    cusolver_int_t ldb=n;
    // copy input data
    size_t dpitch = ws->ldda * sizeof(T);
    size_t spitch = lda * sizeof(T);
    size_t width = n * sizeof(T);
    size_t height = n;
    CUDA_CHECK(cudaMemcpy2D(ws->dA, dpitch, a, spitch, width, height,
        cudaMemcpyDefault));
    dpitch = ws->lddb * sizeof(T);
    spitch = ldb * sizeof(T);
    height = nrhs;
    CUDA_CHECK(cudaMemcpy2D(ws->dB, dpitch, b, spitch, width,height,
        cudaMemcpyDefault));
    cusolverStatus_t gesv_status =
        cusolverDnIRSXgesv(ws->handle, ws->gesv_params, ws->gesv_info, n, nrhs, 
            ws->dA, ws->ldda, ws->dB, ws->lddb, ws->dX, ws->lddx,
                           ws->dwork, ws->dwork_size, &ws->iter, ws->dinfo); // 138 ms
    CUSOLVER_CHECK(gesv_status);
    // check solve status
    int info = 0;
    CUDA_CHECK(
        cudaMemcpyAsync(&info, ws->dinfo, 
            sizeof(cusolver_int_t), cudaMemcpyDeviceToHost, ws->stream));
    CUDA_CHECK(cudaStreamSynchronize(ws->stream));
    CUDA_CHECK(cudaMemcpy2D(b, n * sizeof(T), ws->dX, 
            ws->lddx * sizeof(T), n * sizeof(T), nrhs,
                            cudaMemcpyDefault));
    CUDA_CHECK(cudaGetLastError());
    return 0;
}
