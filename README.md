# VOLLEYBOT:
A Reinforcement Learning environment for teaching quad-wheeled robots to play volleyball.

Currently undergoing refactoring to use a custom, lightweight C/C++ physics simulator. 

Todo elements:
- [x] Kinematics (verlet integration)
- [x] C++ integration for creating objects
- [x] lighting (verlet integration)
- [x] basic material properties
- [x] camera angle mathematics
- [] Collision detection & processing
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
conda env create -f environment.yml 
conda activate volleybot
```

# Testing Instructions:
From within the volleybot folder, run:
``` python -m agents.test```





