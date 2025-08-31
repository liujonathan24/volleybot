#ifndef COMPOSITE_OBJECT_H
#define COMPOSITE_OBJECT_H

#include "primitive.h"
#include <vector>
#include <memory>

// Represents a single part of a composite object, holding the shape
// and its transform relative to the object's center of mass.
struct BodyPart {
    std::unique_ptr<Primitive> primitive;
    Mat4 local_transform; 
};

class CompositeObject : public Primitive {
public:
    CompositeObject(std::shared_ptr<Material> mat);

    // Add a primitive to this composite object at a specific local position and orientation
    void add_part(std::unique_ptr<Primitive> part, Vec3 local_position, Vec3 local_rotation_axis, float local_rotation_angle_rad);

    // Overridden functions from Primitive
    void compute_aabb() override;
    
    // This will be called by the scene to update all parts' world positions
    void update_child_transforms();

    const std::vector<BodyPart>& get_parts() const { return parts; }

private:
    std::vector<BodyPart> parts;
};

#endif // COMPOSITE_OBJECT_H
