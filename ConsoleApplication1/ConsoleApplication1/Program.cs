using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows.Forms;

using SharpDX;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using SharpDX.Windows;

namespace TerrainTest {

    class Program {

        static Window window;
        static Terrain terrain;
        static InputManager input;

        static float camAlphaDst, camBetaDst = 3.1416f / 4.0f;
        static float camAlpha, camBeta = 3.1416f / 4.0f;
        static float camDistDst = 175.0f, camDist = 175.0f;
        static float camHeight = 1.7f;
        //static Vector3 camView = new Vector3( 0, 10, 0 );

        static Camera camera = new Camera();

        static long prevTicks;
        static long startTicks;
        static float Time { get { return ((DateTime.Now.Ticks - startTicks) / 10000) / 1000.0f; } }

        static bool camMode = true;

        static void Main( string[] args ) {
            window = new Window();
            window.Init();

            input = new InputManager( window );
            input.MouseWheel += MouseWheel;
            input.KeyPress += KeyPressed;

            camera.View = new Vector3( 0, 10, 0 );

            terrain = new Terrain( window, camera );

            prevTicks = startTicks = DateTime.Now.Ticks;
            RenderLoop.Run( window.Form, Run );

        }

        static void MouseWheel( int delta ) {
            camDistDst -= camDistDst * 0.0005f * delta;
        }

        static void KeyPressed( Keys key ) {
            if ( key == Keys.C ) {
                camMode = !camMode;
            }
        }
    
        static void Run() {
            var ctx = window.Context;
            var view = window.RenderTargetView;

            long deltaTicks = DateTime.Now.Ticks - prevTicks;
            prevTicks = DateTime.Now.Ticks;
            float t = Time;
            float deltaT = ((deltaTicks) / 10000) / 1000.0f;

            terrain.Update();

            // Compute shader constants ////////////////////////////////
            if ( camMode ) {
                camera.Position = camera.View + new Vector3( (float)Math.Sin( camAlpha ) * (float)Math.Sin( camBeta ) * camDist, (float)Math.Cos( camBeta ) * camDist, (float)Math.Cos( camAlpha ) * (float)Math.Sin( camBeta ) * camDist );
            } else {
                camera.Position.Y = terrain.HeightMap.GetHeight( camera.Position.X, camera.Position.Z ) + camHeight;
                camera.View = camera.Position + new Vector3( (float)Math.Sin( camAlpha ) * (float)Math.Sin( camBeta ) * camDist, (float)Math.Cos( camBeta ) * camDist, (float)Math.Cos( camAlpha ) * (float)Math.Sin( camBeta ) * camDist );
            }

            //Vector3 camView = camPos - camPos.Normalized() + new Vector3( 0, -0.0f, 0 );
            Vector3 lightDir = new Vector3( 1, -2, 0 );

            Matrix objMat = Matrix.Identity;
            Matrix viewMat = Matrix.LookAtLH( camera.Position, camera.View, new Vector3( 0, 1, 0 ) );
            Matrix projMat = Matrix.PerspectiveFovLH( 40.0f * 3.1416f / 180.0f, (float)window.Form.Width / window.Form.Height, 0.1f, terrain.ViewDistance );

            // Draw!
            ctx.ClearRenderTargetView( view, new Color4( 0.0f, 0.0f, 0.0f, 0.0f ) );
            ctx.ClearDepthStencilView( window.DepthStencilView, DepthStencilClearFlags.Depth, 1.0f, 0 );
            terrain.Draw( objMat, viewMat, projMat, lightDir, terrain.ViewDistance );
            window.SwapChain.Present( 0, PresentFlags.None );

            // Handle input ////////////////////////////////////////////////
            if ( input.MouseLeftDown ) {
                if ( camMode ) {
                    camAlphaDst += input.MouseDelta.X * 0.01f;
                    camBetaDst -= input.MouseDelta.Y * 0.005f;
                } else {
                    camAlphaDst += input.MouseDelta.X * 0.01f / 4.0f;
                    camBetaDst += input.MouseDelta.Y * 0.005f / 4.0f;
                }
            }
            Vector3 dir = camera.View - camera.Position;
            Vector3 left = Vector3.Cross( dir, new Vector3( 0, 1, 0 ) );
            dir.Normalize();
            left.Normalize();
            if ( input.MouseRightDown ) {
                Vector3 up = Vector3.Cross( dir, left );
                up.Normalize();
                camera.View += (left * input.MouseDelta.X - up * input.MouseDelta.Y) * camDist.Clamp( 1.0f, 1000.0f ) * 0.001f;
            }
            if ( camMode ) {
                if ( input.IsKeyDown( System.Windows.Forms.Keys.W ) ) 
                    camera.View += dir;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.S ) ) 
                    camera.View -= dir;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.A ) ) 
                    camera.View += left;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.D ) ) 
                    camera.View -= left;
            } else {
                float speed = 0.05f;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.ShiftKey ) )
                    speed *= 50.0f;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.W ) ) 
                    camera.Position += dir * speed;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.S ) )
                    camera.Position -= dir * speed;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.A ) )
                    camera.Position += left * speed;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.D ) )
                    camera.Position -= left * speed;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.Q ) )
                    camHeight += speed * 5.0f;
                if ( input.IsKeyDown( System.Windows.Forms.Keys.E ) )
                    camHeight -= speed * 5.0f;
            }
            
            camDistDst = camDistDst.Clamp( 0.1f, 40000.0f );
            camBetaDst = camBetaDst.Clamp( 0.001f, 3.1415f );
            float dalpha = (camAlphaDst - camAlpha) * (deltaT * 10.0f).Clamp( 0.0f, 1.0f );
            camAlpha += dalpha;
            float dbeta = (camBetaDst - camBeta) * (deltaT * 10.0f).Clamp( 0.0f, 1.0f );
            camBeta += dbeta;
            float distalpha = (camDistDst - camDist) * (deltaT * 10.0f).Clamp( 0.0f, 1.0f );
            camDist += distalpha;

        }


    }
}
