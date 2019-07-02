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

#include "../top_element/SPointer.h"
#include "SGeomMath.h"
#include "Surface.h"
#include "../physics3D/PObject3D.h"

namespace fl {
	namespace geom {

		// Class SObject3D inherits class AutoPtr, which means it must be allocated on the heap.
		// Class SObject3D can be inherited.
		// ATTENTION:
		// The design of 3D transform operation still needs to be improved, 
		// as it seems to have poor performance.
		class SObject3D :public AutoPtr {
		public:
			Vector3D pos;

			// They can be used to represent the transformation matrix.
			// But actually the transformation part should be improved later
			// and it needs a matrix class to save the transformation matrix.
			Vector3D m_x, m_y, m_z;

			aligned_vector<Vector3D> vertex;
			aligned_vector<Vector3D> normal;  // the normal vectors of vertexes
			aligned_vector<Surface3D> surface;  // the surface that should be painted

			// It points to the physical object to which the instance of class SObject3D is bound.
			// It can be nullptr, which means the this instance is not bound to any physical object.
			fl::physics::PObject3D* pobj;  

			SObject3D(const Vector3D& pos) :pos(pos), m_x(1, 0, 0), m_y(0, 1, 0), m_z(0, 0, 1), pobj(nullptr) {}
			virtual ~SObject3D() {}

			// It adds a point with its normal vector and returns the index of 'p' in array 'vertex'.
			int addPoint(const Vector3D& p, const Vector3D& n);

			// It adds a point with default normal vector(is equal to 'p') and returns the index of 'p' in array 'vertex'. 
			int addPoint(const Vector3D& p);

			// It adds a surface determined by three points A,B,C(clockwise) and texture,
			// and returns the index of this surface in array 'surface'.
			// Parameters 'pa','pb','pc' are respectively the indexes of points A,B,C in array 'vertex'.
			// Parameter 'texture' is the texture of this surface.
			// Parameters 'uva', 'uvb', 'uvc' are respectively the UV coordinates of points A,B,C, 
			// which are represented as an interger that equals ((u << 16) + v). See function fl::geom::UV in SGeomMath.h.
			int addSurface(int pa, int pb, int pc, const Texture& texture, int uva = 0, int uvb = 0, int uvc = 0);

			// It sets the physical object, and it returns the 'this' pointer for convenience.
			SObject3D* addPObject(fl::physics::PObject3D* obj);

			void rotateX(const Rad& rad); // That rad > 0 means rotating clockwise.
			void rotateY(const Rad& rad); // That rad > 0 means rotating clockwise.
			void rotateZ(const Rad& rad); // That rad > 0 means rotating clockwise.
			void scale(scalar factor); // scale by a certain ratio
			ILL_INLINE void move(const Vector3D& dir) { pos += dir; } // move along a vector
			void rotateX(const Rad& rad, const Vector3D& refv); // rotate around the point 'refv'
			void rotateY(const Rad& rad, const Vector3D& refv); // rotate around the point 'refv'
			void rotateZ(const Rad& rad, const Vector3D& refv); // rotate around the point 'refv'
			void scale(scalar factor, const Vector3D& refv); // scale around the point 'refv'

			// It is called in the same name function of class Stage3D in every LOGICAL FRAME.
			// In fact, what always happens is that it is called directly or indirectly in the same name function of class Stage.
			// ATTENTION:
			// 1. You can also override this method in your custom derived class.
			//    But if you do so, remember to add this statement as well.
			//    Otherwise, the instance will not follow the physical object it is bound to.
			// 2. The term LOGICAL FRAME is completely different from the term PAINT FRAME.
			//    See the same name function in Stage.h.
			virtual void framing() { if (pobj) pos = pobj->pos; };
			
		};

		// Class Sprite3D inherits class AutoPtr, which means it must be allocated on the heap.
		// Class Sprite3D cannot be inherited.
		// It is not responsible for the destruction of children.
		// It is used to perform batch transformation of SObject3D.
		class Sprite3D :public AutoPtr {
		public:
			std::list<SObject3D*> children;

			Sprite3D() {}
			ILL_INLINE void addObject(SObject3D* p0) { children.push_back(p0); }
			ILL_INLINE void addObject(SObject3D* p0, SObject3D* p1) { addObject(p0); addObject(p1); }
			ILL_INLINE void addObject(SObject3D* p0, SObject3D* p1, SObject3D* p2) { addObject(p0, p1); addObject(p2); }
			ILL_INLINE void rotateX(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateX(rad, refv); }
			ILL_INLINE void rotateY(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateY(rad, refv); }
			ILL_INLINE void rotateZ(const Rad& rad, const Vector3D& refv) { for (SObject3D* it : children) it->rotateZ(rad, refv); }
			ILL_INLINE void scale(scalar factor, const Vector3D& refv) { for (SObject3D* it : children) it->scale(factor, refv); }
			ILL_INLINE void move(const Vector3D& dir) { for (SObject3D* it : children) it->move(dir); }
		};
	}
}