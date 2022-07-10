#pragma once
#include "Common.h"
#include <fcpw/fcpw.h>

#define SINGLE_SIDE 1
// #define MIX_COLOR 1

struct BoundaryVauleSample
{
	Vec2 pos;
	Vec2 normal;
	Vec3 bv;
};

struct BoundaryData
{
	std::vector<Vec2> points;
	std::vector<int>  index;
};

class Boundary {
public: 
	static std::shared_ptr<Boundary> GetImageBoundary()
	{
		double x = 1.0 * ScreenSize;

		vector<Vec2> points = {
			{x, -x}, {x, x}, {-x, x}, {-x, -x}
		};

		return make_shared<Boundary>(points);
	}

	static BoundaryData ReadObjCurve(string file);

	Boundary(std::vector<Vec2>& Points, std::vector<int>& Index)
		: m_Points(Points), m_Index(Index)
	{
		InitBoundaryLength();
		InitBVHScene();
	}

	// Default loop line segments
	Boundary(std::vector<Vec2> Points)
		: m_Points(Points)
	{
		for (int i = 0; i < m_Points.size(); ++i)
		{
			m_Index.push_back(i);
			m_Index.push_back((i + 1) % m_Points.size());
		}
		InitBoundaryLength();
		InitBVHScene();
	}


	ClosePoint GetClosestPoint(const Vec2& p) const;

	// Return true if p is inside the boundary
	bool CheckInterior(const Vec2& p) const;

	virtual bool HasBoundaryValue() const { return false;  }

	virtual Vec3 BoundaryValue(const Vec2& p) const { return { 0.0, 0.0, 0.0 }; }

	// Uniform sample
	virtual BoundaryVauleSample SampleBoundary(std::mt19937& gen, double& pdf) const;

protected:
	std::vector<Vec2> m_Points;
	std::vector<int>  m_Index;
	fcpw::Scene<3> m_Scene;

	// If we just do uniform sample over boundary
	double m_pdf;
	double m_len;

	// Test a ray parallel to X axis intersect line segment ab or not
	bool intersect(const Vec2& p, const Vec2& a, const Vec2& b) const;

	void InitBoundaryLength();

	void InitBVHScene()
	{
		m_Scene.setObjectTypes({ {fcpw::PrimitiveType::LineSegment} });

		m_Scene.setObjectVertexCount(m_Points.size(), 0);
		m_Scene.setObjectLineSegmentCount(m_Index.size() / 2, 0);
		for (int i = 0; i < m_Points.size(); ++i)
		{
			fcpw::Vector3 p{ (float)m_Points[i][0], (float)m_Points[i][1], 0.0f };
			m_Scene.setObjectVertex(p, i, 0);
		}

		for (int i = 0; i < m_Index.size(); i += 2)
		{
			int a[2] = { m_Index[i], m_Index[i + 1] };
			m_Scene.setObjectLineSegment(a, i / 2, 0);
		}
		m_Scene.build(fcpw::AggregateType::Bvh_OverlapSurfaceArea, true);
	}
};


// This is for curves since usually they will have more points
class SegmentBoundaryNew : public Boundary
{
public:
	SegmentBoundaryNew(std::vector<Vec2>& Points, std::vector<Vec3>& ColorSet, int col = 1, int black = 1, bool importanceSampling = true, int offset=0)
		: Boundary(Points), m_ImportanceSampling(importanceSampling)
	{
		Init(ColorSet, col, black, offset); CalculateEnergy();
	}

	SegmentBoundaryNew(std::vector<Vec2>& Points, std::vector<Vec3>& Colors, std::vector<int>& index, bool importanceSampling = true)
		: Boundary(Points, index), m_FrontVertexColor(Colors), m_BackVertexColor(Colors),
		m_ImportanceSampling(importanceSampling)
	{
		CalculateEnergy();
	}

	SegmentBoundaryNew(std::vector<Vec2>& Points, std::vector<Vec3>& FrontColors, std::vector<Vec3>& BackColors, std::vector<int>& index, bool importanceSampling = true)
		: Boundary(Points, index), m_FrontVertexColor(FrontColors), m_BackVertexColor(BackColors), m_ImportanceSampling(importanceSampling)
	{
		CalculateEnergy();
	}

	bool HasBoundaryValue() const { return true; }

	virtual Vec3 BoundaryValue(const Vec2& p) const override
	{
		return SegmentBoundaryValue(p);
	}


	Vec3 SegmentBoundaryValue(const Vec2& p) const
	{
		fcpw::Interaction<3> interaction;
		m_Scene.findClosestPoint({ (float)p[0], (float)p[1], 0.f }, interaction);
		int index = interaction.primitiveIndex;

		const Vec2& A = m_Points[m_Index[2 * index]];
		const Vec2& B = m_Points[m_Index[2 * index + 1]];
		auto H = LineHelper(A, B, p);
		return ColorHelper(index, H);
	}


	// Importance sampling according to the vertex colors
	BoundaryVauleSample SampleBoundary(std::mt19937& gen, double& pdf) const override {
		if (!m_ImportanceSampling)
		{
			Vec2 pos, n;
			Vec3 col;
			double r = Rand01(gen) * m_len;
			double len = 0.0;
			
			for (int i = 0; i < m_Index.size(); i += 2) {
				Vec2 A = m_Points[m_Index[i]];
				Vec2 B = m_Points[m_Index[i + 1]];
				double ab = (A - B).norm();
				if (r > len && r < len + ab) {
					
					pos = A + (r - len) / ab * (B - A);
					Vec2 AB = (A - B).normalized();
					n = Vec2{ -AB[1], AB[0] };
#ifdef  SINGLE_SIDE
					col = BoundaryValue(pos); // :( lazy
#else
					double t = 1 - (r - len) / ab;
					bool sign = Rand01(gen) > 0.5;
					n = sign ? -n : n;
					col = ColorHelper(i / 2, { t, sign });
#endif
					break;
				}
				len += ab;
			}
			pdf = m_pdf;
			return { pos, n, col };
		}
		else
		{

			Vec2 pos, n;
			Vec3 col;

			double sample_energy = Rand01(gen) * m_TotalEnergy;
			double col_sum = 0.0;
			double len = 0.0;
			for (int i = 0; i < m_Index.size(); i += 2) {

				auto E_line = LineEnergy(i);
				col_sum += E_line;

				if (col_sum >= sample_energy)
				{
					double t = (col_sum - sample_energy) / E_line;

					const Vec2& A = m_Points[m_Index[i]];
					const Vec2& B = m_Points[m_Index[i + 1]];
					double AB = (B - A).norm();
					pos = B - t * (B - A);

					Vec2 ab = (A - B) / AB;
					n = { -ab[1], ab[0] };
#ifdef  SINGLE_SIDE
					col = BoundaryValue(pos); // :( lazy
#else
					bool sign = Rand01(gen) > 0.5;
					n = sign ? -n : n;
					col = ColorHelper(i / 2, {t, sign});
#endif //  SINGLE_SIDE
					pdf = E_line / m_TotalEnergy / AB;
					break;
				}
			}
			return { pos, n, col };
		}
	}


	// Do simple color mix, if MIX_COLOR is not defined will return the first color
	static Vec3 LineColorMix(const Vec3& a1, const Vec3& a2, double t)
	{
#ifdef MIX_COLOR
		return a1 * t + a2 * (1.0 - t);
#else
		return a1;
#endif // MIX_COLOR

		
	}

	inline Vec3 ColorHelper(int index, ProjectedDistanceAndSign H) const
	{

		const Vec2& A = m_Points[m_Index[2 * index]];
		const Vec2& B = m_Points[m_Index[2 * index + 1]];

#ifdef SINGLE_SIDE
		const Vec3& c1 = m_FrontVertexColor[m_Index[2 * index]];
		const Vec3& c2 = m_FrontVertexColor[m_Index[2 * index + 1]];
		return LineColorMix(c1, c2, H.t);
#else
		if (H.sign)
		{
			const Vec3& c1 = m_BackVertexColor[m_Index[2 * index]];
			const Vec3& c2 = m_BackVertexColor[m_Index[2 * index + 1]];
			return LineColorMix(c1, c2, H.t);
		}
		else
		{
			const Vec3& c1 = m_FrontVertexColor[m_Index[2 * index]];
			const Vec3& c2 = m_FrontVertexColor[m_Index[2 * index + 1]];
			return LineColorMix(c1, c2, H.t);
		}
#endif // SINGLE_SIDE
	}

	double m_TotalEnergy;
	bool m_ImportanceSampling;
	std::vector<Vec3> m_FrontVertexColor;
	std::vector<Vec3> m_BackVertexColor;

	void Init(std::vector<Vec3>& ColorSet, int col, int black, int offset)
	{
		for (int i = 0; i < m_Index.size(); i+=2)
		{
			const Vec2& A = m_Points[m_Index[i]];
			const Vec2& B = m_Points[m_Index[i + 1]];
			int lineNum = i / 2 + offset;
			int group = lineNum /(col + black);
			bool isCol = lineNum % (col + black) < col;
			if (isCol)
			{
				const Vec3& vertex_color = ColorSet[group % ColorSet.size()];
				m_FrontVertexColor.push_back(vertex_color);
				m_BackVertexColor.push_back(vertex_color);
				continue;
			}
			else {
				m_FrontVertexColor.push_back(Vec3{ 0.0, 0.0, 0.0 });
				m_BackVertexColor.push_back(Vec3{ 0.0, 0.0, 0.0 });
				continue;
			}
		}

	}

	void CalculateEnergy()
	{
		m_TotalEnergy = 0.0;
		for (int i = 0; i < m_Index.size(); i += 2)
		{
			m_TotalEnergy += LineEnergy(i);
		}
	}


	double LineEnergy(int i) const
	{
		const Vec2& A = m_Points[m_Index[i]];
		const Vec2& B = m_Points[m_Index[i + 1]];

		auto& Fa = m_FrontVertexColor[m_Index[i]];
		auto& Fb = m_FrontVertexColor[m_Index[i + 1]];
#ifdef SINGLE_SIDE
		return  ToGrey((Fa + Fb) / 2) * (A - B).norm();

#else
		auto& Ba = m_BackVertexColor[m_Index[i]];
		auto& Bb = m_BackVertexColor[m_Index[i + 1]];
		return ToGrey((Fa + Fb + Ba + Bb) / 4) * (A - B).norm();

#endif
	}
};

class OneBoundary : public Boundary
{
public:
	OneBoundary(std::vector<Vec2>& Points, std::vector<int>& Index)
		: Boundary(Points, Index) {}

	OneBoundary(std::vector<Vec2>& Points)
		: Boundary(Points) {}

	virtual bool HasBoundaryValue() const { return true; }

	virtual Vec3 BoundaryValue(const Vec2& p) const { return { 1.0, 1.0, 1.0 }; }
};