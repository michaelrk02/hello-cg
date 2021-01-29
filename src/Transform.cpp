#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

////////////////////////////////////////////////////////////
// Transform implementation

Transform::Transform(void) {
    this->matrix = glm::identity<glm::mat4>();
}

const glm::mat4 &Transform::getMatrix(void) const {
    return this->matrix;
}

////////////////////////////////////////////////////////////
// WorldTransform implementation

WorldTransform::WorldTransform(void) {
    this->scale = glm::vec3(1.0f);
    this->commit();
}

void WorldTransform::commit(void) {
    glm::mat4 scaling = glm::mat4(
        this->scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, this->scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, this->scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glm::vec3 sines = glm::sin(this->rotation);
    glm::vec3 cosines = glm::cos(this->rotation);

    glm::mat4 mrotx = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosines.x, -sines.x, 0.0f,
        0.0f, sines.x, cosines.x, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glm::mat4 mroty = glm::mat4(
        cosines.y, 0.0f, sines.y, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sines.y, 0.0f, cosines.y, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glm::mat4 mrotz = glm::mat4(
        cosines.z, -sines.z, 0.0f, 0.0f,
        sines.z, cosines.z, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glm::mat4 rotation = mrotx * mroty * mrotz;

    glm::mat4 translation = glm::mat4(
        1.0f, 0.0f, 0.0f, this->position.x,
        0.0f, 1.0f, 0.0f, this->position.y,
        0.0f, 0.0f, 1.0f, this->position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    this->matrix = scaling * rotation * translation;
}

////////////////////////////////////////////////////////////
// CameraTransform implementation

CameraTransform::CameraTransform(void) {
    this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->commit();
}

void CameraTransform::commit(void) {
    glm::vec3 w = glm::normalize(this->direction);
    glm::vec3 u = glm::normalize(glm::cross(this->up, w));
    glm::vec3 v = glm::cross(w, u);

    glm::mat4 move = glm::mat4(
        1.0f, 0.0f, 0.0f, -this->position.x,
        0.0f, 1.0f, 0.0f, -this->position.y,
        0.0f, 0.0f, 1.0f, -this->position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    glm::mat4 align = glm::mat4(
        u.x, u.y, u.z, 0.0f,
        v.x, v.y, v.z, 0.0f,
        w.x, w.y, w.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    this->matrix = move * align;
}

////////////////////////////////////////////////////////////
// ProjectionTransform implementation

ProjectionTransform::ProjectionTransform(void) {
    this->aspectRatio = 1.0f;
    this->fov = 1.221730f;
    this->nearPlane = 1.0f;
    this->farPlane = 100.0f;
    this->commit();
}

void ProjectionTransform::commit(void) {
    float tanHalfFOV = glm::tan(this->fov * 0.5f);
    float n = this->nearPlane;
    float f = this->farPlane;

    this->matrix = glm::mat4(
        1.0f / (this->aspectRatio * tanHalfFOV), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f,
        0.0f, 0.0f, (f + n) / (n - f), 2.0f * f * n / (f - n),
        0.0f, 0.0f, 1.0f, 0.0f
    );
}

