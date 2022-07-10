#include "ReverseWoS.h"

int main()
{
	// PoissonEquation equation = ObjCurve(WorkDirectory + "curve.obj");
	// PoissonEquation equation = ObjCurveWithBV(WorkDirectory + "curve1.obj", false);
	// PoissonEquation equation = BugDiffusionCurve();
	// PoissonEquation equation = SinglePointSourceSphere();
	// PoissonEquation equation = FinalGatherScene();
	// PoissonEquation equation = NarrowTunnelLeft();
	// PoissonEquation equation = SinCurve();
	// PoissonEquation equation = RoundTriangle();

	// DrawrPDE::Draw(equation);
	PoissonEquation equation = ObjCurve();
	ReverseWoSSolver solver;
	solver.SolveMultiThread(equation, 16, 1);	
}