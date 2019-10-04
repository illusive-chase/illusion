/*
MIT License

Copyright (c) 2019 illusive-chase

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/
#pragma once


// This file help with packages managing.

#include "System.h"


// package:

#if defined(import_all)
#define import_fl
#define import_events
#define import_display
#define import_geom
#define import_ui
#define import_loader
#define import_physics
#endif
#if defined(import_fl)
#include "Stage.h"
using namespace fl;
#endif
#if defined(import_events)
#include "SEvent.h"
using namespace fl::events;
#endif
#if defined(import_display)
#include "../display/Sprite.h"
#include "../display/SGeomShape.h"
#include "../display/SMovieClip.h"
#include "../display/SBitmap.h"
#include "../display/SText.h"
#include "../display/SColor.h"
using namespace fl::display;
#endif
#if defined(import_geom)
#include "../geom3D/Stage3D.h"
#include "../geom3D/SGeom3DShape.h"
using namespace fl::geom;
#endif
#if defined(import_ui)
#include "../geom3D/SUI.h"
#include "../top_element/SCommand.h"
using namespace fl::ui;
#endif
#if defined(import_loader)
#include "SLoader.h"
using namespace fl::io;
#endif
#if defined(import_physics)
#include "../physics3D/Phase.h"
using namespace fl::physics;
#endif


// specific module:
#ifdef import_struct
#include "Struct.h"
#endif
#ifdef import_align
#include "SAlignedAllocator.h"
#endif
#ifdef import_3dmath
#include "../geom3D/SGeomMath.h"
#endif