#include "Stage3D.h"
#include "Shader.h"
#include "SMLAAA.h"

void fl::geom::Stage3D::render() {
	memset(swap_chain->map_trait, 0, (size << sample_offset) * sizeof(MapTrait));
	if (update_Shadee) {
		if (vertex2D) delete[] vertex2D;
		vertex2D = new Shadee[size_Shadee];
		update_Shadee = false;
	}

	Vector3D persPos = camera.pos - camera.dir;

	scalar cameraDir_mod = camera.dir.mod();

	std::sort(obj.begin(), obj.end(), [&](SObject3D* a, SObject3D* b)->bool {
		return (a->pos - persPos).mod2() > (b->pos - persPos).mod2();
	});

	if (skybox) skybox->pos = camera.pos;

	for (SText3D* it : txt) {
		Vector3D temp = it->obj->pos - persPos;
		if (temp * camera.dir > cameraDir_mod * camera.farPlatform) continue;
		Shadee tmp;
		project(tmp, temp + it->pos, cameraDir_mod);
		it->x = (int)tmp.x;
		it->y = (int)tmp.y;
		it->renderEventListener(it);
	}

	int cnt = 0;
	for (SObject3D* it : obj) {
		Vector3D temp = it->pos - persPos;
		int index = 0;
		if (temp * camera.dir > cameraDir_mod * camera.farPlatform) continue;
		for (Vector3D& p : it->vertex) {
			project(vertex2D[index], p + temp, it->normal[index], cameraDir_mod);
			index++;
		}
		cnt++;
		for (Surface3D& s : it->surface) {
			Shadee p[3];
			Vector3D temp_cut0, temp_cut1, n_cut1, n_cut0;
			Shadee cut0, cut1;
			Vector3D intensity;
			Vector3D v3D[3];

			for (int i = 0; i < 3; ++i) {
				p[i] = vertex2D[s.link_index[i]];
				v3D[i] = it->vertex[s.link_index[i]];
				v3D[i] += temp;
			}

			BYTE ZTag = 0;
			bool reverse = (p[0].z < 0) ^ (p[1].z < 0) ^ (p[2].z < 0);
			if (1 / p[0].z < camera.nearPlatform + cameraDir_mod) ZTag |= 1;
			if (1 / p[1].z < camera.nearPlatform + cameraDir_mod) ZTag |= 2;
			if (1 / p[2].z < camera.nearPlatform + cameraDir_mod) ZTag |= 4;

			if (ZTag < 7 && (Shadee::clockwise(p[0], p[1], p[2]) ^ reverse)) {

				//light render
				
				Vector3D n = normalVector(v3D[2] - v3D[0], v3D[1] - v3D[0]);
				n.normalize();

				for (Light3D* light : lit) {
					if (light->type == 2) {
						Vector3D& lit_dir = dynamic_cast<DirectionalLight3D*>(light)->dir;
						scalar tmp = n * lit_dir;
						if (tmp > 0.0) continue;
						Vector3D tmp_h = n * (2.0 * tmp);
						tmp_h -= lit_dir;
						tmp = tmp_h * camera.dir / cameraDir_mod;
						if (tmp > 0.0) intensity += light->intensity * (s.texture.Ks * tmp);
					} else if (light->type == 0) intensity += light->intensity * s.texture.Ka;
				}

				for (int i = 0; i < 3; ++i) {
					p[i].u = scalar(s.uv[i] >> 16) * p[i].z;
					p[i].v = scalar(s.uv[i] & 0xffff) * p[i].z;
					p[i].r = (p[i].r * s.texture.Kd + intensity.x) * p[i].z;//fac:256
					p[i].g = (p[i].g * s.texture.Kd + intensity.y) * p[i].z;
					p[i].b = (p[i].b * s.texture.Kd + intensity.z) * p[i].z;
				}
				switch (ZTag) {
				case 0:
				{
					drawTriangle(&p[0], &p[1], &p[2], &(s.texture), static_cast<void*>(it));
				}
				break;
				case 4:
				case 2:
					std::swap(p[0], p[ZTag >> 1]);
					std::swap(v3D[0], v3D[ZTag >> 1]);
				case 1:
				{
					LerpZ::cut(cut0, temp_cut0, v3D[0], v3D[1], p[0], p[1], camera.dir, cameraDir_mod, cameraDir_mod + camera.nearPlatform);
					LerpZ::cut(cut1, temp_cut1, v3D[0], v3D[2], p[0], p[2], camera.dir, cameraDir_mod, cameraDir_mod + camera.nearPlatform);
					project(cut0, temp_cut0, cameraDir_mod);
					project(cut1, temp_cut1, cameraDir_mod);
					drawTriangle(&cut0, &p[1], &p[2], &(s.texture), static_cast<void*>(it));
					drawTriangle(&cut0, &p[2], &cut1, &(s.texture), static_cast<void*>(it));
				}
				break;
				case 3:
				case 5:
					std::swap(p[0], p[3 - (ZTag >> 1)]);
					std::swap(v3D[0], v3D[3 - (ZTag >> 1)]);
				case 6:
				{
					LerpZ::cut(cut0, temp_cut0, v3D[0], v3D[1], p[0], p[1], camera.dir, cameraDir_mod, cameraDir_mod + camera.nearPlatform);
					LerpZ::cut(cut1, temp_cut1, v3D[0], v3D[2], p[0], p[2], camera.dir, cameraDir_mod, cameraDir_mod + camera.nearPlatform);
					project(cut0, temp_cut0, cameraDir_mod);
					project(cut1, temp_cut1, cameraDir_mod);
					drawTriangle(&p[0], &cut0, &cut1, &(s.texture), static_cast<void*>(it));
				}
				break;
				default:
					break;
				}
			}
		}
	}
	DWORD* p1 = reinterpret_cast<DWORD*>(swap_chain->colors);
	DWORD* p2 = swap_chain->sample;
	MapTrait* p3 = swap_chain->map_trait;
	for (int i = 0; i < size; ++i) {
#ifdef ILL_SSE
		__m128 tmp = _mm_setzero_ps();
		for (int j = 0; j < sample_num; ++j) {
			BYTE* pick = reinterpret_cast<BYTE*>(p2);
			tmp = _mm_add_ps(tmp, 
				_mm_mul_ps(
					_mm_div_ps(_mm_set_ps(0, p3->r, p3->g, p3->b), _mm_set1_ps(p3->z_depth)),
					_mm_cvtepi32_ps(_mm_set_epi32(0, pick[2], pick[1], pick[0]))
				)
			);
			p2++;
			p3++;
		}
		tmp = _mm_div_ps(tmp, _mm_set1_ps(sample_num));
		const __m128i zero = _mm_setzero_si128();
		__m128i tmpi = _mm_packus_epi16(_mm_packs_epi32(_mm_cvtps_epi32(tmp), zero), zero);
		*p1 = ~_mm_cvtsi128_si32(tmpi);
		p1++;
#else
		unsigned r = 0, g = 0, b = 0;
		for (int j = 0; j < sample_num; ++j) {
			BYTE* pick = reinterpret_cast<BYTE*>(p2);
			r += MIX(pick[2], p3->r / p3->z_depth);
			g += MIX(pick[1], p3->g / p3->z_depth);
			b += MIX(pick[0], p3->b / p3->z_depth);
			p2++, p3++;
		}
		*p1 = ~RGB3D(r / sample_num, g / sample_num, b / sample_num);
		p1++;
#endif
	}
	if (render_mode & MODE_MLAA) post_filtering_MLAA();
	//printf("%d %d\n", cnt, clock() - cl);
}

void fl::geom::Stage3D::drawTriangle(Shadee* a, Shadee* b, Shadee* c, Texture* t, void* obj) {
	if (a->y > b->y) std::swap(a, b);
	if (b->y > c->y) std::swap(b, c);
	if (a->y > b->y) std::swap(a, b);
	if (sample_mode == 2) return drawTriangle_MSAA(a, b, c, t, obj);
	Shader shader(swap_chain->sample, swap_chain->map_trait, width, obj, *t, sample_offset);
	Shadee r_cut(a, c, b->y);
	Shadee* cut = &r_cut;
	if (b->x > cut->x) std::swap(cut, b);

	LerpY ab(*a, *b), ac(*a, *cut);
	LerpY bc(*b, *c), cc(*cut, *c);

	for (int j = 0; j < sample_num; ++j) {
		int ay = (int)(a->y + 1.0 - sample_y[j]), by = (int)(b->y + 1.0 - sample_y[j]), cy = (int)(c->y + 1.0 - sample_y[j]);
		if (ay < 0) ay = 0;
		if (by < 0) by = 0;
		if (cy < 0) return;
		//drawBottomFlatTriangle
		{
			if (j) ab.reset(*a), ac.reset(*a);
			ab.start(ay, sample_y[j]);
			ac.start(ay, sample_y[j]);
			for (int i = ay; i < by && i < height; ++i) {
				int y = height - 1 - i;
				int lx = int(ab.x + 1.0 - sample_x[j]), rx = int(ac.x + 1.0 - sample_x[j]);
				if (lx < 0) lx = 0;
				if (lx < rx) {
					LerpX ip(ab, ac);
					shader.moveTo(lx, y, j);
					ip.start(lx, sample_x[j]);
					for (int k = lx; k < rx && k < width; ++k) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				}
				ab.move();
				ac.move();
			}
		}
		//drawTopFlatTriangle
		{
			if (j) bc.reset(*b), cc.reset(*cut);
			bc.start(by, sample_y[j]);
			cc.start(by, sample_y[j]);
			for (int i = by; i < cy && i < height; ++i) {
				int y = height - 1 - i;
				int lx = int(bc.x + 1.0 - sample_x[j]), rx = int(cc.x + 1.0 - sample_x[j]);
				if (lx < 0) lx = 0;
				if (lx < rx) {
					LerpX ip(bc, cc);
					shader.moveTo(lx, y, j);
					ip.start(lx, sample_x[j]);
					for (int k = lx; k < rx && k < width; ++k) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				}
				bc.move();
				cc.move();
			}
		}
	}
}


void fl::geom::Stage3D::drawTriangle_MSAA(Shadee* a, Shadee* b, Shadee* c, Texture* t, void* obj) {
	Shader shader(swap_chain->sample, swap_chain->map_trait, width, obj, *t, sample_offset);
	Shadee r_cut(a, c, b->y);
	Shadee* cut = &r_cut;
	if (b->x > cut->x) std::swap(cut, b);

	int ay = (int)(a->y), by = (int)(b->y), cy = (int)(c->y);

	int a_k = int((a->y - ay + 0.125) * 4.0);
	int b_k = int((b->y - by + 0.125) * 4.0);
	int c_k = int((c->y - cy + 0.125) * 4.0);

	if (a->y < 0) ay = 0, a_k = 0;
	if (b->y < 0) by = 0, b_k = 0;
	if (c->y < 0) return;

	scalar left_bound_f[4];
	scalar right_bound_f[4];
	{
		LerpY left(*a, *b, 0.25), right(*a, *cut, 0.25);
		if (a_k == 4) {
			++ay;
			a_k = 0;
		}
		left.move((ay + sample_y[a_k] - a->y) * 4.0);
		right.move((ay + sample_y[a_k] - a->y) * 4.0);

		for (int k = 0; k < 4; ++k) {
			left_bound_f[k] = 4.0 * left.dx * (ay + sample_y[k] - a->y) + a->x + 1.0 - sample_x[k];
			right_bound_f[k] = 4.0 * right.dx * (ay + sample_y[k] - a->y) + a->x + 1.0 - sample_x[k];
		}

		for (int i = ay; i <= by && i < height; ++i) {
			int max_left_bound = 0;
			int min_right_bound = width;
			int left_bound[4], right_bound[4];
			for (int k = 0; k < 4; ++k) {
				left_bound[k] = (int)left_bound_f[k];
				if (max_left_bound < left_bound[k]) max_left_bound = left_bound[k];
				if (left_bound[k] < 0) left_bound[k] = 0;
				right_bound[k] = (int)right_bound_f[k];
				if (right_bound[k] > width) right_bound[k] = width;
				if (min_right_bound > right_bound[k]) min_right_bound = right_bound[k];
			}

			bool shade_all = (i != by && max_left_bound < min_right_bound && i != ay);

			for (int k = (i == ay ? a_k : 0); k < 4; ++k) {
				if (i == by && k == b_k) break;
				LerpX ip(left, right);
				shader.moveTo(left_bound[k], height - 1 - i, k);
				ip.move(left_bound[k] + sample_x[k] - left.x);
				if (k && shade_all) {
					for (int j = left_bound[k]; j < max_left_bound; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
					for (int j = max_left_bound; j < min_right_bound; ++j) {
						shader.shade_follow(ip.z, k);
						ip.move();
						shader.move();
					}
					for (int j = min_right_bound; j < right_bound[k]; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				} else {
					for (int j = left_bound[k]; j < right_bound[k]; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				}
				left.move();
				right.move();
			}

			for (int k = 0; k < 4; ++k) {
				left_bound_f[k] += 4.0 * left.dx;
				right_bound_f[k] += 4.0 * right.dx;
			}
		}
	}
	{
		LerpY left(*b, *c, 0.25), right(*cut, *c, 0.25);
		if (b_k == 4) {
			++by;
			b_k = 0;
		}
		left.move((by + sample_y[b_k] - b->y) * 4.0);
		right.move((by + sample_y[b_k] - b->y) * 4.0);

		for (int k = 0; k < 4; ++k) {
			left_bound_f[k] = 4.0 * left.dx * (by + sample_y[k] - b->y) + b->x + 1.0 - sample_x[k];
			right_bound_f[k] = 4.0 * right.dx * (by + sample_y[k] - b->y) + cut->x + 1.0 - sample_x[k];
		}

		for (int i = by; i <= cy && i < height; ++i) {
			int max_left_bound = 0;
			int min_right_bound = width;
			int left_bound[4], right_bound[4];
			for (int k = 0; k < 4; ++k) {
				left_bound[k] = (int)left_bound_f[k];
				if (max_left_bound < left_bound[k]) max_left_bound = left_bound[k];
				if (left_bound[k] < 0) left_bound[k] = 0;
				right_bound[k] = (int)right_bound_f[k];
				if (right_bound[k] > width) right_bound[k] = width;
				if (min_right_bound > right_bound[k]) min_right_bound = right_bound[k];
			}

			bool shade_all = (i != by && max_left_bound < min_right_bound && i != cy);

			for (int k = (i == by ? b_k : 0); k < 4; ++k) {
				if (i == cy && k == c_k) break;
				LerpX ip(left, right);
				shader.moveTo(left_bound[k], height - 1 - i, k);
				ip.move(left_bound[k] + sample_x[k] - left.x);
				if (k && shade_all) {
					for (int j = left_bound[k]; j < max_left_bound; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
					for (int j = max_left_bound; j < min_right_bound; ++j) {
						shader.shade_follow(ip.z, k);
						ip.move();
						shader.move();
					}
					for (int j = min_right_bound; j < right_bound[k]; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				} else {
					for (int j = left_bound[k]; j < right_bound[k]; ++j) {
						shader.shade(ip, 0x00FEFEFFU);
						ip.move();
						shader.move();
					}
				}
				left.move();
				right.move();
			}

			for (int k = 0; k < 4; ++k) {
				left_bound_f[k] += 4.0 * left.dx;
				right_bound_f[k] += 4.0 * right.dx;
			}
		}
	}
}

void fl::geom::Stage3D::post_filtering_MLAA() {
#ifdef ILL_SSE
	MorphologicalAntialiasingAgent agent;
	agent.Execute((DWORD*)swap_chain->colors, width, height);
#endif
}

void fl::geom::Stage3D::project(Shadee& src, Vector3D p, const Vector3D& normal, scalar cameraDir_mod) {
	scalar z_val = cameraDir_mod / (p * camera.dir);
	Vector3D intensity_diff;

	p *= (cameraDir_mod + camera.nearPlatform) * z_val;
	p -= (camera.dir * (1 + camera.nearPlatform / cameraDir_mod));

	for (Light3D* light : lit) {
		scalar tmp;
		if (light->type == 2) {
			tmp = -(((DirectionalLight3D*)light)->dir * normal);
			if (tmp > 0.0) intensity_diff += light->intensity * tmp;
		}
	}

	src.set(
		width * 0.5 + p * camera.dir_h * camera.scale,
		height * 0.5 - p * camera.dir_v * camera.scale,
		intensity_diff, z_val
	);
}

void fl::geom::Stage3D::project(Shadee& src, Vector3D p, scalar cameraDir_mod) {
	scalar z_val = cameraDir_mod / (p * camera.dir);
	p *= cameraDir_mod * z_val;
	p -= camera.dir;

	src.x = (width * 0.5 + p * camera.dir_h * camera.scale);
	src.y = (height * 0.5 - p * camera.dir_v * camera.scale);
}

void fl::geom::Stage3D::showPosition(fl::events::SimpleEvent<fl::geom::SText3D*> p) {
	p.value->caption.str(L"");
	p.value->caption.clear();
	p.value->caption << p.value->pos.x + p.value->obj->pos.x << L',' 
		<< p.value->pos.y+ p.value->obj->pos.y << L',' 
		<< p.value->pos.z + p.value->obj->pos.z;
}


fl::geom::Stage3D::Stage3D(int x, int y, int width, int height, scalar nearPlatform, scalar farPlatform, 
	scalar scale, DWORD renderMode, int swapChainNum, SkyBox * skybox, Shape * parent) :
	Shape(parent), camera(nearPlatform, farPlatform, scale), size(width * height), skybox(skybox), vertex2D(nullptr),
	update_Shadee(false), size_Shadee(0), render_mode(renderMode), sample_mode(renderMode & 3),
	sample_num(sample_mode ? 4 : 1), sample_offset(sample_mode ? 2 : 0),
	sample_x(SAMPLE_X[sample_mode]), sample_y(SAMPLE_Y[sample_mode]) 
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	setCamera(Vector3D(), Vector3D(0, 0, -100));

	if (skybox) addObject(skybox);

	SwapChain* head = new SwapChain(width, height, size, sample_num, nullptr);
	swap_chain = head;
	for (int i = 1; i < swapChainNum; i++) 
		swap_chain = (swap_chain->next = new SwapChain(width, height, size, sample_num, swap_chain));
	swap_chain->next = head;
	head->prev = swap_chain;
}

fl::geom::Stage3D::~Stage3D() {
	for (SObject3D* p : obj) delete p;
	for (SText3D* p : txt) delete p;
	for (Light3D* l : lit) delete l;
	swap_chain->prev->next = nullptr;
	delete swap_chain;
}

bool fl::geom::Stage3D::hitTestPoint(int gx, int gy) {
	transGlobalPosToLocal(gx, gy);
	return gx >= x && gx <= x + width && gy >= y && gy <= y + height;
}

void fl::geom::Stage3D::paint(HDC hdc) {
	if (visible) {
		int x0 = x, y0 = y;
		transLocalPosToGlobal(x0, y0);
		render();
		HDC mdc = CreateCompatibleDC(hdc);
		HBITMAP hbp_old = (HBITMAP)SelectObject(mdc, swap_chain->hbmp);
		BitBlt(hdc, x0, y0, width, height, mdc, 0, 0, NOTSRCCOPY);
		SelectObject(mdc, hbp_old);
		DeleteDC(mdc);
		memset(swap_chain->colors, 0, size * sizeof(*(swap_chain->sample)));
		for (SText3D* it : txt) { it->paint(hdc); }
		swap_chain = swap_chain->next;
	}
}

void fl::geom::Stage3D::framing() {
	for (SObject3D* it : obj) it->framing();
}

void fl::geom::Camera::rotateH(const Rad& rad) {//right rotate
	dir.rotateY(rad);
	update();
}

void fl::geom::Camera::rotateV(const Rad& rad) {//up rotate
	//what if ss == 0?
	//here remains a bug
	scalar cc = dir.y;
	scalar ss = sqrt(dir.mod2() - cc * cc);
	scalar k = rad.c - cc * rad.s / ss;
	dir.x *= k;
	dir.z *= k;
	dir.y = cc * rad.c + ss * rad.s;
	update();
}

void fl::geom::Camera::setCamera(const Vector3D & pos, const Vector3D & dir, int width, int height) {
	this->dir = dir;
	this->pos = pos;
	update();
}

void fl::geom::Camera::update() {
	dir_h = (dir.z == 0.0 ? Vector3D(0, 0, 1) : (dir.z < 0.0 ? Vector3D(1, 0, -dir.x / dir.z) : Vector3D(-1, 0, dir.x / dir.z)));
	dir_h.normalize();
	dir_v = normalVector(dir, dir_h);
	if (dir_v.y < 0) dir_v *= -1;
	dir_v.normalize();
}