using System;
using System.Collections.Generic;

class Heap<T> where T : IComparable<T> {

	public enum HeapType {
		MIN, MAX
	}

	private int compareMul;

	List<T> container = new List<T>();


	private void Swap( int n0, int n1 ) {
		T t = container[n0];
		container[n0] = container[n1];
		container[n1] = t;
	}

	private void SiftUp( int n ) {
		while ( n > 0 && compareMul * container[n/2].CompareTo( container[n] ) < 0 ) {
			Swap( n/2, n );
			n /= 2;
		}
	}

	private void SiftDown( int n ) {
		while ( n*2 < container.Count && 
				(compareMul * container[n].CompareTo( container[n*2] ) < 0 || 
				(n*2+1 < container.Count && compareMul * container[n].CompareTo( container[n*2+1] ) < 0)
				) ) {

			if ( n*2+1 < container.Count ) {
				if ( compareMul * container[n*2].CompareTo( container[n*2+1] ) < 0 ) {
					Swap( n*2+1, n );
					n = n*2+1;
				} else {
					Swap( n*2, n );
					n = n*2;
				}
			} else {
				Swap( n*2, n );
				n *= 2;
			}

		}
	}

	public void Add( T element ) {
		int n = container.Count;
		container.Add( element );
		SiftUp(n);
	}

	public int Count { get { return container.Count; } }

	public T Pop() {
		T first = container[0];

		container[0] = container[container.Count-1];
		container.RemoveAt( container.Count-1 );
		SiftDown( 0 );

		return first;
	}

	public T Peek() {
		return container[0];
	}

	public Heap( HeapType type ) {
		compareMul = 1;
		if ( type == HeapType.MIN ) {
			compareMul = -1;
		}
	}

}
