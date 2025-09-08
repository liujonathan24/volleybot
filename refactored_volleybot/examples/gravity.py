
"""
gravity.py - Gravity and Ground Collision Test

This script tests the effect of gravity and basic collision resolution
with a static ground plane. An object is dropped from a height, and its
position and velocity are logged over time.
"""

import volleybot_physics as vbp
import time

def main():
    print("--- Gravity and Collision Test ---")

    # 1. Create the scene
    scene = vbp.Scene()

    # 2. Set gravity to a realistic value
    # scene.gravity = vbp.Vec3(0, -9.81, 0) # Standard Earth gravity

    # 3. Create materials
    ball_material = vbp.Material()
    ball_material.mass = 1.0
    ball_material.restitution = 0.8 # Make it a bit bouncy

    ground_material = vbp.Material()
    ground_material.mass = 0.0 # Static object
    ground_material.restitution = 0.5

    # 4. Create the test object (a sphere) and ground plane
    test_ball = vbp.Sphere(0.5, ball_material)
    test_ball.set_position(vbp.Vec3(0, 5, 0)) # Drop from 5 units high
    scene.add_primitive(test_ball)

    ground_plane = vbp.Box(vbp.Vec3(10, 0.5, 10), ground_material) # Large flat box
    ground_plane.set_position(vbp.Vec3(0, -0.25, 0)) # Position it so top is at Y=0
    scene.add_primitive(ground_plane)

    print(f"Initial Ball Position: {test_ball.get_position().x:.2f}, {test_ball.get_position().y:.2f}, {test_ball.get_position().z:.2f}")
    print(f"Initial Ball Velocity: {test_ball.get_velocity().x:.2f}, {test_ball.get_velocity().y:.2f}, {test_ball.get_velocity().z:.2f}")

    # 5. Run the simulation
    dt = 1.0 / 60.0 # Time step
    total_sim_time = 5.0 # Simulate for 5 seconds
    num_steps = int(total_sim_time / dt)

    print("\nRunning simulation...")
    for i in range(num_steps):
        scene.step(dt)
        
        if i % 30 == 0: # Log every half second
            pos = test_ball.get_position()
            vel = test_ball.get_velocity()
            print(f"Time: {i*dt:.2f}s | Pos: ({pos.x:.2f}, {pos.y:.2f}, {pos.z:.2f}) | Vel: ({vel.x:.2f}, {vel.y:.2f}, {vel.z:.2f})")
        
        # time.sleep(0.001) # Optional: slow down for real-time observation

    print("\nSimulation finished.")

if __name__ == "__main__":
    main()
