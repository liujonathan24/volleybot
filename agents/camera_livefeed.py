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
                   viewer="robot", noise=True)
print("Episode length:", env.episode_len)
print("Observation space:", env.observation_space)


env.step([0,0])
# env.data.joint("ball").qpos[1] = -0.45
cv2.namedWindow("Volleybot Robot Feed", cv2.WINDOW_NORMAL)

# Simulation loop
for i in range(10000):
    # Get observations
    obs = env._get_obs()
    frame = env.camera_obs
    
    if frame is not None and isinstance(frame, np.ndarray):
        # Convert from RGB to BGR (which cv2 uses)
        frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

        # Overlay bounding boxes if 'bounding_box' observation exists
        if "bounding_box" in obs and obs["bounding_box"] is not None:
            # The structure of obs["bounding_box"] depends on your environment.
            # Assuming obs["bounding_box"] is a list of dictionaries,
            # where each dictionary has 'x_min', 'y_min', 'x_max', 'y_max' keys
            # representing pixel coordinates.
            # You might need to adjust this based on the actual output of your env.
            # for bbox_info in obs["bounding_box"]:
            bbox_info = obs["bounding_box"]
                # Ensure the bbox_info contains the expected keys and are integers
            
            x_min = int(bbox_info[0].item())
            y_min = int(bbox_info[1].item())
            x_max = int(bbox_info[2].item())
            y_max = int(bbox_info[3].item())
            print(bbox_info)

            # Define color (BGR) and thickness for the bounding box
            color = (0, 255, 0)  # Green color
            thickness = 2

            # Draw the rectangle on the frame
            cv2.rectangle(frame_bgr, (x_min, y_min), (x_max, y_max), color, thickness)
        
        # Display the frame in the OpenCV window
        cv2.imshow("Volleybot Camera Feed", frame_bgr)
        
    
    env.step([0,0])
    
    time.sleep(0.25)
    
    # Exit early by pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

env.close()
cv2.destroyAllWindows()
glfw.terminate()