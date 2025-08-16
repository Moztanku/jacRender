# jacRender
Renderer written from scratch with C++ and Vulkan.

TODO: think about moving Framebuffer into Swapchain, as Framebuffer doesn't do a lot

TODO: get separate transfer queue working for staging buffers

TODO: consider moving stuff like BufferTypeTraits, CommandBufferCommands, etc. into separate files, maybe use namespace like `impl` to avoid polluting global namespace

TODO: create wrapper functions for all vkFunctions that check for errors and throw exceptions

TODO: look into VulkanMemoryAllocator (VMA) for better memory management
