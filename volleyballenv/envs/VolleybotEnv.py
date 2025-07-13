import numpy as np
import os
from gymnasium import utils, error, spaces
from gymnasium.envs.mujoco.mujoco_env import MujocoEnv
import random

class VolleybotEnv(MujocoEnv, utils.EzPickle):
    metadata = {
        "render_modes": ["human", "rgb_array"], 
        "render_fps": 100
    }

    def __init__(self, episode_length, obs_space=["bounding_box", "camera"], noise = False, random_seed=42, **kwargs):
        utils.EzPickle.__init__(self)
        random.seed(random_seed)

        court_path = os.path.join(os.path.dirname(__file__), "assets", "court.xml")

        frame_skip = 5

        self.obs_type = obs_space
        observation_space = {}
        if "bounding_box" in obs_space:
            # [ball bounding box min x, max x, min y, max y]
            observation_space["bounding_box"] = spaces.Box(low=0, high=1.0, shape=(4, 1), dtype=np.float32)
        if "camera" in obs_space:
            # 640 by 640 pixel grayscale image.
            observation_space["camera"] = spaces.Box(low=0, high=1.0, shape=(640, 640), dtype=np.float32)
        if not observation_space:
            raise ValueError("The observation space must include at least one of 'bounding_box' or 'camera'")

        MujocoEnv.__init__(
                self, 
                model_path=court_path, 
                frame_skip=frame_skip, 
                observation_space = observation_space
        )

        self.step_number = 0
        self.episode_len = episode_length
        self.noise = noise 

    def step(self, a):
        # Carry out one step 
        self.do_simulation(a, self.frame_skip)
        self.step_number += 1
        pass

    def viewer_setup(self):
        # Position the camera for 
        pass

    def reset_model(self):
        # Reset model to original state. 
        pass

    def _get_obs(self):
      # Observation of environment fed to agent. 
      pass
