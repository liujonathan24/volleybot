import gymnasium as gym
import glfw
import time
import cv2
import numpy as np

from stable_baselines3 import PPO
from volleyballenv.envs.VolleybotEnv import VolleybotEnv

# Initialize GLFW
glfw.init()

# Initialize the environment
env = VolleybotEnv(100, render_mode="rgb_array", 
                   obs_space=["bounding_box", "camera"], random_seed=0,
                   viewer="robot")

# Parallel environments
# vec_env = make_vec_env("CartPole-v1", n_envs=4)

model = PPO("MultiInputPolicy", env, verbose=1, n_steps=256, batch_size=64, n_epochs=1)
t1 = time.time()
t_steps = 32768
model.learn(total_timesteps=t_steps) # 32768
t2 = time.time()
print("Trained in ", t2-t1, " seconds.")

# # TODO: log what experiment this goes with (same id as wandb maybe?)
model.save(f"ppo_robot_{t_steps}steps") 

# del model # remove to demonstrate saving and loading

# model = PPO.load("ppo_robot")

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
        
    time.sleep(0.1)
    # Exit early by pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break