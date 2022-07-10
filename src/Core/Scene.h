#pragma once
#include "PDE.h"

PoissonEquation BoundaryCompare(bool importancesampling=true);

PoissonEquation SourceCompare();

// PoissonEquation SinCurve();

PoissonEquation BugDiffusionCurve();

PoissonEquation PointSourceInSphere();
