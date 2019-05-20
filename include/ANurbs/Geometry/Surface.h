#pragma once

#include "../Define.h"

#include "Interval.h"
#include "NurbsSurfaceGeometry.h"

namespace ANurbs {

template <int TDimension, typename TRef=Pointer<NurbsSurfaceGeometry<TDimension>>>
class Surface : public SurfaceBase<TDimension>
{
private:    // variables
    TRef m_surface_geometry;
    Interval m_domain_u;
    Interval m_domain_v;

public:     // constructors
    Surface(TRef geometry) : Surface(geometry, geometry->domain_u(),
        geometry->domain_v())
    {
    }

    Surface(TRef geometry, const Interval& domain_u, const Interval& domain_v)
        : m_surface_geometry(geometry), m_domain_u(domain_u),
        m_domain_v(domain_v)
    {
    }

public:     // methods
    TRef surface_geometry() const
    {
        return m_surface_geometry;
    }

    int degree_u() const override
    {
        return m_surface_geometry->degree_u();
    }

    int degree_v() const override
    {
        return m_surface_geometry->degree_v();
    }

    Interval domain_u() const override
    {
        return m_domain_u;
    }

    Interval domain_v() const override
    {
        return m_domain_v;
    }

    Vector point_at(const double u, const double v) const override
    {
        return m_surface_geometry->point_at(u, v);
    }

    std::vector<Vector> derivatives_at(const double u, const double v,
        const int order) const override
    {
        return m_surface_geometry->derivatives_at(u, v, order);
    }

    std::vector<Interval> spans_u() const override
    {
        auto knots = m_surface_geometry->knots_u();

        int first_span = KnotVector::upper_span(degree_u(), knots, domain_u().t0());
        int last_span = KnotVector::lower_span(degree_u(), knots, domain_u().t1());

        int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            double u0 = surface_geometry()->knot_u(first_span + i);
            double u1 = surface_geometry()->knot_u(first_span + i + 1);

            result[i] = Interval(u0, u1);
        }

        return result;
    }

    std::vector<Interval> spans_v() const override
    {
        auto knots = m_surface_geometry->knots_v();

        int first_span = KnotVector::upper_span(degree_v(), knots, domain_v().t0());
        int last_span = KnotVector::lower_span(degree_v(), knots, domain_v().t1());

        int nb_spans = last_span - first_span + 1;

        std::vector<Interval> result(nb_spans);

        for (int i = 0; i < nb_spans; i++) {
            double v0 = surface_geometry()->knot_v(first_span + i);
            double v1 = surface_geometry()->knot_v(first_span + i + 1);

            result[i] = Interval(v0, v1);
        }

        return result;
    }

public:     // python
    template <typename TModule, typename TModel>
    static void register_python(TModule& m, TModel& model)
    {
        using namespace pybind11::literals;
        namespace py = pybind11;

        using Geometry = NurbsSurfaceGeometry<TDimension>;

        using Type = Surface<TDimension, Ref<Geometry>>;
        using Base = SurfaceBase<TDimension>;
        using Holder = Pointer<Type>;

        const std::string name = "Surface" + std::to_string(TDimension) + "D";

        py::class_<Type, Base, Holder>(m, name.c_str())
            .def(py::init<Ref<Geometry>>(), "geometry"_a)
            .def("Geometry", &Type::surface_geometry)
        ;

        RegisterDataType<Type>(m, model, name);
    }
};

} // namespace ANurbs
