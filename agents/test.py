from volleyballenv.envs.VolleybotEnv import VolleybotEnv
import time
import glfw
glfw.init()

env = VolleybotEnv(100, ["bounding_box", "camera"], random_seed=1)
print(env.episode_len)
env.step([])
time.sleep(5)
# print(env.observation_space)
for i in range(300):
    print(env._get_obs())
    env.step([])
    time.sleep(.5)

# print(env._get_obs())
# env.step([])
# print(env._get_obs())
# env.step([])
# print(env._get_obs())
# env.step([])
# print(env._get_obs())
# print(env.camera_obs)
# print(env.camera_obs.shape)
print()
# print(env.render())
# img = Image.fromarray(env.camera_obs)
# img.save(f"mujoco_frame_{1}.png")

# env.close()