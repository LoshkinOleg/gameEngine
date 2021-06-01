#include <iostream>
#include <limits>

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
    Axis axis_;
    Triangle triA_;
    Triangle triB_;

    void IsError(const std::string& file, int line);
    glm::vec2 Collision(Triangle a, Triangle b);
};

glm::vec2 GetNormal(glm::vec2 v)
{
    return glm::vec2(-v.y, v.x);
}

glm::vec2 HelloTriangle::Collision(Triangle a, Triangle b)
{
    glm::vec2 axis[6] = {
        glm::normalize(GetNormal(a.points_[1] - a.points_[0])),
        glm::normalize(GetNormal(a.points_[2] - a.points_[1])),
        glm::normalize(GetNormal(a.points_[0] - a.points_[2])),

        glm::normalize(GetNormal(b.points_[1] - b.points_[0])),
        glm::normalize(GetNormal(b.points_[2] - b.points_[1])),
        glm::normalize(GetNormal(b.points_[0] - b.points_[2]))
    };
    glm::vec2 mtv = glm::vec2(0.0f, 0.0f);
    float smallestOverlap = std::numeric_limits<float>::max();

    for (size_t ax = 0; ax < 6; ax++)
    {
        float aMin = std::numeric_limits<float>::max();
        float aMax = -std::numeric_limits<float>::max();
        float bMin = std::numeric_limits<float>::max();
        float bMax = -std::numeric_limits<float>::max();
        for (size_t side = 0; side < 3; side++)
        {
            float projOfA = glm::dot(a.points_[side] + a.GetPosition(), axis[ax]);
            if (projOfA < aMin) aMin = projOfA;
            if (projOfA > aMax) aMax = projOfA;
            float projOfB = glm::dot(b.points_[side] + b.GetPosition(), axis[ax]);
            if (projOfB < bMin) bMin = projOfB;
            if (projOfB > bMax) bMax = projOfB;
        }
        if ((bMin >= aMin && bMin <= aMax))
        {
            float overlap = aMax - bMin;
            assert(overlap >= 0.0f);
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = -axis[ax]; // Direction is lost when working with differences, hence the reintroduction of the direction.
            }
        }
        else if ((bMax >= aMin && bMax <= aMax))
        {
            float overlap = bMax - aMin;
            assert(overlap >= 0.0f);
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = axis[ax];
            }
        }
        else if ((bMin <= aMin && bMax >= aMax))
        {
            float overlap = aMax - aMin;
            assert(overlap >= 0.0f);
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                mtv = axis[ax];
            }
        }
        else // No collision.
        {
            return glm::vec2(0.0f, 0.0f);
        }
    }
    assert(smallestOverlap >= 0.0f);
    mtv *= smallestOverlap;
    std::cout << "MTV: (" << mtv.x << ";" << mtv.y << ")" << std::endl;
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

    axis_.Init();
    glm::vec2 pointsOfA[3] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(-1.0f, 1.0f),
        glm::vec2(-2.0f, -2.0f)
    };
    glm::vec2 pointsOfB[3] = {
        glm::vec2(3.0f, 3.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(2.0f, 1.0f)
    };
    triA_.Init(pointsOfA, glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    triB_.Init(pointsOfB, glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void HelloTriangle::Update(seconds dt)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    IsError(__FILE__, __LINE__);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    IsError(__FILE__, __LINE__);

    triA_.Move(Collision(triA_, triB_));

    axis_.Draw(camera_);
    triA_.Draw(camera_);
    triB_.Draw(camera_);
}

void HelloTriangle::Destroy()
{
    axis_.Destroy();
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
            triA_.Move(glm::vec2(0.0f, 1.0f) * 0.1f);
            break;
        case SDLK_s:
            triA_.Move(glm::vec2(0.0f, -1.0f) * 0.1f);
            break;
        case SDLK_a:
            triA_.Move(glm::vec2(-1.0f, 0.0f) * 0.1f);
            break;
        case SDLK_d:
            triA_.Move(glm::vec2(1.0f, 0.0f) * 0.1f);
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
