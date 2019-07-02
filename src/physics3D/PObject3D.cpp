#include "PObject3D.h"
using namespace fl::physics;

#define DECLARE_UID(shape) unsigned shape::uid() const { return PShapeArray::getIndex<shape>(); }

DECLARE_UID(PSphere)
DECLARE_UID(Platform)
