#pragma once
#include "Core/SolverWoS.h"
#include "Core/Scene.h"


class ReverseWoSSolver : public Solver
{
public:
	bool Normalize_PoissonKernel = false;
	int SourceSamples = 1e5;
	int BoundarySamples = 1e5;

	string GetName() override 
	{ 
		// https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
		std::stringstream s1;
		s1 << std::fixed << std::setprecision(1) << std::log10(m_Epsilon);
		std::string ep = s1.str();

		std::stringstream s2;
		s2 << std::fixed << std::setprecision(1) << std::log10(m_Xi);
		std::string xi = s2.str();

		std::stringstream s3;
		s3 << std::fixed << std::setprecision(1) << std::log10(BoundarySamples);
		std::string sample = s3.str();

		return "./Reverse" + 
			string("_Epsilon") + ep + 
			string("_Xi") + xi +
			string("_Samples") + sample;
	}

	void Solve(const PoissonEquation& equation, ImageBuffer* buffer, int id) override
	{
		std::mt19937 gen(GetThreadSeed(id));

		// Handle the source term
		if (equation.Source->HasSource()) {
			for (int sample = 0; sample < SourceSamples; ++sample)
			{
				SourceSinglePoint(equation, buffer, gen);
			}
		}

		
		// Handle the boundary value
		if (equation.Boundary->HasBoundaryValue())
		{
			for (int sample = 0; sample < BoundarySamples; ++sample)
			{
				BoundarySinglePoint(equation, buffer, gen);
			}

			if(Normalize_PoissonKernel) {
				for (int i = 0; i < buffer->res; ++i) {
					for (int j = 0; j < buffer->res; ++j) {
						if (buffer->PoissonKernelNormalizer[i][j] != 0.0) {
							buffer->buffer[i][j] /= buffer->PoissonKernelNormalizer[i][j];
						}
					}
				}
			}

		}
	}

	void SourceSinglePoint(const PoissonEquation& equation, ImageBuffer* buffer, std::mt19937& gen);

	void BoundarySinglePoint(const PoissonEquation& equation, ImageBuffer* buffer, std::mt19937& gen);
};


void ReverseWoSSolver::SourceSinglePoint(const PoissonEquation& equation, ImageBuffer* buffer, std::mt19937& gen)
{
	ptrSourceTerm source = equation.Source;
	ptrBoundary boundary = equation.Boundary;

	double pdf;
	PointSource pointsource = source->SampleSource(gen, pdf);

	Vec2 p = pointsource.pos;
	Vec3 v = pointsource.source;
	for (int i = 0; i < MaxPathLength; ++i)
	{
		ClosePoint cp = boundary->GetClosestPoint(p);
		double r = cp.distance;

		if (r < m_Epsilon)
		{
			break;
		}

		// Reuse the same walk to all pixels by drawing the Green's disk
		buffer->DrawGreenSphere(p, r, v / pdf / SourceSamples, gen);


		// Continue the walk
		SamplePoint sp = Uniform_dB(p, r, gen);
		p = sp.pos;
	}
}

void ReverseWoSSolver::BoundarySinglePoint(const PoissonEquation& equation, ImageBuffer* buffer, std::mt19937& gen)
{
	ptrSourceTerm source = equation.Source;
	ptrBoundary boundary = equation.Boundary;

	double pdf;
	BoundaryVauleSample boundarysample = boundary->SampleBoundary(gen, pdf);

	Vec2 dp = m_Xi * boundarysample.normal;
	Vec2 p;

	if(equation.InteriorOnly()) {
		p = boundarysample.pos - dp;
	} else {
		if (Rand01(gen) > 0.5) {
			p = boundarysample.pos + dp;
		}
		else {
			p = boundarysample.pos - dp;
		}
	}

	Vec3 v = boundarysample.bv;
	for (int i = 0; i < MaxPathLength; ++i)
	{ 
		ClosePoint cp = boundary->GetClosestPoint(p);
		double r = cp.distance;

		if (r < m_Epsilon || r > 5.0 * ScreenSize)
		{
			break;
		}

		// Divide Xi for the finite difference method
		double weight = 1.0 / pdf / (m_Xi * BoundarySamples);

		// Reuse the same walk to all pixels by drawing the Green's disk
		buffer->DrawGreenSphere(p, r, weight, v, gen);
		
		// Continue the walk
		SamplePoint sp = Uniform_dB(p, r, gen);
		p = sp.pos;
	}
}
