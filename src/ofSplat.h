#pragma once

#include "ofMain.h"
#include "ply.h"



struct VertexData {
    float x, y, z;
    float opacity;
    float scale[3];
    float rot[4];
    float f_dc[3];
    float f_rest[15*3];
};



class ofSplat : public ofBaseApp{
    
    public:
    

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		ofVboMesh mesh;
		ofShader shader;
		ofEasyCam cam;
        glm::vec3 sh[15];
        float camAngle =0;

        vector < VertexData > vertices;

};
