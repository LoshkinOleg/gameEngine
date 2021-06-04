#pragma once

// TODO: Refactor this shit

#include <array>
#include <functional>

#include <glm/gtx/quaternion.hpp>

#include "shader.h"
#include "camera.h"

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
        shader_ = Shader("../data/shaders/plain.vert", "../data/shaders/plain.frag");

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
        glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
        shader_.Bind();
        shader_.SetMat4("model", glm::mat4(1.0f));
        shader_.SetMat4("projection", perspective);

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
        shader_.SetVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
        glDrawArrays(GL_LINES, 0, 2);
        shader_.SetVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_LINES, 2, 2);
        shader_.SetVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
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

        shader_ = Shader("../data/shaders/plain.vert", "../data/shaders/plain.frag");

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
        shader_.SetMat4("projection", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f));
        shader_.SetMat4("model", glm::mat4(1.0f));
        shader_.SetVec3("color", color);
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

        shader_ = Shader("../data/shaders/plain.vert", "../data/shaders/plain.frag");
        shader_.Bind();
        shader_.SetVec3("color", color);
        shader_.SetMat4("projection", glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f));
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

struct Transform3D
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat quaternion = glm::quat();
    glm::vec3 rotation = glm::vec3();
};

template <size_t Size>
glm::vec3 AccumulateVec3Array(std::array<glm::vec3, Size> arr)
{
    glm::vec3 returnVal = glm::vec3(0.0f, 0.0f, 0.0f);
    for (size_t i = 0; i < arr.size(); i++)
    {
        returnVal += arr[i];
    }
    return returnVal;
}

class Box
{
public:
    Box() {};
    /*
    @ shadersFilename: Don't include the extension. Ex: "myShader" or "hello_myProject/myShader"
    */
    void Init(std::array<glm::vec3, 8> points, glm::vec3 position, glm::vec3 rotation, const std::string& shadersFilename, std::function<void(Shader)> shaderOnInit, std::function<void(Shader)> shaderOnDraw)
    {
        // Setup transform.
        transform_.pos = position;
        transform_.rotation = rotation;
        transform_.rotation = rotation;
        transform_.quaternion = glm::quat(rotation);
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos) * glm::toMat4(transform_.quaternion);

        // Center the points on the origin (0;0) for consistency sake.
        const glm::vec3 centroid = AccumulateVec3Array<8>(points) / 8.0f;
        for (size_t i = 0; i < 8; i++)
        {
            points_[i] = points[i] - centroid; // Note that the points are left in local space centered on origin.
        }

        shader_ = Shader("../data/shaders/" + shadersFilename + ".vert", "../data/shaders/" + shadersFilename + ".frag");
        shaderOnDraw_ = shaderOnDraw;
        shader_.Bind();
        try
        {
            shaderOnInit(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: failed to call shaderOnInit in Box::Init(): " << e.what() << std::endl;
            abort;
        }
        shader_.UnBind();

        float vertices[108] = {
            // Back
            points_[0].x, points_[0].y, points_[0].z,
            points_[1].x, points_[1].y, points_[1].z,
            points_[2].x, points_[2].y, points_[2].z,

            points_[0].x, points_[0].y, points_[0].z,
            points_[2].x, points_[2].y, points_[2].z,
            points_[3].x, points_[3].y, points_[3].z,

            // Front
            points_[4].x, points_[4].y, points_[4].z,
            points_[5].x, points_[5].y, points_[5].z,
            points_[6].x, points_[6].y, points_[6].z,

            points_[4].x, points_[4].y, points_[4].z,
            points_[6].x, points_[6].y, points_[6].z,
            points_[7].x, points_[7].y, points_[7].z,

            // Left
            points_[0].x, points_[0].y, points_[0].z,
            points_[4].x, points_[4].y, points_[4].z,
            points_[7].x, points_[7].y, points_[7].z,

            points_[0].x, points_[0].y, points_[0].z,
            points_[7].x, points_[7].y, points_[7].z,
            points_[3].x, points_[3].y, points_[3].z,

            // Right
            points_[5].x, points_[5].y, points_[5].z,
            points_[1].x, points_[1].y, points_[1].z,
            points_[2].x, points_[2].y, points_[2].z,

            points_[5].x, points_[5].y, points_[5].z,
            points_[2].x, points_[2].y, points_[2].z,
            points_[6].x, points_[6].y, points_[6].z,

            // Up
            points_[7].x, points_[7].y, points_[7].z,
            points_[6].x, points_[6].y, points_[6].z,
            points_[2].x, points_[2].y, points_[2].z,

            points_[7].x, points_[7].y, points_[7].z,
            points_[2].x, points_[2].y, points_[2].z,
            points_[3].x, points_[3].y, points_[3].z,

            // Down
            points_[0].x, points_[0].y, points_[0].z,
            points_[5].x, points_[5].y, points_[5].z,
            points_[1].x, points_[1].y, points_[1].z,

            points_[0].x, points_[0].y, points_[0].z,
            points_[4].x, points_[4].y, points_[4].z,
            points_[5].x, points_[5].y, points_[5].z,
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
    }
    void Draw()
    {
        shader_.Bind();
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        try
        {
            shaderOnDraw_(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: failed to call shaderOnDraw in Box::Draw(): " << e.what() << std::endl;
            abort;
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);

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
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos) * glm::toMat4(transform_.quaternion);
    }
    Transform3D GetTransform3D()
    {
        return transform_;
    }
    Shader& GetShader()
    {
        return shader_;
    }
    std::array<glm::vec3, 8> GetLocalPoints()
    {
        return points_;
    }
    std::array<glm::vec3, 8> GetWorldPoints()
    {
        std::array<glm::vec3, 8> returnVal;
        for (size_t i = 0; i < 8; i++)
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
    Transform3D transform_ = Transform3D(); // Contains offset for all points, rotation and the model mat representing these transformations.
    std::array<glm::vec3, 8> points_; // cube has it's centroid on (0;0)
    std::function<void(Shader)> shaderOnDraw_ = nullptr;
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

class Quad
{
public:
    Quad() {};
    /*
    @ shadersFilename: Don't include the extension. Ex: "myShader" or "hello_myProject/myShader"
    */
    void Init(std::array<glm::vec3, 4> points, glm::vec3 position, glm::vec3 rotation, const std::string& shadersFilename, std::function<void(Shader)> shaderOnInit, std::function<void(Shader)> shaderOnDraw)
    {
        // Setup transform.
        transform_.pos = position;
        transform_.rotation = rotation;
        transform_.rotation = rotation;
        transform_.quaternion = glm::quat(rotation);
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos) * glm::toMat4(transform_.quaternion);

        // Center the points on the origin (0;0) for consistency sake.
        const glm::vec3 centroid = AccumulateVec3Array<4>(points) / 4.0f;
        for (size_t i = 0; i < 4; i++)
        {
            points_[i] = points[i] - centroid; // Note that the points are left in local space centered on origin.
        }

        shader_ = Shader("../data/shaders/" + shadersFilename + ".vert", "../data/shaders/" + shadersFilename + ".frag");
        shaderOnDraw_ = shaderOnDraw;
        shader_.Bind();
        try
        {
            shaderOnInit(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: failed to call shaderOnInit in Box::Init(): " << e.what() << std::endl;
            abort;
        }
        shader_.UnBind();

        float vertices[18] = {
            points_[0].x, points_[0].y, points_[0].z,
            points_[1].x, points_[1].y, points_[1].z,
            points_[2].x, points_[2].y, points_[2].z,

            points_[0].x, points_[0].y, points_[0].z,
            points_[2].x, points_[2].y, points_[2].z,
            points_[3].x, points_[3].y, points_[3].z
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
    }
    void Draw()
    {
        shader_.Bind();
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        try
        {
            shaderOnDraw_(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: failed to call shaderOnDraw in Box::Draw(): " << e.what() << std::endl;
            abort;
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

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
        transform_.model = glm::translate(glm::mat4(1.0f), transform_.pos) * glm::toMat4(transform_.quaternion);
    }
    Transform3D GetTransform3D()
    {
        return transform_;
    }
    Shader& GetShader()
    {
        return shader_;
    }
    std::array<glm::vec3, 4> GetLocalPoints()
    {
        return points_;
    }
    std::array<glm::vec3, 4> GetWorldPoints()
    {
        std::array<glm::vec3, 4> returnVal;
        for (size_t i = 0; i < 4; i++)
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
    Transform3D transform_ = Transform3D(); // Contains offset for all points, rotation and the model mat representing these transformations.
    std::array<glm::vec3, 4> points_; // quad has it's centroid on (0;0)
    std::function<void(Shader)> shaderOnDraw_ = nullptr;
    Shader shader_ = Shader();
    unsigned int VAO_ = 0, VBO_ = 0;
};

} //!gl