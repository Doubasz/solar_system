#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "SkyBox.h"
#include "Planet.h"
#include "Player.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class World {
private:
    
    // Shaders
    Shader* sunShader;
    Shader* planetShader;

    // Scene objects
    Planet* planet1;
    Planet* sun;
    Player* player;
    Camera* camera;
    SkyBox* skybox;
    

public:
    World();
    ~World();

    // Main loop methods
    void toggleCameraMode();
    void handleInput(int input, glm::vec2 mousePos, float dt);
    void update(float deltaTime);
    void render(float dt, float aspect);

    // Accessors
    Camera* getCamera() const { return camera; }
};



#endif // WORLD_H