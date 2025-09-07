#include "volleybot_physics/composite_object.h"

CompositeObject::CompositeObject(std::shared_ptr<Material> mat): Primitive(mat) {
    type = PrimitiveType::COMPOSITE;
}

int CompositeObject::add_part(std::shared_ptr<Primitive> part, Vec3 local_position, Vec3 local_rotation_axis, float local_rotation_angle_rad) {
    Mat4 translation, rotation;
    mat4_translate(local_position, &translation);
    mat4_rotate(local_rotation_axis, local_rotation_angle_rad, &rotation);
    
    Mat4 final_transform;
    mat4_multiply(&translation, &rotation, &final_transform);

    parts.push_back({ std::move(part), final_transform });

    // Recalculate the aggregate properties whenever a new part is added
    compute_mass_and_inertia();

    // Return the index of the part we just added as its ID
    return parts.size() - 1;
}

RevoluteJoint* CompositeObject::get_revolute_joint(int joint_id) {
    if (joint_id >= joints.size()) {
        return nullptr;
    }
    // Use dynamic_cast to ensure the joint is of the correct type
    return dynamic_cast<RevoluteJoint*>(joints[joint_id].get());
}

void CompositeObject::add_revolute_joint(int part_id_a, int part_id_b, const Vec3& world_anchor, const Vec3& axis) {
    if (part_id_a >= parts.size() || part_id_b >= parts.size()) {
        // Handle error: part ID out of bounds
        return;
    }

    Primitive* bodyA = parts[part_id_a].primitive.get();
    Primitive* bodyB = parts[part_id_b].primitive.get();

    joints.push_back(std::make_unique<RevoluteJoint>(bodyA, bodyB, world_anchor, axis));
}

void CompositeObject::compute_mass_and_inertia() {
    // Calculate Total Mass and Combined Center of Mass

    float total_mass = 0.0f;
    Vec3 com_accumulator = {0,0,0}; // A temporary vector to sum weighted positions

    // Combine the first two loops into one for efficiency.
    for (const auto& part : parts) {
        float part_mass = part.primitive->get_material()->mass;
        total_mass += part_mass;

        // Get the part's local position
        Vec3 part_pos = mat4_transform_point(&part.local_transform, {0,0,0});
        
        // Scale the position by the mass before adding it to the sum.
        Vec3 scaled_pos;
        vec3_scale(&part_pos, part_mass, &scaled_pos);
        vec3_add(&com_accumulator, &scaled_pos, &com_accumulator);
    }

    // Update the final mass and calculate the center of mass for the whole object
    this->material->mass = total_mass;
    if (total_mass > 0.0f) {
        vec3_scale(&com_accumulator, 1.0f / total_mass, &this->center_of_mass);
    }

    // Calculate Combined Inertia Tensor
    Mat4 total_inertia_tensor;
    mat4_zero(&total_inertia_tensor); // Use helper to initialize the matrix to zeros

    for (const auto& part : parts) {
        // Get the inertia tensor of the part relative to its own center of mass
        Mat4 part_inertia = part.primitive->get_inertia_tensor();
        float part_mass = part.primitive->get_material()->mass;
        Vec3 part_pos = mat4_transform_point(&part.local_transform, {0,0,0});

        // Calculate the displacement vector 'd' from the composite CoM to the part's CoM
        Vec3 d;
        vec3_sub(&part_pos, &this->center_of_mass, &d);

        // This is the Parallel Axis Theorem: I_new = I_part + M * (d²E - d⊗d)
        Mat4 parallel_axis_term;
        float d_sq = vec3_dot(&d, &d);
        parallel_axis_term.m[0][0] = part_mass * (d_sq - d.x * d.x);
        parallel_axis_term.m[0][1] = part_mass * (0.0f - d.x * d.y);
        parallel_axis_term.m[0][2] = part_mass * (0.0f - d.x * d.z);

        parallel_axis_term.m[1][0] = part_mass * (0.0f - d.y * d.x);
        parallel_axis_term.m[1][1] = part_mass * (d_sq - d.y * d.y);
        parallel_axis_term.m[1][2] = part_mass * (0.0f - d.y * d.z);

        parallel_axis_term.m[2][0] = part_mass * (0.0f - d.z * d.x);
        parallel_axis_term.m[2][1] = part_mass * (0.0f - d.z * d.y);
        parallel_axis_term.m[2][2] = part_mass * (d_sq - d.z * d.z);
        
        // Ensure the 4th row/col are 0s and 1 for a proper transform matrix
        parallel_axis_term.m[3][3] = 1.0f; 
        parallel_axis_term.m[0][3] = parallel_axis_term.m[1][3] = parallel_axis_term.m[2][3] = 0.0f;
        parallel_axis_term.m[3][0] = parallel_axis_term.m[3][1] = parallel_axis_term.m[3][2] = 0.0f;

        // add the part's own inertia and the parallel axis term to the total
        Mat4 temp_sum;
        mat4_add(&total_inertia_tensor, &part_inertia, &temp_sum);
        mat4_add(&temp_sum, &parallel_axis_term, &total_inertia_tensor);
    }

    // Update Final Properties
    this->inertia_tensor = total_inertia_tensor;
    mat3_inverse(&this->inertia_tensor, &this->inverse_inertia_tensor);
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
