#pragma once
#include "SObject3D.h"

namespace fl {
	namespace geom {
		class Sphere3D :public SObject3D {
		public:
			const int row, col, r;

			Sphere3D(const Vector3D& pos, const Texture& texture, int row = 15, int col = 15, int r = 60);
		};

		class Cube3D :public SObject3D {
		public:
			const int size;

			Cube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& front,
				const Texture& behind, const Texture& left, const Texture& right, int size = 60) :SObject3D(pos), size(size) {
				init(pos, top, bottom, front, behind, left, right);
			}
			Cube3D(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& around, int size = 60) :SObject3D(pos), size(size) {
				init(pos, top, bottom, around, around, around, around);
			}
			Cube3D(const Vector3D& pos, const Texture& texture, int size = 60) :SObject3D(pos), size(size) {
				init(pos, texture, texture, texture, texture, texture, texture);
			}
		private:
			void init(const Vector3D& pos, const Texture& top, const Texture& bottom, const Texture& front,
				const Texture& behind, const Texture& left, const Texture& right);
		};




		/*                 ^ height_vec
		 *                 |
		 *                 |
		 *                 o------------------> width_vec
		 *
		 */
		class SQuadr3D : public SObject3D {
		public:

			SQuadr3D(const Vector3D& pos, const Vector3D& width_vec, const Vector3D& height_vec, const Texture& texture);
		};
	}
}