# VOLLEYBOT:
A Reinforcement Learning environment for teaching quad-wheeled robots to play volleyball.

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
- environment.yml


# Installation Instructions:
```
conda env create -f environment.yml 
conda activate volleybot
```

# Testing Instructions:
From within the volleybot folder, run:
``` python -m agents.test```





