#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#pragma pack(push, 1)
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
};
#pragma pack(pop)

class WorldTransform;
class CameraTransform;
class ProjectionTransform;

class Application {
public:
    Application(void);
    ~Application(void);

    void run(void);

private:
    static void framebufferSizeCallback(GLFWwindow *, int, int);
    static void keyActionCallback(GLFWwindow *, int, int, int, int);

    void initInstance(void);
    void initResources(void);
    void initScene(void);
    void update(float);
    void render(void);
    void destroyScene(void);
    void destroyResources(void);
    void destroyInstance(void);

    void commitViewingTransform(void);

    void onFramebufferSize(int, int);
    void onKeyAction(int, int, int, int);

    GLFWwindow *window;

    GLuint effect;

    GLuint objectVAO, objectVBO, objectEBO;
    WorldTransform *objectTransform;

    GLuint axisVAO, axisVBO, axisEBO;
    WorldTransform *axisTransform;

    GLint objectTransformUV, viewingTransformUV;
    CameraTransform *cameraTransform;
    ProjectionTransform *projectionTransform;
    glm::mat4 viewingMatrix;
    GLint cameraPositionUV;
    GLint planeDistanceUV;

    GLint lightPositionUV;
};

