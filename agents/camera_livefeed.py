import time
import glfw
import cv2
import numpy as np
from volleyballenv.envs.VolleybotEnv import VolleybotEnv

# Initialize GLFW
glfw.init()

# Initialize the environment
env = VolleybotEnv(100, render_mode="rgb_array", 
                   obs_space=["bounding_box", "camera"], random_seed=0,
                   viewer="robot")
print("Episode length:", env.episode_len)
print("Observation space:", env.observation_space)


env.step([])
cv2.namedWindow("Volleybot Robot Feed", cv2.WINDOW_NORMAL)

# Simulation loop
for i in range(10000):
    # Get observations
    obs = env._get_obs()
    frame = env.camera_obs
    
    if frame is not None and isinstance(frame, np.ndarray):
        # Convert from RGB to BGR (which cv2 uses)
        frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        
        # Display the frame in the OpenCV window
        cv2.imshow("Volleybot Camera Feed", frame_bgr)
    
    env.step([])
    # time.sleep(0.25)
    
    # Exit early by pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

env.close()
cv2.destroyAllWindows()
glfw.terminate()