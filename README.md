unhooked-Engine
Unhooked Engine is currently under active development as the "Engine Development" subject project for the CITM Bachelor's degree in Video Game Design and Development.

En este proyecto hemos tenido varios problemas, por ejemplo el viewport no nos ha salido bien, a la hora de hacer el frustum y el viewport, se nos ha petado las escenas y no se renderizaban correctamente,
así que hay muchas cosas que no hemos podido hacer por culpa de esto. Aún así hay bastantes cosas que estan hechas aunque no se vean directamente en el proyecto. Le hemos puesto demasiadas horas a solucionar
los problemas que teniamos y ni siquiera hemos podido arreglarlos.

No sabemos porqué la camara no se hace correctamente y en la pantalla solo se ven las meshes, sin los gameobjects.

A parte, en la release, la ventana de game no se renderiza igual que cuando está en debug.


Link: https://github.com/xdavido/UnhookedEngine.v2


Team Members: 
- David Ruiz Luengo --> GitHub: https://github.com/xdavido
- Pau Garriga Brotons --> GitHub: https://github.com/DonnoNonno

Controls: 
Camera
W - Move forward
A - Move Left
S - Move backwards
D - Move Right
R - Move Up
F - Move Down
Ctrl + Mouse Wheel - Zoom in/Zoom out
Right Click - Free rotation
Holding Shift + 'WASD' - Duplicates movement speed
Alt + Left Click - Rotate around object

Features:
Drag&Drop
The engine allows the user to Drag&Drop FBX files from anywhere in the hard drive and later, drag and drop files to put textures for the GameObjects.

Move between windows
Configuration
Application
Frame Information.
Slider to limitate FPS.
Show Hardware Information and software versions
Window
Window Options:
Full Screen, resizable, full desk and borderless.
Change brightness, width and height.
File System
Shows base path.
Input
Mouse Information (Position and Motion).
Hardware Information
SDL Version.
Memory consumption.

Inspector
Move, rotate and scale the GameObject selected. And also GameObject's name.
Console
Shows the user geometry loading process from ASSIMP and the external libraries initialization process.

External libraries
Externals libraries used in our engine:
SDL
ImGui
Assimp
Glew
MathGeoLib
DevIL
PhysFS

In process:
Serialization
