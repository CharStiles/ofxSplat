#include "ofApp.h"
#include "dataset.h"


//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("vert.glsl", "frag.glsl");
	//mesh.load("lofi-bunny.ply");
	glPointSize(4);
	//ofSetDrawMode(OF_DRAW_POINTS);

	 ddset = dataset::from_ply("C:\\Users\\charl\\Documents\\of_v0.12.0_vs_release\\apps\\myApps\\ofSplat\\bin\\data\\point_cloud.ply");
	 //ddset.sort();
	// dataset::Dataset ddset = dataset::from_ply("C:\\Users\\charl\\Downloads\\output\\output\\input.ply");

	 len = ddset.buffer().size();

	 mesh.setMode(OF_PRIMITIVE_POINTS);

	 // loop through the image in the x and y axes
	 int skip = 4; // load a subset of the points
	 for (int i = 0; i < len; i+=skip) {
		 dataset::Splat s = ddset.buffer()[i];
		 float scale = 100;
		 glm::vec3 pos((float)s.center[0] * scale, (float)s.center[1] * scale, (float)s.center[2] * scale);
		 mesh.addVertex(pos);
	 }


}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	//use shader program

	shader.begin();
	//set 
	//set mat4 uniform of current view projection matrix
	shader.setUniformMatrix4f("view", cam.getModelViewProjectionMatrix());
	shader.setUniformMatrix4f("projection", cam.getProjectionMatrix());
	shader.setUniform2f("focal", cam.getFov(), ofGetWidth() / ofGetHeight());
	shader.setUniform3f("cam_pos", cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
	shader.setUniform1i("sh_degree", 3);


GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
		ddset.buffer().size(),
                 ddset.buffer().data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 2);


	//ofPushMatrix();
	mesh.drawVertices();
	shader.end();
	//ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	
	
	
	//ofPopMatrix();
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
