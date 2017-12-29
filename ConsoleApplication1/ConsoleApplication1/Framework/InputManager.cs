using System;
using System.Collections.Generic;

using System.Windows.Forms;
using Vector2 = SharpDX.Vector2;

namespace TerrainTest {
class InputManager {

    private readonly Form form;

    private bool isMouseLeftDown;
    private bool isMouseRightDown;
    private Vector2 prevMousePos;
    private Vector2 mousePos;
    private Vector2 mouseDelta;

    private HashSet<Keys> keysDown = new HashSet<Keys>();

    public bool MouseLeftDown { get { return isMouseLeftDown; } }
    public bool MouseRightDown { get { return isMouseRightDown; } }

    public bool MouseDown { get { return isMouseLeftDown || isMouseRightDown; } }
    public Vector2 MousePos { get { return mousePos; } }
    public Vector2 MouseDelta { get { return mouseDelta; } }

    public delegate void MouseWheelHandler( int delta );
    public event MouseWheelHandler MouseWheel;

    public delegate void KeyPressedHandler( Keys key );
    public event KeyPressedHandler KeyPress;

    private void ReleaseMouse() {
        isMouseLeftDown = false;
        isMouseRightDown = false;
    }

    void OnMouseDown( object sender, MouseEventArgs e ) {
        if ( e.Button == MouseButtons.Left ) {
            isMouseLeftDown = true;
        }
        if ( e.Button == MouseButtons.Right ) {
            isMouseRightDown = true;
        }
    }

    void OnMouseLeave( object sender, EventArgs e ) {
        ReleaseMouse();
        mousePos = new Vector2( -1, -1 );
        mouseDelta = new Vector2( 0, 0 );
    }

    void OnMouseUp( object sender, MouseEventArgs e ) {
        if ( e.Button == MouseButtons.Left ) {
            isMouseLeftDown = false;
        }
        if ( e.Button == MouseButtons.Right ) {
            isMouseRightDown = false;
        }
    }

    void OnMouseMove( object sender, MouseEventArgs e ) {
        mousePos = new Vector2( e.Location.X, e.Location.Y );
        mouseDelta = mousePos - prevMousePos;
        prevMousePos = mousePos;
    }
    void OnMouseHover( object sender, EventArgs e ) {
        prevMousePos = mousePos;
        mouseDelta = new Vector2( 0, 0 );
    }

    void OnMouseWheel( object sender, MouseEventArgs e ) {
        if ( MouseWheel != null ) {
            MouseWheel( e.Delta );
        }
    }

    void OnKeyDown( object sender, KeyEventArgs e ) {
        if ( !keysDown.Contains( e.KeyCode ) ) {
            keysDown.Add( e.KeyCode );
        }
        if ( KeyPress != null ) {
            KeyPress( e.KeyCode );
        }
    }
    void OnKeyUp( object sender, KeyEventArgs e ) {
        if ( keysDown.Contains(e.KeyCode) ) {
            keysDown.Remove( e.KeyCode );
        }
    }

    public bool IsKeyDown( Keys key ) {
        return keysDown.Contains( key );
    }

    public InputManager( Window window ) {
        this.form = window.Form;

        form.MouseDown += OnMouseDown;
        form.MouseUp += OnMouseUp;
        form.MouseMove += OnMouseMove;
        form.MouseHover += OnMouseHover;
        form.MouseLeave += OnMouseLeave;
        form.MouseWheel += OnMouseWheel;

        form.KeyDown += OnKeyDown;
        form.KeyUp += OnKeyUp;
    }

}
}