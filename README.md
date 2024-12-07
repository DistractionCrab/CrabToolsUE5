# CrabToolsUE5
This is a plugin library implementing various subsystems that are helpful for promoting
efficient and optimized design. The following systems are currently available:

- State Machine Blueprints: A specialized blueprint for handling hierarchical state machines that can be extended, edited on instances, and composed.

- RPG Subsystem: A generalized RPG system as an alternative to UE's gameplay attribute system. Supports int and float attributes, both with recursively dependent values (such as min and max health, and maxmax and minmin health), attribute operators that can be used to non-destuctively modify attributes with priority, and status effects which are at their core simple effects, but are made so any RPG effect can be implemented.

- Ability System: An ability system that is designed to work with our state machines and have some native implementations that cover common use cases.


- Interaction System: Simple base system for creating and interacting with objects in the environment. Useful for both players and AI.
