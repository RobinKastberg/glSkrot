#include "stdafx.h"
#pragma once

struct entity
{
	void(*init)();
	void(*render)();

};