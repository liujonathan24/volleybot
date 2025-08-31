#ifndef SCENE_H
#define SCENE_H

#include "primitive.h"
#include "composite_object.h"
#include "joint.h"
#include "camera.h"
#include "light.h"
#include <string>
#include <vector>
#include <memory>

struct CollisionConstraint {
    Primitive* a;
    Primitive* b;
    Vec3 normal;
    float depth;
    // For solver
    float accumulated_impulse;
};

class Scene {
public:
    Scene();
    ~Scene();

    void step(float dt);
    void render();

    void add_primitive(std::unique_ptr<Primitive> primitive);
    void add_composite_object(std::unique_ptr<CompositeObject> object);
    void add_joint(std::unique_ptr<Joint> joint);
    void add_light(std::unique_ptr<Light> light);
    void set_camera(std::unique_ptr<Camera> camera);

    Primitive* load_obj_as_primitive(const std::string& filepath, std::shared_ptr<Material> material);

private:
    void broad_phase();
    void narrow_phase(Primitive* a, Primitive* b);
    void solve_constraints(float dt);

    /**
     * TODO: This is for you to implement!
     * After the solver corrects velocities, this method should correct positions.
     * Loop through all constraints in 'collision_constraints' and move the objects
     * apart along the collision normal by the penetration depth.
     * A simple approach is to move each object by half the depth.
     * A more advanced approach (Baumgarte stabilization) uses a factor to prevent over-correction.
     */
    void resolve_penetration();

    std::vector<std::unique_ptr<Primitive>> physics_bodies;
    std::vector<std::unique_ptr<Joint>> joints;
    std::vector<std::unique_ptr<Light>> lights;
    std::unique_ptr<Camera> active_camera;
    Vec3 gravity;

    std::vector<CollisionConstraint> collision_constraints;
};

#endif // SCENE_H
