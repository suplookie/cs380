#include <GL/glut.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "MyGL.h"
#include "wavefront_obj.h"

//------------------------
// function declarations
//------------------------

// glut callbacks
void display();
void keyboard( unsigned char k, int x, int y );
void special( int k, int x, int y );
void init();

// misc functions
void checkForGLError( const char *msg );
bool loadPPM( const char *filename, unsigned char *&rgbData, int &w, int &h );


//------------------------
// global data
//------------------------
MyGL myGL;
float angleZ = 0, angleY = 0;
float tx = -.3, ty = -.3, tz = -.1;

unsigned texID = 0;

// rendering options
bool optWireframe = false;
bool optLighting = false;
bool optTextures = false;
bool optBilerp = false;       // bilinear interpolation
bool optMipmap = false;
bool optPerspective = false;
bool optDepthTest = false;
int  optGeometryMode = 1;
bool optBackFaceCulling = false;

const char *textureFile = "bricks.ppm";
const char *objFile = "sphere.obj";
wavefront_obj_t *model = NULL;

//------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "MyGL" );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( special );

    init();

    glutMainLoop();
}

void printRenderingStatus();

//-----------------------------------------------------------------------------
void init() {
    // Causes viewer vector to vary over the surface when using perspective
    glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, 1 );

    // Turn off the global ambient
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, glm::value_ptr( glm::vec4( 0, 0, 0, 0 ) ) );

    glEnable( GL_NORMALIZE );

    // load and initialize the texture
    unsigned char *rgbData;
    int w, h;
    if( loadPPM( textureFile, rgbData, w, h ) ) {
        glGenTextures( 1, &texID );
        glBindTexture( GL_TEXTURE_2D, texID );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGB, GL_UNSIGNED_BYTE,
                           rgbData );

        // use the texture value directly
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    }

    // load the wavefront model
    model = new wavefront_obj_t( objFile );

    // make sure GL calls get routed to my subclass of GLRenderer
    GLRenderer::SetGlobalInstance( &myGL );

    myGL.SetSoftwareRendering( true );
    //myGL.SetDoClipping(true);
    //myGL.SetDoTriangulate(true);
    //myGL.SetDoRasterize(true);
    printRenderingStatus();
}

//------------------------------------------------------------------------------
void printRenderingStatus() {
    printf( "Software rendering:%s - T(1):%s R(2):%s\n",
            myGL.GetSoftwareRendering() ? "on " : "off",
            myGL.GetDoTriangulate() ? "on " : "off",
            myGL.GetDoRasterize()   ? "on " : "off" );
}

//-----------------------------------------------------------------------------
void keyboard( unsigned char k, int x, int y ) {
    switch( k ) {
    // toggles for software rendering and various stages
    case ' ':
        myGL.SetSoftwareRendering( !myGL.GetSoftwareRendering() );
        printRenderingStatus();
        break;    
    case '1':
        myGL.SetDoTriangulate( !myGL.GetDoTriangulate() );
        printRenderingStatus();
        break;
    case '2':
        myGL.SetDoRasterize( !myGL.GetDoRasterize() );
        printRenderingStatus();
        break;

    // toggles for state
    /*case 'd':
        optDepthTest = !optDepthTest;
        break;*/
    case 't':
        optTextures = !optTextures;
        break;
    case 'b':
        optBilerp = !optBilerp;
        break;
    case 'm':
        optMipmap = !optMipmap;
        break;
    case 'l':
        optLighting = !optLighting;
        break;
    case 'p':
        optPerspective = !optPerspective;
        break;
    case 'w':
        optWireframe = !optWireframe;
        break;
    case 'B':
        optBackFaceCulling = !optBackFaceCulling;
        break;


    // transforms
    case '[':
        angleZ -= 5;
        break;
    case ']':
        angleZ += 5;
        break;
    case '{':
        angleY -= 5;
        break;
    case '}':
        angleY += 5;
        break;
    }
    glutPostRedisplay();
}

//------------------------------------------------------------------------------
void special( int k, int x, int y ) {
    const float inc = .1;
    switch( k ) {
    case GLUT_KEY_UP:         ty += inc;  break;
    case GLUT_KEY_DOWN:       ty -= inc;  break;
    case GLUT_KEY_LEFT:       tx -= inc;  break;
    case GLUT_KEY_RIGHT:      tx += inc;  break;
    case GLUT_KEY_PAGE_UP:    tz -= inc;  break;
    case GLUT_KEY_PAGE_DOWN:  tz += inc;  break;

    // change the geometry mode
    case GLUT_KEY_F1: optGeometryMode = 1; break;
    case GLUT_KEY_F2: optGeometryMode = 2; break;
    case GLUT_KEY_F3: optGeometryMode = 3; break;
    case GLUT_KEY_F4: optGeometryMode = 4; break;

    }
    glutPostRedisplay();
}


//------------------------------------------------------------------------------
// Draw a single polygon
void drawPolygon() {
    glColor3f( 1, 0, 0 );
    glBegin( GL_POLYGON );
    glNormal3f( 0, 0, 1 );
    glVertex2f( -1.2, -1.2 );
    glVertex2f( .7, -1.1 );
    glVertex2f( 1.2, .1 );
    glVertex2f( .1, .6 );
    glVertex2f( -.5, .4 );
    glEnd();
}

//------------------------------------------------------------------------------
// Draw a lit and textured quad
void drawQuad() {
    if( optTextures ) {
        glBindTexture( GL_TEXTURE_2D, texID );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                         optBilerp ? GL_LINEAR : GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                         optMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST );
        glEnable( GL_TEXTURE_2D );
    }

    if( optLighting )
        glEnable( GL_LIGHTING );

    glPushMatrix();
    glTranslatef( tx, ty, tz );
    glRotatef( angleY, 0, 1, 0 );
    glRotatef( angleZ, 0, 0, 1 );
    glScalef( 0.5, 0.5, 0.5 );
    glBegin( GL_QUADS );
    glNormal3f( 0, 0, 1 );

    glColor3f( 1, 1, 0 ); glTexCoord2f( 0, 1 ); glVertex2f( -1, -1 );
    glColor3f( 0, 0, 1 ); glTexCoord2f( 1, 1 ); glVertex2f(  1, -1 );
    glColor3f( 1, 0, 1 ); glTexCoord2f( 1, 0 ); glVertex2f(  1,  1 );
    glColor3f( 1, 0, 0 ); glTexCoord2f( 0, 0 ); glVertex2f( -1,  1 );

    glEnd();
    glPopMatrix();

    glDisable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
}

//------------------------------------------------------------------------------
void drawOBJ() {
    if( optLighting )
        glEnable( GL_LIGHTING );

    // determine the scale factor for the model
    glm::vec3 bbmin( model->aabb.first );
    glm::vec3 bbmax( model->aabb.second );
    glm::vec3 diag = bbmax - bbmin;
    float r = glm::length( diag ) / 2;
    float s = 1 / r;

    glPushMatrix();
    glTranslatef( tx, ty, tz );
    glRotatef( angleY, 0, 1, 0 );
    glRotatef( angleZ, 0, 0, 1 );
    glScalef( s, s, s );
    glColor3f( 0.4, 1, 1 );
    model->draw();
    glPopMatrix();

    glDisable( GL_LIGHTING );
}

//-----------------------------------------------------------------------------
void display() {
    checkForGLError( "display() - begin" );

    // Init matrices to transform to NDC
    glMatrixMode( GL_PROJECTION ); glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );  glLoadIdentity();

    // make the viewport smaller than the window so we can see if we are
    // clipping correctly
    int w = glutGet( GLUT_WINDOW_WIDTH );
    int h = glutGet( GLUT_WINDOW_HEIGHT );
    //glViewport( 100, 100, w - 200, h - 200 );
	glViewport(1, 1, w - 2, h - 2);

    // clear screen to black
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Render a background quad to fill the viewport
    if( myGL.GetSoftwareRendering() )
        glColor3f( 0.2, 0.2, .4 );
    else
        glColor3f( 0.2, 0.2, 0.2 );

    // draw background quad
    glBegin( GL_QUADS );
    glVertex3f( -1, -1, -1 );
    glVertex3f( 1, -1, -1 );
    glVertex3f( 1, 1, -1 );
    glVertex3f( -1, 1, -1 );
    glEnd();

    if( optPerspective ) {
        // Fit a perspective frustum such that a quad from (-1,-1) to (1,1) at z=0
        // fills the whole viewport. Make the depth range from z=-1 to z=1
        const float fov = 60;
        float d = 1 / tan( 60 / 2 * glm::pi<float>() / 180 );
        float n = d - 1;
        float f = n + 2;
        glMatrixMode( GL_PROJECTION );
        gluPerspective( 60, float( w ) / h, n, f );
        glMatrixMode( GL_MODELVIEW );
        gluLookAt( 0, 0, d,  0, 0, 0,  0, 1, 0 );
    } else {
        // NDC flips the z direction
        glMatrixMode( GL_PROJECTION );
        glScalef( 1, 1, -1 );
        glMatrixMode( GL_MODELVIEW );
    }

    //-------------------------
    // Set up render state
    //-------------------------
    if( optWireframe ) {
        // OpenGL will render just the edges of the original polygon. Since we
        // break the polygon up into triangles and then send them to GL,
        // we will be able to see them
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }

    if( optDepthTest && !myGL.GetSoftwareRendering() )
        glEnable( GL_DEPTH_TEST );

    if( optBackFaceCulling ) {
        glFrontFace( GL_CCW );
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
    } else {
        glDisable( GL_CULL_FACE );
    }

    // setup a directional light in camera space
    glPushMatrix(); glLoadIdentity();
    glLightfv( GL_LIGHT0, GL_POSITION, glm::value_ptr( glm::vec4( 1, 1, 1, 0 ) ) );
    glLightfv( GL_LIGHT0, GL_AMBIENT, glm::value_ptr( glm::vec4( .1, .1, .1, 1 ) ) );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, glm::value_ptr( glm::vec4( 1, 1, 1, 1 ) ) );
    glLightfv( GL_LIGHT0, GL_SPECULAR, glm::value_ptr( glm::vec4( 1, 1, 1, 1 ) ) );
    glEnable( GL_LIGHT0 ); // we don't actually get anything if lighting is not enabled
    glPopMatrix();

    // setup material properties - used only if lighting is enabled
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glm::value_ptr( glm::vec4( .4, .8, .2, 1 ) ) );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr( glm::vec4( .4, .4, .4, 1 ) ) );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 32 );
    glMaterialfv( GL_BACK, GL_EMISSION, glm::value_ptr( glm::vec4( 1, 1, 1, 1 ) ) );

    //------------------------------
    // draw something
    //------------------------------
    checkForGLError( "display() - 2" );

    if( optGeometryMode == 2 || optGeometryMode == 3 )
        drawPolygon();


    if( optGeometryMode == 1 || optGeometryMode == 2 )
        drawQuad();


    if( optGeometryMode == 4 )
        drawOBJ();


    //----------------------------------
    // restore render state
    //----------------------------------
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glDisable( GL_DEPTH_TEST );

    // done with this frame
    myGL.Display();
    glutSwapBuffers();

    checkForGLError( "display() - end" );
}

//------------------------------------------------------------------------------
// returns false on failure.
bool loadPPM( const char *filename, unsigned char *&rgbData, int &w, int &h ) {
    // This code lifted from GLVU
	FILE *fp;
	fopen_s(&fp, filename, "rb");
    if ( fp == NULL ) {
        printf( "Unable to open %s!\n", filename );
        return false;
    }
    int c, s;
    do { do { s = fgetc( fp ); } while ( s != '\n' ); } while ( ( c = fgetc( fp ) ) == '#' );
    ungetc( c, fp );
    fscanf_s( fp, "%d %d\n255\n", &w, &h );
    int numComponents = w * h * 3;
    rgbData = new unsigned char[numComponents];
    fread( rgbData, numComponents, 1, fp );
    fclose( fp );

    return true;
}
