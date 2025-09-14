from stable_baselines3 import A2C

import gymnasium as gym
import glfw
import time
import cv2
import numpy as np

from volleyballenv.envs.VolleybotEnv import VolleybotEnv


# Initialize GLFW
glfw.init()

def train_PPO(env):
    pass


def main():
    # Initialize the environment
    env = VolleybotEnv(100, render_mode="rgb_array", 
                    obs_space=["ball_landing_location", "robot_location"], random_seed=0,
                    viewer="robot")

    # Parallel environments
    # vec_env = make_vec_env("CartPole-v1", n_envs=4)

    model = A2C("MultiInputPolicy", env, verbose=1, n_steps=256)
    t1 = time.time()
    t_steps = 8192 # 1024*8*8 # 32768
    model.learn(total_timesteps=t_steps) # 1024
    t2 = time.time()
    print("Trained in ", t2-t1, " seconds.")

    # TODO: log what experiment this goes with (same id as wandb maybe?)
    model.save(f"models/a2c_robot_{t_steps}steps") 

if __name__ == "__main__":
    main()