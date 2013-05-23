#pragma once

#include "QSplat.h"

class IOcclusionFilter {
public:
	
	CoverageData Add(QSplat *);
	bool IsCulled(CoverageData &);
};

