#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "volleybot_physics/scene.h"
#include "volleybot_physics/primitive.h"
#include "volleybot_physics/material.h"

namespace py = pybind11;

PYBIND11_MODULE(volleybot_physics, m) {
    m.doc() = "Python bindings for the VolleyBot physics engine";

    // --- Bind Core Structs ---
    py::class_<Vec3>(m, "Vec3")
        .def(py::init<float, float, float>())
        .def_readwrite("x", &Vec3::x)
        .def_readwrite("y", &Vec3::y)
        .def_readwrite("z", &Vec3::z);

    py::class_<std::shared_ptr<Material>>(m, "MaterialPtr");
    py::class_<Material, std::shared_ptr<Material>>(m, "Material")
        .def(py::init<>())
        .def_readwrite("mass", &Material::mass)
        .def_readwrite("friction", &Material::friction)
        .def_readwrite("restitution", &Material::restitution)
        .def_readwrite("color", &Material::color)
        .def_readwrite("shininess", &Material::shininess);

    // --- Bind Classes ---
    py::class_<Scene>(m, "Scene")
        .def(py::init<>())
        .def("step", &Scene::step, "Advance the simulation by one time step")
        .def("add_primitive", [](Scene &self, const std::string& type, std::shared_ptr<Material> mat) {
            if (type == "sphere") {
                self.add_primitive(std::make_unique<Sphere>(1.0f, mat));
            }
            // Add other types later
        }, "Add a primitive to the scene");

    // We don't need to bind Primitive directly if we only interact via the Scene
}
