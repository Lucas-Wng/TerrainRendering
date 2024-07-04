//
// Created by Lucas Wang on 2024-07-03.
//

#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(int width, int depth) {
    m_width = width;
    m_depth = depth;
}

// Fade function to smooth the interpolation
double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

// Gradient function
double grad(int hash, double x, double y) {
    int h = hash & 7;      // Convert low 3 bits of hash code
    double u = h < 4 ? x : y; // into 8 simple gradient directions,
    double v = h < 4 ? y : x; // and compute the dot product.
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Perlin noise function
double perlin(double x, double y, const std::vector<int> &p) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x);
    y -= floor(y);
    double u = fade(x);
    double v = fade(y);
    int A = p[X] + Y;
    int AA = p[A];
    int AB = p[A + 1];
    int B = p[X + 1] + Y;
    int BA = p[B];
    int BB = p[B + 1];
    return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
                lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));
}

// Fractal Brownian Motion (fBM) to combine multiple octaves of Perlin noise
double fbm(double x, double y, const std::vector<int> &p, int octaves, double persistence, double lacunarity) {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;  // Used for normalizing result to [-1, 1]

    for (int i = 0; i < octaves; ++i) {
        total += perlin(x * frequency, y * frequency, p) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total / maxValue;
}

double ridgedPerlin(double x, double y, const std::vector<int> &p, int octaves, double persistence, double lacunarity) {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;
    double ridgeValue;

    for (int i = 0; i < octaves; ++i) {
        double noiseValue = perlin(x * frequency, y * frequency, p);
        ridgeValue = 1.0 - fabs(noiseValue);
        ridgeValue *= ridgeValue;
        total += ridgeValue * amplitude;

        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total / maxValue;
}

double domainWarp(double x, double y, const std::vector<int> &p, double warpFactor) {
    double warpX = perlin(x * warpFactor, y * warpFactor, p);
    double warpY = perlin((x + 5) * warpFactor, (y + 5) * warpFactor, p);
    return perlin(x + warpX, y + warpY, p);
}

void hydraulicErosion(std::vector<double> &heightmap, int length, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        for (int x = 1; x < length - 1; ++x) {
            double deltaHeight = heightmap[x] - heightmap[x + 1];
            if (deltaHeight > 0.1) {
                double sediment = deltaHeight * 0.1;
                heightmap[x] -= sediment;
                heightmap[x + 1] += sediment;
            }
        }
    }
}

void applyGaussianBlur(std::vector<double> &heightmap, int length, double sigma) {
    int kernelRadius = static_cast<int>(ceil(2 * sigma));
    int kernelSize = 2 * kernelRadius + 1;
    std::vector<double> kernel(kernelSize);

    double sum = 0.0;
    for (int i = -kernelRadius; i <= kernelRadius; ++i) {
        kernel[i + kernelRadius] = exp(-(i * i) / (2 * sigma * sigma));
        sum += kernel[i + kernelRadius];
    }
    for (int i = 0; i < kernelSize; ++i) {
        kernel[i] /= sum;
    }

    // Apply the kernel in the x direction
    std::vector<double> temp(length);
    for (int x = 0; x < length; ++x) {
        double value = 0.0;
        for (int k = -kernelRadius; k <= kernelRadius; ++k) {
            int nx = std::min(std::max(x + k, 0), length - 1);
            value += heightmap[nx] * kernel[k + kernelRadius];
        }
        temp[x] = value;
    }

    // Copy the blurred values back to the heightmap
    for (int x = 0; x < length; ++x) {
        heightmap[x] = temp[x];
    }
}

double terrace(double value, int steps) {
    double stepSize = 1.0 / steps;
    return floor(value / stepSize) * stepSize;
}

void PerlinNoise::GenerateHeightMap() {
    std::vector<int> p(512);
    std::vector<int> permutation = { 151,160,137,91,90,15,39,
                                    131, 36, 8, 64, 52, 41, 105, 13, 25, 63, 161, 1, 57, 20, 16, 41, 51, 5,
                                    47, 59, 87, 29, 0, 43, 98, 21, 19, 35, 62, 9, 11, 27, 71, 42, 39, 31, 54, 24, 68, 46, 73, 72, 7, 14, 37, 79, 75, 44, 69, 53, 92, 60, 50, 56, 48, 55, 38, 70, 78, 17, 12, 49, 32, 6, 22, 76, 33, 67, 77, 18, 34, 3, 88, 74, 85, 30, 2, 23, 58, 61, 40, 28, 26, 45, 65, 66, 10, 4, 99, 82, 80, 81, 83, 84, 86, 89, 100, 101, 102, 103, 104, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };
    for (int i = 0; i < 256 ; i++)
        p[256 + i] = p[i] = permutation[i];

    // Parameters for fBM
    int octaves = 5; // Number of octaves
    double persistence = 1.0; // Amplitude factor
    double lacunarity = 2.2; // Frequency factor
    double warpFactor = 0.05;
    double inputScale = 0.004;

    for (int z = 0; z < m_depth; z++) {
        for (int x = 0; x < m_width; x++) {
            double noiseValue = ridgedPerlin(x * inputScale, z * inputScale, p, octaves, persistence, lacunarity);
            noiseValue = domainWarp(x * inputScale, z * inputScale, p, warpFactor) * 0.3 + noiseValue * 0.7; // Example of domain warping
            noiseValue = terrace(noiseValue, 10); // Example of terracing
            std::cout << noiseValue << std::endl;
            m_data.push_back(noiseValue);
        }
    }

    // Apply hydraulic erosion to the heightmap
    hydraulicErosion(m_data, m_width * m_depth, 1000);

    // Apply Gaussian blur to smooth the heightmap
    applyGaussianBlur(m_data, m_width * m_depth, 2.0);
}

std::vector<double> PerlinNoise::GetHeightMap() {
    return m_data;
}
