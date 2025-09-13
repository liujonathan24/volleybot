import subprocess
import argparse
import logging
# import stable_agent_PPO as PPO
from stable_baselines3 import PPO
from volleyballenv.envs.VolleybotEnv import VolleybotEnv

def get_model(args, env):
    #TODO: make wrappers for each model
    if args.model == "PPO":
        return PPO("MultiInputPolicy", env, 
                   verbose=1, 
                   n_steps=args.n_steps, 
                   batch_size=args.batch_size, 
                   n_epochs=args.n_epochs
                   )
    raise ValueError(f"{args.model} not yet implemented.")

def main(args):
    logging.basicConfig(filename=f"../logs/training_{args.run_name}")
    logger = logging.Logger()
    logger.info(f"Writing log to ../logs/training_{args.run_name}")

    # Execute a command and capture output
    # result = subprocess.run(['ls', '-l'], capture_output=True, text=True, check=True)

    # Execute a command without capturing output
    subprocess.run(['mkdir', f'../logs/training_{args.run_name}'])
 
    # Initialize the environment
    env = VolleybotEnv(100, render_mode="rgb_array", 
                    obs_space=["ball_landing_location"], random_seed=0,
                    viewer="robot")
    
    # Training loop:
    model = get_model(args, env)
    model.train() # add this function to wrappers so that it's easy to run 


if __name__ == "__main__":
    # Specify parameters and models 
    parser = argparse.ArgumentParser("Provide arguments for the RL model to train.")
    parser.add_argument("--model", "-m", default="PPO", type=str, help="Current implemented algorithms is: PPO")
    parser.add_argument("--n_steps", "-n", default=100, type=int)
    parser.add_argument("--batch_size", "-b", default=1, type=int)
    parser.add_argument("--n_epochs", "-ep", default=10, type=int)
    parser.add_argument("--run_name", "-n", default="", type=str)

    args = parser.parse_args()
    main(args)