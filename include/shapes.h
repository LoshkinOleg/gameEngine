#pragma once

// TODO: Refactor this shit

#include <array>

#include "shader.h"

namespace gl {

enum class BodyPhysicsType
{
    STATIC,
    DYNAMIC
};

class Axis
{
public:
    Axis() {}
    void Init()
    {
        // Internal variables.
        shader_ = Shader("../data/shaders/axis.vert", "../data/shaders/axis.frag");

        // Data.
        float axisVertices[18] = {
            0.0f, 0.0f, 0.0f, // X
            1.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f, // Y
            0.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f, // Z
            0.0f, 0.0f, 1.0f
        };

        // Fill out buffers.
        glGenVertexArrays(1, &VAO_);
        glBindVertexArray(VAO_);
        glGenBuffers(1, &VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Set up uniforms.
        glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 1000.0f);
        shader_.Bind();
        shader_.SetMat4("model", glm::mat4(1.0f));
        shader_.SetMat4("perspective", perspective);

        shader_.UnBind();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void Draw(const Camera& camera)
    {
        // Draw world axis.
        shader_.Bind();
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        shader_.SetMat4("view", camera.GetViewMatrix());
        shader_.SetVec3("lineColor", glm::vec3(1.0f, 0.0f, 0.0f));
        glDrawArrays(GL_LINES, 0, 2);
        shader_.SetVec3("lineColor", glm::vec3(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_LINES, 2, 2);
        shader_.SetVec3("lineColor", glm::vec3(0.0f, 0.0f, 1.0f));
        glDrawArrays(GL_LINES, 4, 2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void Destroy()
    {
        shader_.Destroy();
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
private:
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

class Arrow
{
public:
    Arrow() {};
    void Init(glm::vec2 pos, glm::vec2 direction, float length, glm::vec3 color)
    {
        direction = glm::normalize(direction) * length;

        shader_ = Shader("../data/shaders/axis.vert", "../data/shaders/axis.frag");

        const glm::vec2 crossLineHorizontalStart = pos - glm::vec2(0.1f, 0.0f);
        const glm::vec2 crossLineHorizontalEnd = pos + glm::vec2(0.1f, 0.0f);
        const glm::vec2 crossLineVerticalStart = pos - glm::vec2(0.0f, 0.1f);
        const glm::vec2 crossLineVerticalEnd = pos + glm::vec2(0.0f, 0.1f);

        float points[18] = {
             pos.x,                          pos.y,                         0.0f,
            (pos + direction).x,            (pos + direction).y,            0.0f,
            crossLineHorizontalStart.x,     crossLineHorizontalStart.y,     0.0f,
            crossLineHorizontalEnd.x,       crossLineHorizontalEnd.y,       0.0f,
            crossLineVerticalStart.x,       crossLineVerticalStart.y,       0.0f,
            crossLineVerticalEnd.x,         crossLineVerticalEnd.y,         0.0f
        };

        glGenVertexArrays(1, &VAO_);
        glBindVertexArray(VAO_);
        glGenBuffers(1, &VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_.Bind();
        shader_.SetMat4("perspective", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 1000.0f));
        shader_.SetMat4("model", glm::mat4(1.0f));
        shader_.SetVec3("lineColor", color);
        shader_.UnBind();
    }
    void Draw(const Camera& camera)
    {
        shader_.Bind();
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        shader_.SetMat4("view", camera.GetViewMatrix());
        glDrawArrays(GL_LINES, 0, 6);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void Destroy()
    {
        shader_.Destroy();
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
private:
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

struct Transform
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    float rotation = 0.0f;
};

class Triangle
{
public:
    Triangle() {};
    void Init(glm::vec3 points[3], glm::vec3 position, glm::vec3 color, float rotation)
    {
        // Setup transform.
        transform_.pos = position;
        transform_.rotation = rotation;
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos);
        transform_.model = glm::rotate(transform_.model, transform_.rotation, glm::vec3(0.0f, 0.0f, 1.0f));

        // Center the points on the origin (0;0) for consistency sake.
        const glm::vec3 centroid = ((points[0] + points[1] + points[2]) / 3.0f);
        for (size_t i = 0; i < 3; i++)
        {
            points_[i] = points[i] - centroid; // Note that the points are left in local space centered on origin.
        }

        shader_ = Shader("../data/shaders/plainColor.vert", "../data/shaders/plainColor.frag");
        shader_.Bind();
        shader_.SetVec3("color", color);
        shader_.SetMat4("perspective", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 1000.0f));
        shader_.SetMat4("model", transform_.model);

        float vertices[9] = {
            points_[0].x, points_[0].y, 0.0f,
            points_[1].x, points_[1].y, 0.0f,
            points_[2].x, points_[2].y, 0.0f
        };

        glGenVertexArrays(1, &VAO_);
        glBindVertexArray(VAO_);
        glGenBuffers(1, &VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void Draw(const Camera& camera)
    {
        shader_.Bind();
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        shader_.SetMat4("view", camera.GetViewMatrix());
        shader_.SetMat4("model", transform_.model);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void Destroy()
    {
        shader_.Destroy();
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
    void Move(glm::vec3 movement)
    {
        transform_.pos += movement;
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos);
        transform_.model = glm::rotate(transform_.model, transform_.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    Transform GetTransform()
    {
        return transform_;
    }
    Shader& GetShader()
    {
        return shader_;
    }
    std::array<glm::vec3, 3> GetAxis()
    {
        std::array<glm::vec3, 3> returnVal;
        auto points = GetWorldPoints();
        returnVal[0] = glm::normalize(points[1] - points[0]);
        returnVal[1] = glm::normalize(points[2] - points[1]);
        returnVal[2] = glm::normalize(points[0] - points[2]);
        return returnVal;
    }
    std::array<glm::vec3, 3> GetLocalPoints()
    {
        return points_;
    }
    std::array<glm::vec3, 3> GetWorldPoints()
    {
        std::array<glm::vec3, 3> returnVal;
        for (size_t i = 0; i < 3; i++)
        {
            returnVal[i] = transform_.model * glm::vec4(points_[i], 1.0f);
        }
        return returnVal;
    }
    glm::vec3 GetPosition()
    {
        return transform_.pos;
    }
private:
    Transform transform_ = Transform(); // Contains offset for all points, rotation and the model mat representing these transformations.
    std::array<glm::vec3, 3> points_; // triangle has it's centroid on (0;0)
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

} //!gl