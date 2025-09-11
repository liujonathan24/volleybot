# VOLLEYBOT:
A Reinforcement Learning environment for teaching quad-wheel robots to play volleyball.

To begin, run scripts from the scripts/ folder to test out different views.

To see the robot's POV:
``` python -m agents.camera_livefeed```

To train a basic PPO policy and spectate the robot:
``` python -m scripts.train_agent_PPO.py ```
``` python -m scripts.spectate_agent ```


Environment structure:
├── agents /
│   ├── camera_livefeed.py
│   ├── spectate_agent.py
│   ├── stable_agent_PPO.py
│   ├── test.py
│   └── train_agent.py
├── logs /
│   └── wandb
├── models /
├── README.md
├── refactored_volleybot /
│   ├── examples /
│   ├── physics_engine /
│   │   ├── bindings /
│   │   ├── build /
│   │   ├── CMakeLists.txt
│   │   ├── include /
│   │   └── src /
│   └── setup.py
├── requirements.txt
├── scripts /
│   └── train.py
├── src /
│   ├── models /
│   │   ├── agent.py
│   │   └── ppo_model.py
│   └── utils /
│       ├── calculate_reward.py
│       └── extract_observation.py
└── volleyballenv /
    ├── envs /
    │   ├── assets /
    │   └── VolleybotEnv.py
    ├── README.md
    └── wrappers /

# Installation Instructions:
```
conda create --name volleybot --file requirements.txt
conda activate volleybot
```

# Testing Instructions:
From within the volleybot folder, run:
``` python -m agents.camera_livefeed```






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
