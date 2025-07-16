import numpy as np
import os
from gymnasium import utils, error, spaces
from gymnasium.envs.mujoco.mujoco_env import MujocoEnv
import xml.etree.ElementTree as ET
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
                observation_space = observation_space,
                render_mode="human", # or rgb_array
                #width=640, height=640
        )
        self._init_ball()

        self.step_number = 0
        self.episode_len = episode_length
        self.noise = noise 

    def step(self, action):
        # Carry out one step 
        self.do_simulation(action, self.frame_skip)
        self.step_number += 1

        obs = self._get_obs()
        reward = self._get_reward(obs)
        done = None # ball location outside grid.
        truncated = self.step_number > self.episode_len
        return obs, reward, done, truncated, {}

        pass

    def viewer_setup(self):
        # Position the camera for audience
        pass

    def reset_model(self):
        # Reset model to original state. 
        self.step_number = 0
        # load the ball in at a random speed and location each episode
        pass

    def _get_obs(self):
        # Observation of environment fed to agent. 
        observation = np.array([])
        # self._get_reward()
        if "bounding_box" in self.obs_type:
            pass
        if "camera" in self.obs_type:
            pass
            self.render()
            self.camera_obs = np.array(self.render())
        # Ball position: print(np.array(self.data.joint("ball").qpos))
        return self.data.joint("ball").qpos
    
    def _get_reward(self, obs):
        ball_joint = self.data.joint("ball")
        ball_qpos = np.array(ball_joint.qpos)
        ball_qvel = np.array(ball_joint.qvel)
        # print(ball_joint.qpos, ball_qvel)
        # print(dir(ball_joint))
        return 0
        # calculate trajectory of ball

    def _init_ball(self):

        # TODO: AVOID THE NET LOL

        width = 0.25
        length = 0.5
        net_height = 0.135
        ball_radius = 0.02
        min_height = net_height + ball_radius
        init_x_pos, init_y_pos = random.uniform(-width, width), random.uniform(length, 0)
        self.data.joint("ball").qpos = [init_x_pos, init_y_pos, 0, 0, 0, 0, 0]
        # self.data.joint("ball").qpos = [0, init_x_pos, init_y_pos, 0, 0, 0, 0]
        
        final_x_pos, final_y_pos = random.uniform(-width, width), random.uniform(-length, 0)
        
        random_max_height = random.uniform(min_height, 0.5)
        airtime = np.sqrt(2*random_max_height/9.81)
        print(final_x_pos, final_y_pos, airtime/2)
        init_z_vel = 9.81*airtime/2
        init_x_vel = (final_x_pos-init_x_pos)/airtime
        init_y_vel = (final_y_pos-init_y_pos)/airtime
        
        #z_vel-9.81t=0
        # t=np.sqrt(random/9.81*2)
        self.data.joint("ball").qvel = [init_x_vel,init_y_vel,init_z_vel, 0, 0, 0]
        # print(self.data.joint("ball").qpos)
        # court_root = ET.parse(court_path).getroot()
        # # court_obj[1][3].qpos = "0 0 1"
        # print(dir(court_root[1][3]))
        # static_ball = court_root.find(".//body")
        # # static_ball.set('pos', )
        # static_ball.set('vel', "0 0 1")
        # # court_root.remove(static_ball)

        # # court_root[1][3].attrib["vel"] = "0 0 1" #("vel", "0 0 1")
        # os.path.join(os.path.dirname(__file__), "assets", "court_temp.xml")
