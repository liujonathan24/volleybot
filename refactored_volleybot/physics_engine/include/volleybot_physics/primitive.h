#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "physics_core/vec3.h"
#include "physics_core/mat4.h"
#include "material.h"
#include <memory>
#include <vector>

// Defines an Axis-Aligned Bounding Box
struct AABB {
    Vec3 min;
    Vec3 max;
};

enum class PrimitiveType {
    SPHERE,
    BOX,
    CYLINDER,
    MESH,
    COMPOSITE // For composite objects
};

class Primitive {
public:
    Primitive(std::shared_ptr<Material> mat);
    virtual ~Primitive() = default;

    void update_physics(float dt, Vec3 gravity);

    // Computes the world-space AABB for the primitive
    virtual void compute_aabb() = 0;

    PrimitiveType get_type() const { return type; }
    std::shared_ptr<Material> get_material() const { return material; }

    Vec3 get_position() const { return position; }
    Vec3 get_velocity() const { return velocity; }
    Vec3 get_angular_velocity() const { return angular_velocity; }
    Vec3 get_center_of_mass() const { return center_of_mass; }
    const Mat4& get_transform() const { return transform; }
    const AABB& get_aabb() const { return aabb; }
    const Mat4& get_inertia_tensor() const { return inertia_tensor; }
    const Mat4& get_inverse_inertia_tensor() const { return inverse_inertia_tensor; }

    void set_position(const Vec3& pos);
    void set_velocity(const Vec3& vel);
    void apply_force(const Vec3& force);

    // Applies an impulse at a specific point, affecting both linear and angular velocity
    void apply_impulse(const Vec3& impulse, const Vec3& world_contact_point);
    void apply_angular_impulse(const Vec3& impulse);

protected:
    // Linear Motion
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;

    // Angular Motion
    Vec3 angular_velocity;
    Vec3 center_of_mass; // In local coordinates
    Mat4 inertia_tensor; // In local coordinates
    Mat4 inverse_inertia_tensor;

    // General Properties
    Mat4 transform;
    std::shared_ptr<Material> material;
    AABB aabb;
    PrimitiveType type;
};

class Sphere : public Primitive {
public:
    Sphere(float radius, std::shared_ptr<Material> mat);
    void compute_aabb() override;
    float get_radius() const { return radius; }
private:
    float radius;
};

class Box : public Primitive {
public:
    Box(const Vec3& extents, std::shared_ptr<Material> mat);
    void compute_aabb() override;
    Vec3 get_extents() const { return extents; }
private:
    Vec3 extents;
};

// Represents geometry loaded from a file, like an .obj file.
class TriangleMesh : public Primitive {
public:
    TriangleMesh(std::shared_ptr<Material> mat);

    const std::vector<Vec3>& get_vertices() const { return vertices; }
    const std::vector<unsigned int>& get_indices() const { return indices; }

private:
    // The Scene will be responsible for filling this data during load time.
    friend class Scene;
    std::vector<Vec3> vertices;
    std::vector<unsigned int> indices;
};

class Cylinder : public Primitive {
public:
    Cylinder(float height, float radius, int sides, std::shared_ptr<Material> mat);
    float get_height() const { return height; }
    float get_radius() const { return radius; }
    int get_sides() const { return sides; }
private:
    float height;
    float radius;
    int sides;
};

#endif // PRIMITIVE_H
