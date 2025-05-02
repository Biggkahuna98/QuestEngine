# Quest Engine TODO List
## Rendering
- [ ] Refactor Vulkan Graphics Device
  - [ ] Hoist out mesh loading
  - [ ] Add generic draw mesh method to GraphicsDevice
  - [ ] Fix the random crap accumulated in the Vulkan version from a tutorial
  - [ ] Fix the swapchain/drawing pipeline
  - [ ] No validation errors
  - [ ] Add basic 2D drawing
    - [ ] Quad
    - [ ] Circle
    - [ ] Line
    - [ ] Triangle
  - [ ] Properly split out the graphics context for command recording (drawing)
  - [ ] Only have the device do resource and context creation/destruction, no command recording
- [ ] Start work on a barebones 3D Renderer abstraction over the device/context
- [ ] Start work on  a barebones 2D Renderer abstraction over the device/context (combine with 3D..? probably not)
- [ ] Split out ImGui from the vulkan device
- [ ] Shader hot reload
- [ ] Shader compiling from runtime
## General/Misc
- [ ] Add a CVAR system for int/float/double/string
- [ ] Add loadable config options
- [ ] Interface some of the loadable configs to the CVAR system
- [ ] DLL plugin loading system
- [ ] Optionally loaded SQL-Lite DB (DLL probably)
## User Application Interfacing
- [ ] User created application, maybe called Game (still deciding)
## The Engine
- [ ] Figure out how I want to store systems
    - Maybe a vector of systems?
    - Template system storage?
    - Should I store engine level systems directly as a member or in the system storage?
    - Don't want major header pollution for compile times and namespace pollution if everything is included...