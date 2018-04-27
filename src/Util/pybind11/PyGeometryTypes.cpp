/*!
  @author Shin'ichiro Nakaoka
*/

#include "PyEigenTypes.h"
#include "../PolyhedralRegion.h"

namespace py = pybind11;
using namespace cnoid;

namespace cnoid {

void exportPyGeometryTypes(py::module& m)
{
    py::class_<PolyhedralRegion>(m, "PolyhedralRegion")
        .def(py::init())
        .def_property_readonly("numBoundingPlanes", &PolyhedralRegion::numBoundingPlanes)
        .def("getNumBoundingPlanes", &PolyhedralRegion::numBoundingPlanes)
        .def("clear", &PolyhedralRegion::clear)
        .def("addBoundingPlane", &PolyhedralRegion::addBoundingPlane)
        .def("checkInside", &PolyhedralRegion::checkInside)
        ;
}

}
