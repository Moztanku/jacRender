/**
 * @file main.cpp
 * @brief Main entry point for the application.
 */
#include "Window.hpp"
#include "Renderer.hpp"

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

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    Window window{};
    Renderer renderer(window);

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
    };

    const auto model = renderer.loadModel("models/Character_Male.fbx").value();

    const auto modelMatrices = get_model_matrices();

    while (!window.shouldClose()) {
        const std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();

        const auto events = window.pollEvents(keysToPoll);

        glm::vec3 cameraMovement{0.f};
        glm::vec3 cameraRotation{0.f};
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
                }

        camera.move(cameraMovement);
        camera.roll(cameraRotation.z);

        for (const auto& modelMatrix : modelMatrices) {
            renderer.submit(model, modelMatrix);
        }

        renderer.render();

        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float, std::milli> duration = end - begin;

        const float FPS = 1000.f / duration.count();
        // std::println("FPS: {:.2f}", FPS);
    }

    return 0;
}
