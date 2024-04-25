// Adapted from https://github.com/antimatter15/splat
#version 150

//precision highp float;
//precision highp int;


in vec3 position;
in vec3 color;
in vec4 customData1;
in vec4 customData2;
in vec3 customData3;
in vec2 customData4;

out vec4 vColor;
out vec2 vPosition;

uniform mat4 projection, view;
uniform vec2 focal;
uniform vec2 viewport;

uniform float time;

// vec3 ComputeRadianceFromSH(const vec3 v)
// {
// // #ifdef FULL_SH
//     float b[16];
// // #else
// //     float b[4];
// //#endif

//     float vx2 = v.x * v.x;
//     float vy2 = v.y * v.y;
//     float vz2 = v.z * v.z;

//     // zeroth order
//     // (/ 1.0 (* 2.0 (sqrt pi)))
//     b[0] = 0.28209479177387814f;

//     // first order
//     // (/ (sqrt 3.0) (* 2 (sqrt pi)))
//     float k1 = 0.4886025119029199f;
//     b[1] = -k1 * v.y;
//     b[2] = k1 * v.z;
//     b[3] = -k1 * v.x;

// // #ifdef FULL_SH
// //     // second order
// //     // (/ (sqrt 15.0) (* 2 (sqrt pi)))
// //     float k2 = 1.0925484305920792f;
// //     // (/ (sqrt 5.0) (* 4 (sqrt  pi)))
// //     float k3 = 0.31539156525252005f;
// //     // (/ (sqrt 15.0) (* 4 (sqrt pi)))
// //     float k4 = 0.5462742152960396f;
// //     b[4] = k2 * v.y * v.x;
// //     b[5] = -k2 * v.y * v.z;
// //     b[6] = k3 * (3.0f * vz2 - 1.0f);
// //     b[7] = -k2 * v.x * v.z;
// //     b[8] = k4 * (vx2 - vy2);

// //     // third order
// //     // (/ (* (sqrt 2) (sqrt 35)) (* 8 (sqrt pi)))
// //     float k5 = 0.5900435899266435f;
// //     // (/ (sqrt 105) (* 2 (sqrt pi)))
// //     float k6 = 2.8906114426405543f;
// //     // (/ (* (sqrt 2) (sqrt 21)) (* 8 (sqrt pi)))
// //     float k7 = 0.4570457994644658f;
// //     // (/ (sqrt 7) (* 4 (sqrt pi)))
// //     float k8 = 0.37317633259011546f;
// //     // (/ (sqrt 105) (* 4 (sqrt pi)))
// //     float k9 = 1.4453057213202771f;
// //     b[9] = -k5 * v.y * (3.0f * vx2 - vy2);
// //     b[10] = k6 * v.y * v.x * v.z;
// //     b[11] = -k7 * v.y * (5.0f * vz2 - 1.0f);
// //     b[12] = k8 * v.z * (5.0f * vz2 - 3.0f);
// //     b[13] = -k7 * v.x * (5.0f * vz2 - 1.0f);
// //     b[14] = k9 * v.z * (vx2 - vy2);
// //     b[15] = -k5 * v.x * (vx2 - 3.0f * vy2);

// //     float re = (b[0] * r_sh0.x + b[1] * r_sh0.y + b[2] * r_sh0.z + b[3] * r_sh0.w +
// //                 b[4] * r_sh1.x + b[5] * r_sh1.y + b[6] * r_sh1.z + b[7] * r_sh1.w +
// //                 b[8] * r_sh2.x + b[9] * r_sh2.y + b[10]* r_sh2.z + b[11]* r_sh2.w +
// //                 b[12]* r_sh3.x + b[13]* r_sh3.y + b[14]* r_sh3.z + b[15]* r_sh3.w);

// //     float gr = (b[0] * g_sh0.x + b[1] * g_sh0.y + b[2] * g_sh0.z + b[3] * g_sh0.w +
// //                 b[4] * g_sh1.x + b[5] * g_sh1.y + b[6] * g_sh1.z + b[7] * g_sh1.w +
// //                 b[8] * g_sh2.x + b[9] * g_sh2.y + b[10]* g_sh2.z + b[11]* g_sh2.w +
// //                 b[12]* g_sh3.x + b[13]* g_sh3.y + b[14]* g_sh3.z + b[15]* g_sh3.w);

// //     float bl = (b[0] * b_sh0.x + b[1] * b_sh0.y + b[2] * b_sh0.z + b[3] * b_sh0.w +
// //                 b[4] * b_sh1.x + b[5] * b_sh1.y + b[6] * b_sh1.z + b[7] * b_sh1.w +
// //                 b[8] * b_sh2.x + b[9] * b_sh2.y + b[10]* b_sh2.z + b[11]* b_sh2.w +
// //                 b[12]* b_sh3.x + b[13]* b_sh3.y + b[14]* b_sh3.z + b[15]* b_sh3.w);
// // #else
//     float re = (b[0] * r_sh0.x + b[1] * r_sh0.y + b[2] * r_sh0.z + b[3] * r_sh0.w);
//     float gr = (b[0] * g_sh0.x + b[1] * g_sh0.y + b[2] * g_sh0.z + b[3] * g_sh0.w);
//     float bl = (b[0] * b_sh0.x + b[1] * b_sh0.y + b[2] * b_sh0.z + b[3] * b_sh0.w);
// // #endif
//     return vec3(0.5f, 0.5f, 0.5f) + vec3(re, gr, bl);
// }


void main () {
    
    float x = customData1.x;
    float y = customData1.y;
    float z = customData1.z;

    float r = customData1.a;
    float g = customData2.x;
    float b = customData2.y;
    float a = customData2.z;
    
    float sigma0 = customData2.a;
    float sigma1 = customData3.x;
    float sigma2 = customData3.y;
    float sigma3 = customData3.z;
    float sigma4 = customData4.x;
    float sigma5 = customData4.y;
    
    vec4 cen = vec4(x,y,z,0)*500.;              // weird scale factor
    vec4 cam = view * vec4(cen.xyz, 1.);
    
    vec4 pos2d = projection * cam;
    float clip = 1.2 * pos2d.w;
    if (pos2d.z < -clip || pos2d.x < -clip || pos2d.x > clip || pos2d.y < -clip || pos2d.y > clip) {
        gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
        return;
    }

    // cen = vec4(x,y,z,1)*100.;              // weird scale factor
    // cam = view * vec4(cen.xyz, 1.);
    // pos2d = projection * cam;

    vec2 u1 = vec2(sigma0, sigma1);
    vec2 u2 = vec2(sigma2, sigma3);
    vec2 u3 = vec2(sigma4, sigma5);
    
    mat3 Vrk = mat3(u1.x, u1.y, u2.x, u1.y, u2.y, u3.x, u2.x, u3.x, u3.y);

    mat3 J = mat3(
        focal.x / cam.z, 0., -(focal.x * cam.x) / (cam.z * cam.z),
        0., -focal.y / cam.z, (focal.y * cam.y) / (cam.z * cam.z),
        0., 0., 0.
    );

    mat3 T = transpose(mat3(view)) * J;
    mat3 cov2d = transpose(T) * Vrk * T;

    float mid = (cov2d[0][0] + cov2d[1][1]) / 2.0;
    float radius = length(vec2((cov2d[0][0] - cov2d[1][1]) / 2.0, cov2d[0][1]));
    float lambda1 = mid + radius, lambda2 = mid - radius;

    if(lambda2 < 0.0) return;
    vec2 diagonalVector = normalize(vec2(cov2d[0][1], lambda1 - cov2d[0][0]));
    
    
    vec2 majorAxis = min(sqrt(2.0 * lambda1), 2048.0) * diagonalVector;
    vec2 minorAxis = min(sqrt(2.0 * lambda2), 2048.0) * vec2(diagonalVector.y, -diagonalVector.x);
    
//    float angle = 3.14; // Rotate 180 degrees
//    mat2 rotate180 = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
//    majorAxis = rotate180 * majorAxis;
//    minorAxis = rotate180 * minorAxis;
//
    
    vec2 vCenter = vec2(pos2d) / pos2d.w;

    vColor = vec4(color, 1.);
    
    vColor =clamp(pos2d.z/pos2d.w, 0.0, 1.0) * vec4(r,g,b,a);
    //vec4((cov.w) & 0xffu, (cov.w >> 8) & 0xffu, (cov.w >> 16) & 0xffu, (cov.w >> 24) & 0xffu) / 255.0;
        
    
    vPosition = position.xy*4.;

    // 0.1 is a scale factor I wasn't sure
    
    gl_Position = vec4(
        vCenter
        + position.x*500. * majorAxis / viewport
        + position.y*500. * minorAxis / viewport, 0.0, 1.0);
    
      
}

