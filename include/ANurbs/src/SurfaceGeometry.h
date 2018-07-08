#pragma once

#include "Grid.h"
#include "Point.h"
#include "SurfaceGeometryBase.h"

#include <stdexcept>
#include <vector>

namespace ANurbs {

template <typename TScalar, int TDimension, typename TVector = Point<TScalar, TDimension>>
class SurfaceGeometry
    : public SurfaceGeometryBase<TScalar, TVector>
{
public:
    using SurfaceGeometryBaseType = SurfaceGeometryBase<TScalar, TVector>;
    using typename SurfaceGeometryBaseType::KnotsType;
    using typename SurfaceGeometryBaseType::ScalarType;
    using typename SurfaceGeometryBaseType::VectorType;

protected:
    Grid<VectorType> m_poles;
    Grid<ScalarType> m_weights;

public:
    SurfaceGeometry(
        const int& degreeU,
        const int& degreeV,
        const int& nbPolesU,
        const int& nbPolesV,
        const bool& isRational)
        : SurfaceGeometryBaseType(degreeU, degreeV, nbPolesU, nbPolesV)
        , m_poles(nbPolesU, nbPolesV)
        , m_weights(isRational ? nbPolesU : 0, isRational ? nbPolesV : 0)
    {
    }

    VectorType
    Pole(
        const int& indexU,
        const int& indexV) const override
    {
        return m_poles(indexU, indexV);
    }

    void
    SetPole(
        const int& indexU,
        const int& indexV,
        const VectorType& value) override
    {
        return m_poles.SetValue(indexU, indexV, value);
    }

    ScalarType
    Weight(
        const int& indexU,
        const int& indexV) const override
    {
        if (IsRational()) {
            return m_weights(indexU, indexV);
        } else {
            return 1;
        }
    }

    void
    SetWeight(
        const int& indexU,
        const int& indexV,
        const ScalarType& value) override
    {
        if (IsRational()) {
            return m_weights.SetValue(indexU, indexV, value);
        } else {
            throw std::invalid_argument("Geometry is not rational");
        }
    }

    bool
    IsRational() const override
    {
        return m_weights.NbValues() != 0;
    }
};

using SurfaceGeometry1D = SurfaceGeometry<double, 1>;
using SurfaceGeometry2D = SurfaceGeometry<double, 2>;
using SurfaceGeometry3D = SurfaceGeometry<double, 3>;

} // namespace ANurbs
