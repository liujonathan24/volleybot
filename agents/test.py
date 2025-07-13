from volleyballenv.envs.VolleybotEnv import VolleybotEnv

env = VolleybotEnv(100, ["bounding_box", "camera"])
print(env.episode_len)
print(env.observation_space)