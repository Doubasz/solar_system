#ifndef MATERIAL_H
#define MATERIAL_H


#include "Shader.h"
#include <unordered_map> 

class Material{
public:
    Shader* shader;

    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, glm::vec3> vec3s;
    std::unordered_map<std::string, GLuint> textures;

    Material(Shader* s);

    bool uniformExists(const std::string& name);
    void warnIfMissing(const std::string& name);

    void set(const std::string& name, float value);
    void set(const std::string& name, glm::vec3 v);
    void set(const std::string& name, GLuint tex);

    void apply();

};


#endif