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
#include "PObject3D.h"

namespace fl {
	namespace physics {


		// Class PNarrowCollision detects narrow collision of objects with certain shape and DEALS with it.
		// Here used is some template meta-programming, but the purpose is very simple:
		// determine an array of function pointers at compile time.
		// This array is used in the only interface PNarrowCollision::collide,
		// just in order to avoid excessive shape type checking at running time.
		// If you are interested in the details of TMP, see typedef PShapeArray in PObject3DImpl.h first.
		// ATTENTION:
		// 1. PNarrowCollision detects potential collision, that is, the collision which will occur in the next frame.
		// 2. Not like PBroadCollision, PNarrowCollision does not only detect collision, but alse deals with it.
		class PNarrowCollision {
		private:

			template<typename A, typename B>
			static void collide_ref(A& a, B& b) {}

			template<unsigned I, unsigned J>
			struct collide_base{
				ILL_INLINE static void value(PObject3D a, PObject3D b) {
					if (I > J) collide_ref(*PShapeArray::type<J>(a), *PShapeArray::type<I>(b));
					else collide_ref(*PShapeArray::type<I>(a), *PShapeArray::type<J>(b));
				}
			};
			

			typedef void(*callback)(PObject3D, PObject3D);

			template<unsigned I, unsigned J, callback ...CallBack>
			struct CallBackMatrix {
				using type = typename CallBackMatrix<I, J - 1, collide_base<I, J - 1>::value, CallBack...>::type;
			};

			template<callback ...CallBack>
			struct CallBackMatrix<0, 0, CallBack...> {
				using type = CallBackMatrix<0, 0, CallBack...>;
				static constexpr callback func[PShapeArray::length * PShapeArray::length] = { CallBack... };
			};

			template<unsigned I, callback ...CallBack>
			struct CallBackMatrix<I, 0, CallBack...> {
				using type = typename CallBackMatrix<I - 1, PShapeArray::length - 1, collide_base<I - 1, PShapeArray::length - 1>::value, CallBack...>::type;
			};

			using CallBack = CallBackMatrix<PShapeArray::length - 1, PShapeArray::length - 1,
				collide_base<PShapeArray::length - 1, PShapeArray::length - 1>::value>::type;

		public:
			// It is the only interface, and is a static function.
			// When PBroadCollision detects that 'a' and 'b' will collide in the next frame,
			// PNarrowCollision::collide will be used to check if narrow collision occurs.
			// And if so, PNarrowCollision will deal with it to avoid the overlap of 'a' and 'b'.
			ILL_INLINE static void collide(PObject3D a, PObject3D b) {
				CallBack::func[a->uid() * PShapeArray::length + b->uid()](a, b);
			}

		};

		// Template specialization here.

		template<>
		ILL_INLINE static void PNarrowCollision::collide_ref<PSphereImpl, PSphereImpl>(PSphereImpl& a, PSphereImpl& b) {
			Vector3D d = a.pos - b.pos;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sd = d.mod();
			d /= sd;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sa = a.vel * d, sb = b.vel * d;
			ILL_ATTRIBUTE_ALIGNED16(scalar) k = scalar(1) - (sd - a.radius - b.radius) / (sb - sa);
			// If two objects(will collide in the next frame) are still getting closer to each other, avoid the overlap.
			if (sb > sa && k > 0) {  
				ILL_ATTRIBUTE_ALIGNED16(scalar) rec = a.recovery * b.recovery * (scalar(2) - k);
				if (a.mass) {
					Vector3D va = d * sa;
					a.acc -= va * rec + d * (a.acc * d), a.vel -= va * k;
				}
				if (b.mass) {
					Vector3D vb = d * sb;
					b.acc -= vb * rec + d * (b.acc * d), b.vel -= vb * k;
				}
				// I think the position should not be changed at this stage,
				// and the velocity should not be reversed immediately(in that case, two objects will possibly never contact).
				// So I remove the excess velocity along the line connecting two objects
				// in order to let them exactly contact with each other in the next frame;
				// I also compensate for this loss by add the loss velocity to their acceleration(Item 'va*rec' and 'vb*rec').
				// But I notice that this will let their velocity add an additional acceleration along the line connecting two objects,
				// which leads to violation of energy conservation.
				// Finally, I compensate this loss as well(Item 'd*(a.acc*d)' and 'd*(b.acc*d)').
			}
			// Otherwise, there is no need to deal with the collsion.
		}

		template<>
		ILL_INLINE static void PNarrowCollision::collide_ref<PSphereImpl, PQuadImpl>(PSphereImpl& a, PQuadImpl& b) {
			Vector3D vx = a.pos - b.pos;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sd = vx * b.sn;
			Vector3D d(b.sn);
			if (sd < 0) sd = -sd, d = -d;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sa = a.vel * d, sb = b.vel * d;
			ILL_ATTRIBUTE_ALIGNED16(scalar) k = scalar(1) - (sd - a.radius) / (sb - sa);
			if (sb > sa && k > 0) {
				Vector3D p = vx - d * sd;
				// Assume p = b.sa*a + b.sb*b.
				// Then a = (b.sb.x*p.y-b.sb.y*p.x)/(b.sb.x*b.sa.y-b.sa.x*b.sb.y),
				//      b = (b.sa.x*p.y-b.sa.y*p.x)/(b.sa.x*b.sb.y-b.sb.x*b.sa.y).
				// A valid collide should guarantee that -1<a<1 and -1<b<1.
				// Let exy = |b.sa.x*b.sb.y - b.sb.x*b.sa.y|.
				// Therefore, |b.sa.x*p.y-b.sa.y*p.x| < exy and |b.sb.y*p.x-b.sb.x*p.y| < exy.
				
				scalar exy, esb, esa;
				if (d == Vector3D(0, 1, 0)) { // avoid degeneracy
					exy = illFabs(b.sa.x*b.sb.z - b.sb.x*b.sa.z);
					esb = illFabs(b.sa.x*p.z - b.sa.z*p.x);
					esa = illFabs(b.sb.z*p.x - b.sb.x*p.z);
				} else if (d == Vector3D(1, 0, 0)) { // avoid degeneracy
					exy = illFabs(b.sa.z*b.sb.y - b.sb.z*b.sa.y);
					esb = illFabs(b.sa.z*p.y - b.sa.y*p.z);
					esa = illFabs(b.sb.y*p.z - b.sb.z*p.y);
				} else {
					exy = illFabs(b.sa.x*b.sb.y - b.sb.x*b.sa.y);
					esb = illFabs(b.sa.x*p.y - b.sa.y*p.x);
					esa = illFabs(b.sb.y*p.x - b.sb.x*p.y);
				}
				if (exy < esa || exy < esb);
				else {
					ILL_ATTRIBUTE_ALIGNED16(scalar) rec = a.recovery * b.recovery * (scalar(2) - k);
					if (a.mass) {
						Vector3D va = d * sa;
						a.acc -= va * rec + d * (a.acc * d), a.vel -= va * k;
					}
					if (b.mass) {
						Vector3D vb = d * sb;
						b.acc -= vb * rec + d * (b.acc * d), b.vel -= vb * k;
					}
				}
			}
		}

		template<>
		ILL_INLINE static void PNarrowCollision::collide_ref<PQuadImpl, PQuadImpl>(PQuadImpl& a, PQuadImpl& b) {
			// pass
		}

	}
}