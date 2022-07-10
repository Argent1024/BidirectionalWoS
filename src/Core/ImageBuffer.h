#pragma once
#include "Common.h"
#include <vector>
#include<iostream>
#include<fstream>

class ImageBuffer {
public:
	ImageBuffer(int res) : res(res)
	{ 
		buffer = std::vector<std::vector<Vec3>>(res, std::vector<Vec3>(res, {0, 0, 0}));
		PoissonKernelNormalizer = std::vector<std::vector<double>>(res, std::vector<double>(res, 0));
		
		scene_size = ScreenSize;
		pixel_size = 2.0 * scene_size/ res;

	}
	
	// Read PFM
	ImageBuffer(const string& name) {
		ifstream file;
		file.open(name.c_str(), ios::in | ios::binary);

		string format;
		std::getline(file, format); // Dont' care this yet

		// Get resolution and ignore line
		file >> res;

		char lineEnd = 0x0a;
		file.ignore(numeric_limits<streamsize>::max(), lineEnd);
		file.ignore(numeric_limits<streamsize>::max(), lineEnd); // Skip two lines


		// Will only use buffer
		buffer = std::vector<std::vector<Vec3>>(res, std::vector<Vec3>(res, { 0, 0, 0 }));
		scene_size = ScreenSize;
		pixel_size = 2.0 * scene_size / res;

		float* float_buffer = new float[3 * res * res];
		file.read((char*)float_buffer, sizeof(float) * 3 * res * res);

		for (int i = 0; i < res; ++i) {
			for (int j = 0; j < res; ++j) {
				Vec3& v = buffer[j][i];
				v[0] = float_buffer[i * 3 * res + 3 * j + 0];
				v[1] = float_buffer[i * 3 * res + 3 * j + 1];
				v[2] = float_buffer[i * 3 * res + 3 * j + 2];
			}
		}
		delete[] float_buffer;


		// End read file

		PoissonKernelNormalizer = std::vector<std::vector<double>>(res, std::vector<double>(res, 0));
	}

	Vec3 GetPixel(Vec2 p) const {
		if (-scene_size < p[0] && p[0] < scene_size &&
			-scene_size < p[1] && p[1] < scene_size) {
			Vec2i coor = world2img(p);
			return buffer[coor[0]][coor[1]];
		}
		return Vec3(0.0, 0.0, 0.0);
	}

	double GetPoissonKerynelEstimate(Vec2 p) const
	{
		if (-scene_size < p[0] && p[0] < scene_size &&
			-scene_size < p[1] && p[1] < scene_size) {
			Vec2i coor = world2img(p);
			return PoissonKernelNormalizer[coor[0]][coor[1]];
		}
		return 0.0;
	}
	
	inline Vec2i world2img(Vec2 p) const {
		int x = (p[0] + scene_size) * res / (2.0 * scene_size);
		int y = (p[1] + scene_size) * res / (2.0 * scene_size);
		Vec2i coor = Vec2i(x, y);
		return coor;
	}

	inline Vec2 img2world(Vec2i coor) const {
		double x = 2.0 * scene_size * (coor[0] + 0.5) / (double)res - scene_size;
		double y = 2.0 * scene_size * (coor[1] + 0.5) / (double)res - scene_size;
		return Vec2(x, y);
	}

	// w is here to help normalize Poisson kernel
	void WritePixel(Vec2 p, Vec3 v, double w=1.0) {
		if (-scene_size < p[0] && p[0] < scene_size &&
			-scene_size < p[1] && p[1] < scene_size) {
			Vec2i coor = world2img(p);
			buffer[coor[0]][coor[1]] += w * v;
			PoissonKernelNormalizer[coor[0]][coor[1]] += w;
		}
	}

	void DrawGreenSphere(Vec2 pos, double r, Vec3 source, std::mt19937& gen) {
		Vec2i coor = world2img(pos);
		
#ifdef UniformInPixel
		double dx = (Rand01(gen) - 0.5) * pixel_size;
		double dy = (Rand01(gen) - 0.5) * pixel_size;
		Vec2 offset{ dx, dy };
#else 
		Vec2 offset{ 0.0, 0.0 };
#endif
		int pixel_r = int(r / pixel_size + 1);
		for (int x = coor[0] - pixel_r; x <= coor[0] + pixel_r; ++x) {
			for (int y = coor[1] - pixel_r; y <= coor[1] + pixel_r; ++y) {
				Vec2 p = img2world({x, y}) + offset;

				double l = (p - pos).norm();
				if(l > r) { 
					continue; 
				}
				double g = Green(r, l);
				WritePixel(p, g * source);
			}
		}
	}

	// Need to pass in weight s.t. every pixel's Poisson Kernel is normalized
	void DrawGreenSphere(Vec2 pos, double r, double weight, Vec3 source, std::mt19937& gen) {
		Vec2i coor = world2img(pos);
#ifdef UniformInPixel
		double dx = (Rand01(gen) - 0.5) * pixel_size;
		double dy = (Rand01(gen) - 0.5) * pixel_size;
		Vec2 offset{ dx, dy };
#else
		Vec2 offset{ 0.0, 0.0 };
#endif

		int pixel_r = int(r / pixel_size + 1);
		for (int x = coor[0] - pixel_r; x <= coor[0] + pixel_r; ++x) {
			for (int y = coor[1] - pixel_r; y <= coor[1] + pixel_r; ++y) {
				Vec2 p = img2world({ x, y }) + offset;

				double l = (p - pos).norm();
				if (l > r) {
					continue;
				}
				double g = Green(r, l);
				WritePixel(p, source, weight * g);
			}
		}
	}

	// Not saving PNG using stb anymore
	//void SaveImage(const string& name) 
	//{
	//	FloatImage img(res, res, 3);
	//	for (int i = 0; i < res; ++i) {
	//		for (int j = 0; j < res; ++j) {
	//			const Vec3& v = buffer[i][j];
	//			img(i, j, 0) = v[0];
	//			img(i, j, 1) = v[1];
	//			img(i, j, 2) = v[2];
	//		}
	//	}
	//	img.write(name);
	//}

	void SaveImagePFM(const string& name) {
		_WritePFM(name, buffer);
	}

	void _WritePFM(const string& name, std::vector<std::vector<Vec3>>& imgbuffer)
	{
		ofstream file;
		file.open(name.c_str(), ios::out | ios::trunc | ios::binary);

		char type[2] = {'P', 'F'};
		char lineEnd = 0x0a;
		file << type[0] << type[1] << lineEnd;

		file << res << ' ' << res << lineEnd;
		float endian = 0.0f;
		{
			// https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program
			int num = 1;
			if (*(char *)&num == 1) {
				endian = -1.0;
			} else {
				endian = 1.0;
			}
		}
		file << endian << lineEnd;
		

		float* float_buffer = new float[3 * res * res];
		for (int i = 0; i < res; ++i) {
			for (int j = 0; j < res; ++j) {
				const Vec3& v = imgbuffer[j][i];
				float_buffer[i * 3 * res + 3 * j + 0] = v[0];
				float_buffer[i * 3 * res + 3 * j + 1] = v[1];
				float_buffer[i * 3 * res + 3 * j + 2] = v[2];
			}
		}
		file.write((char*)float_buffer, sizeof(float) * 3 * res * res);

		delete[] float_buffer;
		file.close();
	}

	std::vector<std::vector<Vec3>> buffer;
	std::vector< std::vector<double>> PoissonKernelNormalizer;

	int res;
	double pixel_size;
	double scene_size; // The img will show [-scene_size, scene_size]
};