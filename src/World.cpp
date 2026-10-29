
#include "World.h"
#include <iostream>

World::World() {

    camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));

    sunShader = new Shader("shaders/vertex.glsl", "shaders/sun.frag");
    skybox = new SkyBox("shaders/skybox.vert", "shaders/skybox.frag");

    glm::vec3 sunColor = {1.0f, 0.75f, 0.2f};
    
    sun = new Planet(10.0f, {0.0f , 0.5f, 1.0f}, sunShader);
    sun->position = glm::vec3(0.0, 0.0, -20.0f);
    sun->radius = 10.0f;
    sun->material->set("sunColor", sunColor);
    sun->material->set("intensity", 2.0f);

    planetShader = new Shader("shaders/vertex.glsl", "shaders/planet.frag");

    planet1 = new Planet(5.0f, {0.0f, 0.5f, 1.0f}, planetShader);
    planet1->position = glm::vec3(0.0f, 0.0f, 20.0f);
    planet1->radius = 5.0f;

    glm::vec3 sunPos = {0.0f, 0.0f, -5.0f};

    planet1->material->set("sunPos", sun->position);
    planet1->material->set("sunColor", sunColor);
    planet1->material->set("sunIntensity", 2.0f);
    planet1->material->set("planetColor", glm::vec3{0.0f, 0.5f, 1.0f});
    planet1->material->set("sunRadius", 10.0f);
    planet1->material->set("overrideColor", glm::vec3(0.0f));


    glm::vec3 spawnDirection = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    float spawnDistance = planet1->radius + 1.8f;  // radius + player height
    glm::vec3 playerStartPos = planet1->position + spawnDirection * spawnDistance;
    playerStartPos = glm::vec3(70.0f, 12.0f, 20.0f);
    

    player = new Player(playerStartPos, 70.0f, glm::vec3(0.0f), 1.8f);
    player->setPlanet(planet1);

    camera->attachToPlayer(player);
    camera->setMode(FREE_MODE);

    // Create shaders
    planetShader = new Shader("shaders/vertex.glsl", 
                                "shaders/planet.frag");

}




void World::render(float dt, float aspect){
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // light sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->fov),
        aspect,
        0.1f,
        2000000.0f
    );

    skybox->draw(view, projection, false);


    planetShader->use();
    planetShader->setMat4("view", view);
    planetShader->setMat4("projection", projection);

    glm::mat4 planetModel = glm::translate(glm::mat4(1.0), planet1->position);
    planetModel = glm::scale(planetModel, glm::vec3(planet1->radius));
    planetShader->setMat4("model", planetModel);

    glm::mat4 mvp = projection * view * planetModel;
    
    planet1->draw();


    sunShader->use();
    sunShader->setMat4("view", view);
    sunShader->setMat4("projection", projection);

    planetModel = glm::translate(glm::mat4(1.0), sun->position);
    planetModel = glm::scale(planetModel, glm::vec3(sun->radius));
    sunShader->setMat4("model", planetModel);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive blending
    glDepthMask(GL_FALSE);

    sun->draw();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}


void World::update(float deltaTime){
    if(camera->getMode() == PLAYER_MODE){
        player->updatePhysics(deltaTime, camera->getYaw(), camera->getPitch());
    }
}


void World::toggleCameraMode(){
    if(camera->getMode() == FREE_MODE){
        player->setPosition(camera->getPosition());
        player->setOnSurface(planet1);
        camera->setMode(PLAYER_MODE);
    } else{
        camera->setMode(FREE_MODE);
    }
}


void World::handleInput(int input, glm::vec2 mousePos, float dt){
    // Only process mouse movement if input is -1 (no keyboard input)
    // or if it's the first input in the frame
    static bool mouseProcessedThisFrame = false;
    
    if(input == -1 || !mouseProcessedThisFrame) {
        camera->processMouseMovement(mousePos.x, mousePos.y, true);
        mouseProcessedThisFrame = (input != -1);
    }
    
    // Reset flag at the start of next frame (when input is -1)
    if(input == -1) {
        mouseProcessedThisFrame = false;
    }

    if(input >= 0) {  // Only process keyboard if valid input
        if(camera->getMode() == FREE_MODE){
            camera->processKeyboard(input, dt);
        }
        else{
            player->processInput(input);
        }
    }
    //player->setPosition(camera->getPosition());

}