#ifndef _MYGL_H_
#define _MYGL_H_

#include "GLRenderer.h"

typedef vector<GLVertex>::size_type size_type;

//
// MAIN CLASS TO IMPLEMENT FOR SOFTWARE RENDERING PIPELINE
//

//==============================================================================
class MyGL : public GLRenderer
//==============================================================================
{
  public:
    MyGL();
    ~MyGL();

    // Methods to turn the various stages on/off
    bool GetDoTriangulate()         { return _doTriangulate; }
    void SetDoTriangulate( bool v ) { _doTriangulate = v; }
    bool GetDoRasterize()           { return _doRasterize; }
    void SetDoRasterize( bool v )   { _doRasterize = v; }

  protected:
    // The following functions are called by GLRenderer::processPolygon().
    virtual bool TriangulatePolygon( const vector<GLVertex> &polygonVerts,
                                     vector<GLVertex> &triangleVerts );
    virtual bool RasterizeTriangle( GLVertex triVerts[3] );

  private:
    bool _doTriangulate;
    bool _doRasterize;
};

#endif // _MYGL_H_