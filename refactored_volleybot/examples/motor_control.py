
# motor_control.py - Interactive Motor Control Example

# This script demonstrates how to control a motor on a composite object
# and read back its state.



import time
import volleybot_physics as vbp

def create_car_example():
    """
    Builds a simple car composite object.
    """
    car_material = vbp.Material()
    car_material.mass = 10.0
    car_material.friction = 0.8
    car = vbp.CompositeObject(car_material)

    chassis_shape = vbp.Box(vbp.Vec3(1.0, 0.5, 2.0), car_material)
    chassis_id = car.add_part(chassis_shape, vbp.Vec3(0, 0.5, 0), vbp.Vec3(0, 0, 0), 0)

    wheel_radius = 0.4
    wheel_positions = [
        vbp.Vec3(-1.1, 0, 1.5),  # Front-left
        vbp.Vec3( 1.1, 0, 1.5),  # Front-right
    ]
    wheel_ids = []
    for pos in wheel_positions:
        wheel_id = car.add_part(vbp.Sphere(wheel_radius, car_material), pos, vbp.Vec3(0, 0, 0), 0)
        wheel_ids.append(wheel_id)

    axle_axis = vbp.Vec3(1, 0, 0)
    # We will only control the first wheel (joint 0)
    car.add_revolute_joint(chassis_id, wheel_ids[0], wheel_positions[0], axle_axis)
    # The second wheel is just a passive hinge for now
    car.add_revolute_joint(chassis_id, wheel_ids[1], wheel_positions[1], axle_axis)

    return car

def main():
    scene = vbp.Scene()
    car = create_car_example()
    scene.add_composite_object(car)

    ground_material = vbp.Material()
    ground_material.mass = 0.0
    ground_material.friction = 10.0
    ground = vbp.Box(vbp.Vec3(50, 1, 50), ground_material)
    scene.add_primitive(ground)

    # --- Simulation Loop ---
    print("\n--- Running Interactive Simulation ---")
    dt = 1.0 / 60.0
    
    # Get the first joint (the one we will control)
    # Note: The CompositeObject stores joints in the order they are added.
    # So, the first joint we added has ID 0.
    wheel_motor = car.get_revolute_joint(0)

    if not wheel_motor:
        print("Error: Could not get joint 0")
        return

    # Set a target speed (radians/sec) and max torque for the motor
    target_speed = 1.0 
    max_torque = 1000.0
    # wheel_motor.set_motor(target_speed, max_torque)

    print(f"Motor control active. Target Speed: {target_speed:.2f} rad/s, Max Torque: {max_torque:.2f}")
    print("-----------------------------------------------------")

    for i in range(180): # Simulate for 3 seconds
        scene.step(dt)
        
        if i % 10 == 0:
            current_speed = wheel_motor.get_relative_speed()
            print(f"Step {i:03d} | Current Wheel Speed: {current_speed:.2f} rad/s")
        
        # This sleep is just to make the output readable in real-time.
        # In a real application, you would not have a sleep here.
        time.sleep(0.01)

    print("-----------------------------------------------------")
    print("\nSimulation finished.")

if __name__ == "__main__":
    main()
