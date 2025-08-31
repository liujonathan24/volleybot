#include "volleybot_physics/composite_object.h"

CompositeObject::CompositeObject(std::shared_ptr<Material> mat)
    : Primitive(mat) {
    type = PrimitiveType::COMPOSITE;
}

void CompositeObject::add_part(std::unique_ptr<Primitive> part, Vec3 local_position, Vec3 local_rotation_axis, float local_rotation_angle_rad) {
    Mat4 translation, rotation;
    mat4_translate(local_position, &translation);
    mat4_rotate(local_rotation_axis, local_rotation_angle_rad, &rotation);
    
    Mat4 final_transform;
    mat4_multiply(&translation, &rotation, &final_transform);

    parts.push_back({ std::move(part), final_transform });
}

void CompositeObject::update_child_transforms() {
    const Mat4& parent_transform = get_transform();
    for (auto& part : parts) {
        Mat4 child_world_transform;
        mat4_multiply(&parent_transform, &part.local_transform, &child_world_transform);
        
        // This is a simplified update. A full rigid body system would be more complex.
        // For now, we just place the children relative to the parent.
        Vec3 child_pos = mat4_transform_point(&child_world_transform, {0,0,0});
        part.primitive->set_position(child_pos);
    }
}

void CompositeObject::compute_aabb() {
    if (parts.empty()) {
        return;
    }

    // First, ensure all children have their AABBs computed in their local space
    // and then transform them to world space to find the composite AABB.
    update_child_transforms();

    bool first = true;
    for (const auto& part : parts) {
        part.primitive->compute_aabb();
        const AABB& child_aabb = part.primitive->get_aabb();

        if (first) {
            aabb = child_aabb;
            first = false;
        } else {
            // Expand the main AABB to include the child's AABB
            aabb.min.x = fminf(aabb.min.x, child_aabb.min.x);
            aabb.min.y = fminf(aabb.min.y, child_aabb.min.y);
            aabb.min.z = fminf(aabb.min.z, child_aabb.min.z);
            aabb.max.x = fmaxf(aabb.max.x, child_aabb.max.x);
            aabb.max.y = fmaxf(aabb.max.y, child_aabb.max.y);
            aabb.max.z = fmaxf(aabb.max.z, child_aabb.max.z);
        }
    }
}
