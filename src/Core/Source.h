#pragma once
#include "Common.h"
#include "ImageBuffer.h"

//class Sampler {
//	virtual Vec2 Sample(const Vec2& p, double r, std::mt19937& gen)
//	{
//		return SampleGreen(p, r, gen);
//	}
//};



struct PointSource {
	Vec2 pos;
	Vec3 source;
};


class SourceTerm
{
public:

	// For continous sources
	virtual Vec3 Source(const Vec2& p) const
	{
		return { 0., 0., 0. };
	}

	// Sample Source points used for reverse
	// pdf : the pdf of sampling this pointsource
	virtual PointSource SampleSource(std::mt19937& gen, double& pdf) const = 0;


	// Integrate Sphere according to source sampling
	virtual Vec3 IntegrateSphere(const Vec2& p, const double& r, std::mt19937& gen) const = 0;


	virtual bool HasSource() const { return false; }
	virtual bool IsDiscrete() const { return false; }
	virtual bool ShouldSampleGreen() const { return false; }
};


class EmptySource : public SourceTerm
{
public:
	PointSource SampleSource(std::mt19937& gen, double& pdf) const override
	{
		assert(false && "Shouldn't call here");
		return PointSource();
	}

	Vec3 IntegrateSphere(const Vec2& p, const double& r, std::mt19937& gen) const override
	{
		assert(false && "Shouldn't call here");
		return { 0.0, 0.0, 0.0 };
	}
};

// HACK add some background sources
// Contain several point sources
class DiscreteSource : public SourceTerm
{
public:
	DiscreteSource(vector<PointSource>& point_sources) : m_PointSources(point_sources) { }

	bool IsDiscrete() const override { return true; }
	bool HasSource() const override { return true; }

	PointSource SampleSource(std::mt19937& gen, double& pdf) const override 
	{
		// Uniform boundary
		/*double dx = 2.0 * (Rand01(gen) - 0.5) * ScreenSize;
		double dy = 2.0 * (Rand01(gen) - 0.5) * ScreenSize;
		Vec2 p{ dx, dy };
		pdf = 0.25;
		return PointSource{ p, Source(p) };*/
		
		int i = Rand01(gen) * m_PointSources.size();
		PointSource s = m_PointSources[i];
		pdf = 1 / m_PointSources.size();
		return s;
	}

	// HACK Add a wave 
	Vec3 Source(const Vec2& p) const override
	{
		double r = (p - Vec2{0.5, 0.0}).norm();
		double b = (p + Vec2{ 0.5, 0.0 }).norm();

		double intensity = 10.0;

		int T = 16;
		int R = 2 * (int(T * r) % 2) - 1;
		int B = 2 * (int(T * b) % 2) - 1;

		return { intensity * R, 0., intensity * B};
	}

	Vec3 IntegrateSphere(const Vec2& p, const double& r, std::mt19937& gen) const override
	{
		Vec3 ans = {0.0, 0.0, 0.0};
		for (auto& v : m_PointSources) {
			double l = (p - v.pos).norm();
			if (l < r) {
				ans += Green(r, l) * v.source;
			}
		}

		/*SamplePoint source_point = SampleGreen(p, r, gen);
		double pdf = source_point.pdf;
		double l = (p - source_point.pos).norm();
		auto v = Green(r, l) * Source(source_point.pos) / pdf;*/

		return ans;
	}

private:
	vector<PointSource> m_PointSources;
};


// Contain several disks
class DiskSource : public SourceTerm
{
public:
	struct SphereSource {
		Vec2 c;
		double r;
		Vec3 source;

		bool contain(Vec2 p) const { return (p - c).norm() < r; }
			
		double area() const { return PI * r * r;  }
			
		double energy() const { return ToGrey(source) * area(); }

		double IntersectArea(Vec2 c2, double r2) const {
			// https://mathworld.wolfram.com/Circle-CircleIntersection.html
			const Vec2& c1 = c;
			const double r1 = r;

			double d = (c1 - c2).norm();
			if (d >= r1 + r2) { return 0.0; }
			if (r1 - r2 > d) { return volume(r2); }
			if (r2 - r1 > d) { return volume(r1); }


			return r1 * r1 * acos((d * d + r1 * r1 - r2 * r2) / (2.0 * d * r1))
				+ r2 * r2 * acos((d * d + r2 * r2 - r1 * r1) / (2.0 * d * r2))
				- 0.5 * sqrt((-d + r1 + r2) * (d + r1 - r2) * (d - r1 + r2) * (d + r1 + r2));

		}
	};

	DiskSource(vector<SphereSource>& spheres) : m_SphereSources(spheres)
	{
		m_TotalSourceArea = 0;
		m_TotalEnergy = 0;
		for (const auto& s : m_SphereSources) {
			double energy = s.energy();
			m_Energy.push_back(energy);

			m_TotalSourceArea += s.area();
			m_TotalEnergy += energy;
		}
	}

	bool IsDiscrete() const override { return false; }
	bool HasSource() const override { return true; }

	Vec3 Source(const Vec2& p) const override {
		Vec3 source = { 0.0, 0.0, 0.0 };
		for (const auto& s : m_SphereSources) {
			if (s.contain(p)) { source += s.source; }
		}
		return source;
	}

	// This sample point source based on the energy of each sphere
	PointSource SampleSource(std::mt19937& gen, double& pdf) const override
	{
		double sample_energy = Rand01(gen) * m_TotalEnergy;

		int sample_source = 0;
		while (true) {
			sample_energy -= m_Energy[sample_source];
			if (sample_energy < 0.0) { break; }
			sample_source++;
		}

		const Vec2& c_source = m_SphereSources[sample_source].c;
		const double r_source = m_SphereSources[sample_source].r;
		const Vec3& v_source = m_SphereSources[sample_source].source;

		SamplePoint sample_p = Uniform_Ball(c_source, r_source, gen);

		pdf = ToGrey(v_source) / m_TotalEnergy;
		PointSource ps{ sample_p.pos, m_SphereSources[sample_source].source };
		return ps;
	}

	// Integrate sphere according to source sampling
	Vec3 IntegrateSphere(const Vec2& p, const double& r, std::mt19937& gen) const override
	{
		double pdf;
		PointSource ps = SampleSource(p, r, pdf, gen);
		if (pdf > 0.0) {
			double g = 0.0;
			double l = (ps.pos - p).norm();
			if (l < r) {
				g = Green(r, l);
			}
			return g * ps.source / pdf;
		} 
		return {0.0, 0.0, 0.0};
	}

private:
	PointSource	SampleSource(const Vec2& c, const double& r, double& pdf, std::mt19937& gen) const
	{
		const int N = m_SphereSources.size();
		vector<double> IntersectArea(N, 0); // instead of area, using total energy
		double totalArea = 0.0;
		for (int i = 0; i < N; ++i) {
			const auto& s = m_SphereSources[i];
			IntersectArea[i] = s.IntersectArea(c, r) * ToGrey(s.source);
			totalArea += IntersectArea[i];
		}

		double sample_area = Rand01(gen) * totalArea;
		// no intersection, just return a point outside the sphere
		if (totalArea <= 0.0) { return { {c[0] + r, c[1] + r}, {0.0, 0.0, 0.0} }; }

		int sample_source = 0;
		while (true) {
			sample_area -= IntersectArea[sample_source];
			if (sample_area < 0.0) { break; }
			sample_source++;
		}

		const Vec2& c_source = m_SphereSources[sample_source].c;
		const double r_source = m_SphereSources[sample_source].r;

		// Do rejection sampling
		int i = 0;
		SamplePoint p = Uniform_Ball(c_source, r_source, gen);
		while ((p.pos - c).norm() > r) {
			p = Uniform_Ball(c_source, r_source, gen);
			i++;
			if (i > 1000) { p.pdf = 0.0; break; } // The area is too small
		}

		PointSource sampled_source{ {0.0, 0.0}, {0.0, 0.0, 0.0} };
		if (p.pdf != 0.0) {
			sampled_source = PointSource{ p.pos, m_SphereSources[sample_source].source };
			pdf = ToGrey(m_SphereSources[sample_source].source) / totalArea;
		}
			
		return sampled_source;
	}

	double m_TotalEnergy;
	double m_TotalSourceArea;
	vector<SphereSource> m_SphereSources;
	vector<double> m_Energy;
};
	
//class LineSource : public SourceTerm
//{
//	// :( the 3rd code about line... should reuse code with Boundary...
//public:
//	LineSource(std::vector<Vec2>& Points, std::vector<int>& Index, std::vector<Vec3>& colors)
//		:m_Points(Points), m_Index(Index), m_Colors(colors)
//	{
//		
//	}
//
//	DiscreteSource ChangeToPointSources(int samples)
//	{
//	
//	}
//private:
//	std::vector<Vec2> m_Points;
//	std::vector<Vec3> m_Colors;
//	std::vector<int>  m_Index;
//};

class ImageSource : public SourceTerm
{
public:
	ImageSource(string file)
		: m_img(file)
	{

		m_Energy = 0.0;
		for (int i = 0; i < m_img.res; ++i) {
			for (int j = 0; j < m_img.res; ++j) {
				if (ToGrey(m_img.buffer[i][j]) >= 0.8)
				{
					//m_img.buffer[i][j] = { 0.0, 0.0, 0.0 };
				}
				m_Energy += ToGrey(m_img.buffer[i][j]);
			}
		}
		m_img.SaveImagePFM(WorkDirectory + "imageSource.pfm");
	}

	bool IsDiscrete() const override { return false; }

	bool HasSource() const override { return true; }

	Vec3 Source(const Vec2& p) const override 
	{
		return m_img.GetPixel(p);
	}

	// This sample point source based on the energy of each sphere
	PointSource SampleSource(std::mt19937& gen, double& pdf) const override
	{
		double e = Rand01(gen) * m_Energy;
		
		PointSource ps;
		for (int i = 0; i < m_img.res; ++i) {
			for (int j = 0; j < m_img.res; ++j) {
				e -= ToGrey(m_img.buffer[i][j]);
				if (e < 0)
				{
					pdf = ToGrey(m_img.buffer[i][j]) / m_Energy;
					ps.pos = m_img.img2world({i, j});
					ps.source = m_img.buffer[i][j];
					break;
				}
				
			}
		}
		return ps;
	}

	// Integrate sphere according to source sampling
	Vec3 IntegrateSphere(const Vec2& pos, const double& r, std::mt19937& gen) const override
	{
		Vec2i coor = m_img.world2img(pos);
		Vec3 ans{ 0.0, 0.0, 0.0 };
		int pixel_r = int(r / m_img.pixel_size + 1);
		for (int x = coor[0] - pixel_r; x <= coor[0] + pixel_r; ++x) {
			for (int y = coor[1] - pixel_r; y <= coor[1] + pixel_r; ++y) {
				Vec2 p = m_img.img2world({ x, y });
				Vec3 source = m_img.buffer[x][y];
				double l = (p - pos).norm();
				if (l > r) {
					continue;
				}
				double g = Green(r, l);
				ans += g * source;
			}
		}
		return ans;
	}

	ImageBuffer m_img;
	double m_Energy = 0.0;
};
