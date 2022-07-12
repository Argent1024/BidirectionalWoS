#include "Core/Scene.h"
#include "ReverseWoS.h"
#include "ForwardWoS.h"

void RunSourceCompare(bool forward, int number_thread)
{
	PoissonEquation equation = SourceScene();
	ReverseWoSSolver reverseSolver;
	ForwardWoSSolver forwardSolver;

	reverseSolver.SourceSamples = 1e4;
	forwardSolver.PixelSamples = 1;
	
	cout << "begin reverse wos" << endl;
	reverseSolver.SolveMultiThread(equation, number_thread);

	if (forward) {
		cout << "begin forward wos" << endl;
		forwardSolver.SolveMultiThread(equation, number_thread);
	}
}


void RunBoundaryCompare(bool forward, int number_thread)
{
	PoissonEquation equation = BoundaryScene(true);
	ReverseWoSSolver reverseSolver;
	ForwardWoSSolver forwardSolver;

	reverseSolver.BoundarySamples = 2e5;
	forwardSolver.PixelSamples = 1;

	cout << "begin reverse wos" << endl;
	reverseSolver.SolveMultiThread(equation, number_thread);
	if (forward) {
		cout << "begin forward wos" << endl;
		forwardSolver.SolveMultiThread(equation, number_thread);
	}
}

int main()
{
	// RunSourceCompare(true, 16);
	RunBoundaryCompare(true, 16);
}