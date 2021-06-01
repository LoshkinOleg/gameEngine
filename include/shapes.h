#pragma once

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

class Rectangle
{
public:
    Rectangle() {};
    void Init(glm::vec2 center, float width, float height, glm::vec3 color, BodyPhysicsType physicsType, float mass)
    {
        const glm::vec2 halfDimensions = glm::vec2(width, height) * 0.5f;
        bottomLeft_ = center - halfDimensions;
        topRight_ = center + halfDimensions;
        type_ = physicsType;
        mass_ = mass;

        shader_ = Shader("../data/shaders/plainColor.vert", "../data/shaders/plainColor.frag");
        shader_.Bind();
        shader_.SetVec3("color", color);
        shader_.SetMat4("perspective", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 1000.0f));
        shader_.SetMat4("model", glm::mat4(1.0f));

        float vertices[18] = {
            -halfDimensions.x,  -halfDimensions.y,  0.0f,
             halfDimensions.x,  -halfDimensions.y,  0.0f,
             halfDimensions.x,   halfDimensions.y,  0.0f,

            -halfDimensions.x,  -halfDimensions.y,  0.0f,
             halfDimensions.x,   halfDimensions.y,  0.0f,
            -halfDimensions.x,   halfDimensions.y,  0.0f
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
        shader_.SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(GetCenter(), 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void PhysicsUpdate(float dt)
    {
        lastDt_ = dt;

        switch (type_)
        {
        case gl::BodyPhysicsType::STATIC:
            break;
        case gl::BodyPhysicsType::DYNAMIC:
        {
            linearVelocity_ += GRAVITY_ * GRAVITY_FACTOR_;
            Move(linearVelocity_);
        }
            break;
        default:
            break;
        }
    }
    void Destroy()
    {
        shader_.Destroy();
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
    void Move(glm::vec2 movement)
    {
        bottomLeft_ += movement * lastDt_;
        topRight_ += movement * lastDt_;
    }
    glm::vec2 GetCenter() const
    {
        return glm::vec2(bottomLeft_ + topRight_) * 0.5f;
    }
    glm::vec2 GetBottomLeft() const
    {
        return bottomLeft_;
    }
    glm::vec2 GetBottomRight() const
    {
        return glm::vec2(topRight_.x, bottomLeft_.y);
    }
    glm::vec2 GetTopRight() const
    {
        return topRight_;
    }
    glm::vec2 GetTopLeft() const
    {
        return glm::vec2(bottomLeft_.x, topRight_.y);
    }
    glm::vec2 GetRight() const
    {
        return glm::normalize(GetBottomRight() - GetBottomLeft());
    }
    glm::vec2 GetUp() const
    {
        return glm::normalize(GetTopLeft() - GetBottomLeft());
    }
    Shader& GetShader()
    {
        return shader_;
    }
    void SetCenter(glm::vec2 pos)
    {
        const glm::vec2 widthAndHeight = glm::vec2(topRight_.x - bottomLeft_.x, topRight_.y - bottomLeft_.y);
        topRight_ = pos + (widthAndHeight * 0.5f);
        bottomLeft_ = pos - (widthAndHeight * 0.5f);
    }
private:

    Shader shader_ = Shader();
    glm::vec2 bottomLeft_ = glm::vec2(0.0);
    glm::vec2 topRight_ = glm::vec2(0.0);
    unsigned int VAO_ = 0, VBO_ = 0;
    BodyPhysicsType type_ = BodyPhysicsType::STATIC;
    const glm::vec2 GRAVITY_ = glm::vec2(-10.0f, -10.0f);
    float mass_ = 1.0f;
    glm::vec2 linearVelocity_ = glm::vec2(0.0f, 0.0f);
    const float GRAVITY_FACTOR_ = 0.1f;
    float lastDt_ = 0.0f;
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

class Triangle
{
public:
    Triangle() {};
    void Init(glm::vec2 points[3], glm::vec2 position, glm::vec3 color)
    {
        for (size_t i = 0; i < 3; i++)
        {
            points_[i] = points[i];
        }
        position_ = position;

        shader_ = Shader("../data/shaders/plainColor.vert", "../data/shaders/plainColor.frag");
        shader_.Bind();
        shader_.SetVec3("color", color);
        shader_.SetMat4("perspective", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 1000.0f));
        shader_.SetMat4("model", glm::mat4(1.0f));

        float vertices[9] = {
            points[0].x, points[0].y, 0.0f,
            points[1].x, points[1].y, 0.0f,
            points[2].x, points[2].y, 0.0f
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
        shader_.SetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(position_, 0.0f)));
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
    void Move(glm::vec2 movement)
    {
        position_ += movement;
    }
    glm::vec2 GetPosition() const
    {
        return position_;
    }
    Shader& GetShader()
    {
        return shader_;
    }
    void SetCenter(glm::vec2 pos)
    {
        position_ = pos;
    }
    glm::vec2 points_[3];
private:
    glm::vec2 position_ = glm::vec2(0.0f);
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

} //!gl