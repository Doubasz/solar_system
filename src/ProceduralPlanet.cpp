#include "ProceduralPlanet.h"
#include <glm/gtc/noise.hpp>
#include <map>
#include <cmath>
#include <algorithm>

ProceduralPlanet::ProceduralPlanet(const glm::vec3& pos, const PlanetSettings& s,
                                   Shader* planetShader, Shader* atmosphereShader)
    : position(pos), settings(s), planetMesh(nullptr), atmosphereMesh(nullptr)
{
    radius = settings.radius;
    planetMaterial = new Material(planetShader);
    atmosphereMaterial = new Material(atmosphereShader);
    
    generate();
}

ProceduralPlanet::~ProceduralPlanet() {
    if (planetMesh) delete planetMesh;
    if (atmosphereMesh) delete atmosphereMesh;
    if (planetMaterial) delete planetMaterial;
    if (atmosphereMaterial) delete atmosphereMaterial;
}

MeshData ProceduralPlanet::generateIcosahedron(float radius) {
    MeshData meshData;
    
    const float phi = (1.0f + sqrt(5.0f)) / 2.0f;  // Golden ratio
    const float a = 1.0f;
    const float b = 1.0f / phi;
    
    // Normalize to sphere
    float len = sqrt(a*a + b*b);
    float na = a / len * radius;
    float nb = b / len * radius;
    
    // 12 vertices of icosahedron
    std::vector<glm::vec3> positions = {
        {-nb, na, 0}, {nb, na, 0}, {-nb, -na, 0}, {nb, -na, 0},
        {0, -nb, na}, {0, nb, na}, {0, -nb, -na}, {0, nb, -na},
        {na, 0, -nb}, {na, 0, nb}, {-na, 0, -nb}, {-na, 0, nb}
    };
    
    for (const auto& pos : positions) {
        Vertex v;
        v.position = pos;
        v.normal = glm::normalize(pos);
        v.color = glm::vec3(1.0f);
        meshData.vertices.push_back(v);
    }
    
    // 20 triangular faces
    std::vector<std::vector<int>> faces = {
        {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
        {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
        {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
        {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };
    
    for (const auto& face : faces) {
        meshData.indices.push_back(face[0]);
        meshData.indices.push_back(face[1]);
        meshData.indices.push_back(face[2]);
    }
    
    return meshData;
}

glm::vec3 ProceduralPlanet::getMiddlePoint(const glm::vec3& p1, const glm::vec3& p2,
                                           std::map<std::pair<int, int>, int>& cache,
                                           std::vector<Vertex>& vertices, float r) {
    glm::vec3 middle = (p1 + p2) / 2.0f;
    glm::vec3 normalized = glm::normalize(middle) * r;
    return normalized;
}

void ProceduralPlanet::subdivideMesh(MeshData& meshData, int subdivisions) {
    for (int sub = 0; sub < subdivisions; sub++) {
        std::vector<unsigned int> newIndices;
        std::map<std::pair<int, int>, int> midPointCache;
        
        auto getMidPoint = [&](int i1, int i2) -> int {
            std::pair<int, int> key = (i1 < i2) ? std::make_pair(i1, i2) : std::make_pair(i2, i1);
            
            if (midPointCache.find(key) != midPointCache.end()) {
                return midPointCache[key];
            }
            
            glm::vec3 p1 = meshData.vertices[i1].position;
            glm::vec3 p2 = meshData.vertices[i2].position;
            glm::vec3 middle = (p1 + p2) / 2.0f;
            glm::vec3 normalized = glm::normalize(middle) * settings.radius;
            
            Vertex v;
            v.position = normalized;
            v.normal = glm::normalize(normalized);
            v.color = glm::vec3(1.0f);
            
            int index = meshData.vertices.size();
            meshData.vertices.push_back(v);
            midPointCache[key] = index;
            
            return index;
        };
        
        // Subdivide each triangle into 4
        for (size_t i = 0; i < meshData.indices.size(); i += 3) {
            int v1 = meshData.indices[i];
            int v2 = meshData.indices[i + 1];
            int v3 = meshData.indices[i + 2];
            
            int a = getMidPoint(v1, v2);
            int b = getMidPoint(v2, v3);
            int c = getMidPoint(v3, v1);
            
            newIndices.push_back(v1); newIndices.push_back(a); newIndices.push_back(c);
            newIndices.push_back(v2); newIndices.push_back(b); newIndices.push_back(a);
            newIndices.push_back(v3); newIndices.push_back(c); newIndices.push_back(b);
            newIndices.push_back(a); newIndices.push_back(b); newIndices.push_back(c);
        }
        
        meshData.indices = newIndices;
    }
}

float ProceduralPlanet::generateNoise(const glm::vec3& point) {
    // Simple 3D Perlin noise using GLM
    glm::vec3 scaledPoint = point * settings.noiseScale;
    return glm::perlin(scaledPoint + glm::vec3(settings.seed * 0.01f));
}

float ProceduralPlanet::fbm(const glm::vec3& point) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < settings.octaves; i++) {
        glm::vec3 p = point * frequency;
        value += generateNoise(p) * amplitude;
        
        amplitude *= settings.persistence;
        frequency *= settings.lacunarity;
    }
    
    return value;
}

glm::vec3 ProceduralPlanet::getTerrainColor(float height) {
    // Normalize height to 0-1 range
    float minHeight = settings.radius * (1.0f - settings.mountainHeight);
    float maxHeight = settings.radius * (1.0f + settings.mountainHeight);
    float normalizedHeight = (height - minHeight) / (maxHeight - minHeight);
    normalizedHeight = glm::clamp(normalizedHeight, 0.0f, 1.0f);
    
    float waterThreshold = settings.waterLevel;
    float beachThreshold = waterThreshold + 0.05f;
    float grassThreshold = waterThreshold + 0.3f;
    float rockThreshold = waterThreshold + 0.6f;
    
    if (normalizedHeight < waterThreshold) {
        // Deep to shallow water
        float t = normalizedHeight / waterThreshold;
        return glm::mix(settings.deepWaterColor, settings.shallowWaterColor, t);
    } else if (normalizedHeight < beachThreshold) {
        // Beach
        float t = (normalizedHeight - waterThreshold) / (beachThreshold - waterThreshold);
        return glm::mix(settings.shallowWaterColor, settings.beachColor, t);
    } else if (normalizedHeight < grassThreshold) {
        // Grass/lowlands
        float t = (normalizedHeight - beachThreshold) / (grassThreshold - beachThreshold);
        return glm::mix(settings.beachColor, settings.grassColor, t);
    } else if (normalizedHeight < rockThreshold) {
        // Rocky mountains
        float t = (normalizedHeight - grassThreshold) / (rockThreshold - grassThreshold);
        return glm::mix(settings.grassColor, settings.rockColor, t);
    } else {
        // Snow peaks
        float t = (normalizedHeight - rockThreshold) / (1.0f - rockThreshold);
        return glm::mix(settings.rockColor, settings.snowColor, t);
    }
}

void ProceduralPlanet::generate() {
    // Generate base icosahedron
    planetMeshData = generateIcosahedron(settings.radius);
    
    // Subdivide to create sphere
    subdivideMesh(planetMeshData, settings.subdivisions);
    
    // Apply noise to vertices
    for (auto& vertex : planetMeshData.vertices) {
        glm::vec3 normalizedPos = glm::normalize(vertex.position);
        
        // Get noise value
        float noise = fbm(normalizedPos);
        
        // Map noise from [-1, 1] to height variation
        float heightVariation = noise * settings.mountainHeight * settings.radius;
        
        // Apply height
        float newRadius = settings.radius + heightVariation;
        vertex.position = normalizedPos * newRadius;
        
        // Update normal (points outward from center)
        vertex.normal = normalizedPos;
        
        // Set color based on height
        vertex.color = getTerrainColor(newRadius);
    }
    
    // Recalculate smooth normals
    std::vector<glm::vec3> smoothNormals(planetMeshData.vertices.size(), glm::vec3(0.0f));
    
    for (size_t i = 0; i < planetMeshData.indices.size(); i += 3) {
        int i0 = planetMeshData.indices[i];
        int i1 = planetMeshData.indices[i + 1];
        int i2 = planetMeshData.indices[i + 2];
        
        glm::vec3 v0 = planetMeshData.vertices[i0].position;
        glm::vec3 v1 = planetMeshData.vertices[i1].position;
        glm::vec3 v2 = planetMeshData.vertices[i2].position;
        
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        smoothNormals[i0] += normal;
        smoothNormals[i1] += normal;
        smoothNormals[i2] += normal;
    }
    
    for (size_t i = 0; i < planetMeshData.vertices.size(); i++) {
        planetMeshData.vertices[i].normal = glm::normalize(smoothNormals[i]);
    }
    
    // Create mesh
    if (planetMesh) delete planetMesh;
    planetMesh = new Mesh(planetMeshData);
    
    // Generate atmosphere
    atmosphereMeshData = generateAtmosphere();
    if (atmosphereMesh) delete atmosphereMesh;
    atmosphereMesh = new Mesh(atmosphereMeshData);
}

MeshData ProceduralPlanet::generateAtmosphere() {
    // Create a slightly larger sphere for atmosphere
    float atmosphereRadius = settings.radius * (1.0f + settings.mountainHeight) + settings.atmosphereThickness;
    
    MeshData meshData = generateIcosahedron(atmosphereRadius);
    subdivideMesh(meshData, 3);  // Less detailed than planet
    
    // Set atmosphere color
    for (auto& vertex : meshData.vertices) {
        vertex.color = settings.atmosphereColor;
    }
    
    return meshData;
}

void ProceduralPlanet::draw(bool drawAtmosphere) {
    if (planetMesh) {
        if (planetMaterial) planetMaterial->apply();
        planetMesh->draw();
    }
    
    if (drawAtmosphere && atmosphereMesh) {
        if (atmosphereMaterial) atmosphereMaterial->apply();
        
        // Enable blending for atmosphere
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);  // Don't write to depth buffer
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);  // Cull front faces, render back faces
        
        atmosphereMesh->draw();
        
        // Restore state
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}

float ProceduralPlanet::getHeightAt(const glm::vec3& worldPos) const {
    glm::vec3 localPos = worldPos - position;
    float distance = glm::length(localPos);
    return distance - settings.radius;
}

glm::vec3 ProceduralPlanet::getNormalAt(const glm::vec3& worldPos) const {
    glm::vec3 localPos = worldPos - position;
    return glm::normalize(localPos);
}

bool ProceduralPlanet::isUnderWater(const glm::vec3& worldPos) const {
    glm::vec3 localPos = worldPos - position;
    float distance = glm::length(localPos);
    float waterRadius = settings.radius * (1.0f - settings.mountainHeight + settings.waterLevel * settings.mountainHeight);
    return distance < waterRadius;
}