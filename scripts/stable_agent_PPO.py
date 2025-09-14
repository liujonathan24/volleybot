import gymnasium as gym
import glfw
import time
import cv2
import numpy as np

from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv
from volleyballenv.envs.VolleybotEnv import VolleybotEnv


# Initialize GLFW
glfw.init()


def make_env(seed=0, idx=0):
    """
    Returns a function that initializes a single VolleybotEnv instance.
    Each environment gets a different seed to avoid identical rollouts.
    """
    def _init():
        env = VolleybotEnv(
            100,
            render_mode="rgb_array",
            obs_space=["ball_landing_location", "robot_location"],
            random_seed=seed + idx,
            viewer="robot"
        )
        return env
    return _init


def main():
    # Number of parallel environments
    num_envs = 32

    # Create vectorized environment
    env = DummyVecEnv([make_env(seed=0, idx=i) for i in range(num_envs)])
    # For debugging or single-threaded, you could use DummyVecEnv instead:
    # env = DummyVecEnv([make_env(seed=0, idx=i) for i in range(num_envs)])

    # PPO requires n_steps to be divisible by num_envs
    n_steps = 256  # per update
    assert n_steps % num_envs == 0, "n_steps must be divisible by num_envs"

    model = PPO(
        "MultiInputPolicy",
        env,
        verbose=1,
        n_steps=n_steps,
        batch_size=16,
        n_epochs=256,
    )

    t1 = time.time()
    t_steps = 1048576  # 1024 # 32768
    model.learn(total_timesteps=t_steps)
    t2 = time.time()
    print("Trained in ", t2 - t1, " seconds.")

    model.save(f"models/ppo_robot_{t_steps}steps")


if __name__ == "__main__":
    main()


# import gymnasium as gym
# import glfw
# import time
# import cv2
# import numpy as np

# from stable_baselines3 import PPO
# from stable_baselines3.common.env_util import make_vec_env
# from stable_baselines3.common.vec_env import SubprocVecEnv
# from volleyballenv.envs.VolleybotEnv import VolleybotEnv


# # Initialize GLFW
# glfw.init()

# def train_PPO(env):
#     pass


# def main():
#     # Initialize the environment
#     env = VolleybotEnv(100, render_mode="rgb_array", 
#                     obs_space=["ball_landing_location", "robot_location"], random_seed=0,
#                     viewer="robot")

#     model = PPO("MultiInputPolicy", env, verbose=1, n_steps=256, batch_size=16, n_epochs=256)
#     t1 = time.time()
#     t_steps = 32768 # 1024*8*8 # 32768
#     model.learn(total_timesteps=t_steps) # 1024
#     t2 = time.time()
#     print("Trained in ", t2-t1, " seconds.")

#     # TODO: log what experiment this goes with (same id as wandb maybe?)
#     model.save(f"models/ppo_robot_{t_steps}steps") 

# if __name__ == "__main__":
#     main()