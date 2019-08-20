#pragma once

#include "../Define.h"

#include "../Model/Attributes.h"

#include <vector>

namespace ANurbs {

struct Polygon
{
public:     // types
    using Vector = Vector<2>;
    using Path = std::vector<Vector>;

public:     // variables
    Path outer_path;
    std::vector<Path> inner_paths;

public:     // methods
    int nb_loops() const
    {
        return static_cast<int>(inner_paths.size()) + 1;
    }

    int nb_vertices_of_loop(const int index) const
    {
        if (index == 0) {
            return static_cast<int>(outer_path.size());
        } else {
            return static_cast<int>(inner_paths[index - 1].size());
        }
    }

    Vector vertex_of_loop(int loopIndex, int vertexIndex) const
    {
        if (loopIndex == 0) {
            return outer_path[vertexIndex];
        } else {
            return inner_paths[loopIndex - 1][vertexIndex];
        }
    }

    int nb_vertices() const
    {
        int nbVertices = static_cast<int>(outer_path.size());

        for (const auto& path : inner_paths) {
            nbVertices += static_cast<int>(path.size());
        }

        return nbVertices;
    }

    Vector vertex(int index) const
    {
        if (index < outer_path.size()) {
            return outer_path[index];
        }

        std::size_t offset = outer_path.size();

        for (const auto& path : inner_paths) {
            if (index < offset + path.size()) {
                return path[index - offset];
            }

            offset += path.size();
        }

        throw std::exception();
    }

public:     // python
    static void register_python(pybind11::module& m)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = Polygon;

        pybind11::class_<Type>(m, "Polygon")
            .def(py::init<>())
            .def("nb_loops", &Type::nb_loops)
            .def("nb_vertices_of_loop", &Type::nb_vertices_of_loop, "index"_a)
            .def("vertex_of_loop", &Type::vertex_of_loop, "loop_index"_a,
                "vertex_index"_a)
            .def("nb_vertices", &Type::nb_vertices)
            .def("vertex", &Type::vertex, "index"_a)
        ;
    }
};

} // namespace ANurbs