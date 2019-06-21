#pragma once
#include "SGeomMath.h"

namespace fl {
	namespace geom {
#ifdef ILL_SSE
		class MorphologicalAntialiasingAgent {
		private:
			static constexpr DWORD FlagH = 1 << 31;
			static constexpr const DWORD FlagV = 1 << 30;
			static constexpr const scalar UZ_Weight = 0.125f;
			static constexpr const scalar L_Weight = 0.25f;

			int height, width, size;
			DWORD* colors;
			void FindDiscontinuities();
			void ComparePixels(__m128i& vpix0, __m128i& vpix1, const DWORD flag);
			void ComputeH(DWORD* line);
			void ComputeV(DWORD* line);
			void BlendH_UZ(DWORD* line, int start, int end, int left_offset, int right_offset);
			void BlendH_L(DWORD* line, int start, int end, BOOL left_flat, int offset);
			void BlendV_UZ(DWORD* line, int start, int end, int top_offset, int bottom_offset);
			void BlendV_L(DWORD* line, int start, int end, BOOL top_flat, int offset);

		public:
			MorphologicalAntialiasingAgent() :colors(nullptr), height(0), width(0), size(0) {}
			void Execute(DWORD* src, int width, int height);
		};
#endif
	}
}