
#include "poopie/defs.h"

namespace poopie {

	class RefCounted {
	public:
		RefCounted() : m_refCount(0)
			IF_POOP_DEBUG( , m_invalid(false) ) 
		{
		}

		IF_POOP_DEBUG(
			virtual ~RefCounted() {
				POOP_ASSERT( !m_invalid );
				POOP_ASSERT( m_refCount == 0 );
				IF_POOP_DEBUG(
					m_invalid = true;
				)
			}
		)

		void release() {
			POOP_ASSERT( !m_invalid );
			POOP_ASSERT( m_refCount > 0 );
			m_refCount--;
			if ( m_refCount <= 0 ) {
				delete this;
			}
		}

		void acquire() {
			POOP_ASSERT( !m_invalid );
			m_refCount++;
		}

	private:

		IF_POOP_DEBUG( 
			bool m_invalid;
		)

		uint m_refCount;

	};

}


