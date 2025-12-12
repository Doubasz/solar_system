

#include "Planet.h"

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <algorithm>

Planet::Planet(){
    //meshData = constructPlanetMesh(1.0f, 32, 16, glm::vec3(1.0f));
    meshData = SphereMesh(3, 1.0f);
    mesh = new Mesh(meshData);
}


Planet::Planet(float radius, glm::vec3 color, Shader* s){
    //meshData = constructPlanetMesh(radius, 32, 32, color);
    meshData = SphereMesh(6, 1.0f);
    mesh = new Mesh(meshData);

    material = new Material(s);

    position = glm::vec3(0.0f);
    this->radius = radius;
}

Planet::~Planet(){
    if(mesh) delete mesh;
    if(material) delete material;
}

void Planet::draw(){
    if(material) material->apply();
    mesh->draw();
}

MeshData Planet::constructPlanetMesh(float radius, int sectorCount, int stackCount, glm::vec3 color){
    int r = color.r;
    int g = color.g;
    int b = color.b;

    MeshData meshData;
    const float PI = 3.14159265359f;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float u, v;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep  = PI / stackCount;

    // ---- Generate vertices ----
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI/2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z  = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;

            // position
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // texture
            u = (float)j / sectorCount;
            v = (float)i / stackCount;

            meshData.vertices.push_back({
                {x, y, z},
                {nx, ny, nz},
                {r, g, b},
                {u, v}
            });
        }
    }

    // ---- Generate indices ----
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {

            if (i != 0) { // top cap
                meshData.indices.push_back(k1);
                meshData.indices.push_back(k2);
                meshData.indices.push_back(k1 + 1);
            }

            if (i != stackCount - 1) { // bottom cap
                meshData.indices.push_back(k1 + 1);
                meshData.indices.push_back(k2);
                meshData.indices.push_back(k2 + 1);
            }
        }
    }

    return meshData;
}


MeshData Planet::isoheadronSphereMesh(int resolution){

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);

    glm::vec3 left    = glm::vec3(-1.0f,  0.0f,  0.0f);
    glm::vec3 right   = glm::vec3( 1.0f,  0.0f,  0.0f);

    glm::vec3 forward = glm::vec3( 0.0f,  0.0f,  1.0f);
    glm::vec3 back    = glm::vec3( 0.0f,  0.0f, -1.0f);

    int vertexPairs[24] = {0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2, 3, 3, 4, 4, 1, 5, 1, 5, 2, 5, 3, 5, 4};
    int edgeTriplets[24] = {0, 1, 4, 1, 2, 5, 2, 3, 6, 3, 0, 7, 8, 9, 4, 9, 10, 5, 10, 11, 6, 11, 8, 7};
    glm::vec3 baseVertices[6] = {up, left, back, right, forward, down};

    int numDivisions = std::max(0, resolution);
    int numVertsPerFace = ((numDivisions + 3) * (numDivisions + 3) - (numDivisions + 3)) / 2;
    int numVerts = numVertsPerFace * 8 - (numDivisions + 2) * 12 + 6;
    int numTrisPerFace = (numDivisions + 1) * (numDivisions + 1);

    MeshData data;


}


// Generates a subdivided octahedron -> sphere mesh with normals, UVs, indices
// Includes simple LOD generation (multiple subdivision levels)




// Helper: combine two uint32 into a single uint64 as a key (order-independent)
// We use to store efficiently the indices and triangles in a hashmap
uint64_t pair_key(uint32_t a, uint32_t b) {
    if (a < b) return (uint64_t(a) << 32) | uint64_t(b);
    else      return (uint64_t(b) << 32) | uint64_t(a);
}

// normalize and scale helper
glm::vec3 normalize_scale(const glm::vec3 &v, float scale) {
    return glm::normalize(v) * scale;
}

// compute equirectangular UVs from a point on sphere (assumes point is normalized)
glm::vec2 spherical_uv(const glm::vec3 &n) {
    // longitude (theta) and latitude (phi)
    float u = 0.5f + (std::atan2(n.z, n.x) / (2.0f * glm::pi<float>()));
    float v = 0.5f - (std::asin(n.y) / glm::pi<float>());
    return glm::vec2(u, v);
}

// Subdivide triangle list once using midpoint cache to avoid duplicate vertices
void subdivide_once(std::vector<uint32_t> &indices,
                           std::vector<glm::vec3> &positions,
                           std::unordered_map<uint64_t, uint32_t> &midpointCache) {
    std::vector<uint32_t> newIndices;
    newIndices.reserve(indices.size() * 4);

    for (size_t i = 0; i < indices.size(); i += 3) {
        uint32_t i0 = indices[i+0];
        uint32_t i1 = indices[i+1];
        uint32_t i2 = indices[i+2];

        auto getMid = [&](uint32_t a, uint32_t b)->uint32_t {
            uint64_t k = pair_key(a,b);
            auto it = midpointCache.find(k);
            if (it != midpointCache.end()) return it->second;

            glm::vec3 mid = positions[a] + positions[b];
            mid *= 0.5f;
            uint32_t newIndex = (uint32_t)positions.size();
            positions.push_back(mid);
            midpointCache[k] = newIndex;
            return newIndex;
        };

        uint32_t a = getMid(i0, i1);
        uint32_t b = getMid(i1, i2);
        uint32_t c = getMid(i2, i0);

        // Create 4 new triangles
        newIndices.push_back(i0); newIndices.push_back(a); newIndices.push_back(c);
        newIndices.push_back(i1); newIndices.push_back(b); newIndices.push_back(a);
        newIndices.push_back(i2); newIndices.push_back(c); newIndices.push_back(b);
        newIndices.push_back(a);  newIndices.push_back(b); newIndices.push_back(c);
    }

    indices.swap(newIndices);
}

// Create a sphere mesh from an octahedron base. 'subdivisions' is the LOD level.
// Higher subdivisions => more triangles (exponential). For example:
// subdivisions=0 -> octahedron (8 faces), 1 -> 32 faces, 2 -> 128 faces, etc.
MeshData build_sphere_from_octahedron(int subdivisions, float radius){
    MeshData mesh;

    // --------------------------
    // Base octahedron vertices
    // --------------------------
    std::vector<glm::vec3> positions =
    {
        { 0,  1,  0 }, // 0 up
        {-1,  0,  0 }, // 1 left
        { 0,  0, -1 }, // 2 back
        { 1,  0,  0 }, // 3 right
        { 0,  0,  1 }, // 4 forward
        { 0, -1,  0 }  // 5 down
    };

    // Base octahedron faces
    std::vector<uint32_t> indices =
    {
        // top pyramid (up is vertex 0)
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,1,
        // bottom pyramid (down is vertex 5)
        5,2,1,
        5,3,2,
        5,4,3,
        5,1,4
    };


    // Subdivision
    std::unordered_map<uint64_t, uint32_t> midpointCache;

    for (int i = 0; i < subdivisions; ++i){
        subdivide_once(indices, positions, midpointCache);
        midpointCache.clear();
    }


    // Create normals + UVs
    std::vector<glm::vec3> normals(positions.size());
    std::vector<glm::vec2> uvs(positions.size());

    for (size_t i = 0; i < positions.size(); ++i){
        glm::vec3 n = glm::normalize(positions[i]);
        positions[i] = n * radius;
        normals[i]   = n;
        uvs[i]       = spherical_uv(n);
    }


    // UV seam fix
    std::vector<uint32_t> fixedIndices;
    fixedIndices.reserve(indices.size());

    auto duplicateVertex = [&](uint32_t idx){
        uint32_t newIndex = (uint32_t)positions.size();
        positions.push_back(positions[idx]);
        normals.push_back(normals[idx]);

        glm::vec2 uv = uvs[idx];
        uv.x += 1.0f; // shift over the seam
        uvs.push_back(uv);

        return newIndex;
    };

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t a = indices[i];
        uint32_t b = indices[i+1];
        uint32_t c = indices[i+2];

        float ua = uvs[a].x;
        float ub = uvs[b].x;
        float uc = uvs[c].x;

        float maxU = std::max({ua, ub, uc});
        float minU = std::min({ua, ub, uc});

        // triangle crosses seam
        if (maxU - minU > 0.5f)
        {
            if (ua < 0.5f) a = duplicateVertex(a);
            if (ub < 0.5f) b = duplicateVertex(b);
            if (uc < 0.5f) c = duplicateVertex(c);
        }

        fixedIndices.push_back(a);
        fixedIndices.push_back(b);
        fixedIndices.push_back(c);
    }

    indices.swap(fixedIndices);

    // Fill MeshData
    mesh.vertices.reserve(positions.size());

    for (size_t i = 0; i < positions.size(); ++i)
    {
        mesh.vertices.push_back({
            positions[i],
            normals[i],
            glm::vec3(1.0f), // white color
            uvs[i]           // texCoord
        });
    }

    mesh.indices = std::move(indices);

    return mesh;
}


// Public interface: generate a sphere mesh with resolution and radius
// 'resolution' here means the number of subdivisions. 0 => octahedron, 1 => subdivided once, etc.
MeshData SphereMesh(int resolution, float radius) {
    if (resolution < 0) resolution = 0;
    // clamp resolution to avoid exploding triangle counts
    if (resolution > 6) resolution = 6; // 6 subdivisions -> ~32768 triangles (approx)
    return build_sphere_from_octahedron(resolution, radius);
}


// Generate multiple LODs (from highRes down to lowRes).
// returns vector where index 0 is highest LOD (highRes) and last element is lowest.
std::vector<MeshData> GenerateLODs(int highRes, int lodCount, float radius) {
    std::vector<MeshData> lods;
    if (highRes < 0) highRes = 0;
    for (int i = 0; i < lodCount; ++i) {
        int res = std::max(0, highRes - i);
        lods.push_back(SphereMesh(res, radius));
    }
    return lods;
}


// Example usage (pseudo):
// MeshData planetMesh = SphereMesh(3, 6000.0f);
// auto lods = GenerateLODs(4, 3, 6000.0f); // LODs with subdivisions 4,3,2

// Notes & tips:
// - Subdivision grows triangles by ~4x per subdivision level. Keep resolutions small for realtime.
// - For very large planets, use LODs and horizon/atmosphere tricks so you don't need extreme tessellation.
// - For GPU tessellation, consider sending a low-res base mesh and tessellating on the GPU.
// - Tangent vectors aren't computed here. If you need normal mapping, add tangent calculation per vertex.

