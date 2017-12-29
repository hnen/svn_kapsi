using System;
using System.Collections.Generic;

using SharpDX;

namespace TerrainTest {
    class Camera {

        //public Vector3 Position { get; set; }
        public Vector3 Position = new Vector3(0,0,0);

        private Vector3 view = new Vector3( 0, 0, 1 );
        public Vector3 View { get { return view; } set { view = value; } }

        private Vector3 up = new Vector3( 0, 1, 0 );
        public Vector3 Up { get { return up; } set { up = value; } }

        public Camera() {

        }

    }
}
