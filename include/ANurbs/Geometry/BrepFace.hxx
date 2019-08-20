#pragma once

#include "BrepLoop.h"

#include "NurbsSurfaceGeometry.h"

#include "../Model/Json.h"
#include "../Model/Ref.h"
#include "../Model/Model.h"

#include <vector>

namespace ANurbs {

Ref<ANurbs::Brep> BrepFace::brep() const
{
    return m_brep;
}

size_t BrepFace::nb_loops() const
{
    return m_loops.size();
}

Ref<BrepLoop> BrepFace::loop(size_t index) const
{
    return m_loops[index];
}

std::vector<Ref<BrepLoop>> BrepFace::loops() const
{
    return m_loops;
}

Ref<NurbsSurfaceGeometry<3>> BrepFace::surface_geometry() const
{
    return m_geometry;
}

std::vector<Ref<BrepEdge>> BrepFace::edges() const
{
    std::vector<Ref<BrepEdge>> result;
    for (size_t i = 0; i < nb_loops(); i++) {
        auto loop = this->loop(i);
        for (size_t j = 0; j < loop->nb_trims(); j++) {
            result.push_back(loop->trim(j)->edge());
        }
    }
    return result;
}

std::vector<Ref<BrepTrim>> BrepFace::trims() const
{
    std::vector<Ref<BrepTrim>> result;
    for (size_t i = 0; i < nb_loops(); i++) {
        auto loop = this->loop(i);
        for (size_t j = 0; j < loop->nb_trims(); j++) {
            result.push_back(loop->trim(j));
        }
    }
    return result;
}

// serialization

std::string BrepFace::type_name()
{
    return "BrepFace";
}

Unique<BrepFace> BrepFace::load(Model& model, const Json& data)
{
    auto result = new_<BrepFace>();

    // Read Brep
    {
        const std::string key = data.at("Brep");
        result->m_brep = model.get_lazy<ANurbs::Brep>(key);
    }

    // Read loops
    {
        const auto loops = data.at("Loops");

        result->m_loops.resize(loops.size());

        for (size_t i = 0; i < loops.size(); i++) {
            const std::string key = loops[i];
            result->m_loops[i] = model.get_lazy<BrepLoop>(key);
        }
    }

    // Read Geometry
    {
        const std::string key = data.at("Geometry");
        result->m_geometry = model.get_lazy<NurbsSurfaceGeometry<3>>(key);
    }

    return result;
}

void BrepFace::save(const Model& model, const BrepFace& data, Json& target)
{
    target["Brep"] = ToJson(data.m_brep);
    target["Loops"] = ToJson(data.m_loops);
    target["Geometry"] = ToJson(data.m_geometry);
}

} // namespace ANurbs