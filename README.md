# CrabToolsUE5
This is a plugin library implementing various subsystems that are helpful for promoting
efficient and optimized design. The following systems are currently available:

- State Machine Blueprints: A specialized blueprint for handling hierarchical state machines that can be extended, edited on instances, and composed.

- RPG Subsystem: A generalized RPG system as an alternative to UE's gameplay attribute system. Supports int and float attributes, both with recursively dependent values (such as min and max health, and maxmax and minmin health), attribute operators that can be used to non-destuctively modify attributes with priority, and status effects which are at their core simple effects, but are made so any RPG effect can be implemented.

- Ability System: An ability system that is designed to work with our state machines and have some native implementations that cover common use cases.


- Interaction System: Simple base system for creating and interacting with objects in the environment. Useful for both players and AI.

## State Machine Usage

![alt text](https://raw.githubusercontent.com/RuinDelvers/CrabToolsUE5/refs/heads/main/SampleImages/StateMachineSample.PNG)

Our State Machines are a generalized blueprint that can be used for a variety of tasks: From AI control, to player state 
management, to dialogue. Each state machine is event based; Meaning that state changes will only occur upon an event being
sent from an external source or emitted from the execution nodes in each state. This allows for a very performative method of
controlling entities. Events, are effectively just Names, that are used to identify what happened; For example, the AI_LOST event 
is emitted by some of our movement nodes to indicate that it is lost (for any reason). While the AI_ARRIVE event is used when 
arriving at their destination.

Events also act as a sort of interface for state machines; You can create a State Machine Interfaces that contain events and 
other data that defines how your state machine can be interacted with. These are also used with some custom nodes to safely send events to state machines without having the save random Name variables everywhere.
