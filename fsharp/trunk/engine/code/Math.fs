
namespace Engine

module Math =


    type Vector2 =
        struct 
            val mutable x:float32
            val mutable y:float32

            new(x0, y0) = { x = x0; y = y0 }

            member t.xy with get() = Vector2(t.x,t.y) and set(v:Vector2) = t.x <- v.x; t.y <- v.y
            member t.yx with get() = Vector2(t.y,t.x) and set(v:Vector2) = t.y <- v.x; t.x <- v.y

            member t.LengthSq with get() = t.x*t.x + t.y*t.y
            member t.Length with get() = sqrt( t.LengthSq )

            // operator overloads
            static member (*) (v:Vector2, t) = Vector2(v.x * t, v.y * t)
            static member (/) (v:Vector2, t) = Vector2(v.x / t, v.y / t)

            static member (~-) (v:Vector2) = Vector2(-v.x, -v.y)
        end

    and Vector3 =
        struct
            val mutable x : float32
            val mutable y : float32
            val mutable z : float32

            new( x0, y0, z0 ) = { x = x0; y = y0; z = z0; }
            new( xy:Vector2, z0 ) = { x = xy.x; y = xy.y; z = z0 }
            new( x0, yz:Vector2 ) = { x = x0; y = yz.y; z = yz.y }
            new( xyz:Vector3 ) = { x = xyz.x; y = xyz.y; z = xyz.z }

            member t.xy with get() = Vector2(t.x,t.y) and set(v:Vector2) = t.x <- v.x; t.y <- v.y
            member t.xz with get() = Vector2(t.x,t.z) and set(v:Vector2) = t.x <- v.x; t.z <- v.y
            member t.yx with get() = Vector2(t.y,t.x) and set(v:Vector2) = t.y <- v.x; t.x <- v.y
            member t.zx with get() = Vector2(t.z,t.x) and set(v:Vector2) = t.z <- v.x; t.x <- v.y
            member t.yz with get() = Vector2(t.y,t.z) and set(v:Vector2) = t.y <- v.x; t.z <- v.y
            member t.zy with get() = Vector2(t.z,t.y) and set(v:Vector2) = t.z <- v.x; t.y <- v.y
            member t.xyz with get() = Vector3(t.x,t.y,t.z) and set(v:Vector3) = t.x <- v.x; t.y <- v.y; t.z <- v.z
            member t.xzy with get() = Vector3(t.x,t.z,t.y) and set(v:Vector3) = t.x <- v.x; t.z <- v.y; t.y <- v.z
            member t.yxz with get() = Vector3(t.y,t.x,t.z) and set(v:Vector3) = t.y <- v.x; t.x <- v.y; t.z <- v.z
            member t.yzx with get() = Vector3(t.y,t.z,t.x) and set(v:Vector3) = t.y <- v.x; t.z <- v.y; t.x <- v.z
            member t.zxy with get() = Vector3(t.z,t.x,t.y) and set(v:Vector3) = t.z <- v.x; t.x <- v.y; t.y <- v.z
            member t.zyx with get() = Vector3(t.z,t.y,t.x) and set(v:Vector3) = t.z <- v.x; t.y <- v.y; t.x <- v.z

            member t.LengthSq with get() = t.x*t.x + t.y*t.y + t.z*t.z
            member t.Length with get() = sqrt( t.LengthSq )
            member t.Length with set(nl:float32) = let (nv:Vector3) = (t / t.Length) * nl in t.x <- nv.x; t.y <- nv.y; t.z <- nv.z

            // operator overloads
            static member (*) (v:Vector3, t) = Vector3(v.x * t, v.y * t, v.z * t)
            static member (/) (v:Vector3, t) = Vector3(v.x / t, v.y / t, v.z / t)
            static member (*) (t, v:Vector3) = Vector3(v.x * t, v.y * t, v.z * t)
            static member (/) (t, v:Vector3) = Vector3(v.x / t, v.y / t, v.z / t)

            static member (+) (v:Vector3, t:Vector3) = Vector3(v.x + t.x, v.y + t.y, v.z + t.z)
            static member (-) (v:Vector3, t:Vector3) = Vector3(v.x - t.x, v.y - t.y, v.z - t.z)

            member a.Dot (b:Vector3) = a.x * b.x + a.y * b.y + a.z * b.z

            static member (~-) (v:Vector3) = Vector3(-v.x, -v.y, -v.z)
        end

    and Vector4 =
        val mutable x : float32
        val mutable y : float32
        val mutable z : float32
        val mutable w : float32

        new( x0, y0, z0, w0 ) = { x = x0; y = y0; z = z0; w = w0 }
        new( x, yzw:Vector3 ) = Vector4(x, yzw.x, yzw.y, yzw.z)
        new( x, y, zw:Vector2 ) = Vector4(x, y, zw.x, zw.y)
        new( x, yz:Vector2, w ) = Vector4(x, yz.x, yz.y, w)
        new( xy:Vector2, z, w ) = Vector4(xy.x, xy.y, z, w)
        new( xy:Vector2, zw:Vector2 ) = Vector4(xy.x, xy.y, zw.x, zw.y)
        new( xyz:Vector3, w ) = Vector4(xyz.x, xyz.y, xyz.z, w)
    
        member t.xyz with get() = Vector3(t.x,t.y,t.z) and set(v:Vector3) = t.x <- v.x; t.y <- v.y; t.z <- v.z

        // operator overloads
        static member (*) (v:Vector4, t) = Vector4(v.x * t, v.y * t, v.z * t, v.w * t)
        static member (/) (v:Vector4, t) = Vector4(v.x / t, v.y / t, v.z / t, v.w * t)

        static member (+) (v:Vector4, t:Vector4) = Vector4(v.x + t.x, v.y + t.y, v.z + t.z, v.w + t.w)
        static member (-) (v:Vector4, t:Vector4) = Vector4(v.x - t.x, v.y - t.y, v.z - t.z, v.w + t.w)

        member a.Dot (b:Vector4) = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w

        static member (~-) (v:Vector4) = Vector4(-v.x, -v.y, -v.z, -v.w)

    and Matrix2 =
        struct
            val mutable a:float32
            val mutable b:float32
            val mutable c:float32
            val mutable d:float32

            new(a0, b0, c0, d0) = { a = a0; b = b0; c = c0; d = d0 }
            new(v0:Vector2, v1:Vector2) = { a = v0.x; b = v0.y; c = v1.x; d = v1.y }

            static member (*) (v:Matrix2, t) = Matrix2( v.a * t, v.b * t, v.c * t, v.d * t )
            static member (*) (A:Matrix2, B:Matrix2) = Matrix2( A.a * B.a + A.b * B.c, A.a * B.b + A.b * B.d, A.c * B.a + A.d * B.c, A.c * B.b + A.d * B.d )

            member this.Item with get x =
                                        match x with
                                            | 0 -> Vector2(this.a, this.b)
                                            | 1 -> Vector2(this.c, this.d)
                                            | _ -> raise ( new System.Exception("Index out of range") )
                             and set x (v:Vector2) =
                                        match x with
                                            | 0 -> this.a <- v.x; this.b <- v.y
                                            | 1 -> this.c <- v.x; this.d <- v.y
                                            | _ -> raise ( new System.Exception("Index out of range") )

            static member Identity() = Matrix2(1.0f, 0.0f, 0.0f, 1.0f)
            static member Rotate(angle) = let s, c = ( (System.Math.Sin(angle) |> float32), (System.Math.Cos(angle) |> float32) ) in Matrix2( c,-s, s, c )
            static member Scale(scale:float32) = Matrix2( scale, 0.0f, 0.0f, scale )

        end
    and Matrix4 =
        struct
            val mutable _00:float32
            val mutable _01:float32
            val mutable _02:float32
            val mutable _03:float32

            val mutable _10:float32
            val mutable _11:float32
            val mutable _12:float32
            val mutable _13:float32

            val mutable _20:float32
            val mutable _21:float32
            val mutable _22:float32
            val mutable _23:float32

            val mutable _30:float32
            val mutable _31:float32
            val mutable _32:float32
            val mutable _33:float32

            new(v0:Vector4, v1:Vector4, v2:Vector4, v3:Vector4) = { 
                _00 = v0.x; _01 = v0.y; _02 = v0.z; _03 = v0.w
                _10 = v1.x; _11 = v1.y; _12 = v1.z; _13 = v0.w
                _20 = v2.x; _21 = v2.y; _22 = v2.z; _23 = v0.w
                _30 = v3.x; _31 = v3.y; _32 = v3.z; _33 = v0.w
            }

            new(v0:Vector3, v1:Vector3, v2:Vector3, v3:Vector3) = { 
                _00 = v0.x; _01 = v0.y; _02 = v0.z; _03 = 0.0f
                _10 = v1.x; _11 = v1.y; _12 = v1.z; _13 = 0.0f
                _20 = v2.x; _21 = v2.y; _22 = v2.z; _23 = 0.0f
                _30 = v3.x; _31 = v3.y; _32 = v3.z; _33 = 1.0f
            }

            new(v0:Vector3, v1:Vector3, v2:Vector3) = { 
                _00 = v0.x; _01 = v0.y; _02 = v0.z; _03 = 0.0f
                _10 = v1.x; _11 = v1.y; _12 = v1.z; _13 = 0.0f
                _20 = v2.x; _21 = v2.y; _22 = v2.z; _23 = 0.0f
                _30 = 0.0f; _31 = 0.0f; _32 = 0.0f; _33 = 1.0f
            }

            static member Identity() = Matrix4(
                                        Vector3(1.0f, 0.0f, 0.0f),
                                        Vector3(0.0f, 1.0f, 0.0f),
                                        Vector3(0.0f, 0.0f, 1.0f) )

            static member Translate(tr:Vector3) = Matrix4(
                                                    Vector3(1.0f, 0.0f, 0.0f),
                                                    Vector3(0.0f, 1.0f, 0.0f),
                                                    Vector3(0.0f, 0.0f, 1.0f),
                                                    tr )

            static member RotateZ(angle:float32) = let s, c = ( (System.Math.Sin(angle |> float) |> float32), (System.Math.Cos(angle |> float) |> float32) ) in 
                                                    Matrix4(
                                                        Vector3( c  , -s  , 0.0f),
                                                        Vector3( s  ,  c  , 0.0f),
                                                        Vector3(0.0f, 0.0f, 1.0f) )

            static member RotateX(angle:float32) = let s, c = ( (System.Math.Sin(angle |> float) |> float32), (System.Math.Cos(angle |> float) |> float32) ) in 
                                                    Matrix4(
                                                        Vector3(1.0f, 0.0f, 0.0f),
                                                        Vector3(0.0f,  c  ,  -s ),
                                                        Vector3(0.0f,  s  ,   c ) )
        
            static member RotateY(angle:float32) = let s, c = ( (System.Math.Sin(angle |> float) |> float32), (System.Math.Cos(angle |> float) |> float32) ) in 
                                                    Matrix4(
                                                        Vector3( c  , 0.0f,  -s ),
                                                        Vector3(0.0f, 1.0f, 0.0f),
                                                        Vector3( s  , 0.0f,   c ) )

            static member Scale(scale:float32) = Matrix4(
                                                    Vector4(scale, 0.0f, 0.0f, 0.0f),
                                                    Vector4(0.0f, scale, 0.0f, 0.0f),
                                                    Vector4(0.0f, 0.0f, scale, 0.0f),
                                                    Vector4(0.0f, 0.0f, 0.0f, scale) )

            member this.Item with get x =
                                        match x with
                                            | 0 -> Vector4(this._00, this._01, this._02, this._03 )
                                            | 1 -> Vector4(this._10, this._11, this._12, this._13 )
                                            | 2 -> Vector4(this._20, this._21, this._22, this._23 )
                                            | 3 -> Vector4(this._30, this._31, this._32, this._33 )
                                            | _ -> raise ( new System.Exception("Index out of range") )
                             and set x (v:Vector4) =
                                        match x with
                                            | 0 -> this._00 <- v.x; this._01 <- v.y; this._02 <- v.z; this._03 <- v.w
                                            | 1 -> this._10 <- v.x; this._11 <- v.y; this._12 <- v.z; this._13 <- v.w
                                            | 2 -> this._20 <- v.x; this._21 <- v.y; this._22 <- v.z; this._23 <- v.w
                                            | 3 -> this._30 <- v.x; this._31 <- v.y; this._32 <- v.z; this._33 <- v.w
                                            | _ -> raise ( new System.Exception("Index out of range") )

            static member (*) (A:Matrix4, B:Matrix4) = Matrix4( 
                                                        Vector4(A._00*B._00 + A._01*B._10 + A._02*B._20 + A._03*B._30, A._00*B._01 + A._01*B._11 + A._02*B._21 + A._03*B._31, A._00*B._02 + A._01*B._12 + A._02*B._22 + A._03*B._32, A._00*B._03 + A._01*B._13 + A._02*B._23 + A._03*B._33),
                                                        Vector4(A._10*B._00 + A._11*B._10 + A._12*B._20 + A._13*B._30, A._10*B._01 + A._11*B._11 + A._12*B._21 + A._13*B._31, A._10*B._02 + A._11*B._12 + A._12*B._22 + A._13*B._32, A._10*B._03 + A._11*B._13 + A._12*B._23 + A._13*B._33),
                                                        Vector4(A._20*B._00 + A._21*B._10 + A._22*B._20 + A._23*B._30, A._20*B._01 + A._21*B._11 + A._22*B._21 + A._23*B._31, A._20*B._02 + A._21*B._12 + A._22*B._22 + A._23*B._32, A._20*B._03 + A._21*B._13 + A._22*B._23 + A._23*B._33),
                                                        Vector4(A._30*B._00 + A._31*B._10 + A._32*B._20 + A._33*B._30, A._30*B._01 + A._31*B._11 + A._32*B._21 + A._33*B._31, A._30*B._02 + A._31*B._12 + A._32*B._22 + A._33*B._32, A._30*B._03 + A._31*B._13 + A._32*B._23 + A._33*B._33)
                                                       )
            static member(*) (T:Vector4, A:Matrix4) = Vector4( T.Dot( Vector4( A._00, A._10, A._20, A._30 ) ), T.Dot ( Vector4( A._01, A._11, A._21, A._31 ) ), T.Dot ( Vector4( A._02, A._12, A._22, A._32 ) ), T.Dot ( Vector4( A._03, A._13, A._23, A._33 ) ) )
            static member(*) (A:Matrix4, T:Vector4) = Vector4( T.Dot( Vector4( A._00, A._02, A._02, A._03 ) ), T.Dot ( Vector4( A._10, A._11, A._12, A._13 ) ), T.Dot ( Vector4( A._20, A._21, A._22, A._23 ) ), T.Dot ( Vector4( A._30, A._31, A._32, A._33 ) ) )

         end


