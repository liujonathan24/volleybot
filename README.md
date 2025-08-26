# VOLLEYBOT:
A Reinforcement Learning environment for teaching quad-wheeled robots to play volleyball.

Currently undergoing refactoring to use a custom, lightweight C/C++ physics simulator. 

Todo elements:
[X] Kinematics (verlet integration)
[X] C++ integration for creating objects
[X] lighting (verlet integration)
[X] basic material properties
[X] camera angle mathematics
[] Collision detection & processing
[] Python bindings



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
conda env create -f environment.yml 
conda activate volleybot
```

# Testing Instructions:
From within the volleybot folder, run:
``` python -m agents.test```





