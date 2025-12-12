

#include "glad/glad.h"
#include "Material.h"
#include <iostream>


Material::Material(Shader* s){
    shader = s;
}

bool Material::uniformExists(const std::string& name){
    return shader->hasUniform(name);
}

void Material::warnIfMissing(const std::string& name){
    if(!uniformExists(name)){
        std::cerr << "[Material Warning] Uniform '" << name 
                      << "' does not exist in shader '" 
                      << shader->ID << "'\n";
    }
}

void Material::set(const std::string& name, float f){
    warnIfMissing(name);
    floats[name] = f;
}

void Material::set(const std::string& name, glm::vec3 v){
    warnIfMissing(name);
    vec3s[name] = v;
}

void Material::set(const std::string& name, GLuint tex){
    warnIfMissing(name);
    textures[name] = tex;
}

void Material::apply(){
    if(!shader) return;

    //shader->use();

    for(auto& kv : floats) shader->setFloat(kv.first, kv.second);
    for(auto& kv : vec3s) shader->setVec3(kv.first, kv.second);

    int unit = 0;
    for(auto& kv : textures){
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, kv.second);
        shader->setInt(kv.first, unit);
        unit++;
    }
    
}