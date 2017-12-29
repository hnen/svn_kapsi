
#ifndef __DEMO_GFX
#define __DEMO_GFX

#include "Demo.h"

namespace Gfx {

	enum Align {
		ALIGN_IMG_H = 0x03, ALIGN_IMG_V = 0x09,
		ALIGN_IMG_L = 0x00, ALIGN_IMG_R = 0x01, ALIGN_IMG_HC = 0x02,
		ALIGN_IMG_T = 0x00, ALIGN_IMG_B = 0x04, ALIGN_IMG_VC = 0x08,
		ALIGN_IMG_LT  = ALIGN_IMG_L  | ALIGN_IMG_T, ALIGN_IMG_LVC =  ALIGN_IMG_L  | ALIGN_IMG_VC, ALIGN_IMG_LB =  ALIGN_IMG_L  | ALIGN_IMG_B,
		ALIGN_IMG_HCT = ALIGN_IMG_HC | ALIGN_IMG_T, ALIGN_IMG_HCVC = ALIGN_IMG_HC | ALIGN_IMG_VC, ALIGN_IMG_HCB = ALIGN_IMG_HC | ALIGN_IMG_B,
		ALIGN_IMG_RT  = ALIGN_IMG_R  | ALIGN_IMG_T, ALIGN_IMG_RVC =  ALIGN_IMG_R  | ALIGN_IMG_VC, ALIGN_IMG_RB =  ALIGN_IMG_R  | ALIGN_IMG_B,

		ALIGN_SCR_H = 0x30, ALIGN_SCR_V = 0x90,
		ALIGN_SCR_L = 0x00, ALIGN_SCR_R = 0x10, ALIGN_SCR_HC = 0x20,
		ALIGN_SCR_T = 0x00, ALIGN_SCR_B = 0x40, ALIGN_SCR_VC = 0x80,
		ALIGN_SCR_LT  = ALIGN_SCR_L  | ALIGN_SCR_T, ALIGN_SCR_LVC =  ALIGN_SCR_L  | ALIGN_SCR_VC, ALIGN_SCR_LB =  ALIGN_SCR_L  | ALIGN_SCR_B,
		ALIGN_SCR_HCT = ALIGN_SCR_HC | ALIGN_SCR_T, ALIGN_SCR_HCVC = ALIGN_SCR_HC | ALIGN_SCR_VC, ALIGN_SCR_HCB = ALIGN_SCR_HC | ALIGN_SCR_B,
		ALIGN_SCR_RT  = ALIGN_SCR_R  | ALIGN_SCR_T, ALIGN_SCR_RVC =  ALIGN_SCR_R  | ALIGN_SCR_VC, ALIGN_SCR_RB =  ALIGN_SCR_R  | ALIGN_SCR_B,
	};

	void drawTex( Texture * tex, float x, float y, int align = 0, float scale = 1 );
	void drawTex( Texture * tex, Vector2 apos, int align, Vector2 asize, Shader * shader );
	void drawTexFS( Texture * tex );
	void drawTexFS( Texture * tex, Shader * shader_override );

}

#endif


