#pragma once
#include "SObject3D.h"
#include "Shape.h"
#include "SkyBox.h"
#include "SLight.h"
#include "SText3D.h"
#include "Shader.h"

namespace fl {
	namespace geom {

		class Camera {
		public:
			Vector3D pos;
			Vector3D dir, dir_h, dir_v;
			float nearPlatform, farPlatform;
			float scale;
			Camera(float nearPlatform, float farPlatform, float scale) :
				nearPlatform(nearPlatform), farPlatform(farPlatform), scale(scale) {
			}

			void rotateH(const Rad& rad);

			void rotateV(const Rad& rad);

			//move right
			inline void moveH(int x) { pos += dir_h * x; }

			//move up
			inline void moveV(int x) { pos.y += x; }

			//move forward
			inline void moveD(int x) { pos += dir * (x / dir.mod()); }

			void setCamera(const Vector3D& pos, const Vector3D& dir, int width, int height);

		private:
			void update();
		};


		class Stage3D :public display::Shape {
		public:
			const enum RenderMode {
				MODE_NOSAMPLING = 0,
				MODE_SSAA = 1,
				MODE_MSAA = 2,
				MODE_MLAA = 4
			};
			
			const DWORD sample_mode;
			const DWORD render_mode;
			const int sample_num;
			const int sample_offset;

			Camera camera;

			const int size;
			const float* const sample_x;
			const float* const sample_y;

			const float SAMPLE_X[3][4] = { {0.5,0.0,0.0,0.0}, {0.375,0.875,0.125,0.625}, {0.375,0.875,0.125,0.625} };
			const float SAMPLE_Y[3][4] = { {0.5,0.0,0.0,0.0}, {0.125,0.375,0.625,0.875}, {0.125,0.375,0.625,0.875} };

		private:

			class SwapChain {
			public:
				DWORD* sample;
				MapTrait* map_trait;
				void* colors;
				HBITMAP hbmp;
				BITMAPINFO info;

				SwapChain* next, *prev;

				SwapChain(int width, int height, int size, int sample_num, SwapChain* prev) :
					map_trait(new MapTrait[sample_num * size]),
					next(nullptr), prev(prev)
				{
					info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					info.bmiHeader.biWidth = width;
					info.bmiHeader.biHeight = height;
					info.bmiHeader.biPlanes = 1;
					info.bmiHeader.biBitCount = 32;
					info.bmiHeader.biCompression = BI_RGB;
					hbmp = CreateDIBSection(NULL, &info, DIB_RGB_COLORS, &colors, NULL, 0);
					if (sample_num > 1) sample = new DWORD[sample_num * size];
					else sample = static_cast<DWORD*>(colors);
				}

				~SwapChain() { if (next) delete next; DeleteObject(hbmp); }
			};

			vector<SObject3D*> obj;
			list<Light3D*> lit;
			list<SText3D*> txt;
			SkyBox* const skybox;
			
			SwapChain* swap_chain;

			Shadee* vertex2D;
			bool update_Shadee;
			int size_Shadee;

			void render();
			void drawTriangle(Shadee* a, Shadee* b, Shadee* c, Texture* t, void* obj);
			void drawTriangle_MSAA(Shadee* a, Shadee* b, Shadee* c, Texture* t, void* obj);
			void postFiltering_MLAA();
			void project(Shadee& src, Vector3D p, const Vector3D& normal, float cameraDir_mod);
			void project(Shadee& src, Vector3D p, float cameraDir_mod);
			static void showPosition(fl::events::SimpleEvent<fl::geom::SText3D*> p);

		public:
			Stage3D(int x, int y, int width, int height, float nearPlatform = 20, float farPlatform = 2000, float scale = 12.0,
				DWORD renderMode = MODE_NOSAMPLING, int swapChainNum = 1, SkyBox* skybox = nullptr, Shape* parent = nullptr);

			virtual ~Stage3D();

			bool hitTestPoint(int gx, int gy);
			void paint(HDC hdc);
			virtual void framing();

			inline void addObject(SText3D* text) { txt.push_back(text); }

			inline void addObject(SObject3D* tar) {
				obj.push_back(tar);
				update_Shadee = true;
				size_Shadee += (int)tar->vertex.size();
			}

			void addObjectWithPosition(SObject3D* tar) {
				addObject(tar);
				for (Vector3D& v : tar->vertex) {
					SText3D* p = new SText3D(v, L"undefined");
					p->renderEventListener.add(0, showPosition);
					addObject(p);
				}
			}

			inline void addObject(Sprite3D* tar) { for (SObject3D* it : tar->children) addObject(it); }

			inline void removeObject(SObject3D* tar) {
				for (auto it = obj.begin(); it != obj.end();) {
					if (*it == tar) it = obj.erase(it);
					else ++it;
				}
			}

			inline void setCamera(const Vector3D& pos, const Vector3D& dir) { camera.setCamera(pos, dir, width, height); }

			inline void addLight(Light3D* light) { lit.push_back(light); }
			inline const vector<SObject3D*>& getObjects() const { return obj; }
			inline const SObject3D* getObjectAt(int x, int y) const { 
				return static_cast<SObject3D*>(swap_chain->prev->map_trait[y * width + x].object);
			}
		};
	}
}