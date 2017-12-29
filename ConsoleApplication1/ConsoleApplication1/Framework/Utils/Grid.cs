using System;
using System.Collections.Generic;

using Vector2 = SharpDX.Vector2;

// 2D Grid That contains points

public class Grid<T> {

    private readonly float cellSize;
    private readonly int dataSize;

    private class Data {
        public Data() { }
        public Data( Vector2 pos, T data ) {
            this.pos = pos;
            this.data = data;
        }
        public Vector2 pos;
        public T data;
    }

    List<Data> [,] data;

    public Grid( float cellSize, int dataSize ) {
        this.cellSize = cellSize;
        this.dataSize = dataSize;

        data = new List<Data>[dataSize, dataSize];
        for ( int y = 0 ; y < dataSize ; y++ ) {
            for ( int x = 0 ; x < dataSize ; x++ ) {
                data[x, y] = new List<Data>();
            }
        }
    }

    public void Remove( Vector2 pos ) {
        int ix = (int)(pos.X / cellSize);
        int iy = (int)(pos.Y / cellSize);
        List<Data> t = this.data[ix.Mod( dataSize ), iy.Mod( dataSize )];
        for( int n = 0; n < t.Count; n++ ) {
            Data data = t[n];
            if ( (data.pos.X / cellSize) == ix && (data.pos.Y / cellSize) == iy ) {
                t.RemoveAt( n );
                break;
            }
        }
    }

    public T this[float x, float y] {
        get {
            return this.Get( new Vector2( x, y ) );
        }
        set {
            Set( new Vector2( x, y ), value );
        }
    }

    public T this[Vector2 p] {
        get {
            return this.Get( p );
        }
        set {
            Set( p, value );
        }
    }

    public void Set( Vector2 pos, T data ) {
        Remove( pos );
        int ix = (int)(pos.X / cellSize);
        int iy = (int)(pos.Y / cellSize);
        this.data[ix.Mod( dataSize ), iy.Mod( dataSize )].Add( new Data( pos, data ) );
    }

    public T Get( Vector2 pos ) {
        int ix = (int)(pos.X / cellSize);
        int iy = (int)(pos.Y / cellSize);

        foreach ( Data data in this.data[ix.Mod( dataSize ), iy.Mod( dataSize )] ) {
            if ( (data.pos.X / cellSize) == ix && (data.pos.Y / cellSize) == iy ) {
                return data.data;
            }
        }
        return default(T);
    }


}
