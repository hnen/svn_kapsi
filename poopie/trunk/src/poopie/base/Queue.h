
#ifndef __POOPIE_QUEUE
#define __POOPIE_QUEUE

#include "DynamicArray.h"

namespace poopie {


	template<class A>
	class Queue {
	public:

		Queue() : m_front(0), m_back(0), m_container(512) {}
		Queue( int containerSize ) : m_front(0), m_back(0), m_container(containerSize) {}

		void push( const A & el ) {
			if ( (m_back + 1) % m_container.getLength() == m_front ) {
				resize(m_container.getLength() * 2);
			}
			m_container[m_back] = el;
			m_back++;
			m_back%=m_container.getLength();
		}

		A & peek() {
			return m_container[m_front];
		}

		A & pop() {
			A & ret = m_container[m_front];
			m_front++;
			m_front%=m_container.getLength();
			return ret;
		}

		int getLength() const {
			if ( m_back >= m_front ) {
				return m_back - m_front;
			} else {
				return m_back + m_container.getLength() - m_front;
			}
			
		}

	private:

		void resize( int nsize ) {
			Array<A> newContainer(m_container.getLength());
			int n = 0;
			int i = m_front;
			while( i != m_back ) {
				newContainer[n] = m_container[i];
				n++;
				i++;
				i%=m_container.getLength();
			}
			m_front = 0;
			m_back = n;
		}

		int m_front;
		int m_back;
		
		Array<A> m_container;

	};



}


#endif

