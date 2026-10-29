#ifndef PLANET_H
#define PLANET_H


#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"

struct PlanetParams {
    float baseRadius;
    float heightScale;
    float noiseFreq;
    int noiseOctaves;
    float seaLevel; // e.g. 0.0 means baseRadius; positive values = meters/units
    glm::vec3 oceanColor, sandColor, grassColor, rockColor, snowColor;
    glm::vec3 atmosphereColor;
};

class Planet{
public:
    MeshData meshData;
    Mesh* mesh;

    glm::vec3 position;
    float radius;
    float mass;
    
    Material* material;

    Planet();
    Planet(float radius, glm::vec3 color, Shader* s);

    ~Planet();

    void draw();

};

uint64_t pair_key(uint32_t a, uint32_t b);
glm::vec3 normalize_scale(const glm::vec3 &v, float scale);
glm::vec2 spherical_uv(const glm::vec3 &n);
void subdivide_once(std::vector<uint32_t> &indices, std::vector<glm::vec3> &positions, std::unordered_map<uint64_t, uint32_t> &midpointCache);
MeshData build_sphere_from_octahedron(int subdivisions, float radius);
MeshData SphereMesh(int resolution, float radius = 1.0f);
std::vector<MeshData> GenerateLODs(int highRes, int lodCount, float radius = 1.0f);

#endif