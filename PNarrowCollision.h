#pragma once
#include "PObject3D.h"

namespace fl {
	namespace physics {

		class PNarrowCollision {
		private:

			template<typename A, typename B>
			static void collide_ref(A& a, B& b) {}

			template<unsigned I, unsigned J>
			struct collide_base{
				ILL_INLINE static void value(PObject3D* a, PObject3D* b) {
					if (I > J) collide_ref(*static_cast<PShapeArray::type<J>*>(a), *static_cast<PShapeArray::type<I>*>(b));
					else collide_ref(*static_cast<PShapeArray::type<I>*>(a), *static_cast<PShapeArray::type<J>*>(b));
				}
			};
			

			typedef void(*callback)(PObject3D*, PObject3D*);

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

			ILL_INLINE static void collide(PObject3D* a, PObject3D* b) {
				CallBack::func[a->uid() * PShapeArray::length + b->uid()](a, b);
			}

		};

		template<>
		ILL_INLINE static void PNarrowCollision::collide_ref<PSphere, PSphere>(PSphere& a, PSphere& b) {
			Vector3D d = a.pos - b.pos;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sd = d.mod();
			d /= sd;
			ILL_ATTRIBUTE_ALIGNED16(scalar) sa = a.vel * d, sb = b.vel * d;
			ILL_ATTRIBUTE_ALIGNED16(scalar) k = scalar(1) - (sd - a.radius - b.radius) / (sb - sa);
			if (sb > sa && k > 0) {
				ILL_ATTRIBUTE_ALIGNED16(scalar) rec = a.recovery * b.recovery * (scalar(2) - k);
				Vector3D va = d * sa , vb = d * sb;
				a.acc -= va * rec + d * (a.acc * d), a.vel -= va * k;
				b.acc -= vb * rec + d * (b.acc * d), b.vel -= vb * k;
			}
		}

	}
}