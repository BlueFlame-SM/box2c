// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#pragma once

#include "box2d/types.h"
#include "x86/sse2.h"
#include "x86/avx.h"

// Ray cast an AABB
b2CastOutput b2AABB_RayCast(b2AABB a, b2Vec2 p1, b2Vec2 p2);

// Get the perimeter length
static inline float b2Perimeter(b2AABB a)
{
	float wx = a.upperBound.x - a.lowerBound.x;
	float wy = a.upperBound.y - a.lowerBound.y;
	return 2.0f * (wx + wy);
}

/// Enlarge a to contain b
/// @return true if the AABB grew
static inline bool b2EnlargeAABB(b2AABB* a, b2AABB b)
{
	bool changed = false;
	if (b.lowerBound.x < a->lowerBound.x)
	{
		a->lowerBound.x = b.lowerBound.x;
		changed = true;
	}

	if (b.lowerBound.y < a->lowerBound.y)
	{
		a->lowerBound.y = b.lowerBound.y;
		changed = true;
	}

	if (a->upperBound.x < b.upperBound.x)
	{
		a->upperBound.x = b.upperBound.x;
		changed = true;
	}

	if (a->upperBound.y < b.upperBound.y)
	{
		a->upperBound.y = b.upperBound.y;
		changed = true;
	}

	return changed;
}

static inline bool b2AABB_ContainsWithMargin(b2AABB a, b2AABB b, float margin)
{
	return (a.lowerBound.x <= b.lowerBound.x - margin) & (a.lowerBound.y <= b.lowerBound.y - margin) &
		   (b.upperBound.x + margin <= a.upperBound.x) & (b.upperBound.y + margin <= a.upperBound.y);
}

/// Do a and b overlap
static inline bool b2AABB_Overlaps(b2AABB a, b2AABB b)
{
	simde__m128 tmpA = simde_mm_loadu_ps((simde_float32*)&a); // a.lowerBound.x, a.lowerBound.y, a.upperBound.x, a.upperBound.y
	simde__m128 tmpB = simde_mm_loadu_ps((simde_float32*)&b); // b.lowerBound.x, b.lowerBound.y, b.upperBound.x, b.upperBound.y
	simde__m128 lower = simde_mm_movelh_ps(tmpA, tmpB);       // a.lowerBound.x, a.lowerBound.y, b.lowerBound.x, b.lowerBound.y
	simde__m128 upper = simde_mm_movehl_ps(tmpA, tmpB);       // b.upperBound.x, b.upperBound.y, a.upperBound.x, a.upperBound.y
	simde__m128 res = simde_mm_cmpgt_ps(lower, upper);
	return simde_mm_testz_ps(res, res);

	//return (b.lowerBound.x <= a.upperBound.x) & (b.lowerBound.y <= a.upperBound.y) &
	//       (a.lowerBound.x <= b.upperBound.x) & (a.lowerBound.y <= b.upperBound.y);
}
