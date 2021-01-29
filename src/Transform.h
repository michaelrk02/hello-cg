#pragma once

#include <glm/glm.hpp>

class Transform {
public:
    Transform(void);

    virtual void commit(void) = 0;

    const glm::mat4 &getMatrix(void) const;

protected:
    glm::mat4 matrix;
};

class WorldTransform :
    public Transform
{
public:
    WorldTransform(void);

    void commit(void);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class CameraTransform :
    public Transform
{
public:
    CameraTransform(void);

    void commit(void);

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
};

class ProjectionTransform :
    public Transform
{
public:
    ProjectionTransform(void);

    void commit(void);

    float aspectRatio;
    float fov;
    float nearPlane;
    float farPlane;
};

