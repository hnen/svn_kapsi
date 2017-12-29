
#ifndef __POOPIE_GFX_MESH
#define __POOPIE_GFX_MESH


namespace poopie {
namespace gfx {

	class Buffer;

	class Mesh {
	public:

		public Mesh( Buffer * vertices, Buffer * indices );

	private:

		Buffer m_vertices;
		Buffer m_indices;

	};


}
}




#endif