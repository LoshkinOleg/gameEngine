#include <iostream>
#include <limits>

// #define IDENTITY glm::mat4(1.0f)
// #define RIGHT glm::vec3(1.0f, 0.0f, 0.0f)
// #define UP glm::vec3(0.0f, 1.0f, 0.0f)
// #define FORWARD glm::vec3(0.0f, 0.0f, 1.0f)
// #define ZERO glm::vec3(0.0f, 0.0f, 0.0f)
// #define ONE glm::vec3(1.0f, 1.0f, 1.0f)

// TODO: Refactor this shit

#include "engine.h"
#include "camera.h"
#include "shapes.h"

namespace gl {

class HelloTriangle : public Program
{
public:
    void Init() override;
    void Update(seconds dt) override;
    void Destroy() override;
    void OnEvent(SDL_Event& event) override;
    void DrawImGui() override;

protected:
    Camera camera_;
    Triangle triA_;
    Triangle triB_;

    void IsError(const std::string& file, int line);
    glm::vec3 Collision(Triangle a, Triangle b);
};

glm::vec3 GetNormal(glm::vec3 v)
{
    return glm::vec3(-v.y, v.x, v.z);
}

void PrintOut(glm::mat4 m)
{
    std::cout << 
        m[0][0] << "\t" << m[0][1] << "\t" << m[0][2] << "\t" << m[0][3] << "\n" <<
        m[1][0] << "\t" << m[1][1] << "\t" << m[1][2] << "\t" << m[1][3] << "\n" <<
        m[2][0] << "\t" << m[2][1] << "\t" << m[2][2] << "\t" << m[2][3] << "\n" <<
        m[3][0] << "\t" << m[3][1] << "\t" << m[3][2] << "\t" << m[3][3] << "\n=====================================" <<
    std::endl;
}

glm::vec3 HelloTriangle::Collision(Triangle a, Triangle b)
{
    glm::vec3 separatingAxis[6]; // All axis on which to check both shapes's projections overlap.
    glm::vec3 mtv = glm::vec3(0.0f); // Correction vector returned at the end.
    float smallestOverlap = std::numeric_limits<float>::max(); // magnitude of the mtv
    std::array<glm::vec3, 3> pointsOfA = a.GetWorldPoints(); // World coordinates of points. model * localPos
    std::array<glm::vec3, 3> pointsOfB = b.GetWorldPoints();

    // Fill out the list of axis to project onto.
    {
        std::array<glm::vec3, 3> sideDirsOfA = a.GetAxis(); // This returns tangents to sides
        std::array<glm::vec3, 3> sideDirsOfB = b.GetAxis();
        for (size_t i = 0; i < 3; i++)
        {
            separatingAxis[i] = GetNormal(sideDirsOfA[i]); // Where we need normals instead.
        }
        for (size_t i = 3; i < 6; i++)
        {
            separatingAxis[i] = GetNormal(sideDirsOfB[i - 3]);
        }
    }

    // Compute projections.
    for (size_t axis = 0; axis < 6; axis++)
    {
        float aMin = std::numeric_limits<float>::max(); // Set limits to extremes so that anything it's being compared will always overwrite these at the start.
        float aMax = -std::numeric_limits<float>::max();
        float bMin = std::numeric_limits<float>::max();
        float bMax = -std::numeric_limits<float>::max();

        for (size_t side = 0; side < 3; side++)
        {
            float projOfA = glm::dot(pointsOfA[side], separatingAxis[axis]); // Project coordinate vector onto axis.
            if (projOfA < aMin) aMin = projOfA; // Find the edges of the "shadow" of the shape on the axis.
            if (projOfA > aMax) aMax = projOfA;
            float projOfB = glm::dot(pointsOfB[side], separatingAxis[axis]);
            if (projOfB < bMin) bMin = projOfB;
            if (projOfB > bMax) bMax = projOfB;
        } // Detect overlap and store it for mtv.
        if (bMin >= aMin && bMin <= aMax) // B is overlapping A from the right
        {
            float overlap = aMax - bMin;
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = -separatingAxis[axis]; // Direction is lost when working with differences, hence the reintroduction of the direction.
            }
        }
        else if (bMax >= aMin && bMax <= aMax) // B is overlapping A from the left
        {
            float overlap = bMax - aMin;
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = separatingAxis[axis];
            }
        }
        else if (bMin <= aMin && bMax >= aMax) // B is overlapping A on both sides.
        {
            float overlap = aMax - aMin;
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = separatingAxis[axis];
            }
        }
        else // No collision. Exit function and return a zero vector.
        {
            return glm::vec3(0.0f);
        }
    }
    // Collision detected.
    mtv *= smallestOverlap;
    return mtv;
}

void HelloTriangle::IsError(const std::string& file, int line)
{
    auto error_code = glGetError();
    if (error_code != GL_NO_ERROR)
    {
        std::cerr
            << error_code
            << " in file: " << file
            << " at line: " << line
            << "\n";
    }
}

void HelloTriangle::Init()
{
    // GL testings.
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    IsError(__FILE__, __LINE__);

    glm::vec3 pointsOfA[3] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 1.0f, 0.0f),
        glm::vec3(-2.0f, -2.0f, 0.0f)
    };
    glm::vec3 pointsOfB[3] = {
        glm::vec3(3.0f, 3.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(2.0f, 1.0f, 0.0f)
    };
    triA_.Init(pointsOfA, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(45.0f));
    triB_.Init(pointsOfB, glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));
}

void HelloTriangle::Update(seconds dt)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    IsError(__FILE__, __LINE__);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    IsError(__FILE__, __LINE__);

    triA_.Move(Collision(triA_, triB_)); // Correct the position of the triangle if there's a collision.

    triA_.Draw(camera_);
    triB_.Draw(camera_);
}

void HelloTriangle::Destroy()
{
    triA_.Destroy();
    triB_.Destroy();
}

void HelloTriangle::OnEvent(SDL_Event& event)
{
    if ((event.type == SDL_KEYDOWN) &&
        (event.key.keysym.sym == SDLK_ESCAPE))
    {
        exit(0);
    }

    switch (event.type)
    {
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            triA_.Move(glm::vec3(0.0f, 1.0f, 0.0f) * 0.1f);
            break;
        case SDLK_s:
            triA_.Move(-glm::vec3(0.0f, 1.0f, 0.0f) * 0.1f);
            break;
        case SDLK_a:
            triA_.Move(-glm::vec3(1.0f, 0.0f, 0.0f) * 0.1f);
            break;
        case SDLK_d:
            triA_.Move(glm::vec3(1.0f, 0.0f, 0.0f) * 0.1f);
            break;
        case SDLK_SPACE:

            break;
        case SDLK_LCTRL:

            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEMOTION:

        break;
    case SDL_MOUSEWHEEL:

        break;
    default:
        break;
    }
}

void HelloTriangle::DrawImGui()
{
}

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloTriangle program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
