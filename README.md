# CrabToolsUE5
This is a plugin library implementing various subsystems that are helpful for promoting
efficient and optimized design. The following systems are currently available:

- Hierarchical Procedural Statemachines: State machine system that allows one to create organized and optimized control of characters, AI, or other state-based components. This system utilizes a composable node architecture that allows one to have individual states be built up of different pluggable functionality. It is also an event based architecture, so transitions between states are controlled by events first and not needing to check for value changes each frame. Events can have conditions attached to them also, so events which may need more customization can be done. Graph glow can also be controlled with code (hence the procedural part), so nodes can be extended and customized to force specific states if necessary. Also multiple pre-implemented nodes, such as InputNode, are available for some common complex task.

- RPG Subsystem: A generalized RPG system as an alternative to UE's gameplay attribute system. Supports int and float attributes, both with recursively dependent values (such as min and max health, and maxmax and minmin health), attribute operators that can be used to non-destuctively modify attributes with priority, and status effects which are at their core simple effects, but are made so any RPG effect can be implemented.

- Lock-On Camera: Camera used for locking onto an object and locking character orientation as necessary.

- Perspective manager: Tool used for dynamic cameras in the environment. Useful for transitioning seemlessly between Third person, lock-on cameras (if utilizing a specialized one), environmental perspectives, such as a pseudo-2.5D game where the camera depends on the environment.

- Interaction System: Simple base system for creating and interacting with objects in the environment.
