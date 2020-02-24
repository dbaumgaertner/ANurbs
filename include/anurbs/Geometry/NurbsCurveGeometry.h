#pragma once

#include "../Define.h"

#include "CurveBase.h"

#include "../Algorithm/Nurbs.h"
#include "../Algorithm/NurbsCurveShapeFunction.h"

#include "../Model/Json.h"
#include "../Model/Model.h"
#include "../Model/Ref.h"

#include <stdexcept>
#include <vector>

namespace anurbs {

template <Index TDimension>
struct NurbsCurveGeometry : public CurveBase<TDimension>
{
public:     // types
    using Type = NurbsCurveGeometry<TDimension>;
    using Vector = typename CurveBase<TDimension>::Vector;
    using ControlPoint = std::pair<Vector, double>;

private:    // variables
    const Index m_degree;
    std::vector<double> m_knots;
    std::vector<Vector> m_poles;
    std::vector<double> m_weights;

public:     // constructors
    NurbsCurveGeometry(
        const Index degree,
        Index nb_poles,
        bool is_rational)
    : m_degree(degree)
    , m_poles(nb_poles)
    , m_weights(is_rational ? nb_poles : 0)
    , m_knots(Nurbs::nb_knots(degree, nb_poles))
    {
        static_assert(TDimension > 0);
    }

    NurbsCurveGeometry(
        const Index degree,
        const std::vector<double>& knots,
        const std::vector<Vector>& poles)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(poles)
    , m_weights(0)
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, length(poles))) {
            throw std::runtime_error("Number of knots and poles do not match");
        }
    }

    NurbsCurveGeometry(
        const Index degree,
        const std::vector<double>& knots,
        const std::vector<Vector>& poles,
        const std::vector<double>& weights)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(poles)
    , m_weights(weights)
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, length(poles))) {
            throw std::runtime_error("Number of knots and poles do not match");
        }

        if (length(weights) != length(poles)) {
            throw std::runtime_error("Number of weights and poles do not match");
        }
    }

    NurbsCurveGeometry(
        const Index degree,
        const std::vector<double>& knots,
        const std::vector<ControlPoint>& control_points)
    : m_degree(degree)
    , m_knots(knots)
    , m_poles(length(control_points))
    , m_weights(length(control_points))
    {
        static_assert(TDimension > 0);

        if (length(knots) != Nurbs::nb_knots(degree, length(control_points))) {
            throw std::runtime_error("Number of knots and control points do not match");
        }

        for (Index i = 0; i < length(control_points); i++) {
            m_poles[i] = std::get<0>(control_points[i]);
            m_weights[i] = std::get<1>(control_points[i]);
        }
    }

public:     // static methods
    using CurveBase<TDimension>::dimension;

public:     // methods
    Index degree() const override
    {
        return m_degree;
    }
    
    std::vector<Vector> derivatives_at(const double t, const Index order)
        const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, order);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](Index i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        std::vector<Vector> derivatives(shape_function.nb_shapes());

        for (Index order = 0; order < shape_function.nb_shapes(); order++) {
            for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
                Index index = shape_function.first_nonzero_pole() + i;

                if (i == 0) {
                    derivatives[order] = pole(index) * shape_function(order, i);
                } else {
                    derivatives[order] += pole(index) * shape_function(order, i);
                }
            }
        }

        return derivatives;
    }

    Interval domain() const override
    {
        return Interval(m_knots[m_degree - 1], m_knots[nb_knots() - m_degree]);
    }

    bool is_rational() const
    {
        return m_weights.size() != 0;
    }

    double knot(Index i) const
    {
        return m_knots[i];
    }

    const std::vector<double>& knots() const
    {
        return m_knots;
    }

    Index nb_knots() const
    {
        return static_cast<Index>(m_knots.size());
    }

    Index nb_poles() const
    {
        return static_cast<Index>(m_poles.size());
    }

    Vector point_at(const double t) const override
    {
        NurbsCurveShapeFunction shape_function;

        shape_function.resize(m_degree, 0);

        if (m_weights.size() > 0) {
            shape_function.compute(m_knots, [&](Index i) { return weight(i); }, t);
        } else {
            shape_function.compute(m_knots, t);
        }

        Vector point;

        for (Index i = 0; i < shape_function.nb_nonzero_poles(); i++) {
            const Index index = shape_function.first_nonzero_pole() + i;

            if (i == 0) {
                point = pole(index) * shape_function(0, i);
            } else {
                point += pole(index) * shape_function(0, i);
            }
        }

        return point;
    }

    Vector pole(Index i) const
    {
        return m_poles.at(i);
    }

    const std::vector<Vector>& poles() const
    {
        return m_poles;
    }
    
    void set_knot(Index i, double value)
    {
        m_knots[i] = value;
    }
    
    void set_pole(Index i, Vector value)
    {
        m_poles.at(i) = value;
    }
    
    void set_weight(Index i, double value)
    {
        m_weights.at(i) = value;
    }

    std::pair<std::vector<Index>, linear_algebra::MatrixXd> shape_functions_at(const double t,
        const Index order) const
    {
        NurbsCurveShapeFunction shape_function(degree(), order);

        if (is_rational()) {
            shape_function.compute(knots(), [&](Index i) {
                return weight(i); }, t);
        } else {
            shape_function.compute(knots(), t);
        }

        linear_algebra::MatrixXd values(shape_function.nb_shapes(),
            shape_function.nb_nonzero_poles());

        for (Index i = 0; i < shape_function.nb_shapes(); i++) {
            for (Index j = 0; j < shape_function.nb_nonzero_poles(); j++) {
                values(i, j) = shape_function(i, j);
            }
        }

        return {shape_function.nonzero_pole_indices(), values};
    }

    std::vector<Interval> spans() const override
    {
        const Index first_span = degree() - 1;
        const Index last_span = nb_knots() - degree() - 1;

        const Index nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (Index i = 0; i < nb_spans; i++) {
            const double t0 = knot(first_span + i);
            const double t1 = knot(first_span + i + 1);

            result[i] = Interval(t0, t1);
        }

        return result;
    }
    
    double weight(Index i) const
    {
        return m_weights.at(i);
    }
    
    const std::vector<double>& weights() const
    {
        return m_weights;
    }

public:     // serialization
    static std::string type_name()
    {
        return "NurbsCurveGeometry" + std::to_string(dimension()) + "D";
    }

    static Unique<Type> load(Model& model, const Json& source)
    {
        const auto poles = source.at("poles");
        const auto knots = source.at("knots");
        const auto weights = source.value("weights", std::vector<double>());
        
        const Index degree = source.at("degree");
        const Index nb_poles = length(poles);
        const bool is_rational = !weights.empty();

        auto result = new_<Type>(degree, nb_poles, is_rational);

        for (Index i = 0; i < length(knots); i++) {
            result->set_knot(i, knots[i]);
        }

        for (Index i = 0; i < nb_poles; i++) {
            result->set_pole(i, poles[i]);
        }

        if (is_rational) {
            for (Index i = 0; i < length(weights); i++) {
                result->set_weight(i, weights[i]);
            }
        }

        return result;
    }

    static void save(const Model& model, const Type& data, Json& target)
    {
        target["degree"] = data.degree();
        target["knots"] = data.knots();
        target["nb_poles"] = data.nb_poles();
        target["poles"] = ToJson(data.poles());

        if (data.is_rational()) {
            target["weights"] = data.weights();
        }
    }

public:     // python
    template <typename TModel>
    static void register_python(pybind11::module& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Type = NurbsCurveGeometry<TDimension>;
        using Base = CurveBase<TDimension>; 
        using Holder = Pointer<Type>;

        const std::string name = Type::type_name();

        py::class_<Type, Base, Holder>(m, name.c_str())
            // constructors
            .def(py::init<const Index, const Index, const bool>(), "degree"_a,
                "nb_poles"_a, "is_rational"_a)
            .def(py::init<const Index, const std::vector<double>,
                const std::vector<Vector>>(), "degree"_a, "knots"_a, "poles"_a)
            .def(py::init<const Index, const std::vector<double>,
                const std::vector<Vector>, const std::vector<double>>(),
                "degree"_a, "knots"_a, "poles"_a, "weights"_a)
            .def(py::init<const Index, const std::vector<double>,
                const std::vector<ControlPoint>>(), "degree"_a, "knots"_a, "control_points"_a)
            // read-only properties
            .def_property_readonly("is_rational", &Type::is_rational)
            .def_property_readonly("knots", &Type::knots)
            .def_property_readonly("nb_knots", &Type::nb_knots)
            .def_property_readonly("nb_poles", &Type::nb_poles)
            .def_property_readonly("poles", &Type::poles)
            .def_property_readonly("weights", &Type::weights)
            // methods
            .def("knot", &Type::knot, "index"_a)
            .def("set_knot", &Type::set_knot, "index"_a, "value"_a)
            .def("set_pole", &Type::set_pole, "index"_a, "value"_a)
            .def("set_weight", &Type::set_weight, "index"_a, "value"_a)
            .def("shape_functions_at", &Type::shape_functions_at, "t"_a,
                "order"_a)
            .def("weight", &Type::weight, "index"_a)
            // .def("clone", &Type::clone)
        ;

        Model::register_python_data_type<Type>(m, model);
    }
};

} // namespace anurbs