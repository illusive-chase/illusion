#pragma once
#include "SObject3D.h"

namespace fl {
	namespace geom {
		class Sphere3D :public SObject3D {
		public:
			const int row, col, r;

			Sphere3D(const Vector3D& pos, const Texture& texture, int row = 15, int col = 15, int r = 60) :SObject3D(pos), row(row), col(col), r(r) {
				const int width = texture.width - 1;
				const int height = texture.height - 1;

				vector<int> uv;
				vector<int> newv, oldv;
				const float theta = 3.14159265358 / row;
				const float phi = 2 * 3.14159265358 / col;


#define ID(i,j) ((j==col)?(1+((i)-1)*(col)):(1+((i)-1)*(col)+(j)))
#define UVID(i,j) (uv[((i)*(col+1)+(j))])

				addPoint(Vector3D(0.0, float(r), 0.0));
				for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), 0));
				uv.push_back(UV(0, 0));

				for (int i = 1; i <= row; ++i) {
					float sin_ti = sin(theta * i);
					float cos_ti = cos(theta * i);
					for (int j = 0; j < col; ++j) {
						addPoint(Vector3D(sin_ti * r * sin(phi * j), cos_ti * r, sin_ti * r * cos(phi * j)));
						uv.push_back(UV(width * j / col, height * i / row));
					}
					uv.push_back(UV(width, height * i / row));
				}

				addPoint(Vector3D(0.0, float(-r), 0.0));
				for (int j = 0; j < col; ++j) uv.push_back(UV(int((width * j + width / 2.0) / col), height));
				uv.push_back(UV(0, 0));

				//surface
				for (int j = 0; j < col; ++j) {
					addSurface(0, ID(1, j + 1), ID(1, j), texture, UVID(0, j), UVID(1, j + 1), UVID(1, j));
					addSurface((row - 1) * col + 1, ID(row, j), ID(row, j + 1), texture, UVID(row, j), UVID(row, j), UVID(row, j + 1));
				}
				for (int i = 1; i < row; ++i) {
					for (int j = 0; j < col; ++j) {
						addSurface(ID(i, j), ID(i, j + 1), ID(i + 1, j), texture, UVID(i, j), UVID(i, j + 1), UVID(i + 1, j));
						addSurface(ID(i, j + 1), ID(i + 1, j + 1), ID(i + 1, j), texture, UVID(i, j + 1), UVID(i + 1, j + 1), UVID(i + 1, j));
					}
				}

#undef ID
#undef UVID
			}
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

		class SRect3D : public SObject3D {
		public:
			int width, height;

			SRect3D(const Vector3D& pos, const Texture& texture, int width, int height) 
				:SObject3D(pos), width(width), height(height) {
				int w = texture.width - 1, h = texture.height - 1;
				addPoint(Vector3D(width / 2, height / 2, 0));
				addPoint(Vector3D(width / 2, -height / 2, 0));
				addPoint(Vector3D(-width / 2, -height / 2, 0));
				addPoint(Vector3D(-width / 2, height / 2, 0));
				addSurface(0, 1, 2, texture, UV(w, 0), UV(w, h), UV(0, h));
				addSurface(0, 2, 3, texture, UV(w, 0), UV(0, h), UV(0, 0));
			}
		};
	}
}