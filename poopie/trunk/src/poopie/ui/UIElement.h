
#ifndef __POOP_UI_ELEMENT_H
#define __POOP_UI_ELEMENT_H

#include "poopie/defs.h"

namespace poopie {
namespace ui {

	class UIElement {
	public:

		virtual int getXSize() const = 0;
		virtual int getYSize() const = 0;
		virtual bool isFullscreen() const = 0;

		float getAspect() const { return (float)getXSize() / getYSize(); }

	};

}
}


#endif

