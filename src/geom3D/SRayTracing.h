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
#include "../display/SBitmap.h"
#include "SGeomMath.h"
#include <random>

namespace fl {
	namespace geom {


		class Ray {
		public:
			Vector3D pos, dir, intensity;
			Vector3D pointer(scalar t) const { return pos + dir * t; }
			Ray(const Vector3D& pos, const Vector3D& dir, const Vector3D& intensity) :pos(pos), dir(dir), intensity(intensity) {}
		};

		class RObject;
		class World;

		class ReflectDetail {
		public:
			Vector3D pos;
			Vector3D n;
			Vector3D dir;
			scalar t;
			RObject* surface;
			ReflectDetail(const Vector3D& pos, const Vector3D& n, const Vector3D& dir, RObject* surface)
				:pos(pos), n(n), dir(dir), t(INF), surface(surface) {}
			operator bool() const { return n.mod2(); }
		};

		class Material {
		private:
			Vector3D intensity;
			Ray(Material::* reflect) (const ReflectDetail& rd);
		public:
			friend class World;

			Ray reflect_normal(const ReflectDetail& rd);
			template<int k> Ray reflect_metal(const ReflectDetail& rd);
			template<int k> Ray reflect_transparent(const ReflectDetail& rd);
			Ray reflect_light(const ReflectDetail& rd);

			Material(const Vector3D& intensity, Ray(Material::* reflect_function) (const ReflectDetail& rd)) :
				intensity(intensity), reflect(reflect_function) {}
		};


		class RObject {
		public:
			Material mat;
			RObject(const Material& mat) :mat(mat) {}
			virtual ReflectDetail reflect(const Ray& ray, scalar tmin, scalar tmax) const = 0;
			virtual ~RObject() {}
		};

		class RRect :public RObject {
		public:
			Vector3D pos;
			scalar size; 
			ReflectDetail(RRect::* reflect_function)(const Ray&, scalar, scalar) const;
			RRect(const Vector3D& pos, scalar size,
				  ReflectDetail(RRect::* reflect_function)(const Ray&, scalar, scalar) const, const Material& mat)
				:RObject(mat), pos(pos), size(size), reflect_function(reflect_function) {}

			ReflectDetail reflect(const Ray& ray, scalar tmin, scalar tmax) const {
				return (this->*reflect_function)(ray, tmin, tmax);
			}

			ReflectDetail XY(const Ray& ray, scalar tmin, scalar tmax) const {
				ReflectDetail ret(Vector3D(), Vector3D(), Vector3D(), 0);
				if (ray.dir.z == 0) return ret;
				Vector3D d = pos - ray.pos;
				scalar t = d.z / ray.dir.z;
				if (tmin < t && t < tmax) {
					Vector3D temp = ray.pointer(t) - pos;
					if (abs(temp.x) <= size && abs(temp.y) <= size) {
						ret.dir = ray.dir;
						ret.pos = ray.pointer(t);
						ret.n = Vector3D(0, 0, d.z > 0 ? -1 : 1);
						ret.t = t;
					}
				}
				return ret;
			}

			ReflectDetail YZ(const Ray& ray, scalar tmin, scalar tmax) const {
				ReflectDetail ret(Vector3D(), Vector3D(), Vector3D(), 0);
				if (ray.dir.x == 0) return ret;
				Vector3D d = pos - ray.pos;
				scalar t = d.x / ray.dir.x;
				if (tmin < t && t < tmax) {
					Vector3D temp = ray.pointer(t) - pos;
					if (abs(temp.y) <= size && abs(temp.z) <= size) {
						ret.dir = ray.dir;
						ret.pos = ray.pointer(t);
						ret.n = Vector3D(d.x > 0 ? -1 : 1, 0, 0);
						ret.t = t;
					}
				}
				return ret;
			}

			ReflectDetail ZX(const Ray& ray, scalar tmin, scalar tmax) const {
				ReflectDetail ret(Vector3D(), Vector3D(), Vector3D(), 0);
				if (ray.dir.y == 0) return ret;
				Vector3D d = pos - ray.pos;
				scalar t = d.y / ray.dir.y;
				if (tmin < t && t < tmax) {
					Vector3D temp = ray.pointer(t) - pos;
					if (abs(temp.z) <= size && abs(temp.x) <= size) {
						ret.dir = ray.dir;
						ret.pos = ray.pointer(t);
						ret.n = Vector3D(0, d.y > 0 ? -1 : 1, 0);
						ret.t = t;
					}
				}
				return ret;
			}
		};

		

		class RSphere :public RObject {
		public:
			Vector3D pos;
			scalar radius;
			RSphere(const Vector3D& pos, scalar radius, const Material& mat) :RObject(mat), pos(pos), radius(radius) {}
			ReflectDetail reflect(const Ray& ray, scalar tmin, scalar tmax) const {
				ReflectDetail ret(Vector3D(), Vector3D(), Vector3D(), 0);
				Vector3D oc = ray.pos - pos;
				scalar a = ray.dir.mod2();
				scalar b = oc * ray.dir;
				scalar c = oc.mod2() - radius * radius;
				scalar d = b * b - a * c;
				if (d > 0) {
					scalar t = (-b - illSqrt(d)) / a;
					if (t < tmax && t > tmin) {
						ret.pos = ray.pointer(t);
						ret.n = (ret.pos - pos) * (1 / radius);
						ret.dir = ray.dir;
						ret.t = t;
						return ret;
					}
					t = (-b + illSqrt(d)) / a;
					if (t < tmax && t > tmin) {
						ret.pos = ray.pointer(t);
						ret.n = (ret.pos - pos) * (1 / radius);
						ret.dir = ray.dir;
						ret.t = t;
					}
				}
				return ret;
			}
		};

		class World {
		public:
			static constexpr int max_depth = 15;
			static constexpr float eps = 0.2f;
			static constexpr int sample = 100;
			static constexpr float lens_radius = 1.0f;

			display::SBitmap sbmp;
			std::list<RObject*> objs;
			ReflectDetail reflect(const Ray& ray, scalar tmin, scalar tmax, RObject* surface) {
				ReflectDetail ret(Vector3D(), Vector3D(), Vector3D(), 0);
				for (auto& it : objs) {
					ReflectDetail rd = it->reflect(ray, it == surface ? (tmin + eps) : tmin, ret.t);
					if (rd) rd.surface = it, ret = rd;
				}
				return ret;
			}
			Vector3D cast(const Ray& ray, int depth, RObject* caster) {
				if (depth < max_depth) {
					if (!ray.dir.mod2()) return ray.intensity;
					ReflectDetail rd = reflect(ray, 0.0f, INF, caster);
					if (rd) {
						Ray r = (rd.surface->mat.*rd.surface->mat.reflect)(rd);
						return ray.intensity & cast(r, depth + 1, rd.surface);
					}
					scalar t = 0.5f * (ray.dir.y / ray.dir.mod() + 1.0f);
					//return (Vector3D(0.5f, 0.7f, 1.0f) * t + Vector3D(1, 1, 1) * (1.0f - t)) & ray.intensity;
					//return (Vector3D(0.2f, 0.3f, 0.4f) * t + Vector3D(0.4f, 0.4f, 0.4f) * (1.0f - t)) & ray.intensity;
				}
				return Vector3D();
			}

			World() : sbmp(nullptr) { sbmp = display::MakeSBitmap(0, 0, display::MakeBitmap(500, 500)); }
			~World() { for (auto& it : objs) delete it; }

			template<typename T>
			void add(const T& obj) { objs.push_back(new T(obj)); }

			void render(const Camera& cam) {
				DWORD* src = sbmp->content();
				Vector3D plane = cam.dir_h * -250.0f + cam.dir_v * -250.0f + cam.dir * cam.nearPlatform;
				for (int i = 0; i < 500; ++i) {
					for (int j = 0; j < 500; ++j) {
						Vector3D cl;
						for (int k = 0; k < sample; ++k) {
							Vector3D random = random_unit();
							random *= lens_radius;
							random = cam.dir_h * random.x + cam.dir_v * random.y;
							Ray r(cam.pos + random, cam.dir_h * float(i) + cam.dir_v * float(j) + plane - random, Vector3D(1, 1, 1));
							r.dir.normalize();
							cl += cast(r, 0, nullptr);
						}
						cl /= float(sample);
						cl.x = illSqrt(cl.x), cl.y = illSqrt(cl.y), cl.z = illSqrt(cl.z);
						cl *= 255.9f;
						src[i + j * 500] = RGB3D(min(int(cl.x), 0xff), min(int(cl.y), 0xff), min(int(cl.z), 0xff));
					}
				}
			}
		};



		Ray Material::reflect_normal(const ReflectDetail& rd) {
			Ray ra(rd.pos, random_unit(), intensity);
			ra.dir += rd.n;
			if (ra.dir == Vector3D()) ra.dir = rd.n;
			ra.dir.normalize();
			return ra;
		}
		template<int k>
		Ray Material::reflect_metal(const ReflectDetail& rd) {
			Ray ra(rd.pos, random_unit(), intensity);
			Vector3D temp = rd.dir - rd.n * ((rd.dir * rd.n) * 2);
			ra.dir += temp * (k * 0.01f / temp.mod());
			ra.dir.normalize();
			return ra;
		}

		template<int k>
		Ray Material::reflect_transparent(const ReflectDetail& rd) {
			Ray ra(rd.pos, Vector3D(), intensity);
			scalar projection_length = rd.dir * rd.n;
			scalar ni_nt = k * 0.01f;
			float r0 = float(100 - k) / (100 + k);
			r0 = r0 * r0;
			if (projection_length > 0) r0 += (1.0f - r0) * pow(1.0f - projection_length * ni_nt, 5);
			else r0 += (1.0f - r0) * pow(1.0f + projection_length, 5);
			if (rands(0.5f) < r0) {
				//reflect
				ra.dir = rd.dir - rd.n * ((rd.dir * rd.n) * 2);
			} else {
				//refract
				Vector3D u = rd.dir - rd.n * projection_length;
				scalar d;
				scalar symn = 1.0f;
				if (projection_length < 0) ni_nt = 1.0f / ni_nt, symn = -symn;
				if ((d = 1.0f - ni_nt * ni_nt * u.mod2()) > 0) ra.dir = u * ni_nt + rd.n * (illSqrt(d) * symn);
				else ra.dir = rd.dir - rd.n * ((rd.dir * rd.n) * 2);
			}
			return ra;
		}
		Ray Material::reflect_light(const ReflectDetail& rd) {
			return Ray(rd.pos, Vector3D(), intensity);
		}
	}
}



