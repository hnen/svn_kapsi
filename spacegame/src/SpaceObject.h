
#ifndef __SPACE_OBJECT
#define __SPACE_OBJECT

namespace space {
	class SpaceObject {
	public:
		virtual void render() = 0;
		virtual void update() = 0;
	};
}

#endif
