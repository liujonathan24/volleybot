# This script demonstrates how to use the `volleybot_physics` Python API
# to construct and simulate a complex, multi-part rigid body (a simple car).

# Workflow:
# 1. Create a Scene to hold all objects.
# 2. Define Materials for the objects.
# 3. Create a CompositeObject to represent the car.
# 4. Create Primitive shapes (Box, Sphere) for the chassis and wheels.
# 5. Add the shapes as parts to the CompositeObject, defining their local positions.
# 6. Add RevoluteJoints to connect the wheels to the chassis.
# 7. Add the completed car and a ground plane to the Scene.
# 8. Run the simulation for a few steps.


import volleybot_physics as vbp

def create_car_example():
    """
    Demonstrates creating a composite object with joints.
    """
    print("--- Creating Car Example ---")

    # A shared material for all car parts
    car_material = vbp.Material()
    car_material.mass = 10.0
    car_material.friction = 0.8

    # 1. Create the main CompositeObject that will represent the car.
    #    The car itself will be positioned at the world origin by default.
    car = vbp.CompositeObject(car_material)

    # 2. Add parts to the composite object.
    #    Positions are defined relative to the CompositeObject's own origin.
    
    # Add a box for the chassis. It becomes part ID 0.
    chassis_shape = vbp.Box(vbp.Vec3(1.0, 0.5, 2.0), car_material)
    # The chassis is centered at the car's origin.
    chassis_id = car.add_part(chassis_shape, vbp.Vec3(0, 0.5, 0), vbp.Vec3(0, 0, 0), 0)
    print(f"Added chassis with ID: {chassis_id}")

    # Add four spheres for the wheels. They will get IDs 1, 2, 3, and 4.
    wheel_radius = 0.4
    wheel_shape = vbp.Sphere(wheel_radius, car_material)
    
    # Positions of the wheels relative to the car's center
    wheel_positions = [
        vbp.Vec3(-1.1, 0, 1.5),  # Front-left
        vbp.Vec3( 1.1, 0, 1.5),  # Front-right
        vbp.Vec3(-1.1, 0, -1.5), # Back-left
        vbp.Vec3( 1.1, 0, -1.5)  # Back-right
    ]

    wheel_ids = []
    for pos in wheel_positions:
        # Note: We are passing a new Sphere object each time.
        wheel_id = car.add_part(vbp.Sphere(wheel_radius, car_material), pos, vbp.Vec3(0, 0, 0), 0)
        wheel_ids.append(wheel_id)
        print(f"Added wheel with ID: {wheel_id}")

    # 3. Add joints to connect the parts.
    #    Joints are defined by the IDs of the parts they connect, an anchor
    #    point (in WORLD coordinates), and an axis of rotation.
    axle_axis = vbp.Vec3(1, 0, 0) # Wheels rotate around the x-axis

    for i, wheel_id in enumerate(wheel_ids):
        # Since the car is being created at the world origin, the part's local
        # position is currently the same as its world position.
        anchor_point = wheel_positions[i]
        car.add_revolute_joint(chassis_id, wheel_id, anchor_point, axle_axis)
        print(f"Added joint between chassis ({chassis_id}) and wheel ({wheel_id})")

    return car


def main():
    """
    Main function to set up and run the simulation.
    """
    # 1. Create the main simulation scene
    scene = vbp.Scene()

    # 2. Create the car and add it to the scene
    car = create_car_example()
    scene.add_composite_object(car)

    # 3. Create a ground plane for the car to drive on
    ground_material = vbp.Material()
    ground_material.mass = 0.0 # A mass of 0 makes the object static (immovable)
    ground_material.friction = 1.0
    
    # Note: To position this ground plane, you would need to bind the
    # set_position() method for primitives. For now, it's created at the origin.
    ground_shape = vbp.Box(vbp.Vec3(50, 1, 50), ground_material)
    scene.add_primitive(ground_shape)
    print("\nAdded ground plane.")

    # 4. Run the simulation
    print("\n--- Running Simulation ---")
    dt = 1.0 / 60.0
    for i in range(121):
        scene.step(dt)
        if i % 60 == 0:
            print(f"Simulated {i * dt:.2f} seconds...")
            # To get data, you would need to bind getter methods like:
            # car_pos = car.get_position()
            # print(f"  Car position: {car_pos.x}, {car_pos.y}, {car_pos.z}")

    print("\nSimulation finished.")

if __name__ == "__main__":
    main()

