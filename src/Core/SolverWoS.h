#pragma once
#include "PDE.h"
#include "ImageBuffer.h"

constexpr bool UseSameSeed = true;

void ReleativeSD(string prefixname, int start, int end) 
{
	// Calculate avg
	ImageBuffer temp(prefixname + to_string(start) + ".pfm");
	int resolution = temp.res;
	ImageBuffer avg(resolution);
	ImageBuffer var(resolution);
	ImageBuffer rsd(resolution);

	// Avg
	for (int i = start; i <= end; ++i)
	{
		ImageBuffer temp(prefixname + to_string(i) + ".pfm");
		for (int i = 0; i < resolution; ++i) {
			for (int j = 0; j < resolution; ++j) {
				avg.buffer[i][j] += temp.buffer[i][j];
			}
		}
	}

	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			avg.buffer[i][j] /= (end - start + 1);
		}
	}
	avg.SaveImagePFM(prefixname + "Avg" + ".pfm");

	// Variance
#ifdef CALCULATE_VARIANCE
	for (int i = start; i <= end; ++i)
	{
		ImageBuffer temp(prefixname + to_string(i) + ".pfm");
		for (int i = 0; i < resolution; ++i) {
			for (int j = 0; j < resolution; ++j) {
				Vec3 t = temp.buffer[i][j] - avg.buffer[i][j];
				var.buffer[i][j] += Vec3{ t[0] * t[0], t[1] * t[1], t[2] * t[2] };
			}
		}
	}

	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			var.buffer[i][j] /= (end - start);
		}
	}

	// Releative standard deviation
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			const auto& v = var.buffer[i][j];
			const auto& a = avg.buffer[i][j];
			rsd.buffer[i][j] = Vec3{ sqrt(v[0]) / a[0], sqrt(v[1]) / a[1] , sqrt(v[2]) / a[2] };
		}
	}
	var.SaveImagePFM(prefixname + "Var" + ".pfm");
	rsd.SaveImagePFM(prefixname + "RSD" + ".pfm");
#endif // CALCULATE_VARIANCE

#ifndef KEEP_THREAD_RESULTS
	// Remove results from all threads
	for (int i = start; i <= end; ++i)
	{	
		string file_name = prefixname + to_string(i) + ".pfm";
		std::remove(file_name.c_str());
	}
#endif
}

class DrawPDE
{
public:
	static constexpr int Samples = 16;
	static constexpr double m_DrawBoundaryEpsilon = 0.8 * 1e-2;

	static void Draw(const PoissonEquation& equation)
	{

		ptrSourceTerm source = equation.Source;
		ptrBoundary boundary = equation.Boundary;
		ImageBuffer buffer(DefaultResolution);
		std::mt19937 gen(0);


		for (int i = 0; i < buffer.res; ++i) {
			for (int j = 0; j < buffer.res; ++j) {
				for (int sample = 0; sample < Samples; ++sample) {

					Vec2 pos = buffer.img2world({ i, j });
					double dx = (Rand01(gen) - 0.5) * buffer.pixel_size;
					double dy = (Rand01(gen) - 0.5) * buffer.pixel_size;
					pos = pos + Vec2{ dx, dy };
					ClosePoint cp = equation.Boundary->GetClosestPoint(pos);

					// boundary
					double r = cp.distance;
					if (r < m_DrawBoundaryEpsilon)
					{
						buffer.WritePixel(pos, boundary->BoundaryValue(pos) / Samples);
					}
					else if(source->HasSource())
					{
						buffer.WritePixel(pos, source->Source(pos) / Samples);
					}
					else 
					{
						// Put white in the rest area
						buffer.WritePixel(pos, Vec3(1.0, 1.0, 1.0) / Samples);
					}
				}
			}
		}
		buffer.SaveImagePFM(WorkDirectory + "PDE.pfm");
	}

};

class Solver {
public:
	double m_Epsilon = 1e-6;
	double m_Xi      = 1e-3;

	static constexpr int MaxPathLength = 1000;
	int m_CurrentBatch = 0;

	void SolveMultiThread(const PoissonEquation& equation, int numberThread = 16, int batch = 1)
	{
		string name = WorkDirectory + GetName();
		
		double time_used = 0;

		for (m_CurrentBatch = 0; m_CurrentBatch < batch; ++m_CurrentBatch) {
			auto start = std::chrono::system_clock::now();
			std::vector<ImageBuffer> bufferList(numberThread, ImageBuffer(DefaultResolution));
			std::vector<std::thread> threadList(numberThread);

			for (int id = 0; id < numberThread; ++id)
			{
				ImageBuffer* buffer = &(bufferList[id]);
				threadList[id] = std::thread([this, equation, buffer, id] { this->Solve(equation, buffer, id); });
			}
			
			for (auto& t : threadList)
			{
				t.join();
			}
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			std::cout << "Finish batch: " << m_CurrentBatch << " elapsed time: " << elapsed_seconds.count() << std::endl;
			time_used += elapsed_seconds.count();

			Log();

			for (int id = 0; id < numberThread; ++id)
			{
				auto& img = bufferList[id];
				img.SaveImagePFM(name + to_string(m_CurrentBatch * numberThread + id) + ".pfm");
			}
			
		}
		ReleativeSD(name, 0, batch * numberThread - 1);
		
		// Add time to the file name
		ImageBuffer avg(name + "Avg" + ".pfm");

		// https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
		std::stringstream s1;
		s1 << std::fixed << std::setprecision(2) << time_used;
		std::string t = s1.str();
		avg.SaveImagePFM(name + "_Time" + t + "s.pfm");
		std::remove((name + "Avg" + ".pfm").c_str());
	}

	virtual void Solve(const PoissonEquation& equation, ImageBuffer* buffer, int id) = 0;

	virtual string GetName() { return "VirtualClass"; }
	virtual void Log() { }
	
	int GetThreadSeed(int id) {
		if (UseSameSeed) { 
			return id * 173 + 1363 * (m_CurrentBatch + 1);
		} else {
			int seed = rd();
			return seed;
		}
	}

	
};