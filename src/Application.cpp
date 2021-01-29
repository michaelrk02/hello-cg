#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Application.h"
#include "Transform.h"
#include "Utility.h"

#include <glm/ext/matrix_transform.hpp>

Application::Application(void) :
    window(NULL),
    effect(0),
    objectVAO(0), objectVBO(0), objectEBO(0),
    objectTransform(NULL),
    objectTransformUV(-1), viewingTransformUV(-1),
    cameraTransform(NULL),
    projectionTransform(NULL),
    cameraPositionUV(-1),
    planeDistanceUV(-1),
    lightPositionUV(-1)
{
}

Application::~Application(void) {
    this->destroyScene();
    this->destroyResources();
    this->destroyInstance();
}

void Application::run(void) {
    this->initInstance();
    this->initResources();
    this->initScene();

    float step = 1.0f;
    while (!glfwWindowShouldClose(this->window)) {
        double start = glfwGetTime();
        this->update(step);
        this->render();
        double stop = glfwGetTime();
        step = (float)(stop - start);
    }
}

void Application::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    return ((Application *)glfwGetWindowUserPointer(window))->onFramebufferSize(width, height);
}

void Application::keyActionCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    return ((Application *)glfwGetWindowUserPointer(window))->onKeyAction(key, scancode, action, mods);
}

void Application::initInstance(void) {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialization failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if ((this->window = glfwCreateWindow(800, 600, "hello-cg", NULL, NULL)) == NULL) {
        throw std::runtime_error("window creation failed");
    }

    glfwSetWindowUserPointer(this->window, this);
    glfwSetFramebufferSizeCallback(this->window, Application::framebufferSizeCallback);
    glfwSetKeyCallback(this->window, Application::keyActionCallback);
    glfwMakeContextCurrent(this->window);

    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW initialization failed");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
}

void Application::initResources(void) {
    GLuint vs = Utility::compileShader(GL_VERTEX_SHADER, Utility::readFileContents("assets/default_vs.glsl"));
    GLuint fs = Utility::compileShader(GL_FRAGMENT_SHADER, Utility::readFileContents("assets/default_fs.glsl"));

    GLint linkStatus;
    this->effect = glCreateProgram();
    glAttachShader(this->effect, vs);
    glAttachShader(this->effect, fs);
    glLinkProgram(this->effect);
    glGetProgramiv(this->effect, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        std::ostringstream message;
        message << "shader linkage failed" << std::endl;
        char linkLog[2048];
        glGetProgramInfoLog(this->effect, 2048, NULL, linkLog);
        message << linkLog;
        throw std::runtime_error(message.str().c_str());
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    this->objectTransformUV = glGetUniformLocation(this->effect, "u_objectTransform");
    this->viewingTransformUV = glGetUniformLocation(this->effect, "u_viewingTransform");
    this->cameraPositionUV = glGetUniformLocation(this->effect, "u_cameraPosition");
    this->lightPositionUV = glGetUniformLocation(this->effect, "u_lightPosition");
    this->planeDistanceUV = glGetUniformLocation(this->effect, "u_planeDistance");

    glm::vec3 diffuse = glm::vec3(0.5f, 0.2f, 0.0f);

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 back = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 left = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 bottom = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 top = glm::vec3(0.0f, 1.0f, 0.0f);

    Vertex vertices[] = {
        // front face
        /* 0 */ {{-0.5f, 0.5f, -0.5f}, diffuse, front},
        /* 1 */ {{0.5f, 0.5f, -0.5f}, diffuse, front},
        /* 2 */ {{-0.5f, -0.5f, -0.5f}, diffuse, front},
        /* 3 */ {{0.5f, -0.5f, -0.5f}, diffuse, front},

        // back face
        /* 4 */ {{0.5f, 0.5f, 0.5f}, diffuse, back},
        /* 5 */ {{-0.5f, 0.5f, 0.5f}, diffuse, back},
        /* 6 */ {{0.5f, -0.5f, 0.5f}, diffuse, back},
        /* 7 */ {{-0.5f, -0.5f, 0.5f}, diffuse, back},

        // left face
        /* 8 */ {{-0.5f, 0.5f, 0.5f}, diffuse, left},
        /* 9 */ {{-0.5f, 0.5f, -0.5f}, diffuse, left},
        /* 10 */ {{-0.5f, -0.5f, 0.5f}, diffuse, left},
        /* 11 */ {{-0.5f, -0.5f, -0.5f}, diffuse, left},

        // right face
        /* 12 */ {{0.5f, 0.5f, -0.5f}, diffuse, right},
        /* 13 */ {{0.5f, 0.5f, 0.5f}, diffuse, right},
        /* 14 */ {{0.5f, -0.5f, -0.5f}, diffuse, right},
        /* 15 */ {{0.5f, -0.5f, 0.5f}, diffuse, right},

        // bottom face
        /* 16 */ {{-0.5f, -0.5f, -0.5f}, diffuse, bottom},
        /* 17 */ {{0.5f, -0.5f, -0.5f}, diffuse, bottom},
        /* 18 */ {{-0.5f, -0.5f, 0.5f}, diffuse, bottom},
        /* 19 */ {{0.5f, -0.5f, 0.5f}, diffuse, bottom},

        // top face
        /* 20 */ {{-0.5f, 0.5f, 0.5f}, diffuse, top},
        /* 21 */ {{0.5f, 0.5f, 0.5f}, diffuse, top},
        /* 22 */ {{-0.5f, 0.5f, -0.5f}, diffuse, top},
        /* 23 */ {{0.5f, 0.5f, -0.5f}, diffuse, top},
    };

    GLuint indices[] = {
        // front
        2, 1, 0,
        1, 2, 3,

        // back
        6, 5, 4,
        5, 6, 7,

        // left
        10, 9, 8,
        9, 10, 11,

        // right
        14, 13, 12,
        13, 14, 15,

        // bottom
        18, 17, 16,
        17, 18, 19,

        // top
        22, 21, 20,
        21, 22, 23
    };

    glGenVertexArrays(1, &this->objectVAO);
    glBindVertexArray(this->objectVAO);

    glGenBuffers(1, &this->objectVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->objectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    glGenBuffers(1, &this->objectEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->objectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::vec3 axisColor = glm::vec3(1.0f, 0.0f, 1.0f);
    Vertex axisVertices[] = {
        {{-1.0f, 0.0f, 0.0f}, axisColor, {-1.0f, 0.0f, 0.0f}},
        {{1.0f, 0.0f, 0.0f}, axisColor, {1.0f, 0.0f, 0.0f}},
        {{0.0f, -1.0f, 0.0f}, axisColor, {0.0f, -1.0f, 0.0f}},
        {{0.0f, 1.0f, 0.0f}, axisColor, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.0f, -1.0f}, axisColor, {0.0f, 0.0f, -1.0f}},
        {{0.0f, 0.0f, 1.0f}, axisColor, {0.0f, 0.0f, 1.0f}},
    };

    GLuint axisIndices[] = {
        0, 1,
        2, 3,
        4, 5
    };

    glGenVertexArrays(1, &this->axisVAO);
    glBindVertexArray(this->axisVAO);

    glGenBuffers(1, &this->axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STREAM_DRAW);

    glGenBuffers(1, &this->axisEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->axisEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axisIndices), axisIndices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Application::initScene(void) {
    this->objectTransform = new WorldTransform;
    this->axisTransform = new WorldTransform;
    this->cameraTransform = new CameraTransform;
    this->projectionTransform = new ProjectionTransform;

    this->axisTransform->scale = glm::vec3(2.0f, 2.0f, 2.0f);
    this->axisTransform->commit();

    this->cameraTransform->position = glm::vec3(1.0f, 1.0f, -1.0f);
    this->cameraTransform->direction = glm::vec3(-0.5774f, -0.5774f, 0.5774f);
    this->cameraTransform->commit();

    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    this->projectionTransform->aspectRatio = (float)width / (float)height;
    this->projectionTransform->nearPlane = 0.1f;
    this->projectionTransform->commit();

    this->commitViewingTransform();

    glUseProgram(this->effect);
    glUniform3f(this->lightPositionUV, 1.0f, 1.0f, -1.0f);
    glUniform1f(this->planeDistanceUV, this->projectionTransform->farPlane - this->projectionTransform->nearPlane);
}

void Application::update(float time) {
    this->objectTransform->rotation.y += 0.785398f * time;
    this->objectTransform->commit();

    bool cameraChanged = false;

    if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS) {
        this->cameraTransform->direction = glm::vec3(glm::vec4(this->cameraTransform->direction, 1.0f) * glm::rotate(glm::identity<glm::mat4>(), 1.047197f * time, glm::normalize(glm::cross(this->cameraTransform->up, this->cameraTransform->direction))));
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        this->cameraTransform->direction = glm::vec3(glm::vec4(this->cameraTransform->direction, 1.0f) * glm::rotate(glm::identity<glm::mat4>(), 1.047197f * time, glm::normalize(glm::cross(this->cameraTransform->direction, this->cameraTransform->up))));
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        this->cameraTransform->direction = glm::vec3(glm::vec4(this->cameraTransform->direction, 1.0f) * glm::rotate(glm::identity<glm::mat4>(), 1.047197f * time, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        this->cameraTransform->direction = glm::vec3(glm::vec4(this->cameraTransform->direction, 1.0f) * glm::rotate(glm::identity<glm::mat4>(), 1.047197f * time, glm::vec3(0.0f, -1.0f, 0.0f)));
        cameraChanged = true;
    }

    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
        this->cameraTransform->position += this->cameraTransform->direction * time;
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
        this->cameraTransform->position += -this->cameraTransform->direction * time;
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
        this->cameraTransform->position += glm::normalize(glm::cross(this->cameraTransform->direction, this->cameraTransform->up)) * time;
        cameraChanged = true;
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
        this->cameraTransform->position += glm::normalize(glm::cross(this->cameraTransform->up, this->cameraTransform->direction)) * time;
        cameraChanged = true;
    }

    if (cameraChanged) {
        this->cameraTransform->commit();
        this->commitViewingTransform();
    }

    glfwPollEvents();
}

void Application::render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform3fv(this->cameraPositionUV, 1, (const float *)&this->cameraTransform->position);
    glUniformMatrix4fv(this->viewingTransformUV, 1, GL_FALSE, (const float *)&this->viewingMatrix);

    glUniformMatrix4fv(this->objectTransformUV, 1, GL_FALSE, (const float *)&this->axisTransform->getMatrix());
    glBindVertexArray(this->axisVAO);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    glUniformMatrix4fv(this->objectTransformUV, 1, GL_FALSE, (const float *)&this->objectTransform->getMatrix());
    glBindVertexArray(this->objectVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    glfwSwapBuffers(this->window);
}

void Application::destroyScene(void) {
    if (this->objectTransform != NULL) {
        delete this->objectTransform;
    }
    if (this->axisTransform != NULL) {
        delete this->axisTransform;
    }
    if (this->cameraTransform != NULL) {
        delete this->cameraTransform;
    }
    if (this->projectionTransform != NULL) {
        delete this->projectionTransform;
    }
}

void Application::destroyResources(void) {
    glDeleteVertexArrays(1, &this->objectVAO);
    glDeleteBuffers(1, &this->objectEBO);
    glDeleteBuffers(1, &this->objectVBO);

    glDeleteVertexArrays(1, &this->axisVAO);
    glDeleteBuffers(1, &this->axisEBO);
    glDeleteBuffers(1, &this->axisVBO);

    glDeleteProgram(this->effect);
}

void Application::destroyInstance(void) {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Application::commitViewingTransform(void) {
    this->viewingMatrix = this->cameraTransform->getMatrix() * this->projectionTransform->getMatrix();
}

void Application::onFramebufferSize(int width, int height) {
    glViewport(0, 0, width, height);

    this->projectionTransform->aspectRatio = (float)width / (float)height;
    this->projectionTransform->commit();
    this->commitViewingTransform();
}

void Application::onKeyAction(int key, int scancode, int action, int mods) {
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_SPACE)) {
        glm::vec3 camPos = this->cameraTransform->position;
        glm::vec3 camDir = this->cameraTransform->direction;
        std::cout << "P = [" << camPos.x << ", " << camPos.y << ", " << camPos.z << "]" << std::endl;
        std::cout << "D = [" << camDir.x << ", " << camDir.y << ", " << camDir.z << "]" << std::endl;
    }
}

