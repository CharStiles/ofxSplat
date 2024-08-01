#pragma once

#include "ofMain.h"
#include "ply.h"


#ifndef splat_h
#define splat_h

struct VertexData {
    float x, y, z;
    float opacity;
    float scale[3];
    float rot[4];
    float f_dc[3];
    float f_rest[15*3];
};



class ofxSplat {
    
    public:
    

		void setup(string pointCloud);
		void update();
		void draw();
        ofVboMesh mesh;

		ofShader shader;
		ofEasyCam cam;
        glm::vec3 sh[15];
        float camAngle =0;

        vector < VertexData > vertices;

};
#endif /* splat_h */
