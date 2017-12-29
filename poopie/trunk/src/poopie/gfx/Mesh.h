
#ifndef __POOPIE_GFX_MESH
#define __POOPIE_GFX_MESH

#include "poopie/defs.h"
#include "poopie/math/Sphere.h"
#include "poopie/gfx/Buffer.h"

namespace poopie {
namespace gfx {

	class Material;

struct Mesh {
public:
	
	Mesh( Buffer * vertices, Buffer * indices ) : m_vertices(vertices), m_indices(indices), m_material(0) {}
	Mesh() : m_vertices(0), m_indices(0)  {}
	~Mesh() {
		delete m_vertices;
		delete m_indices;
	}

	Buffer * m_vertices;
	Buffer * m_indices;
	Material * m_material;

	Sphere boundingSphere;

};

}
}

#endif
