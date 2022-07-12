#pragma once
#include "ReverseWoS.h"

class FinalGatherSolver : public ReverseWoSSolver
{
public:
	int forward_PixelSamples = 32;

	void Solve(const PoissonEquation& equation, ImageBuffer* buffer, int id) override
	{
		ImageBuffer reverseResult(*buffer);
		std::mt19937 gen(GetThreadSeed(id));

		// First run the reverse WoS and store directly into an image
		if (equation.Boundary->HasBoundaryValue())
		{
			for (int sample = 0; sample < BoundarySamples; ++sample)
			{
				BoundarySinglePoint(equation, &reverseResult, gen);
			}
		}

		if (equation.Source->HasSource()) {
			for (int sample = 0; sample < SourceSamples; ++sample)
			{
				SourceSinglePoint(equation, &reverseResult, gen);
			}
		}

		// Run forward pass
		for (int i = 0; i < buffer->res; ++i) {
			for (int j = 0; j < buffer->res; ++j) {
				for (int sample = 0; sample < forward_PixelSamples; ++sample) {
					Vec2 pos = buffer->img2world({ i, j });
#ifdef UniformInPixel
					double dx = (Rand01(gen) - 0.5) * buffer->pixel_size;
					double dy = (Rand01(gen) - 0.5) * buffer->pixel_size;
					pos = pos + Vec2{ dx, dy };
#endif // UniformInPixel

					if (equation.InteriorOnly()) {
						if (!equation.Boundary->CheckInterior(pos)) {
							continue;
						}
					}
					FinalGather(pos, equation, buffer, reverseResult, gen);
				}
			}
		}
	}

	// Choose what rules to use in the forward walk
	virtual void FinalGather(const Vec2& pixel_pos, const PoissonEquation& equation,
		ImageBuffer* buffer, const ImageBuffer& reverseResult, std::mt19937& gen) = 0;
};


class FinalGatherPoissonKernelThreshold : public FinalGatherSolver
{
public:
	double LookUpThreshold = 1.0;

	string GetName() override { return "finalGather"; }

	void FinalGather(const Vec2& pixel_pos, const PoissonEquation& equation,
		ImageBuffer* buffer, const ImageBuffer& reverseResult, std::mt19937& gen) override
	{
		Vec2 p = pixel_pos;
		ptrSourceTerm source = equation.Source;
		ptrBoundary boundary = equation.Boundary;

		Vec3 v{ 0.0, 0.0, 0.0 };

		for (int i = 0; i < MaxPathLength; ++i)
		{
			ClosePoint cp = boundary->GetClosestPoint(p);
			double r = cp.distance / 2;

			if (r < m_Epsilon)
			{
				v += boundary->BoundaryValue(p);
				buffer->WritePixel(pixel_pos, v / forward_PixelSamples);
				return;
			}

			// Lookup reverse table if we are far away
			if (reverseResult.GetPoissonKerynelEstimate(p) > LookUpThreshold)
			{
				Vec3 reverseWoS = reverseResult.GetPixel(p) / reverseResult.GetPoissonKerynelEstimate(p);
				buffer->WritePixel(pixel_pos, (v + reverseWoS) / forward_PixelSamples);
				return;
			}

			if (source->HasSource())
			{
				v += source->IntegrateSphere(p, r, gen);
			}

			SamplePoint sp = Uniform_dB(p, r, gen);
			p = sp.pos;
		}

		// if we did not hit the boundary yet
		Vec3 reverseWoS = reverseResult.GetPixel(p);
		buffer->WritePixel(pixel_pos, (v + reverseWoS) / forward_PixelSamples);
	}
};
