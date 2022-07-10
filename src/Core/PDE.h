#pragma once
#include "Boundary.h"
#include "Source.h"

using ptrSourceTerm = std::shared_ptr<SourceTerm>;
using ptrBoundary = std::shared_ptr<Boundary>;

class PoissonEquation
{
public:
	PoissonEquation(ptrBoundary boundary, ptrSourceTerm source)
		: Boundary(boundary), Source(source)
	{

	}

	bool InteriorOnly() const { return m_InteriorOnly; }

	bool m_InteriorOnly = true;
	ptrSourceTerm Source;
	ptrBoundary Boundary;
};