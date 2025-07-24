# This file will be invoked to train a bot
# 1) Get the parameters from command line using modal
# 2) Train the bot in the cloud by invoking functions in the agents dir
# 3) Get the results back (in some way, I am still deciding)

import modal

# Set up the modal app
app = modal.App("volleybot-train")

# Set up our container with the dependencies that we need
pytorch_image = modal.Image.debian_slim(python_version="3.12").pip_install(
    "torch==2.7.1",
    "wandb"
)

@app.function(image=pytorch_image)
async def test_modal_pytorch(num : int):

    import torch
    import wandb
    import random

    wandb.login(key="d063406be11d24a3c2758b5b62c8644afbf643c6")
    
    # Initialize wandb
    run = wandb.init(entity="volleybot", project="test", config={"fake-epochs":10})
    
    # Simulate training.
    epochs = 10
    offset = random.random() / 5
    for epoch in range(2, epochs):
        acc = 1 - 2**-epoch - random.random() / epoch - offset
        loss = 2**-epoch + random.random() / epoch + offset

        # Log metrics to wandb.
        run.log({"acc": acc, "loss": loss})
    
    x = torch.tensor([1, 2, 3, 4, 5])
    print("Hello from Modal + Pytorch") 
    print(f"My tensor is {x}")

    y = x + num
    print(f"After adding num ({num}), my tensor is {y}")

    run.finish()

@app.local_entrypoint()
async def main(num : int):
    
    await test_modal_pytorch.remote(num)
