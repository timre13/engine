#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    glm::vec3 m_position;
    float m_fovDeg{45.0f};
    glm::mat4 m_projectionMatrix;
    float m_yawDeg{-90.0f};
    float m_pitchDeg{};
    glm::vec3 m_frontVector{};
    glm::vec3 m_upVector{0.0f, 1.0f, 0.0f};

public:
    Camera(const glm::vec3& position, float windowAspectRatio);

    void moveForward(float amount);
    void moveBackwards(float amount);
    void moveLeft(float amount);
    void moveRight(float amount);

    void recalculateFrontVector();

    void updateShaderUniforms(uint shaderId);

    inline void setYawDeg(float val) { m_yawDeg = val; }
    inline float getYawDeg() { return m_yawDeg; }

    inline void setPitchDeg(float val) { m_pitchDeg = val; }
    inline float getPitchDeg() { return m_pitchDeg; }

    const glm::vec3& getPosition() const { return m_position; }
};

