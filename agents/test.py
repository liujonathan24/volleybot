from volleyballenv.envs.VolleybotEnv import VolleybotEnv
import time
import glfw
from PIL import Image
glfw.init()

env = VolleybotEnv(100, obs_space=["bounding_box", "camera"], random_seed=0)

# time.sleep(5)
print(env.observation_space)
for i in range(1000000):
    # print(env._get_obs())
    env.step([1,-1])

    if i%100 == 0:
        env.reset()
    # time.sleep(0.25)

    # img = Image.fromarray(env.camera_obs)
    # img.save(f"mujoco_frame_{i}.png")


# print(env.camera_obs)
# print(env.camera_obs.shape)
print()
# print(env.render())
# print(env.camera_obs)
env.close()