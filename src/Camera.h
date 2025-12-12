#ifndef CAMERA_H
#define CAMERA_H

#include "Player.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>

enum CameraMode {
    FREE_MODE,
    PLAYER_MODE
};

class Camera {
private:
    // Orientation
    glm::vec3 position;
    glm::vec3 front, up, right;
    glm::vec3 worldUp;

    float yaw, pitch;

    glm::vec3 freeVelocity;

    CameraMode mode;
    Player* attachedPlayer;

    bool firstMouse = true;
    

    
    void updateCameraVectors();

public:
    float movementSpeed;
    float mouseSensitivity;
    float fov;

    Camera();
    Camera(const glm::vec3 startPos);
    ~Camera();

    void setMode(CameraMode newMode);
    CameraMode getMode() const;

    void attachToPlayer(Player* p);
    void processKeyboard(int direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch);

    void update();

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
};

#endif
