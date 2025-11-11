/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include "graphics/Window.hpp"
#include "graphics/Renderer.hpp"

#include <array>

constexpr
auto get_model_matrices() -> std::array<glm::mat4, 100> {
    std::array<glm::mat4, 100> modelMatrices{};

    size_t index = 0;
    const int gridSize = 10;
    const float spacing = 50.0f;

    const glm::mat4 base = glm::scale(
        glm::mat4(1.0f),
        glm::vec3(0.02f, 0.02f, 0.02f)
    );

    for (int x = -gridSize / 2; x < gridSize / 2; ++x) {
        for (int z = -gridSize / 2; z < gridSize / 2; ++z) {
            if (index >= modelMatrices.size()) break;

            glm::mat4 model = glm::translate(
                base,
                glm::vec3(x * spacing, 0.0f, z * spacing)
            );

            modelMatrices[index++] = model;
        }
    }

    return modelMatrices;
}

template <typename T>
void debug_print(T x) {
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        std::print("{:.2f}", x);
    } else {
        std::print("{}", x);
    }

    std::print("\r");
}

template<typename T, typename... Rest>
void debug_print(T x, Rest... rest) {
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        std::print("{:.2f} ", x);
    } else {
        std::print("{} ", x);
    }

    debug_print(rest...);
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    const std::chrono::high_resolution_clock::time_point program_start = std::chrono::high_resolution_clock::now();

    graphics::Window window{};
    graphics::Renderer renderer(window);

    auto& camera = renderer.getCamera();
    camera.changeFov(-30.f);
    
    const float cameraSpeed = 0.0025f;

    const std::vector<int> keysToPoll = {
        // Movement
        GLFW_KEY_W, // Forward
        GLFW_KEY_S, // Backward
        GLFW_KEY_A, // Left
        GLFW_KEY_D, // Right
        GLFW_KEY_E, // Up
        GLFW_KEY_Q, // Down
        // Rotation
        GLFW_KEY_LEFT_BRACKET,  // Roll left
        GLFW_KEY_RIGHT_BRACKET, // Roll right
        // Debug
        GLFW_KEY_1,
        GLFW_KEY_2
    };

    const auto model = renderer.loadModel("models/Character_Male.fbx").value();

    const auto modelMatrices = get_model_matrices();

    while (!window.shouldClose()) {
        const std::chrono::high_resolution_clock::time_point frame_start = std::chrono::high_resolution_clock::now();

        const auto events = window.pollEvents(keysToPoll);

        glm::vec3 cameraMovement{0.f};
        glm::vec3 cameraRotation{0.f};
        
        static bool keyLock1 = false;
        static bool keyLock2 = false;
        for (const auto& [event, key] : events)
            if (event == GLFW_PRESS)
                switch (key) {
                    // Movement
                    case GLFW_KEY_W: cameraMovement.z += cameraSpeed; break;
                    case GLFW_KEY_S: cameraMovement.z -= cameraSpeed; break;
                    case GLFW_KEY_A: cameraMovement.x -= cameraSpeed; break;
                    case GLFW_KEY_D: cameraMovement.x += cameraSpeed; break;
                    case GLFW_KEY_E: cameraMovement.y += cameraSpeed; break;
                    case GLFW_KEY_Q: cameraMovement.y -= cameraSpeed; break;
                    // Rotation
                    case GLFW_KEY_LEFT_BRACKET:  cameraRotation.z += cameraSpeed; break;
                    case GLFW_KEY_RIGHT_BRACKET: cameraRotation.z -= cameraSpeed; break;
                    // Debug
                    case GLFW_KEY_1:
                        renderer.DEBUG_1 = keyLock1 ? renderer.DEBUG_1 : !renderer.DEBUG_1;
                        keyLock1 = true;
                        break;
                    case GLFW_KEY_2:
                        renderer.m_lightCount = keyLock2 ? renderer.m_lightCount : (renderer.m_lightCount + 1) % 2;
                        keyLock2 = true; 
                        break;
                }
            else if (event == GLFW_RELEASE)
                switch (key) {
                    case GLFW_KEY_1: keyLock1 = false; break;
                    case GLFW_KEY_2: keyLock2 = false; break;
                };

        camera.move(cameraMovement);
        camera.roll(cameraRotation.z);

        for (const auto& modelMatrix : modelMatrices) {
            renderer.submit(model, modelMatrix);
        }


        const float current_time = std::chrono::duration<float>(
            frame_start - program_start
        ).count();

        const float RADIUS = 10.0f;
        const float SECS_PER_REV = 5.0f;
        const float x = RADIUS * std::cos((current_time / SECS_PER_REV) * 2.0f * glm::pi<float>());
        const float z = RADIUS * std::sin((current_time / SECS_PER_REV) * 2.0f * glm::pi<float>());
        const float y = 5.0f;
        
        renderer.setLightPos(glm::vec3{x, y, z});
        // renderer.setLightPos(camera.getPosition());

        renderer.render();

        const std::chrono::high_resolution_clock::time_point frame_end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float, std::milli> frame_duration = frame_end - frame_start;

        const float FPS = 1000.f / frame_duration.count();

        debug_print(
            "Camera: (",
            camera.getPosition().x,
            camera.getPosition().y,
            camera.getPosition().z,
            "\b) Light: (",
            x,
            y,
            z,
            "\b)"
        );
    }

    return 0;
}
