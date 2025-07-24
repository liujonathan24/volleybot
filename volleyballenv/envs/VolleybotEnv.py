import numpy as np
import os
from gymnasium import utils, spaces
from gymnasium.envs.mujoco.mujoco_env import MujocoEnv
import random


class VolleybotEnv(MujocoEnv, utils.EzPickle):
    metadata = {
        "render_modes": ["human", "rgb_array"], 
        "render_fps": 100
    }

    def __init__(self, episode_length, 
                 render_mode = "human", obs_space=["bounding_box", "camera"], 
                 reward_type=["proximity", "timing", "accuracy"], noise=False, 
                 random_seed=42, viewer="human", **kwargs):
        utils.EzPickle.__init__(self)
        random.seed(random_seed)
        np.random.seed(random_seed)

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

        self.reward_type = reward_type
        if viewer == "human":
            MujocoEnv.__init__(
                    self, 
                    model_path=court_path, 
                    frame_skip=frame_skip, 
                    observation_space = observation_space,
                    render_mode=render_mode, 
                    width=640, height=640,
            )
        elif viewer == "robot":
            MujocoEnv.__init__(
                    self, 
                    model_path=court_path, 
                    frame_skip=frame_skip, 
                    observation_space = observation_space,
                    render_mode=render_mode, 
                    width=640, height=640,
                    camera_name="robot_camera"
            )
        
        self._init_ball()

        self.step_number = 0
        self.episode_len = episode_length
        self.noise = noise 

        # Helper attributes
        self.hit_ball = False
        self.previous_observations = []
        self.previous_rewards = []

    def step(self, action):
        # Carry out one step 
        self.do_simulation(action, self.frame_skip)
        self.step_number += 1

        obs = self._get_obs()
        reward = self._get_reward()
        done = self._get_done()
        truncated = self.step_number > self.episode_len
        return obs, reward, done, truncated, {}

    def reset_model(self):
        # Reset model to original state. 
        self.step_number = 0
        # load the ball in at a random speed and location each episode
        self._init_ball()

    def _get_obs(self):
        # Observation of environment fed to agent. 
        observation = {}
        observation["ball_pos"] = np.array(self.data.joint("ball").qpos)

        self.camera_obs = np.array(self.render()) 

        # Camera observation
        if "camera" in self.obs_type:
            observation["camera_obs"] = self.camera_obs

        # Bounding box data 
        # if "bounding_box" in self.obs_type:
            # observation["bounding_box"] = None  #TODO:

            # Camera parameters
            # ball_pos = self.data.joint("ball").qpos[:3]
            
            # fovy = np.deg2rad(45)  # Assume fovy=45 degrees (adjust if specified in XML)
            # focal_length = 640 / (2 * np.tan(fovy / 2))  # ~771.8 pixels
            # cx, cy = 320, 320  # Principal point (center of 640x640 image)
            # ball_radius = 0.02  # Ball radius in meters

            # # Camera's world pose
            # robot_pos = self.data.joint("robot").qpos[:3]
            # cam_local_pos = np.array([0, 0, 0.1])   # From <camera pos="0 0 0.1">
            # cam_pos = robot_pos + cam_local_pos     # World position: [0, -0.25, 0.3]
            
            # # Camera orientation (euler="90 0 0" -> rotation around x-axis)
            # R_cam = np.array([
            #     [1, 0, 0],
            #     [0, 0, -1],
            #     [0, 1, 0]
            # ])
            # R_cam_T = R_cam.T  # Inverse rotation (transpose)

            # # Transform ball to camera frame
            # ball_rel = ball_pos - cam_pos  # Relative position
            # ball_cam = R_cam_T @ ball_rel  # Rotate to camera frame: [x_b, z_b - 0.3, -(y_b + 0.25)]
            # x_c, y_c, z_c = ball_cam

            # # Check if ball is in front of camera
            # if z_c <= 0:
            #     observation["bounding_box"] = np.array([0, 0, 0, 0])  # Invalid bounding box
            # else:
            #     # Project to 2D
            #     u = cx + focal_length * x_c / z_c
            #     v = cy - focal_length * y_c / z_c  # Negative for image y-axis (downward)

            #     # Approximate ball radius in pixels
            #     r_pixels = focal_length * ball_radius / z_c

            #     # Bounding box: [u_min, v_min, u_max, v_max]
            #     u_min = np.clip(u - r_pixels, 0, 640)
            #     v_min = np.clip(v - r_pixels, 0, 640)
            #     u_max = np.clip(u + r_pixels, 0, 640)
            #     v_max = np.clip(v + r_pixels, 0, 640)
            #     observation["bounding_box"] = np.array([u_min, v_min, u_max, v_max])



        # Bounding box data
        if "bounding_box" in self.obs_type:
            ball_pos = self.data.joint("ball").qpos[:3]

            # Camera parameters (assuming 640x640 image resolution)
            fovy = np.deg2rad(45)
            focal_length = 640 / (2 * np.tan(fovy / 2)) # ~771.8 pixels
            cx, cy = 320, 320 # Principal point (center of 640x640 image)
            ball_radius = 0.02 # Ball radius in meters

            # Camera's world pose
            robot_pos = self.data.joint("robot").qpos[:3]
            cam_local_pos = np.array([0, 0, 0.005]) # From <camera pos="0 0 0.1">
            cam_pos = robot_pos + cam_local_pos # World position: [0, -0.25, 0.3] (example values)

            cam_xmat = self.data.camera('robot_camera').xmat.reshape(3, 3) # Matrix from camera frame to world frame
            R_world_to_cam = cam_xmat #.T # Transpose to get world frame to camera frame

            # Transform ball to camera frame
            ball_rel_pos = ball_pos - cam_pos # Relative position in world frame
            ball_cam = R_world_to_cam @ ball_rel_pos # Rotate to camera frame
            x_c, y_c, z_c = ball_cam # x_c: camera right, y_c: camera down, z_c: camera forward

            # Check if ball is in front of camera (positive z_c for forward in camera frame)
            if z_c <= 0:
                observation["bounding_box"] = np.array([0, 0, 0, 0]) # Ball behind or on camera plane
            else:
                z_c = np.abs(z_c)
                # Project to 2D image plane (u, v)
                # u increases to the right, v increases downwards
                u = cx + focal_length * x_c / z_c
                v = cy + focal_length * y_c / z_c # This was `-` before, now `+` because y_c is already "camera down"

                # Approximate ball radius in pixels
                r_pixels = focal_length * ball_radius / z_c

                # Bounding box coordinates: [u_min, v_min, u_max, v_max]
                u_min = u - r_pixels
                v_min = v - r_pixels
                u_max = u + r_pixels
                v_max = v + r_pixels

                # Clip coordinates to image boundaries (0 to 640 for a 640x640 image)
                u_min = np.clip(u_min, 0, 640)
                v_min = np.clip(v_min, 0, 640)
                u_max = np.clip(u_max, 0, 640)
                v_max = np.clip(v_max, 0, 640)

                # Ensure valid bounding box (e.g., if clipped too much)
                if u_max <= u_min or v_max <= v_min:
                    observation["bounding_box"] = np.array([0, 0, 0, 0])
                else:
                    observation["bounding_box"] = np.array([u_min, v_min, u_max, v_max])

        # Store in previous_observations (as a dict)
        self.previous_observations.append(observation)

        return observation
    
    
    def _get_reward(self):
        reward = 0
        if "proximity" in self.reward_type:
            # Normalized to a maximum of 1
            # distance is divided by half the width of the court to avoid imbalanced rewards
            reward += 1 - np.linalg.norm(self.data.joint("robot").qpos[:3]-self.final_ball_loc)/0.25
        if "timing" in self.reward_type:
            # Add a reward for hitting the ball
            # TODO: Think about how this will work. 
            # simulation is currently 1 frame per 
            # 0.02s, and it is unrealistic to have the
            # arm hit the ball in 1 frame. Is this an issue?
            if self.hit_ball:
                reward += 50
                self.hit_ball = False
        if "accuracy" in self.reward_type:
            # Check to see if the trajectory of the ball will land in the opponent court.
            land_in_oponnent_side = self._landing_location(self.data.joint("ball"), "opponent")
            if land_in_oponnent_side:
                reward += 50

        self.previous_rewards.append(reward)
        return reward

    def _init_ball(self):
        """
        Set initial properties of the ball such that:
        - The ball spawns in randomly in the playing area.
        - The ball's velocity is set such that the ball clears the net and 
        lands in a random location in the playing area on the other side of the net.
        """

        width  = 0.25
        length = 0.5
        net_height = 0.135 
        g = 9.81
        r = 0.02

        # Random initial and final positions
        init_x_pos = random.uniform(-width, width)
        init_y_pos = random.uniform(length/3, length)  # [0.1667, 0.5]
        final_x_pos = random.uniform(-width, width)
        final_y_pos = random.uniform(-length, length/3)  # [-0.5, 0.1667]

        # Calculate the minimum "peak of trajectory" so that the trajectory still clears
        # the net.
        num = ((init_y_pos**2-init_y_pos)/16+1)**2-12*net_height**2/(init_y_pos**2-init_y_pos)
        denom = 24*net_height/(init_y_pos**2-init_y_pos)
        min_height = -num/denom

        # Randomly choose a peak for the trajectory
        random_max_height = random.uniform(min_height, 2*min_height)  
        airtime = np.sqrt(8 * random_max_height / g)

        # Calculate initial velocities
        init_z_vel = g * airtime / 2
        init_x_vel = (final_x_pos - init_x_pos) / airtime
        init_y_vel = (final_y_pos - init_y_pos) / airtime

        # Set positions and velocities
        self.data.joint("ball").qpos = [init_x_pos, init_y_pos, r, 0, 0, 0, 0]
        self.data.joint("ball").qvel = [init_x_vel, init_y_vel, init_z_vel, 0, 0, 0]

        self.final_ball_loc = [init_x_pos, init_y_pos, 0.02]
        print("Ball Initiation Statistics: ")
        print("Initial position of the ball: ", [init_x_pos, init_y_pos])
        print("Projected landing position of the ball: ", [final_x_pos, final_y_pos])
        print("Initial velocity of the ball: ", [float(i) for i in [init_x_vel, init_y_vel, init_z_vel]])
        print()


    def _landing_location(self, ball_object, location="opponent"):
        """
        Using kinematics, determines if a ball object will land in a designated area
        """
        ball_position = ball_object.qpos[:3]  # [x, y, z]
        ball_velocity = ball_object.qvel[:3]  # [vx, vy, vz]
        z_0 = ball_position[2]  # Initial z-position
        v_z = ball_velocity[2]  # Initial z-velocity
        g = 9.81 

        time_to_land = (-v_z + np.sqrt(v_z**2 + 2 * g * z_0)) / g

        fin_x = ball_position[0] + ball_velocity[0] * time_to_land
        fin_y = ball_position[1] + ball_velocity[1] * time_to_land

        if location == "opponent":
            # Check to make sure the ball will land in opponent side
            x_valid = fin_x <= 0.25 and fin_x >= -0.25
            y_valid = fin_y >=  0.5/3 and fin_y <= 0.5
 
        elif location == "robot":
            # Check to make sure the ball will land in robot's side
            x_valid = fin_x <= 0.25 and fin_x >= -0.25
            y_valid = fin_y <=  -0.5/3 and fin_y >= -0.5

        return x_valid and y_valid
    
    def _get_done(self):
        done = False
        if not self._landing_location(self.data.joint("ball"), location="opponent") and not self._landing_location(self.data.joint("ball"), location="robot"):
            done = True
        return done