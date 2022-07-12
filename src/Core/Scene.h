#pragma once
#include "PDE.h"

PoissonEquation BoundaryScene(bool importancesampling=true);

PoissonEquation SourceScene();

PoissonEquation BugDiffusionCurve();

PoissonEquation PointSourceInSphere();
