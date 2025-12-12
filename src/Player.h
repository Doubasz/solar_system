#ifndef PLAYER_H
#define PLAYER_H

#include "Planet.h"

class Player{
private:
    glm::vec3 position;
    glm::vec3 velocity;

    float mass; 
    float height;
    bool isGrounded;

    glm::vec3 moveInput;
    bool jumpRequested;

    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;

    glm::vec3 cameraFront;

    float moveSpeed;
    float jumpForce;

    Planet* currentPlanet;

    void updateOrientation(float yaw, float pitch);

public:

    Player(const glm::vec3& startPos, float mass, glm::vec3 velocity, float height);

    void setPlanet(Planet* p);

    void setOnSurface(Planet* p);

    void updatePhysics(float fixedDeltaTime, float cameraYaw, float cameraPitch);
    void update(float deltaTime);
    void applyGravity(float deltaTime);
    void applyCollision();
    void processInput(int input);

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getRight() const { return right; }
    glm::vec3 getCameraFront() const { return cameraFront; }
    bool getIsGrounded() const { return isGrounded; }
    float getHeight() const { return height; }
    void setPosition(const glm::vec3 pos) {position = pos;}
};




#endif