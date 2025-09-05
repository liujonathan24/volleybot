# VOLLEYBOT:
A Reinforcement Learning environment for teaching quad-wheeled robots to play volleyball.

Currently undergoing refactoring to use a custom, lightweight C/C++ physics simulator. 

Todo elements:
- [x] Kinematics (verlet integration)
- [x] C++ integration for creating objects
- [x] lighting
- [x] basic material properties
- [x] camera angle mathematics
- [] Collision detection & processing
- [] Renderer
- [] Switch from storing 3d vector position and 3x3 matrix for rotation to a 7-dim state (3d for location, 4d for rotation quaternion)
- [] Python bindings



Outdated:
Initial documentation on the Robot Volleyball project and environment.

Volleyball environment structure:
- VolleyballEnv / 
    - envs/
    - wrappers/ 
- Agents / 
    - test.py
    - VolleyAgent / 
        - train.py
    - CVBaseline /
        - train.py
    - evaluate.py
- README.md
- requirements.txt


# Installation Instructions:
```
conda create --name volleybot --file requirements.txt
conda activate volleybot
```

# Testing Instructions:
From within the volleybot folder, run:
``` python -m agents.camera_livefeed```





