#include "Boundary.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

BoundaryData Boundary::ReadObjCurve(string file)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
	if (!ret) {
		exit(1);
	}

	// Only read the first shape
	auto& lines = shapes[0].lines;

	vector<Vec2> points;
	for (int i = 0; i < attrib.vertices.size(); i += 3)
	{
		double vx = attrib.vertices[i];
		double vy = attrib.vertices[i + 2];
		points.push_back(Vec2{ vx, vy });
	}

	// TODO maybe not correct in complex scene
	vector<int> index;
	int lineIndex = 0;
	for(int i = 0; i < lines.num_line_vertices.size(); ++i)
	{
		// Line i, contains nVertices.
		int nVertices = lines.num_line_vertices[i];
		for (int v = 0; v < nVertices; ++v, ++lineIndex)
		{
			int vIndex = lines.indices[lineIndex].vertex_index;
			index.push_back(vIndex);
		}
	}
	std::cout << "Loaded obj file with: " << points.size() << " points, " << index.size()/2 << " lines." << endl;
	return { points, index };
}


ClosePoint Boundary::GetClosestPoint(const Vec2& p) const {
	ClosePoint cp;
	
	fcpw::Interaction<3> interaction;
	m_Scene.findClosestPoint({ (float)p[0], (float)p[1], 0.f }, interaction);
	cp.position = Vec2{ interaction.p[0], interaction.p[1] };
	cp.distance = interaction.d;
	return cp;
}

bool Boundary::CheckInterior(const Vec2& p) const
{
	bool sign = false;
	for (int i = 0; i < m_Index.size(); i += 2) {
		const Vec2& a = m_Points[m_Index[i]];
		const Vec2& b = m_Points[m_Index[i + 1]];

		if (intersect(p, a, b)) {
			sign = !sign;
		}

	}
	return sign;
}

BoundaryVauleSample Boundary::SampleBoundary(std::mt19937& gen, double& pdf) const {
	Vec2 pos, n;
	double r = Rand01(gen) * m_len;
	double len = 0.0;
	double side = 1.0;
	for (int i = 0; i < m_Index.size(); i+=2) {
		Vec2 A = m_Points[m_Index[i]];
		Vec2 B = m_Points[m_Index[i + 1]];
		double ab = (A - B).norm();
		if (r > len && r < len + ab) {
			
			pos = A + (r - len) / ab * (B - A);
			Vec2 ab = (A - B).normalized();
			n = Vec2{ -ab[1], ab[0] };
			break;
		}
		len += ab;
	}
	pdf = m_pdf;
	return { pos, n, BoundaryValue(pos) };
}

bool Boundary::intersect(const Vec2& p, const Vec2& a, const Vec2& b) const {
	Vec2 d = b - a;
	double l = d.norm();
	d.normalize();

	double t = (p[1] - a[1]) / d[1];
	if (t >= 0 && t <= l) {
		Vec2 x = a + t * d;
		return x[0] > p[0] ? 1 : 0;
	}
	else {
		return false;
	}
}

void Boundary::InitBoundaryLength() {
	m_len = 0.0;
	for (int t = 0; t < m_Index.size(); t+=2) {
		Vec2 A = m_Points[m_Index[t]];
		Vec2 B = m_Points[m_Index[t + 1]];
		double l = (A - B).norm();
		m_len += l;
	}
	m_pdf = 1.0 / m_len;
}


