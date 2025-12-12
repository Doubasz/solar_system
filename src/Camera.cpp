
#include "Camera.h"



Camera::Camera() : position(glm::vec3(0.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(-90.0f), pitch(0.0f),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), movementSpeed(5.0f), mouseSensitivity(0.1f), fov(45.0f), 
    freeVelocity(0.0f), mode(FREE_MODE), attachedPlayer(nullptr)
{
    updateCameraVectors();
}

Camera::Camera(const glm::vec3 startPos) : Camera() {
    position = startPos;
}

Camera::~Camera(){

}

void Camera::updateCameraVectors(){
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    if(mode == FREE_MODE){
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
    
}

void Camera::setMode(CameraMode newMode){
    mode = newMode;
    freeVelocity = glm::vec3(0.0f);
}

CameraMode Camera::getMode() const{
    return mode;
}

glm::vec3 Camera::getPosition() const{
    if (mode == PLAYER_MODE && attachedPlayer) {
        glm::vec3 playerPos = attachedPlayer->getPosition();
        glm::vec3 playerUp = attachedPlayer->getUp();
        return playerPos + playerUp * (attachedPlayer->getHeight() * 0.9f);
    }
    return position;
}

glm::mat4 Camera::getViewMatrix() const{
    if (mode == PLAYER_MODE && attachedPlayer) {
        // Camera follows player with offset
        glm::vec3 playerPos = attachedPlayer->getPosition();
        glm::vec3 playerUp = attachedPlayer->getUp();
        glm::vec3 playerCameraFront = attachedPlayer->getCameraFront();  // Use camera front, not movement front
        glm::vec3 cameraPos = playerPos + playerUp * (attachedPlayer->getHeight() * 0.9f);
        
        // Use player's camera orientation (includes pitch)
        return glm::lookAt(cameraPos, cameraPos + playerCameraFront, playerUp);
    } else {
        // Free camera
        return glm::lookAt(position, position + front, up);
    }
}

void Camera::processKeyboard(int direction, float deltaTime){
    if (mode == FREE_MODE) {
        float vel = movementSpeed * deltaTime;
        
        if (direction == 0) position += front * vel;  // Z
        if (direction == 1) position -= front * vel;  // S
        if (direction == 2) position -= right * vel;  // Q
        if (direction == 3) position += right * vel;  // D
        if (direction == 4) position += worldUp * vel; // Space
        if (direction == 5) position -= worldUp * vel; // Shift

        attachedPlayer->setPosition(position);
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch){
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    
    updateCameraVectors();
}

void Camera::update(){
    updateCameraVectors();
}

void Camera::attachToPlayer(Player* p){
    attachedPlayer = p;
}