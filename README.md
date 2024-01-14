# Unhooked Engine V2

Unhooked Engine V2, forked with Sheeesh Engine, is a 3D game engine made by two students from CITM-UPC Terrassa, David Ruiz Luengo & Pau Garriga Brotons.

- David's GitHub account: [David](https://github.com/xdavido)
- Pau's GitHub account: [Pau](https://github.com/DonnoNonno)
- Our GitHub project: [Unhhoked Engine V2](https://github.com/xdavido/UnhookedEngine.v2)
- Forked GitHub project: [Sheeesh Engine](https://github.com/Urii98/SheeeshEngine)



## Engine Description

### Controls

#### Camera
- While Right clicking:
	- WASD: move forward, left, backward and right
	- QE: move up and down
	- Look around
- F: look at target
- Hold Alt:
	- Mouse Hold Left to orbit the game object selected.
- Mouse wheel to zoom in and zoom out.
- Holding SHIFT duplicates movement speed.

### Features

#### Inspector
The user should can use the inspector to modify a GameObject:
- Hierarchy: delete, reparent, create empty and create children
- Transform: translate, rotate and scale Game Objects.
- Mesh: select or drop any imported mesh
- Texture: select or drop any imported texture
- Camera is a component with settings that can be modified.

#### Game Objects
- Drag and drop game objects and textures in the Hierarchy.
- Create game objects with Game Objects Menu
- Delete game objects by pressing Delete at Inspector
- Delete selected game object at Hierarchy by pressing Delete.
- Move Game Objects in Hierarchy by drag and drop them.
- Can be picked from the world using the mouse

#### Frustrum Culling
All meshes use a bounding volume (AABB) and can be discarded using Frustum Culling. User can visualize this in the editor (debug raycast and boxes).

#### Serialization
We have 2 different json files:
1.Configuration Engine Json
Save/Load:
- FPS
- Window configuration: width, height, brightness, fullscreen, fullscreen desktop, borderless and resizable.
- Camera3D: Position of the camera
- Renderer3D: vsync, wireframe, depthTest, cullFace, Lighting, colorMaterial, Blend, AlphaTest, LineSmooth, PointSmooth, PolygonSmooth.
- Particle Emitters: Position, Speed, Scale, Speed Variation, Life Time.
>[!IMPORTANT]
> Propierties of any emitter can be saved and then loaded into another emitter.

2.Scene Module Serialization
Save:
- Hierarchy Game Objects and Components:
  	- Name
  	- Active
  	- Is Time to Delete
  	- Type of component
  	- Material/Texture Path
  	- Local Transform position
    - Particle emitters

#### Guizmos
Implementation of Guizmo enabling users to manipulate the position, rotation, and scale of an object.

#### Windows
The editor features two windows: one displaying the scene (editor view), and the other showcasing a view captured from a game object with a camera component (main game camera). Each window operates with its own framebuffer, allowing users to simultaneously view both perspectives.

The user can Start / Pause / Stop the simulation and recover its original state.
>[!TIP]
> Animation among the two smokes will be activated when pressing Start button.
#### Mouse Picking
GameObjects can be picked from the world using the mouse.

#### Components
- Component Transform.
- Component Mesh.
- Component Material.
- Component Camera.
- Emitter.

#### Particle System
Stunning visual effects, intuitive customization, and optimal performance for immersive experiences in our unique game engine.

- Continuous loop smoke comes out at least 2 houses in the scene.
- Fireworks cast pressing "1" that follows a  GameObject to the sky with enogh randomization that does not feel repetitive.

#### Additional funcionality
An image of the texture is displayed in the Inspector Window.
Serialization of different Modules such as ModuleWindow and ModuleRenderer.

#### Additional Comments
>[!WARNING]
>We have managed to carry out all the assignments, except Custom file format and Resource Management.
