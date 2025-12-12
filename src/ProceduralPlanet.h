#ifndef PROCEDURAL_PLANET_H
#define PROCEDURAL_PLANET_H

#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>

struct PlanetSettings {
    float radius = 100.0f;
    float waterLevel = 0.4f;  // 0-1, percentage of radius for water
    float mountainHeight = 0.3f;  // 0-1, max height variation
    int subdivisions = 5;  // Number of times to subdivide icosahedron
    
    // Noise parameters
    float noiseScale = 0.02f;
    int octaves = 6;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
    int seed = 12345;
    
    // Colors
    glm::vec3 deepWaterColor = glm::vec3(0.0f, 0.1f, 0.4f);
    glm::vec3 shallowWaterColor = glm::vec3(0.1f, 0.3f, 0.6f);
    glm::vec3 beachColor = glm::vec3(0.9f, 0.85f, 0.7f);
    glm::vec3 grassColor = glm::vec3(0.2f, 0.6f, 0.2f);
    glm::vec3 rockColor = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 snowColor = glm::vec3(0.95f, 0.95f, 0.95f);
    
    // Atmosphere
    glm::vec3 atmosphereColor = glm::vec3(0.5f, 0.7f, 1.0f);
    float atmosphereThickness = 10.0f;  // Distance from surface
    float atmosphereFalloff = 2.0f;  // How quickly atmosphere fades
};

class ProceduralPlanet {
private:
    MeshData planetMeshData;
    MeshData atmosphereMeshData;
    Mesh* planetMesh;
    Mesh* atmosphereMesh;
    Material* planetMaterial;
    Material* atmosphereMaterial;
    
    PlanetSettings settings;
    
    // Icosahedron generation
    MeshData generateIcosahedron(float radius);
    void subdivideMesh(MeshData& meshData, int subdivisions);
    glm::vec3 getMiddlePoint(const glm::vec3& p1, const glm::vec3& p2, 
                             std::map<std::pair<int, int>, int>& cache,
                             std::vector<Vertex>& vertices, float radius);
    
    // Noise generation
    float generateNoise(const glm::vec3& point);
    float fbm(const glm::vec3& point);  // Fractal Brownian Motion
    
    // Terrain coloring
    glm::vec3 getTerrainColor(float height);
    
    // Generate atmosphere mesh
    MeshData generateAtmosphere();

public:
    glm::vec3 position;
    float radius;
    
    ProceduralPlanet(const glm::vec3& pos, const PlanetSettings& settings, 
                     Shader* planetShader, Shader* atmosphereShader);
    ~ProceduralPlanet();
    
    void generate();
    void draw(bool drawAtmosphere = true);
    
    float getHeightAt(const glm::vec3& worldPos) const;
    glm::vec3 getNormalAt(const glm::vec3& worldPos) const;
    bool isUnderWater(const glm::vec3& worldPos) const;
    
    PlanetSettings& getSettings() { return settings; }
    Material* getPlanetMaterial() { return planetMaterial; }
    Material* getAtmosphereMaterial() { return atmosphereMaterial; }
};

#endif