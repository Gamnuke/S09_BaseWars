# Vehicle Simulator

This is a project I've started during highschool at age 17 in 2019, and discontinued in 2021. 

It's based off of and inspired by Robocraft - an old online multiplayer videogame released in 2014, where the player could build their own vehicle to fight other players.

This project along many others, was simply a proof of concept and an experimentation of the capabilities of myself and the Unreal Engine, therefore certain aspects of the project were rushed without future design considerations.

The algorithms for assembling the vehicle involved using several in-built classes from UE4, such as the BoxColliderComponent and VehicleComponent, which also offered vehicle physics and part physics for simulation. 

#
### Building System
![Image3](ReadMEAssets/Gif3.gif)

All parts were saved as a Map of each part and the parts connected to it.
Some parts such as wheels had customizable settings, eg: Wheel's speed and traction

Several rules applied when placing each part:
- Sub-structures (collection of parts) are seperated by a singular movable part, such as a base for a turret, therefore static parts cannot join two sub-structures together.
- Parts had to be within vehicle building bounds and must be connected to a structure containing the cockpit.

#
### Saving/Loading and Simulation
![Image3](ReadMEAssets/Gif2.gif)

Vehicle parts and their settings were encoded and decoded into a binary text file, which could be used to promote easy vehicle save file sharing between players and servers.

#
### Modular sub structures
![Image3](ReadMEAssets/Gif1.gif)

Sub structures that are separated by a movable part has their own physics and can move on their own relative to the parent structure.

#
The project was discontinued due to a few reasons:
- Poor knowledge/regards for code clarity and comments at the time eventually led to the code being difficult to understand when working on the project after a long break.
- The realisation of the exponential amount of work required to bring online multiplayer functionality without causing game-breaking bugs.
- Runtime complexity and storage was taken into consideration when creating the algorithms and data structures needed to save vehicle data.
For example, all the same mesh in the same structure are rendered as one instance of that mesh thanks to the InstancedMeshComponent, which significantly boosts framerate as it can be rendered more efficiently.
However this opened the question of how to remove parts at runtime in the case of the vehicle taking damage, which wasn't taken into consideration and a potential full algorithm rework was realised.
