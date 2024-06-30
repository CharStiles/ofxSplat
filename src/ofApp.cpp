#include "ofApp.h"
#include "ply.h"
#include <cstdint> // For uint32_t and uint16_t
#include <cstring> // For memcpy
#include <cmath>
#include <vector>
#include <string>


// todo:

// radiance / color stuff passed through
// with indices now, the number of points could be reduced from 6 to 4 per splat

float IsOutside(float x1, float y1, float z1, float x2, float y2, float z2, float distance)
{
    float dx = abs(x2 - x1);
    float dy = abs(y2-y1);
    float dz = abs(z2-z1);

    if (dx > distance) return 0; // too far in x direction
    if (dy > distance*2) return 0; // too far in y direction
    if (dz > distance) return 0; // too far in z direction

    return 1; // we're within the cube
}

//--------------------------------------------------------------
void ofApp::setup(){
   
    
    
    
	shader.load("vert.glsl", "frag.glsl");
    shader.bindDefaults();
    const std::string& filename = ofToDataPath("point_cloud_dave_large.ply");

    
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

//     const auto f_rest_0 = ply.accessor<float>("f_rest_0");
//     const auto f_rest_1 = ply.accessor<float>("f_rest_1");
//     const auto f_rest_2 = ply.accessor<float>("f_rest_2");
//
//

    
     std::vector<ply::PlyAccessor<float>> sh;
     for (size_t i = 0; i < 45; ++i)
         sh.push_back(ply.accessor<float>("f_rest_" + std::to_string(i)));
     int vertsRemoved = 0;
    for (size_t row = 0; row < ply.num_vertices(); ++row) {
        
        VertexData temp;
        temp.x = x(row);
        temp.y = y(row);
        temp.z = z(row);
        if (IsOutside(temp.x, temp.y, temp.z,0,1.3,0, 1) == 0) {
            vertsRemoved++;
            continue;
        }
        if (row == 0){
            cout << temp.x << " " << temp.y << " " << temp.z << endl;
        }
        temp.opacity = 1.f / (1.f + std::exp(-opacity(row)));
        temp.scale[0] = exp(scale_0(row));
        temp.scale[1] = exp(scale_1(row));
        temp.scale[2] = exp(scale_2(row));
        
        float qlen = sqrt( pow(rot_qx(row), 2) +
                           pow(rot_qy(row), 2) +
                           pow(rot_qz(row), 2) +
                           pow(rot_qw(row), 2));
        
        temp.rot[0] = (rot_qw(row)/qlen);
        temp.rot[1] = (rot_qx(row)/qlen);
        temp.rot[2] = (rot_qy(row)/qlen);
        temp.rot[3] = (rot_qz(row)/qlen);
        
        if (true){
            // Create a quaternion from these components
            glm::quat quaternion(rot_qx(row),
                                 rot_qy(row),
                                 rot_qz(row),
                                 rot_qw(row));
            
            // Normalize the quaternion
            glm::quat normalizedQuaternion = glm::normalize(quaternion);
            
            // Scale and offset
            glm::vec4 scaled = (glm::vec4(normalizedQuaternion.x, normalizedQuaternion.y, normalizedQuaternion.z, normalizedQuaternion.w) * 128.0f) + 128.0f;
            
            // Clip the values to ensure they are within the 0-255 range
            glm::vec4 clipped;
            clipped.x = std::min(std::max(scaled.x, 0.0f), 255.0f);
            clipped.y = std::min(std::max(scaled.y, 0.0f), 255.0f);
            clipped.z = std::min(std::max(scaled.z, 0.0f), 255.0f);
            clipped.w = std::min(std::max(scaled.w, 0.0f), 255.0f);
            
            // Convert to bytes
            std::vector<uint8_t> bytes = {
                static_cast<uint8_t>(clipped.z),
                static_cast<uint8_t>(clipped.w),
                static_cast<uint8_t>(clipped.x),
                static_cast<uint8_t>(clipped.y)
            };
            
            // Output bytes for verification
//            for (auto byte : bytes) {
//                //std::cout << (int)byte << " ";
//                cout << (float)(byte-128)/128.0 << endl;
//            }
            temp.rot[0] = (float)(bytes[0]-128)/128.0;
            temp.rot[1] = (float)(bytes[1]-128)/128.0;
            temp.rot[2] = (float)(bytes[2]-128)/128.0;
            temp.rot[3] = (float)(bytes[3]-128)/128.0;
//            if (row == 0){
//                cout << temp.rot[0] << endl;
//            }
            //std::cout << std::endl;
        }

        temp.f_dc[0] = f_dc_0(row);
        temp.f_dc[1] = f_dc_1(row);
        temp.f_dc[2] = f_dc_2(row);
        
        for (int i = 0; i < 15*3; ++i) {
            std::string accessor_name = "f_rest_" + std::to_string(i);
            temp.f_rest[i] = ply.accessor<float>(accessor_name)(row);
        }
        
        vertices.push_back(temp);

    }
    
    // subtract the center
    ofPoint center;
    for (int i = 0; i < vertices.size(); i++) {
        center +=ofPoint(vertices[i].x, vertices[i].y, vertices[i].z);
    }
    center/= (float)vertices.size();
    ofMatrix4x4 rot;
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].x -= center.x;
        vertices[i].y -= center.y;
        vertices[i].z -= center.z;
    }

  
    vector < float > data;
    
    for (size_t i = 0; i < vertices.size(); ++i) {
        
    
            const auto& v = vertices[i];

            float SH_C0 = 0.28209479177387814;
            float r = v.f_dc[0];
            float g =  v.f_dc[1];
            float b = v.f_dc[2];
            float a = v.opacity; // Opacity converted to alpha

            // Convert RGBA to 0-255 range and store
            uint32_t color = ((uint8_t)(r * 255) << 24) | ((uint8_t)(g * 255) << 16) | ((uint8_t)(b * 255) << 8) | (uint8_t)(a * 255);

   
        
        std::vector<double> rot = { v.rot[0], v.rot[1], v.rot[2], v.rot[3] };
        std::vector<double> scale = { v.scale[0], v.scale[1], v.scale[2] };
        std::vector<double> M(9, 0.0); // 9 elements for a 3x3 matrix
           M[0] = 1.0 - 2.0 * (rot[2] * rot[2] + rot[3] * rot[3]);
           M[1] = 2.0 * (rot[1] * rot[2] + rot[0] * rot[3]);
           M[2] = 2.0 * (rot[1] * rot[3] - rot[0] * rot[2]);
           M[3] = 2.0 * (rot[1] * rot[2] - rot[0] * rot[3]);
           M[4] = 1.0 - 2.0 * (rot[1] * rot[1] + rot[3] * rot[3]);
           M[5] = 2.0 * (rot[2] * rot[3] + rot[0] * rot[1]);
           M[6] = 2.0 * (rot[1] * rot[3] + rot[0] * rot[2]);
           M[7] = 2.0 * (rot[2] * rot[3] - rot[0] * rot[1]);
           M[8] = 1.0 - 2.0 * (rot[1] * rot[1] + rot[2] * rot[2]);

           // Apply scaling
        
           for (int j = 0; j < 9; ++j) {
               M[j] *= scale[j / 3];
               
               // I found printing this kind of stuff out super helpful to compare with the javascript code:
               
//               if (i == 0){
//                   cout << "M : " << M[j] << endl;
//               }

           }

           // Calculate sigma
           std::vector<float> sigma(6, 0.0);
           sigma[0] = M[0] * M[0] + M[3] * M[3] + M[6] * M[6];
           sigma[1] = M[0] * M[1] + M[3] * M[4] + M[6] * M[7];
           sigma[2] = M[0] * M[2] + M[3] * M[5] + M[6] * M[8];
           sigma[3] = M[1] * M[1] + M[4] * M[4] + M[7] * M[7];
           sigma[4] = M[1] * M[2] + M[4] * M[5] + M[7] * M[8];
           sigma[5] = M[2] * M[2] + M[5] * M[5] + M[8] * M[8];

        
        // helpful to debug
//        if (i == 0){
//            cout << sigma[0] << endl;
//            cout << sigma[1] << endl;
//            cout << sigma[2] << endl;
//            cout << sigma[3] << endl;
//            cout << sigma[4] << endl;
//            cout << sigma[5] << endl;
//        }
        
        std::vector<float> customData = {
            v.x, v.y, v.z, r, g, b, a, sigma[0],
            sigma[1], sigma[2], sigma[3], sigma[4], sigma[5],
            v.f_rest[0], v.f_rest[1], v.f_rest[2], v.f_rest[3],
            v.f_rest[4], v.f_rest[5], v.f_rest[6], v.f_rest[7],
            v.f_rest[8], v.f_rest[9], v.f_rest[10], v.f_rest[11],
            v.f_rest[12], v.f_rest[13], v.f_rest[14], v.f_rest[15],
            v.f_rest[16], v.f_rest[17], v.f_rest[18], v.f_rest[19],
            v.f_rest[20], v.f_rest[21], v.f_rest[22], v.f_rest[23],
            v.f_rest[24], v.f_rest[25], v.f_rest[26], v.f_rest[27],
            v.f_rest[28], v.f_rest[29], v.f_rest[30], v.f_rest[31],
            v.f_rest[32], v.f_rest[33], v.f_rest[34], v.f_rest[35],
            v.f_rest[36], v.f_rest[37], v.f_rest[38], v.f_rest[39],
            v.f_rest[40], v.f_rest[41], v.f_rest[42], v.f_rest[43],
            v.f_rest[44]
            };
        //6 vertices per point
        for (int z = 0; z < 6; z++){
            for (auto & f : customData){
                data.push_back(f);
            }
        }
        
    }

    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    vector < unsigned int > indices;
    for (int i = 0; i < vertices.size(); i++) {
        
        mesh.addVertex(ofPoint(-2, -2));
        mesh.addVertex(ofPoint(2, -2));
        mesh.addVertex(ofPoint(2, 2));
        
        
        mesh.addVertex(ofPoint(-2, -2));
        mesh.addVertex(ofPoint(-2, 2));
        mesh.addVertex(ofPoint(2, 2));
       
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        mesh.addColor(ofFloatColor( 1, 0, 1) );
        
        indices.push_back(i * 6 + 0);
        indices.push_back(i * 6 + 1);
        indices.push_back(i * 6 + 2);
        indices.push_back(i * 6 + 3);
        indices.push_back(i * 6 + 4);
        indices.push_back(i * 6 + 5);
        

    }
    
    mesh.addIndices(indices);
    
    
    shader.begin();
    
    // we pass the data to the shader through a bunch of custom
    // attributes -- vec4, vec4, vec3 and vec2 (13 floats)
    // x,y,z,r,g,b,a,sigma1, sigma2, sigma3, sigma4, sigma5, sigma6
    
    int customData1Loc = shader.getAttributeLocation("customData1");
    int customData2Loc = shader.getAttributeLocation("customData2");
    int customData3Loc = shader.getAttributeLocation("customData3");
    int customData4Loc = shader.getAttributeLocation("customData4");
    // 58 is how large 'data' is
    mesh.getVbo().setAttributeData(customData1Loc, data.data(), 4, vertices.size()*6, GL_STATIC_DRAW, sizeof(float) * 58); // First vec4
    mesh.getVbo().setAttributeData(customData2Loc, data.data() + 4, 4, vertices.size()*6, GL_STATIC_DRAW, sizeof(float) * 58); // Second vec4
    mesh.getVbo().setAttributeData(customData3Loc, data.data() + 8, 3, vertices.size()*6, GL_STATIC_DRAW, sizeof(float) * 58); // First vec3
    mesh.getVbo().setAttributeData(customData4Loc, data.data() + 11, 2, vertices.size()*6, GL_STATIC_DRAW, sizeof(float) * 58); // Remaining vec2 (for the last 2 floats)
    
    int shLocs[15*3];
    for (int i = 0; i < 15*3; ++i) {
        std::string locName = "sh" + std::to_string(i) ;
        shLocs[i] = shader.getAttributeLocation(locName);
    }
    
    for (int i = 0; i < 15 * 3; ++i) {
        mesh.getVbo().setAttributeData(shLocs[i], data.data()+13 +i, 1, vertices.size(), GL_STATIC_DRAW, sizeof(float) * 13);
    }
    
     shader.end();
    
    mesh.enableIndices();
}

//--------------------------------------------------------------
void ofApp::update(){

    if (ofGetFrameNum() % 60 == 0){
        shader.load("vert.glsl", "frag.glsl");
        shader.bindDefaults();
    }
    
    // Increment the camera angle
       camAngle += 0.01;  // Adjust speed as needed
       if(camAngle > TWO_PI) {
           camAngle -= TWO_PI;
       }

       // Calculate new camera position
       float camX = 1900 * cos(camAngle);
       float camZ = 1900 * sin(camAngle);
    
       // Update camera position
      // cam.setPosition(camX, camX*0.2, camZ);
//       cam.rotate(-90, 1,0,0);
//       cam.lookAt(glm::vec3(100, 0, 0));  // Look at the center of the mesh
      
}

//--------------------------------------------------------------
void ofApp::draw(){
    
   
    cam.begin();
    
    
	//use shader program
    ofDisableDepthTest();
    //ofBackground(0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
    
    glBlendFuncSeparate(
        GL_ONE_MINUS_DST_ALPHA,
        GL_ONE,
        GL_ONE_MINUS_DST_ALPHA,
        GL_ONE
    );
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
 
	shader.begin();

    
    
    shader.setUniformMatrix4f("view",cam.getModelViewMatrix());
    shader.setUniform2f("viewport", ofGetWidth(), ofGetHeight());
	shader.setUniformMatrix4f("projection", cam.getProjectionMatrix());
    shader.setUniform1f("time", ofGetElapsedTimef());
    float fov = cam.getFov();
    float viewportWidth = ofGetViewportWidth();
    float viewportHeight = ofGetViewportHeight();

    float focalLengthX = viewportWidth / (2.0f * tan(ofDegToRad(fov) / 2.0f));
    float focalLengthY = viewportHeight / (2.0f * tan(ofDegToRad(fov) / 2.0f));
    
    // I'm not convinced this is right:
	shader.setUniform2f("focal", focalLengthX, focalLengthY);
//    shader.setUniform2f("focal", 1159.5880733038064, 1164.6601287484507);

    shader.setUniform3f("cam_pos", cam.getPosition());
    
    
    // ----------------------------
    // sorting:
    
//    typedef struct {
//        ofPoint pt;
//        float distance;
//        int index;
//    } vertex2;
//    vector < vertex2 > vertsSorted;
//    ofMatrix4x4 projview =  cam.getProjectionMatrix() * cam.getModelViewMatrix();
//    for (int i = 0; i < vertices.size(); i++){
//        vertex2 v;
//        v.pt = ofPoint(vertices[i].x,vertices[i].y,vertices[i].z);
//        v.index = i;
//    
//        // distance calculation from the javascript code:
//        v.distance =((projview(0, 2) * v.pt.x +
//                                      projview(1, 2) * v.pt.y +
//                                      projview(2, 2) * v.pt.z) * 4096);
//       
//        vertsSorted.push_back(v);
//    }
//    std::sort(vertsSorted.begin(), vertsSorted.end(), [](const vertex2 &a, const vertex2 &b)
//    {
//        return a.distance < b.distance;
//    });
    
    
    struct vertex2 {
        ofPoint pt;
        int index;
        int32_t depth;
    };

    vector < vertex2 > vertexsss;
    ofMatrix4x4 projview =  cam.getProjectionMatrix() * cam.getModelViewMatrix();
   
    
    int32_t maxDepth = std::numeric_limits<int32_t>::min();
    int32_t minDepth = std::numeric_limits<int32_t>::max();

    for (int i = 0; i < vertices.size(); i++) {
        vertex2 v;
        v.pt = ofPoint(vertices[i].x, vertices[i].y, vertices[i].z);
        v.index = i;
        
//        v.depth = static_cast<int32_t>((
//            viewProj[2] * v.pt.x +
//            viewProj[6] * v.pt.y +
//            viewProj[10] * v.pt.z
//        ) * 4096);
        
        v.depth =((projview(0, 2) * v.pt.x +
         projview(1, 2) * v.pt.y +
            projview(2, 2) * v.pt.z) * 4096);

        maxDepth = std::max(maxDepth, v.depth);
        minDepth = std::min(minDepth, v.depth);

        vertexsss.push_back(v);
    }

    // Inline lambda for counting sort
    auto countingSort = [&vertexsss, maxDepth, minDepth]() {
        int vertexCount = vertexsss.size();
        double depthRange = static_cast<double>(maxDepth) - minDepth;
        double depthInv = (depthRange > 0) ? ((256.0 * 256.0 - 1) / depthRange) : 1.0;
        
        std::vector<uint32_t> counts0(256 * 256, 0);
        
        // First pass: count occurrences
        for (int i = 0; i < vertexCount; i++) {
            int32_t normalizedDepth = static_cast<int32_t>((vertexsss[i].depth - minDepth) * depthInv);
            normalizedDepth = std::max(0, std::min(normalizedDepth, 256 * 256 - 1)); // Clamp value
            counts0[normalizedDepth]++;
        }

        std::vector<uint32_t> starts0(256 * 256, 0);
        uint32_t totalCount = 0;
        for (int i = 0; i < 256 * 256; i++) {
            starts0[i] = totalCount;
            totalCount += counts0[i];
        }

        std::vector<uint32_t> depthIndex(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            int32_t normalizedDepth = static_cast<int32_t>((vertexsss[i].depth - minDepth) * depthInv);
            normalizedDepth = std::max(0, std::min(normalizedDepth, 256 * 256 - 1)); // Clamp value
            
            if (starts0[normalizedDepth] < vertexCount) {
                depthIndex[starts0[normalizedDepth]++] = i;
            } else {
                std::cout << "Error: starts0[" << normalizedDepth << "] = " << starts0[normalizedDepth]
                          << " is out of bounds for depthIndex of size " << vertexCount << std::endl;
                // As a fallback, place this vertex at the end
                depthIndex[vertexCount - 1] = i;
            }
        }

        std::vector<vertex2> sortedVertices(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            sortedVertices[i] = vertexsss[depthIndex[i]];
        }

        vertexsss = std::move(sortedVertices);
    };

    // Call the counting sort
    countingSort();

    
    
    vector < unsigned int > indices;
    for (int i = 0; i < vertexsss.size(); i++) {
        
        int index = vertexsss[i].index;
        indices.push_back(index*6 + 0);
        indices.push_back(index*6 + 1);
        indices.push_back(index*6 + 2);
        indices.push_back(index*6 + 3);
        indices.push_back(index*6 + 4);
        indices.push_back(index*6 + 5);
    }
    
    mesh.getVbo().updateIndexData(indices.data(), indices.size());
    
	mesh.draw();
	shader.end();

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
