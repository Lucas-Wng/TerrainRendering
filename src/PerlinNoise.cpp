//
// Created by Lucas Wang on 2024-07-03.
//

#include "PerlinNoise.h"
#include <random>
#include <algorithm>
#include <cmath>

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
    int h = hash & 7;        // Convert low 3 bits of hash code
    double u = h < 4 ? x : y;// into 8 simple gradient directions,
    double v = h < 4 ? y : x;// and compute the dot product.
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Perlin noise function
double perlin(double x, double y, const std::vector<int> &p) {
    int X = (int) floor(x) & 255;
    int Y = (int) floor(y) & 255;
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
    double maxValue = 0.0;// Used for normalizing result to [-1, 1]

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

void hydraulicErosion(std::vector<double> &heightmap, int width, int depth, int iterations) {
    std::vector<double> water(heightmap.size(), 0.0);
    std::vector<double> sediment(heightmap.size(), 0.0);

    for (int i = 0; i < iterations; ++i) {
        // Water flow and sediment transport
        for (int z = 1; z < depth - 1; ++z) {
            for (int x = 1; x < width - 1; ++x) {
                int idx = z * width + x;
                double deltaHeight[4] = {
                        heightmap[idx] - heightmap[idx + 1],     // right
                        heightmap[idx] - heightmap[idx - 1],     // left
                        heightmap[idx] - heightmap[idx + width], // down
                        heightmap[idx] - heightmap[idx - width]  // up
                };

                double flow[4] = {0.0, 0.0, 0.0, 0.0};
                double totalDeltaHeight = 0.0;
                for (int j = 0; j < 4; ++j) {
                    if (deltaHeight[j] > 0) {
                        flow[j] = deltaHeight[j];
                        totalDeltaHeight += deltaHeight[j];
                    }
                }

                if (totalDeltaHeight > 0) {
                    for (int j = 0; j < 4; ++j) {
                        flow[j] /= totalDeltaHeight;
                        double sedimentTransport = water[idx] * flow[j] * 0.1;
                        sediment[idx] -= sedimentTransport;
                        water[idx] -= sedimentTransport;
                        if (j == 0) {
                            sediment[idx + 1] += sedimentTransport;
                            water[idx + 1] += sedimentTransport;
                        } else if (j == 1) {
                            sediment[idx - 1] += sedimentTransport;
                            water[idx - 1] += sedimentTransport;
                        } else if (j == 2) {
                            sediment[idx + width] += sedimentTransport;
                            water[idx + width] += sedimentTransport;
                        } else if (j == 3) {
                            sediment[idx - width] += sedimentTransport;
                            water[idx - width] += sedimentTransport;
                        }
                    }
                }
            }
        }

        // Apply sediment to the heightmap
        for (int j = 0; j < heightmap.size(); ++j) {
            heightmap[j] += sediment[j];
        }
    }
}

void applyGaussianBlur(std::vector<double> &heightmap, int width, int depth, double sigma) {
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
    std::vector<double> temp(heightmap.size());
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            double value = 0.0;
            for (int k = -kernelRadius; k <= kernelRadius; ++k) {
                int nx = std::min(std::max(x + k, 0), width - 1);
                value += heightmap[z * width + nx] * kernel[k + kernelRadius];
            }
            temp[z * width + x] = value;
        }
    }

    // Apply the kernel in the y direction
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            double value = 0.0;
            for (int k = -kernelRadius; k <= kernelRadius; ++k) {
                int nz = std::min(std::max(z + k, 0), depth - 1);
                value += temp[nz * width + x] * kernel[k + kernelRadius];
            }
            heightmap[z * width + x] = value;
        }
    }
}

double terrace(double value, int steps) {
    double stepSize = 1.0 / steps;
    return floor(value / stepSize) * stepSize;
}

std::vector<int> PerlinNoise::generatePermutationVector(unsigned int seed) {
    std::vector<int> permutation(256);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::mt19937 rng(seed);
    std::shuffle(permutation.begin(), permutation.end(), rng);
    std::vector<int> p(512);
    for (int i = 0; i < 256; ++i) {
        p[i] = p[256 + i] = permutation[i];
    }
    return p;
}

// Gradients for 2D simplex noise
static const int grad2[8][2] = {
        {1,1}, {-1,1}, {1,-1}, {-1,-1},
        {1,0}, {-1,0}, {0,1}, {0,-1}
};

// Skewing factors for 2D simplex noise
const double F2 = 0.5 * (sqrt(3.0) - 1.0);
const double G2 = (3.0 - sqrt(3.0)) / 6.0;

int fastFloor(double x) {
    return x > 0 ? (int)x : (int)x - 1;
}

double dot(const int* g, double x, double y) {
    return g[0] * x + g[1] * y;
}

double simplexNoise(double x, double y, const std::vector<int> &perm) {
    // Skew the input space to determine which simplex cell we're in
    double s = (x + y) * F2; // Hairy factor for 2D
    int i = fastFloor(x + s);
    int j = fastFloor(y + s);

    double t = (i + j) * G2;
    double X0 = i - t; // Unskew the cell origin back to (x,y) space
    double Y0 = j - t;
    double x0 = x - X0; // The x,y distances from the cell origin
    double y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    int i1, j1; // Offsets for the second (middle) corner of simplex in (i,j) coordinates
    if (x0 > y0) {
        i1 = 1; j1 = 0;
    } else {
        i1 = 0; j1 = 1;
    }

    // A step of (1,0) in (i,j) means (1-c,-c) in (x,y), and (0,1) means (-c,1-c)
    double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coordinates
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coordinates
    double y2 = y0 - 1.0 + 2.0 * G2;

    // Work out the hashed gradient indices of the three simplex corners
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii + perm[jj]] % 8;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 8;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 8;

    // Calculate the contribution from the three corners
    double t0 = 0.5 - x0 * x0 - y0 * y0;
    double n0;
    if (t0 < 0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad2[gi0], x0, y0);
    }

    double t1 = 0.5 - x1 * x1 - y1 * y1;
    double n1;
    if (t1 < 0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad2[gi1], x1, y1);
    }

    double t2 = 0.5 - x2 * x2 - y2 * y2;
    double n2;
    if (t2 < 0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad2[gi2], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the range [-1,1]
    return 70.0 * (n0 + n1 + n2);
}

void PerlinNoise::GenerateHeightMap() {
    unsigned int seed = 0xAABBCCDD;
    std::vector p = generatePermutationVector(seed);
    // Parameters for fBM
    int octaves = 7 + (rand() % 3); // Number of octaves with variability
    double persistence = 0.7 + (rand() % 50) / 100.0; // Dynamic persistence
    double lacunarity = 2.5 + (rand() % 20) / 10.0; // Dynamic lacunarity
    double warpFactor = 0.1;
    double inputScale = 0.002;

    for (int z = 0; z < m_depth; z++) {
        for (int x = 0; x < m_width; x++) {
            double noiseValue = ridgedPerlin(x * inputScale, z * inputScale, p, octaves, persistence, lacunarity);
            noiseValue = domainWarp(x * inputScale, z * inputScale, p, warpFactor) * 0.3 +
                         noiseValue * 0.7; // Example of domain warping
            noiseValue = terrace(noiseValue,
                                 10);                                                           // Example of terracing
//            double noiseValue = simplexNoise(x * 50.0, z * 50.0, p) / 50.0;
            m_data.push_back(noiseValue);
        }
    }

// Apply hydraulic erosion to the heightmap
    hydraulicErosion(m_data, m_width, m_depth, 1000 );

// Apply Gaussian blur to smooth the heightmap
    applyGaussianBlur(m_data, m_width, m_depth, 2.0);
}

std::vector<double> PerlinNoise::GetHeightMap() {
    return m_data;
}