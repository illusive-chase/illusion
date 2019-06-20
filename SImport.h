#pragma once

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
#include "Sprite.h"
#include "SGeomShape.h"
#include "SMovieClip.h"
#include "SBitmap.h"
#include "SText.h"
#include "SColor.h"
using namespace fl::display;
#endif
#if defined(import_geom)
#include "Stage3D.h"
#include "SGeom3DShape.h"
using namespace fl::geom;
#endif
#if defined(import_ui)
#include "SUI.h"
using namespace fl::ui;
#endif
#if defined(import_loader)
#include "SLoader.h"
using namespace fl::loader;
#endif
#if defined(import_physics)
#include "PForce.h"
using namespace fl::physics;
#endif