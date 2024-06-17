// SPDX-FileCopyrightText: 2024 Timo Post
// SPDX-License-Identifier: MIT

#pragma once

#include "box2d/base.h"

#include "box2d/math_functions.h"
#include "x86/sse.h"
#include "x86/fma.h"


/// Transform 2 points (e.g. local space to world space)
B2_INLINE void b2TransformPoints(b2Transform* t1, b2Transform* t2, const b2Vec2* p1, const b2Vec2* p2, b2Vec2* r1, b2Vec2* r2)
{
	// Load the input points
	simde__m128 tmp1 = simde_mm_loadu_ps((simde_float32*)p1); // p1.x, p1.y, ?, ?
	simde__m128 tmp2 = simde_mm_loadu_ps((simde_float32*)p2); // p2.x, p2.y, ?, ?

	simde__m128 xx = simde_mm_shuffle_ps(tmp1, tmp2, 0x00); // p1.x, p1.x, p2.x, p2.x
	simde__m128 yy = simde_mm_shuffle_ps(tmp1, tmp2, 0x55); // p1.y, p1.y, p2.y, p2.y

	// Load the transforms
	simde__m128 tmp3 = simde_mm_loadu_ps((simde_float32*)t1); // t1.p.x, t1.p.y, t1.q.c, t1.q.s
	simde__m128 tmp4 = simde_mm_loadu_ps((simde_float32*)t2); // t2.p.x, t2.p.y, t2.q.c, t2.q.s

	simde__m128 cs = simde_mm_shuffle_ps(tmp3, tmp4, 0xee); //  t1.q.c, t1.q.s, t2.q.c, t2.q.s
	simde__m128 sc = simde_mm_shuffle_ps(tmp3, tmp4, 0xbb); //  t1.q.s, t1.q.c, t2.q.s, t2.q.c
	simde__m128 oo = simde_mm_shuffle_ps(tmp3, tmp4, 0x44); //  t1.p.x, t1.p.y, t2.p.x, t2.p.y

	// Perform the transformation
	simde__m128 res = simde_mm_fmaddsub_ps(xx, cs, simde_mm_fmaddsub_ps(yy, sc, oo));
	// p1.x * t1.q.c - (p1.y * t1.q.s - t1.p.x) === p1.x * t1.q.c - p1.y * t1.q.s + t1.p.x === p1'.x
	// p1.x * t1.q.s + (p1.y * t1.q.c + t1.p.y) === p1.x * t1.q.s + p1.y * t1.q.c + t1.p.y === p1'.y
	// p2.x * t2.q.c - (p2.y * t2.q.s - t2.p.x) === p2.x * t2.q.c - p2.y * t2.q.s + t2.p.x === p2'.x
	// p2.x * t2.q.s + (p2.y * t2.q.c + t2.p.y) === p2.x * t2.q.s + p2.y * t2.q.c + t2.p.y === p2'.y

	// Store the results
	simde_mm_storel_pi((simde__m64*)r1, res);
	simde_mm_storeh_pi((simde__m64*)r2, res);
}
