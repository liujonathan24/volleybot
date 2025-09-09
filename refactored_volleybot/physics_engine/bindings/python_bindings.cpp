#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <memory>

// Core
#include "physics_core/vec3.h"

// C++ Layer
#include "volleybot_physics/scene.h"
#include "volleybot_physics/material.h"
#include "volleybot_physics/primitive.h"
#include "volleybot_physics/composite_object.h"
#include "volleybot_physics/joint.h"

// Concrete Primitives
#include "volleybot_physics/primitive.h"

namespace py = pybind11;

PYBIND11_MODULE(volleybot_physics, m) {
    m.doc() = "Python bindings for the VolleyBot physics engine";

    // --- Core Structs ---
    py::class_<Vec3>(m, "Vec3")
        .def(py::init<float, float, float>(), py::arg("x")=0, py::arg("y")=0, py::arg("z")=0)
        .def_readwrite("x", &Vec3::x)
        .def_readwrite("y", &Vec3::y)
        .def_readwrite("z", &Vec3::z)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * float())
        .def("__repr__", [](const Vec3 &v) {
            return "<Vec3(x=" + std::to_string(v.x) + ", y=" + std::to_string(v.y) + ", z=" + std::to_string(v.z) + ")>";
        });

    py::class_<Material, std::shared_ptr<Material>>(m, "Material")
        .def(py::init<>())
        .def_readwrite("mass", &Material::mass)
        .def_readwrite("friction", &Material::friction)
        .def_readwrite("restitution", &Material::restitution)
        .def_readwrite("color", &Material::color)
        .def_readwrite("shininess", &Material::shininess);

    // --- Primitives (for construction in Python) ---
    py::class_<Primitive, std::shared_ptr<Primitive>>(m, "Primitive")
        .def("get_position", &Primitive::get_position)
        .def("get_velocity", &Primitive::get_velocity)
        .def("get_angular_velocity", &Primitive::get_angular_velocity)
        .def("set_position", &Primitive::set_position, py::arg("pos"));

    py::class_<Box, Primitive, std::shared_ptr<Box>>(m, "Box")
        .def(py::init<const Vec3&, std::shared_ptr<Material>>(), 
             py::arg("extents"), py::arg("material"))
        .def("set_position", &Box::set_position, py::arg("pos"))
        .def("set_velocity", &Box::set_velocity, py::arg("vel"));

    py::class_<Sphere, Primitive, std::shared_ptr<Sphere>>(m, "Sphere")
        .def(py::init<float, std::shared_ptr<Material>>(), 
             py::arg("radius"), py::arg("material"))
        .def("set_position", &Sphere::set_position, py::arg("pos"))
        .def("set_velocity", &Sphere::set_velocity, py::arg("vel"));

    // --- Joints ---
    py::class_<Joint, std::shared_ptr<Joint>>(m, "Joint"); // Base class

    py::class_<RevoluteJoint, Joint, std::shared_ptr<RevoluteJoint>>(m, "RevoluteJoint")
        .def("set_motor", &RevoluteJoint::set_motor, py::arg("speed"), py::arg("max_force"), "Set the target speed and max force of the joint motor.")
        .def("get_relative_speed", &RevoluteJoint::get_relative_speed, "Get the current relative angular speed along the joint axis.");

    // --- Composite Object ---
    py::class_<CompositeObject, Primitive, std::shared_ptr<CompositeObject>>(m, "CompositeObject")
        .def(py::init<std::shared_ptr<Material>>(), py::arg("material"))
        .def("add_part", &CompositeObject::add_part, 
             py::arg("part"), py::arg("local_position"), py::arg("local_rotation_axis"), py::arg("local_rotation_angle_rad"),
             "Adds a primitive part to the composite object relative to its own origin. Returns the integer ID of the new part.")
        .def("add_revolute_joint", &CompositeObject::add_revolute_joint, 
             py::arg("part_id_a"), py::arg("part_id_b"), py::arg("world_anchor"), py::arg("axis"),
             "Adds a revolute joint (hinge/motor) between two parts using their IDs. The anchor point is defined in world coordinates.")
        .def("get_revolute_joint", &CompositeObject::get_revolute_joint, py::arg("joint_id"), py::return_value_policy::reference, "Get a specific revolute joint by its ID.");

    // --- Scene ---
    py::class_<Scene>(m, "Scene")
        .def(py::init<>())
        .def("step", &Scene::step, py::arg("dt"), "Advance the simulation by one time step")
        .def("add_composite_object", &Scene::add_composite_object, py::arg("object"), "Adds a composite object to the scene.")
        .def("add_primitive", &Scene::add_primitive, py::arg("primitive"), "Adds a single primitive to the scene.");
}
