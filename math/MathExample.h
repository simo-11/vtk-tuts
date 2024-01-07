#pragma once
enum SolverImpl {mkl,cuda};
struct Workspaces {
	int n;
	/* for mkl */
	int* ipiv;
	/* for cuda */
	float* hx;
};
extern float* getRandomA(int n);
extern float* getRandomB(int n);
extern int MathSolve(int verbose,SolverImpl impl, int n, float* a, float* b, 
	Workspaces* ws);
extern int lapack_allocate(Workspaces* ws);
extern int lapack_free(Workspaces* ws);
extern int lapack_solve(int verbose, int n, float* a, float* b, Workspaces* ws);
extern const char* get_lapack_error_reason(int n);
extern int cuda_allocate(Workspaces* ws);
extern int cuda_free(Workspaces* ws);
extern int cuda_solve(int verbose, int n, float* a, float* b, Workspaces* ws);
extern const char* get_cuda_error_reason(int n);

class vtkObject;

class MathExampleUI
{
public:
	static void setup(vtkObject* caller, unsigned long eventId, 
			void* clientData, void* callData);
	static void draw(vtkObject* caller, unsigned long eventId, 
		void* clientData, void* callData);
};
