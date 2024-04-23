#pragma once

#include "ofMain.h"
#include "ply.h"

class ofApp : public ofBaseApp{
    
    public:
    
    struct SortResult {
        void reset(size_t num_vertices) {
            depth_index.resize(num_vertices);

            // Scratch space
            depths.resize(num_vertices);
            sizes.resize(num_vertices);
            counts0.fill(0);
            starts0.fill(0);
        }

        size_t num_vertices() const {
            return depth_index.size();
        }

        std::vector<uint32_t> depth_index;

        // Scratch space
        std::vector<float> depths;
        std::vector<int32_t> sizes;
        std::array<float, 256 * 256> counts0;
        std::array<float, 256 * 256> starts0;
    };
    
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
		size_t len;
		ofEasyCam cam;
        ofTexture texture;
        SortResult sr;
    
    
    
    
	//private:
		//dataset::Dataset ddset;
};
