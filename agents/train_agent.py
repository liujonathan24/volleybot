import subprocess
import argparse

def main(args):
    # Execute a command and capture output
    result = subprocess.run(['ls', '-l'], capture_output=True, text=True, check=True)

    # Execute a command without capturing output
    subprocess.run(['mkdir', 'new_directory'])


if __name__ == "__main__":
    # Specify parameters and models 
    parser = argparse.ArgumentParser("Provide arguments for the RL model to train.")
    parser.add_argument("--model", "-m", default="PPO", type=str, help="Current implemented algorithms is: PPO")
    parser.add_argument("--steps", "-t", default=100, type=int)
    parser.add_argument("--batch_size", "-b", default=1, type=int)

    args = parser.parse_args()
    main(args)