

#include "Player.h"


void Player::updateOrientation(float yaw, float pitch){
    if(currentPlanet){
        glm::vec3 toPlanet = currentPlanet->position - position;
        up = glm::normalize(-toPlanet);

        // Create a local coordinate system aligned with the planet surface
        // Find an arbitrary perpendicular vector to 'up'
        glm::vec3 worldForward = glm::vec3(0.0f, 0.0f, 1.0f);
        if(glm::abs(glm::dot(up, worldForward)) > 0.99f) {
            worldForward = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        
        glm::vec3 tangentRight = glm::normalize(glm::cross(up, worldForward));
        glm::vec3 tangentForward = glm::normalize(glm::cross(tangentRight, up));
        
        // Build rotation around the local up axis (yaw)
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);
        
        glm::vec3 localRight = glm::cos(yawRad) * tangentRight + glm::sin(yawRad) * tangentForward;
        glm::vec3 localForward = -glm::sin(yawRad) * tangentRight + glm::cos(yawRad) * tangentForward;
        
        // Apply pitch rotation around the local right axis
        cameraFront = glm::cos(pitchRad) * localForward + glm::sin(pitchRad) * up;
        cameraFront = glm::normalize(cameraFront);
        
        // For movement, project camera front onto tangent plane
        glm::vec3 movementFront = cameraFront - glm::dot(cameraFront, up) * up;
        
        if(glm::length(movementFront) > 0.01f){
            front = glm::normalize(movementFront);
        } else {
            // Looking straight up or down - use last valid forward direction
            front = localForward;
        }

        right = glm::normalize(glm::cross(front, up));
    }
}

Player::Player(const glm::vec3& startPos, float startMass, glm::vec3 startVelocity, float startHeight)
    : position(startPos), mass(startMass), velocity(startVelocity), height(startHeight)
{
    moveInput = glm::vec3(0.0f);
    isGrounded = false;
    jumpRequested = false;
    moveSpeed = 10.0f;
    jumpForce = 5.0f;
    currentPlanet = nullptr;
    front = glm::vec3(0.0f, 0.0f, 1.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);  // Initialize camera front
    right = glm::vec3(1.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Player::setPlanet(Planet* p){
    currentPlanet = p;
}

void Player::setOnSurface(Planet* p){
    currentPlanet = p;

    glm::vec3 up = glm::normalize(position - p->position);
    if(glm::length(up) < 0.1f){
        up = glm::vec3(0.0f, 1.0f, 0.0f); //Player and planet at the same position, we choose a default up vector
    }

    float surfaceDist = p->radius + height;
    position = p->position + up * surfaceDist;

    velocity = glm::vec3(0.0f);
    isGrounded = true;
}

void Player::updatePhysics(float fixedDeltaTime, float cameraYaw, float cameraPitch){
    if(!currentPlanet) return;

    updateOrientation(cameraYaw, cameraPitch);

    if(glm::length(moveInput) > 0.01){
        glm::vec3 moveDir = glm::normalize(moveInput);
        glm::vec3 moveForce = (front * moveDir.z + right * moveDir.x) * moveSpeed;
        velocity += moveForce * fixedDeltaTime;
    }

    if(jumpRequested && isGrounded){
        velocity += up * jumpForce;
        isGrounded = false;
        jumpRequested = false;
    }

    applyGravity(fixedDeltaTime);

    position += velocity * fixedDeltaTime;

    applyCollision();

    // Damping
    velocity *= 0.95;

    moveInput = glm::vec3(0.0f);
}

void Player::update(float deltaTime){

}

void Player::applyGravity(float deltaTime){
    glm::vec3 toPlanet = currentPlanet->position - position;
    float distance = glm::length(toPlanet);

    if(distance > 0.01){
        glm::vec3 gravityDir = glm::normalize(toPlanet);
        float gravityStrength = 20.0f; // changing to planet.gravity later
        glm::vec3 gravityForce  = gravityDir * gravityStrength * deltaTime;
        velocity += gravityForce;
    }
}

void Player::applyCollision(){
    glm::vec3 toPlanet = currentPlanet->position - position;
    float distance = glm::length(toPlanet);
    float surfaceDistance = currentPlanet->radius + height;

    if(distance <= surfaceDistance) {
        // Push to the surface of the planet
        glm::vec3 normal = glm::normalize(-toPlanet);
        position = currentPlanet->position + normal * surfaceDistance;

        // Remove velocity component toward planet
        float velTowardPlanet = glm::dot(velocity, normal);
        if(velTowardPlanet < -0.01f) {
            velocity -= normal * velTowardPlanet;
        } else {
            velocity = glm::vec3(0.0f);
        }
        
        isGrounded = true;
    }
    else{
        // Check if we're close enough to ground
        if(distance - surfaceDistance < 0.2f){
            isGrounded = true;
        } else {
            isGrounded = false;
        }
    }
}


void Player::processInput(int input){
    if (input == 0) // Z
        moveInput.z += 1.0f;
    if (input == 1) // S
        moveInput.z -= 1.0f;
    if (input == 2) // Q
        moveInput.x -= 1.0f;
    if (input == 3) // D
        moveInput.x += 1.0f;
    
    if (input == 4) // Space
        if(isGrounded){
            jumpRequested = true;
        }
}