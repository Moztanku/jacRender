# jacRender
Renderer written from scratch with C++ and Vulkan.

TODO: think about moving Framebuffer into Swapchain, as Framebuffer doesn't do a lot

TODO: get separate transfer queue working for staging buffers

TODO: consider moving stuff like BufferTypeTraits, CommandBufferCommands, etc. into separate files, maybe use namespace like `impl` to avoid polluting global namespace

TODO: look into VulkanMemoryAllocator (VMA) for better memory management

## Example usage
```cpp

#include "jacRender/Window.hpp"
#include "jacRender/Renderer.hpp"

// some code...

int main() {
    // default Window implementation using GLFW
    jac::Window window("My window", 800, 600);
    // renderer takes GLFW handle, possibly could be abstracted to other windowing libraries
    jac::Renderer renderer(window.getWindow());

    // Model is an opaque handle, loading is done via ResourceManager inside Renderer
    jac::Model model = renderer.loadModel("path/to/model.obj");

    while (!window.shouldClose()) {
        const auto events = window.pollEvents();

        // handle events...
        const jac::Camera camera = // setup camera

        // set's camera for the current frame, view and projection matrices are sent to GPU
        renderer.setCamera(camera);

        const glm::mat4 modelMatrix = // get model matrix

        // submit draw command for internal renderer queue
        renderer.submit(model, modelMatrix /* maybe other arguments*/);

        // other rendering code...

        // finally, draw commands are sorted, recorded and submitted to GPU
        renderer.render();
    }

    return 0;
}
```

## Structure
```
Window: GLFWWindow wrapper
Renderer:
    - ResourceManager:
        - MemoryManager: Manages Vulkan memory allocations
        - Model[]:
            - Mesh&[]
            - Material&[]
        - Material[]:
            - Texture& diffuse, normal, ...
            - UBO
            - DescriptorSet
        - Mesh[]:
            - VertexBuffer
            - IndexBuffer
            - indexCount
            - materialIndex
        - Texture[]:
            - Image
            - Path
```

## Assets
- Character model from [elbolilloduro](https://elbolilloduro.itch.io/trailer-park)
