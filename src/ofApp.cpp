#include "ofApp.h"
#include "dataset.h"


//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("vert.glsl", "frag.glsl");
	//mesh.load("lofi-bunny.ply");
	glPointSize(4);
	//ofSetDrawMode(OF_DRAW_POINTS);
	 dataset::Dataset ddset = dataset::from_ply("C:\\Users\\charl\\Documents\\of_v0.12.0_vs_release\\apps\\myApps\\ofSplat\\bin\\data\\point_cloud.ply");

	// dataset::Dataset ddset = dataset::from_ply("C:\\Users\\charl\\Downloads\\output\\output\\input.ply");

	 len = ddset.buffer().size();

	 mesh.setMode(OF_PRIMITIVE_POINTS);

	 // loop through the image in the x and y axes
	 int skip = 4; // load a subset of the points
	 for (int i = 0; i < len; i+=skip) {
		 dataset::Splat s = ddset.buffer()[i];
			
		 ofColor c = ofColor(s.sh[0][0], s.sh[0][1], s.sh[0][2],255);
		 mesh.addColor(c);
		 float scale = 100;
		 glm::vec3 pos((float)s.center[0] * scale, (float)s.center[1] * scale, (float)s.center[2] * scale);
		 mesh.addVertex(pos);
		 if (i < 10) {
			 ofLog(OF_LOG_NOTICE, "2x: " + ofToString(s.center[0]));
			 ofLog(OF_LOG_NOTICE, "2y: " + ofToString(s.center[1]));
			 ofLog(OF_LOG_NOTICE, "2z: " + ofToString(s.center[2]));
		 }
		 
	 }


}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	ofPushMatrix();
	//ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	mesh.drawVertices();
	
	
	ofPopMatrix();
	cam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
