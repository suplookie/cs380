#include "MyGL.h"
#include <glm/gtc/matrix_access.hpp>
//------------------------------------------------------------------------------
MyGL::MyGL()
    :
    _doTriangulate( false ),
    _doRasterize( false ) {
}

//------------------------------------------------------------------------------
MyGL::~MyGL()
{}

//------------------------------------------------------------------------------
bool MyGL::TriangulatePolygon( const vector<GLVertex> &polygonVerts,
                               vector<GLVertex> &triangleVerts ) {
    if( !_doTriangulate )
        return false;

    //
    // YOUR CODE HERE
    //
	
    // Implement triangulation here.
    // Keep in mind that the color of the first scene (F1) will depend on your implementation.
    // You must set it right so that it should not feel so "glitchy".

    // just make a triangle out of the first three vertices for now

	int s = polygonVerts.size();
	if (s < 3) {
		return true;
	}
	for (int i = 1; i + 1 < s; i++) {
		triangleVerts.push_back( polygonVerts[0] );
		triangleVerts.push_back( polygonVerts[i] );
		triangleVerts.push_back( polygonVerts[i + 1] );
	}
	return true;

    if( polygonVerts.size() >= 3 ) {
        triangleVerts.push_back( polygonVerts[0] );
        triangleVerts.push_back( polygonVerts[1] );
        triangleVerts.push_back( polygonVerts[2] );
        return true;

    } else {
        return true;
    }
}

//------------------------------------------------------------------------------
bool MyGL::RasterizeTriangle(GLVertex verts[3]) {
	if (!_doRasterize)
		return false;

	//
	// YOUR CODE HERE
	//
	//printf("raterization\n");
	// Implement rasterization here. You are NOT required to implement wireframe mode (i.e. 'w' key mode).

	// just render the vertices for now
	double x0 = verts[0].position[0];
	double x1 = verts[1].position[0];
	double x2 = verts[2].position[0];
	double y0 = verts[0].position[1];
	double y1 = verts[1].position[1];
	double y2 = verts[2].position[1];

	double r0 = verts[0].color[0];
	double r1 = verts[1].color[0];
	double r2 = verts[2].color[0];
	double g0 = verts[0].color[1];
	double g1 = verts[1].color[1];
	double g2 = verts[2].color[1];
	double b0 = verts[0].color[2];
	double b1 = verts[1].color[2];
	double b2 = verts[2].color[2];

	double A0 = (y1 - y2);
	double A1 = (y2 - y0);
	double A2 = (y0 - y1);
	double B0 = (x2 - x1);
	double B1 = (x0 - x2);
	double B2 = (x1 - x0);
	double C0 = (x1 * y2 - x2 * y1);
	double C1 = (x2 * y0 - x0 * y2);
	double C2 = (x0 * y1 - x1 * y0);

	if ((C0 + C1 + C2 == 0)) {
		//printf("area zero\n");
		return true;
	}
	if (cullFaceEnabled && C0 + C1 + C2 < 0) {
		//printf("culling\n");
		return true;
	}
	triangles += 1;

	double Ar = (r0 * A0 + r1 * A1 + r2 * A2) / (C0 + C1 + C2);
	double Br = (r0 * B0 + r1 * B1 + r2 * B2) / (C0 + C1 + C2);
	double Cr = (r0 * C0 + r1 * C1 + r2 * C2) / (C0 + C1 + C2);
	double Ag = (g0 * A0 + g1 * A1 + g2 * A2) / (C0 + C1 + C2);
	double Bg = (g0 * B0 + g1 * B1 + g2 * B2) / (C0 + C1 + C2);
	double Cg = (g0 * C0 + g1 * C1 + g2 * C2) / (C0 + C1 + C2);
	double Ab = (b0 * A0 + b1 * A1 + b2 * A2) / (C0 + C1 + C2);
	double Bb = (b0 * B0 + b1 * B1 + b2 * B2) / (C0 + C1 + C2);
	double Cb = (b0 * C0 + b1 * C1 + b2 * C2) / (C0 + C1 + C2);

	int minX = x0;
	if (x1 < minX) minX = x1;
	if (x2 < minX) minX = x2;
	int maxX = x0;
	if (x1 > maxX) maxX = x1;
	if (x2 > maxX) maxX = x2;
	int minY = y0;
	if (y1 < minY) minY = y1;
	if (y2 < minY) minY = y2;
	int maxY = y0;
	if (y1 > maxY) maxY = y1;
	if (y2 > maxY) maxY = y2;
	
	int w, h;
	frameBuffer.GetSize(w, h);
	//printf("%f %f %f %f %d %d %d %d\n", r1, r2, g1, g2, minX, maxX, minY, maxY);
	double newdep = verts[0].position[2];

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			if (!(x >= 0 && x < w && y >= 0 && y < h)) {
				//out of the screen
				continue;
			}
			if (frameBuffer.GetDepth(x, y) == 0) {
				frameBuffer.SetDepth(x, y, 1);
			}
			if (newdep > frameBuffer.GetDepth(x, y) || newdep >= 1 || newdep < 0) {
				continue;
			}
			glm::vec4 color;
			bool set = false;
			color[0] = Ar * x + Br * y + Cr;
			color[1] = Ag * x + Bg * y + Cg;
			color[2] = Ab * x + Bb * y + Cb;
			if (C0 + C1 + C2 > 0) {
				if ((x * A2 + y * B2 + C2 > 0) && (x * A0 + y * B0 + C0 > 0) && (x * A1 + y * B1 + C1 > 0)) {
					frameBuffer.SetPixel(x, y, color);
					set = true;
				}
			}
			else {
				if ((x * A2 + y * B2 + C2 < 0) && (x * A0 + y * B0 + C0 < 0) && (x * A1 + y * B1 + C1 < 0)) {
					frameBuffer.SetPixel(x, y, color);
					set = true;
				}
			}

			if ((x * A2 + y * B2 + C2 == 0) && x > x0 && x < x1 && y > y0 && y < y1) {
				if (A2 > 0 || (A2 == 0 && B2 > 0)) {
					frameBuffer.SetPixel(x, y, color);
					set = true;
				}
			}
			if ((x * A1 + y * B1 + C1 == 0) && x > x0 && x < x2 && y > y0 && y < y2) {
				if (A1 > 0 || (A1 == 0 && B1 > 0)) {
					frameBuffer.SetPixel(x, y, color);
					set = true;
				}
			}
			if ((x * A0 + y * B0 + C0 == 0) && x > x2 && x < x1 && y > y2 && y < y1) {
				if (A0 > 0 || (A0 == 0 && B0 > 0)) {
					frameBuffer.SetPixel(x, y, color);
					set = true;
				}
			}
			if (set) {
				frameBuffer.SetDepth(x, y, newdep);
			}
		}
	}


	for (int i = 0; i < 3; i++) {
		GLVertex &v = verts[i];
		glm::vec4 color;
		if (!(v.position[0] >= 0 && v.position[0] < w && v.position[1] >= 0 && v.position[1] < h)) {
			//out of the screen
			continue;
		}
		if (newdep > frameBuffer.GetDepth(v.position[0], v.position[1]) || newdep >= 1 || newdep < 0) {
			continue;
		}
		if (textureEnabled) {
			if (texture.id != 0) {
				// look up color in the texture
				int x = v.texCoord[0] * (texture.width - 1);
				int y = v.texCoord[1] * (texture.height - 1);
				color = texture.GetPixel(x, y, 0);
			}
		}
		else
			color = v.color;

		frameBuffer.SetPixel(v.position[0], v.position[1], color);

		// depthTestEnabled is 0 if the polygon forms a background.
		// depthTestEnabled is 1 otherwise.
			frameBuffer.SetDepth(v.position[0], v.position[1], v.position[2]);
	}


	return true;
}

