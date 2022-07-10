#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "Eigen/Core"
#include <random>
#include <thread>
#include <string>

#include <chrono>
#include <ctime>    
#include <iomanip>

#define UniformInPixel 1

using namespace std;
using namespace Eigen;

using Vec2 = Vector2<double>;
using Vec2i = Vector2<int>;
using Vec3 = Vector3<double>;

const std::string WorkDirectory = "./";

constexpr double ScreenSize = 1.0; // scale

constexpr double PI = 3.1415926;
constexpr double TwoPI =  2 * PI;
constexpr int DefaultResolution = 900;

extern std::random_device rd;
extern std::uniform_real_distribution<> UniformDistribution;


inline double clamp(double v, double a, double b) {
	return min(max(a, v), b);
}

inline double Rand01(std::mt19937& gen) {
	return UniformDistribution(gen);
}


inline double ToGrey(Vec3 c) { return (c[0] + c[1] + c[2]) / 3; }
inline double perimeter(double r) { return 2.0 * PI * r; }
inline double volume(double r) { return PI * r * r; }


inline double FundamentalSolution(double r) {
	return std::log(1 / r) / TwoPI;
}

// Green function G(x,y) for a disk when x is fixed at 0
// R: radius of the disk
// r = |x - y|
inline double Green(double R, double r) 
{
	assert(R > r);
	return std::log(R / r) / TwoPI;
}

// Green function for disk {x^2 + y^2 < R^2}
// Remember put x to be 0 if needed
inline double Green(Vec2 x, Vec2 y, double R)
{
	// normalize x, y into unit disk
	double r = x.norm();
	double s = y.norm() ;
	Vec2 y_dual = R * R * y / (s * s);

	double xy = (x - y).norm();
	double xy_dual = (x - y_dual).norm();

	return std::log(s * xy_dual / (R * xy)) / TwoPI;
}

// The poisson kernel for disk {x^2 + y^2 < R^2}
// http://www.math.umbc.edu/~jbell/pde_notes/22_Greens%20functions-PDEs.pdf
// https://www.math.utk.edu/~freire/teaching/m435s14/PotentialTheory.pdf
// x: point inside disk
// y: point on the boundary
inline double PoissonKernel(Vec2 x, Vec2 y, double R) 
{
	double r = x.norm();
	double c = y.dot(x) / y.norm();
	return (R * R - r * r) / (R * R - 2 * R * c + r * r) / TwoPI / R;
}

struct SamplePoint {
	Vec2 pos;
	double pdf;
};

inline SamplePoint Uniform_dB(Vec2 center, double r, std::mt19937& gen) 
{
	double theta = TwoPI * Rand01(gen);
	Vec2 point = center + r * Vec2(cos(theta), sin(theta));
	return SamplePoint{ point, 1.0 / perimeter(r) };
}

inline SamplePoint Uniform_Ball(Vec2 center, double r, std::mt19937& gen) 
{
	double theta =  TwoPI * Rand01(gen);
	double sample_r = r * sqrt(1 - Rand01(gen));
	Vec2 point = center + sample_r * Vec2(cos(theta), sin(theta));
	return { point, 1.0 / volume(r) };
}

inline float productLogMinus1(float x)
{
    auto f = [&](float y) { return y*std::exp(y); };
    auto g = [&](float y) { return (1.0f + y)*std::exp(y); };
    float w = -2.0f, delta;
    do {
        delta = f(w) - x;
        w -= delta/g(w);
    } while (std::abs(delta) > 1e-6f);

    return w;
}

inline SamplePoint SampleGreen(Vec2 pos, float R, std::mt19937& gen) {
	double x0 = Rand01(gen);
	float r = R * std::sqrt(-x0 / productLogMinus1(-x0 / 2.718282f));
	float phi = Rand01(gen) * TwoPI;
	Vec2 p(r * std::cos(phi), r * std::sin(phi));
	double pdf = Green(R, r) / (R * R / 4.0f);
	return { pos + p, pdf };
}


inline double GreenPdf(double R, double r)
{
	return 4.0 * Green(R, r) / (R * R);
}

class ClosePoint {
public:
	Vec2 position;
	double distance;

	ClosePoint():distance(1000.0) {}
	ClosePoint(Vec2 v, double d) :position(v), distance(d) {}
};

struct ProjectedDistanceAndSign
{
	double t;
	bool  sign;
};

inline ProjectedDistanceAndSign LineHelper(Vec2 a, Vec2 b, Vec2 p)
{
	Vec2 ab = (a - b).normalized();
	Vec2 pa = p - a;
	Vec2 pb = p - b;
	double l = pb.dot(ab);
	double l_ab = (a - b).norm();
	Vec2 n{ -ab[1], ab[0] };

	double orientation = pa[0] * ab[1] - pa[1] * ab[0];
	bool sign = orientation < 0.0;

	double t = clamp(l / l_ab, 0.0, 1.0 );
	return { t, sign };
}


inline ClosePoint SDLine(Vec2 a, Vec2 b, Vec2 p) 
{
	Vec2 ab = (a-b).normalized();
	Vec2 pa = p - a;
	Vec2 pb = p - b;
	double l = pb.dot(ab);
	double l_ab = (a - b).norm();
	Vec2 pos;
	if (l > 0 && l < l_ab) {
		pos = b + l * ab;
	}
	else if (pa.norm() < pb.norm()) {
		pos = a;
	}
	else {
		pos = b;
	}
	Vec2 n{-ab[1], ab[0]};
	return { pos, (p - pos).norm() };
}

inline ClosePoint SDCircle(Vec2 o, double r, Vec2 p) {
	Vec2 n = (p - o).normalized();
	Vec2 pos = o + r * n;
	double distance = abs((p - o).norm() - r);
	return {pos, distance};
}
