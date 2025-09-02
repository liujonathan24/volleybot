#include "volleybot_physics/composite_object.h"

CompositeObject::CompositeObject(std::shared_ptr<Material> mat): Primitive(mat) {
    type = PrimitiveType::COMPOSITE;
}

void CompositeObject::add_part(std::unique_ptr<Primitive> part, Vec3 local_position, Vec3 local_rotation_axis, float local_rotation_angle_rad) {
    Mat4 translation, rotation;
    mat4_translate(local_position, &translation);
    mat4_rotate(local_rotation_axis, local_rotation_angle_rad, &rotation);
    
    Mat4 final_transform;
    mat4_multiply(&translation, &rotation, &final_transform);

    parts.push_back({ std::move(part), final_transform });

    // Recalculate the aggregate properties whenever a new part is added
    compute_mass_and_inertia();
}

void CompositeObject::compute_mass_and_inertia() {
    // TODO: This is for you to implement.
    // This function should calculate the following properties for the composite body:
    // - material->mass (total mass)
    // - center_of_mass (in the composite's local coordinates)
    // - inertia_tensor (relative to the new center of mass)

    // 1. Calculate Total Mass & Weighted Position Sum:
    //    - Initialize total_mass = 0 and a weighted_position_sum vector = {0,0,0}.
    //    - Loop through each `part` in the `parts` vector.
    //    - Add the part's material->mass to the total_mass.
    //    - Get the part's local position from its `local_transform`.
    //    - Add (part_mass * part_local_position) to weighted_position_sum.
    float total_mass = 0;
    Vec3 weighted_position_sum = {0,0,0};
    for (auto& part : parts) {
        total_mass += part.material->mass
    }
    for (auto& part : parts) {
        Vec3 temp_pos;
        float weight = part.material->mass/total_mass;
        vec3_scale(part.local_transform, weight, temp_pos);
        vec3_add(temp_pos, weighted_position_sum, weighted_position_sum)
    }
    // 2. Calculate Combined Center of Mass:
    //    - If total_mass > 0, the new center_of_mass is weighted_position_sum / total_mass.
    //    - This is the center of mass in the composite object's local space.

    // 3. Calculate Combined Inertia Tensor:
    //    - Initialize a total_inertia_tensor matrix to all zeros.
    //    - Loop through each `part` again.
    //    - For each part, you need its inertia tensor *relative to the composite's center of mass*.
    //    - This requires the Parallel Axis Theorem: I_total = I_part + mass * (d^2 * Identity - d*d^T)
    //      where `d` is the vector from the composite's center of mass to the part's center of mass.
    //    - Add this calculated tensor to the total_inertia_tensor.
    //    - (This is a complex step, requiring matrix and vector operations).

    // 4. Update Properties:
    //    - Set this->material->mass = total_mass.
    //    - Set this->center_of_mass = the new calculated center of mass.
    //    - Set this->inertia_tensor = total_inertia_tensor.
    //    - Compute and store the inverse_inertia_tensor.
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
