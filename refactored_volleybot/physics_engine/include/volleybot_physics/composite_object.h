#ifndef COMPOSITE_OBJECT_H
#define COMPOSITE_OBJECT_H

#include "primitive.h"
#include "joint.h"
#include <vector>
#include <memory>

// Represents a single part of a composite object, holding the shape
// and its transform relative to the object's center of mass.
struct BodyPart {
    std::shared_ptr<Primitive> primitive;
    Mat4 local_transform; 
};

class CompositeObject : public Primitive {
public:
    CompositeObject(std::shared_ptr<Material> mat);

    // Add a primitive to this composite object, returning its ID
    int add_part(std::shared_ptr<Primitive> part, Vec3 local_position, Vec3 local_rotation_axis, float local_rotation_angle_rad);

    // Add a joint between two parts using their IDs
    void add_revolute_joint(int part_id_a, int part_id_b, const Vec3& world_anchor, const Vec3& axis);

    // Overridden functions from Primitive
    void compute_aabb() override;
    
    // This will be called by the scene to update all parts' world positions
    void update_child_transforms();

    const std::vector<BodyPart>& get_parts() const { return parts; }
    const std::vector<std::unique_ptr<Joint>>& get_joints() const { return joints; }
    RevoluteJoint* get_revolute_joint(int joint_id);

private:
    // Recalculates the total mass, center of mass, and inertia tensor from all parts
    void compute_mass_and_inertia();

    std::vector<BodyPart> parts;
    std::vector<std::unique_ptr<Joint>> joints;
};

#endif // COMPOSITE_OBJECT_H
