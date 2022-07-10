#pragma once
#include "Core/SolverWoS.h"
#include "Core/Scene.h"

class ForwardWoS : public Solver
{
public:
	int PixelSamples = 1;
	bool ImportanceSampleSource = true;

	string GetName() override { 
		std::stringstream s1;
		s1 << std::fixed << std::setprecision(1) << std::log10(m_Epsilon);
		std::string ep = s1.str();
		return "ForwardWoS"+ string("_Epsilon") + ep + string("_Spp") + to_string(PixelSamples);
	}

	void Solve(const PoissonEquation& equation, ImageBuffer* buffer, int id) override
	{
		std::mt19937 gen(GetThreadSeed(id));
		for (int i = 0; i < buffer->res; ++i) {
			for (int j = 0; j < buffer->res; ++j) {
			
				// Ignore the pixel extirly outside the domain
				if(equation.InteriorOnly()){
					Vec2 pos = buffer->img2world({ i, j });
					ClosePoint cp = equation.Boundary->GetClosestPoint(pos);
					if (!equation.Boundary->CheckInterior(pos) && cp.distance > 2.0 * buffer->pixel_size)
					{
						continue;
					}
				}

				for (int sample = 0; sample < PixelSamples; ++sample) {
					Vec2 pos = buffer->img2world({ i, j });
#ifdef UniformInPixel
					double dx = (Rand01(gen) - 0.5) * buffer->pixel_size;
					double dy = (Rand01(gen) - 0.5) * buffer->pixel_size;
					pos = pos + Vec2{ dx, dy };
#endif // UniformInPixel 1;

					if(equation.InteriorOnly()){
						if (!equation.Boundary->CheckInterior(pos)) {
							continue;
						}
					}
					WoSSinglePoint(pos, equation, buffer, gen);
				}
			}
		}
	}

	void WoSSinglePoint(const Vec2& pixel_pos, const PoissonEquation& equation, 
					    ImageBuffer* buffer, std::mt19937& gen)
	{
		Vec2 p = pixel_pos;
		ptrSourceTerm source = equation.Source;
		ptrBoundary boundary = equation.Boundary;

		Vec3 v{0.0, 0.0, 0.0};

		for (int i = 0; i < MaxPathLength; ++i)
		{
			ClosePoint cp = boundary->GetClosestPoint(p);
			double r = cp.distance;

			if (r < m_Epsilon || r > 5.0 * ScreenSize)
			{
				v += boundary->BoundaryValue(p); // need to use p because we need the side of the boundary value
				buffer->WritePixel(pixel_pos, v / PixelSamples);
				
				break;
			}

			if (source->HasSource())
			{	
				if(ImportanceSampleSource) {
					v += source->IntegrateSphere(p, r, gen);
				} else {
					// Importance sample Green's function
					SamplePoint source_point = SampleGreen(p, r, gen);
					double pdf = source_point.pdf;
					double l = (p - source_point.pos).norm();
					v += Green(r, l) * source->Source(source_point.pos) / pdf;
				}
			}
			SamplePoint sp = Uniform_dB(p, r, gen);
			p = sp.pos;
		}
	}
};
