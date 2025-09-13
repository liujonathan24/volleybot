import gymnasium as gym
import glfw
import time
import cv2
import numpy as np

from stable_baselines3 import PPO
from volleyballenv.envs.VolleybotEnv import VolleybotEnv

t = 32768

glfw.init()

env = VolleybotEnv(100, render_mode="rgb_array", 
                   obs_space=["ball_landing_location", "robot_location"], random_seed=0,
                   )

model = PPO.load(f"models/ppo_robot_{t}steps")

cv2.namedWindow("Volleybot Robot Feed", cv2.WINDOW_NORMAL)

obs, _ = env.reset()
while True:
    action, _states = model.predict(obs)
    obs, rewards, dones, info, _ = env.step(action)
    if dones:
        env.reset()
    # env.render("human")

    frame = env.camera_obs
    
    if frame is not None and isinstance(frame, np.ndarray):
        # Convert from RGB to BGR (which cv2 uses)
        frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

        # Overlay bounding boxes if 'bounding_box' observation exists
        if "bounding_box" in obs and obs["bounding_box"] is not None:
            bbox_info = obs["bounding_box"]
            
            x_min = int(bbox_info[0].item())
            y_min = int(bbox_info[1].item())
            x_max = int(bbox_info[2].item())
            y_max = int(bbox_info[3].item())

            color = (0, 255, 0)  # Green color
            thickness = 2

            # Draw the rectangle on the frame
            cv2.rectangle(frame_bgr, (x_min, y_min), (x_max, y_max), color, thickness)
        
        # Display the frame in the OpenCV window
        cv2.imshow("Volleybot Camera Feed", frame_bgr)
        
    time.sleep(0.05)
    # Exit early by pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break