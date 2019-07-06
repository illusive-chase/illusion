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
#include "SObject3D.h"
#include "../display/Shape.h"
#include "SkyBox.h"
#include "SLight.h"
#include "SText3D.h"
#include "Shader.h"

// As the details of sampling is too complicated to describe
// and I do not think there will be some people that are interested in
// my project and want to know these details,
// I temporarily omitted the annotations about these details.
// If you really want to know, email me at ye_kai@pku.edu.cn and I will add them later.


namespace fl {
	namespace geom {

		// Class Camera is, as the name suggests, a helper class that encapsulates the screen camera.
		class Camera {
		public:
			Vector3D pos; // the position

			// Camera orientation:
			// 'dir' is the direction vector.
			// 'dir_h' is a horizontal vector(y is equal to 0) perpendicular to 'dir'.
			// 'dir_v' is a vector perpendicular to 'dir' and 'dir_h' with y >= 0.
			Vector3D dir, dir_h, dir_v;

			scalar nearPlatform, farPlatform; // the near platform and far platform of the frustum
			scalar scale; // the magnification factor when 3D space is projected onto the screen

			Camera(scalar nearPlatform, scalar farPlatform, scalar scale) :
				nearPlatform(nearPlatform), farPlatform(farPlatform), scale(scale) {
			}

			// rotate horizontally
			void rotateH(const Rad& rad);

			// rotate vertically
			void rotateV(const Rad& rad);

			//move right (that is, along the vector 'dir_h')
			ILL_INLINE void moveH(int x) { pos += dir_h * scalar(x); }

			//move up
			ILL_INLINE void moveV(int x) { pos.y += x; }

			//move forward
			ILL_INLINE void moveD(int x) { pos += dir * (x / dir.mod()); }

			// It sets 'pos' and 'dir'.
			void setCamera(const Vector3D& pos, const Vector3D& dir);

		private:
			// It calculates 'dir_h' and 'dir_v' every time 'dir' is changed.
			void update(); 
		};


		class Stage3DImpl final :public display::ShapeImpl {
		public:

			// The lowest two bits of RenderMode is used to represent the multi-sampling mode.
			// The third lowest bit is used to represent whether MLAA is used.
			// That is, any of MODE_NOSAMPLING, MODE_SSAA, MODE_MSAA can be used along with MODE_MLAA,
			// but any two of those three modes cannot be used at the same time.
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
			const scalar* const sample_x;
			const scalar* const sample_y;

			// the x coordinates of the sampling position in a pixel
			const scalar SAMPLE_X[3][4] = { 
				{scalar(0.5),0,0,0},
			{scalar(0.375),scalar(0.875),scalar(0.125),scalar(0.625)},
			{scalar(0.375),scalar(0.875),scalar(0.125),scalar(0.625)}
			};

			// the y coordinates of the sampling position in a pixel
			const scalar SAMPLE_Y[3][4] = { 
				{scalar(0.5),0,0,0}, 
			{scalar(0.125),scalar(0.375),scalar(0.625),scalar(0.875)},
			{scalar(0.125),scalar(0.375),scalar(0.625),scalar(0.875)}
			};

		private:

			// Class SwapChain is allocated on the heap, and will form a circular linked list.
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
					sample = AlignedAllocator<DWORD, 16>().allocate(sample_num * size);
				}

				~SwapChain() { 
					if (next) delete next; 
					DeleteObject(hbmp); 
					AlignedAllocator<DWORD, 16>().deallocate(sample);
					delete[] map_trait;
				}
			};

			std::vector<SObject3D> obj;
			std::vector<Light3D> lit;
			std::vector<SText3D> txt;
			SkyBox skybox;
			
			SwapChain* swap_chain; // circular linked list

			Shadee* vertex2D; // a temporary array allocated on the heap

			// When the number of Shadee increases, this boolean will be set to true to allow the reallocating of array 'vertex2D'.
			// See function Stage3DImpl::addObject(SObject*).
			bool update_Shadee;

			int size_Shadee; // the number of Shadee

			void render(); // main function to perform rendering
			void drawTriangle(Shadee* a, Shadee* b, Shadee* c, Texture* t); // helper function
			void drawTriangleMSAA(Shadee* a, Shadee* b, Shadee* c, Texture* t); // helper function
			void postFilteringMLAA(); // helper function
			void project(Shadee& src, Vector3D p, const Vector3D& normal, scalar cameraDir_mod); // helper function
			void project(Shadee& src, Vector3D p, scalar cameraDir_mod); // helper function

			// It is used for position showing. See SText3DImpl.h, array Stage3DImpl::txt and function Stage3DImpl::paint.
			static void showPosition(fl::events::SimpleEvent<fl::geom::SText3D> p);

		public:
			// Parameter 'swapChainNum' is the length of the circular linked list.
			Stage3DImpl(int x, int y, int width, int height, scalar nearPlatform = 20, scalar farPlatform = 2000, scalar scale = 12,
				DWORD renderMode = MODE_NOSAMPLING, int swapChainNum = 1, SkyBox skybox = nullptr, display::Shape parent = nullptr);

			virtual ~Stage3DImpl();

			bool hitTestPoint(int gx, int gy) override;
			void paint(HDC hdc) override;
			void framing() override;

			ILL_INLINE void addObject(SText3D text) { txt.push_back(text); }

			ILL_INLINE void addObject(SObject3D tar) {
				obj.push_back(tar);
				update_Shadee = true;
				size_Shadee += (int)tar->vertex.size();
			}

			ILL_INLINE void addObjectWithPosition(SObject3D tar) {
				addObject(tar);
				for (Vector3D& v : tar->vertex) {
					SText3D p = MakeSText3D(v, tar, L"undefined");
					p->renderEventListener.add(0, showPosition);
					addObject(p);
				}
			}

			ILL_INLINE void addObject(Sprite3D tar) { for (SObject3D it : tar->children) addObject(it); }

			ILL_INLINE void removeObject(SObject3D tar) {
				for (auto it = obj.begin(); it != obj.end();) {
					if (*it == tar) it = obj.erase(it);
					else ++it;
				}
			}

			ILL_INLINE void setCamera(const Vector3D& pos, const Vector3D& dir) { camera.setCamera(pos, dir); }

			ILL_INLINE void addLight(Light3D light) { lit.push_back(light); }
			ILL_INLINE const std::vector<SObject3D>& getObjects() const { return obj; }
		};

		using Stage3D = sptr<Stage3DImpl>;
		ILL_INLINE Stage3D MakeStage3D(int x, int y, int width, int height, scalar nearPlatform = 20, scalar farPlatform = 2000, scalar scale = 12,
			DWORD renderMode = Stage3DImpl::MODE_NOSAMPLING, int swapChainNum = 1, SkyBox skybox = nullptr, display::Shape parent = nullptr) {
			return Stage3D(new Stage3DImpl(x, y, width, height, nearPlatform, farPlatform, scale, renderMode, swapChainNum,
				skybox, parent));
		}
	}
}