
# kinematics.py - Basic Kinematics Test

# This script tests the basic kinematic integration (position and velocity updates)
# without gravity or collisions. An object is given an initial velocity,
# and its position and velocity are logged over time.


import volleybot_physics as vbp
import time

def main():
    print("--- Kinematics Test (Constant Velocity) ---")

    # 1. Create the scene
    scene = vbp.Scene()

    # 2. Set gravity to zero for this test
    # scene.gravity = vbp.Vec3(0, 0, 0)

    # 3. Create a material for our test object
    test_material = vbp.Material()
    test_material.mass = 1.0 # Needs mass to be affected by impulses later

    # 4. Create a test object (e.g., a sphere) with an initial position and velocity
    test_object = vbp.Sphere(0.5, test_material)
    test_object.set_position(vbp.Vec3(0, 10, 0)) # Start at (0, 10, 0)
    test_object.set_velocity(vbp.Vec3(1, 0, 0))  # Move along +X axis
    scene.add_primitive(test_object)

    print(f"Initial Position: {test_object.get_position().x:.2f}, {test_object.get_position().y:.2f}, {test_object.get_position().z:.2f}")
    print(f"Initial Velocity: {test_object.get_velocity().x:.2f}, {test_object.get_velocity().y:.2f}, {test_object.get_velocity().z:.2f}")

    # 5. Run the simulation
    dt = 1.0 / 60.0 # Time step
    total_sim_time = 5.0 # Simulate for 5 seconds
    num_steps = int(total_sim_time / dt)

    print("\nRunning simulation...")
    for i in range(num_steps):
        scene.step(dt)
        
        if i % 30 == 0: # Log every half second
            pos = test_object.get_position()
            vel = test_object.get_velocity()
            print(f"Time: {i*dt:.2f}s | Pos: ({pos.x:.2f}, {pos.y:.2f}, {pos.z:.2f}) | Vel: ({vel.x:.2f}, {vel.y:.2f}, {vel.z:.2f})")
        
        # time.sleep(0.001) # Optional: slow down for real-time observation

    print("\nSimulation finished.")

if __name__ == "__main__":
    main()
