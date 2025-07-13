from gymnasium.envs.registration import register

register(
    id="RobotVolleyball/VolleyballCourt-v0",
    entry_point="RobotVolleyball.envs:VolleyballCourt",
)
