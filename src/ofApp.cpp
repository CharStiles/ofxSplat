#include "ofApp.h"
#include "ply.h"


//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("vert.glsl", "frag.glsl");
	//mesh.load("lofi-bunny.ply");
	glPointSize(4);
	//ofSetDrawMode(OF_DRAW_POINTS);
    const std::string& filename = "C:\\Users\\charl\\Documents\\of_v0.12.0_vs_release\\apps\\myApps\\ofSplat\\bin\\data\\point_cloud.ply";
	 //ddset = dataset::from_ply("C:\\Users\\charl\\Documents\\of_v0.12.0_vs_release\\apps\\myApps\\ofSplat\\bin\\data\\point_cloud.ply");
	 //ddset.sort();
	// dataset::Dataset ddset = dataset::from_ply("C:\\Users\\charl\\Downloads\\output\\output\\input.ply");

      //tracing::RecorderGuard tracing_guard("load dataset");
    
     ply::PlyFile ply(filename);

     // Create accessors
     const auto x = ply.accessor<float>("x");
     const auto y = ply.accessor<float>("y");
     const auto z = ply.accessor<float>("z");
     const auto opacity = ply.accessor<float>("opacity");
     const auto scale_0 = ply.accessor<float>("scale_0");
     const auto scale_1 = ply.accessor<float>("scale_1");
     const auto scale_2 = ply.accessor<float>("scale_2");
     const auto rot_qw = ply.accessor<float>("rot_0");
     const auto rot_qx = ply.accessor<float>("rot_1");
     const auto rot_qy = ply.accessor<float>("rot_2");
     const auto rot_qz = ply.accessor<float>("rot_3");

     // Spherical harmonics accessors
     const auto f_dc_0 = ply.accessor<float>("f_dc_0");
     const auto f_dc_1 = ply.accessor<float>("f_dc_1");
     const auto f_dc_2 = ply.accessor<float>("f_dc_2");
     std::vector<ply::PlyAccessor<float>> sh;
     for (size_t i = 0; i < 45; ++i)
         sh.push_back(ply.accessor<float>("f_rest_" + std::to_string(i)));

// declare ofTexture openframeworks with ply.num_vertices() *3 size
	 ofTexture texdata;
	 
	int texwidth = 1024 * 2; // Set to your desired width
    int texheight = ceil((2 * ply.num_vertices()) / texwidth); // Set to your desired height
	 texdata.allocate(texwidth * texheight * 4, 1, GL_RGB32F);


     //SplatBuffer buffer(ply.num_vertices());
     {
         //tracing::RecorderGuard tracing_guard("buffer population");
         for (size_t row = 0; row < ply.num_vertices(); ++row) {
             //Splat& splat = buffer.at(row);

             // Mean of each Gaussian
 //            splat.center[0] = x(row);
   //          splat.center[1] = y(row);
     //        splat.center[2] = z(row);
       //      if (row < 10 * 3.) {
         //        ofLog(OF_LOG_NOTICE, "1x: " + ofToString(splat.center[0]));
           //      ofLog(OF_LOG_NOTICE, "1y: " + ofToString(splat.center[1]));
             //    ofLog(OF_LOG_NOTICE, "1z: " + ofToString(splat.center[2]));
             }
             // Covariance
             const ofMatrix4x4 scale(
                 std::exp(scale_0(row)), 0, 0, 0,
                 0, std::exp(scale_1(row)), 0, 0,
                 0, 0, std::exp(scale_2(row)), 0,
                 0, 0, 0, 1);
             const ofVec4f quat_coeffs(
                 rot_qx(row),
                 rot_qy(row),
                 rot_qz(row),
                 rot_qw(row));
             const ofMatrix4x4 R(
                 ofQuaternion(quat_coeffs.normalized()));

             //// Compute the matrix product of S and R (M = S * R)
             const ofMatrix4x4 M = R * scale;
             splat.covA[0] = M.getRowAsVec3f(0).dot(M.getRowAsVec3f(0));
             splat.covA[1] = M.getRowAsVec3f(0).dot(M.getRowAsVec3f(1));
             splat.covA[2] = M.getRowAsVec3f(0).dot(M.getRowAsVec3f(2));
             splat.covB[0] = M.getRowAsVec3f(1).dot(M.getRowAsVec3f(1));
             splat.covB[1] = M.getRowAsVec3f(1).dot(M.getRowAsVec3f(2));
             splat.covB[2] = M.getRowAsVec3f(2).dot(M.getRowAsVec3f(2));
//TODO MAKE THIS COMPILE
             float sigma[6] = {
    M[0] * M[0] + M[3] * M[3] + M[6] * M[6],
    M[0] * M[1] + M[3] * M[4] + M[6] * M[7],
    M[0] * M[2] + M[3] * M[5] + M[6] * M[8],
    M[1] * M[1] + M[4] * M[4] + M[7] * M[7],
    M[1] * M[2] + M[4] * M[5] + M[7] * M[8],
    M[2] * M[2] + M[5] * M[5] + M[8] * M[8]
             };

             // Pack sigma values into half-precision format and assign to texdata array
             //uint32_t texdata[8]; // Assuming texdata is an array of appropriate size
             texdata[8 * i + 4] = floatToHalf(4 * sigma[0]) | (floatToHalf(4 * sigma[1]) << 16);
             texdata[8 * i + 5] = floatToHalf(4 * sigma[2]) | (floatToHalf(4 * sigma[3]) << 16);
             texdata[8 * i + 6] = floatToHalf(4 * sigma[4]) | (floatToHalf(4 * sigma[5]) << 16);

             //// Alpha
             //splat.alpha = 1.f / (1.f + std::exp(-opacity(row)));

             //// Color (spherical harmonics)
             //splat.sh[0][0] = f_dc_0(row);
             //splat.sh[0][1] = f_dc_1(row);
             //splat.sh[0][2] = f_dc_2(row);
             //for (size_t sh_idx = 1; sh_idx < 15; ++sh_idx) {
             //    splat.sh[sh_idx][0] = sh.at(sh_idx - 1)(row);
             //    splat.sh[sh_idx][1] = sh.at(sh_idx + 14)(row);
             //    splat.sh[sh_idx][2] = sh.at(sh_idx + 29)(row);
             //}


         }
     
     }




	 //len = ddset.buffer().size();

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
