// Adapted from https://github.com/antimatter15/splat
#version 150

precision highp float;
//precision highp int;


in vec3 position;
in vec3 color;
in vec4 customData1;
in vec4 customData2;
in vec3 customData3;
// in vec2 customData4;

//0-47 declare sh
in vec4 sh0;
in vec4 sh1;
in vec4 sh2;
in vec4 sh3;
in vec4 sh4;
in vec4 sh5;
in vec4 sh6;
in vec4 sh7;
in vec4 sh8;
in vec4 sh9;
in vec4 sh10;
in vec4 sh11;
vec3 scaler = vec3(1,1,1);

vec3 sh[16] = vec3[16](
    vec3( customData1.a, customData2.x, customData2.y),
    vec3(sh0.r, sh0.g, sh0.b)* scaler,
    vec3(sh0.a, sh1.r, sh1.g)* scaler,
    vec3(sh1.b, sh1.a, sh2.r)* scaler,
    vec3(sh2.g, sh2.b, sh2.a)* scaler,
    vec3(sh3.r, sh3.g, sh3.b)* scaler,
    vec3(sh3.a, sh4.r, sh4.g)* scaler,
    vec3(sh4.b, sh4.a, sh5.r)* scaler,
    vec3(sh5.g, sh5.b, sh5.a)* scaler,
    vec3(sh6.r, sh6.g, sh6.b)* scaler,
    vec3(sh6.a, sh7.r, sh7.g)* scaler,
    vec3(sh7.b, sh7.a, sh8.r)* scaler,
    vec3(sh8.g, sh8.b, sh8.a)* scaler,
    vec3(sh9.r, sh9.g, sh9.b)* scaler,
    vec3(sh9.a, sh10.r, sh10.g)* scaler,
    vec3(sh10.b, sh10.a, sh11.r)* scaler
    // vec3(sh11.g, sh11.b, sh11.a)* scaler
    // vec3(sh12.r, sh12.g, sh12.b)* scaler
 );
        

out vec4 vColor;
out vec2 vPosition;

uniform mat4 projection, view;
uniform vec2 focal;
uniform vec2 viewport;

uniform float time;

const float SH_C0 = 0.28209479177387814;
const float SH_C1 = 0.4886025119029199;
const float SH_C2[5] = float[5](
    1.0925484305920792,
    -1.0925484305920792,
    0.31539156525252005,
    -1.0925484305920792,
    0.5462742152960396
);
const float SH_C3[7] = float[7](
    -0.5900435899266435,
    2.890611442640554,
    -0.4570457994644658,
    0.3731763325901154,
    -0.4570457994644658,
    1.445305721320277,
    -0.5900435899266435
);

vec3 compute_color_from_sh(vec3 position, vec3 camPos) {
    vec3 dir = normalize(position - camPos);
    vec3 result = (SH_C0 * sh[0]);

    // if deg > 0
    float x = dir.x;
    float y = dir.y;
    float z = dir.z;

    result += SH_C1 * (-y * sh[1] + z * sh[2] - x * sh[3]);

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;

    // if (sh_degree > 1) {
    result +=
        SH_C2[0] * xy * sh[4] +
        SH_C2[1] * yz * sh[5] +
        SH_C2[2] * (2.0 * zz - xx - yy) * sh[6] +
        SH_C2[3] * xz * sh[7] +
        SH_C2[4] * (xx - yy) * sh[8];
    
    // if (sh_degree > 2) {
    result +=
        SH_C3[0] * y * (3.0 * xx - yy) * sh[9] +
        SH_C3[1] * xy * z * sh[10] +
        SH_C3[2] * y * (4.0 * zz - xx - yy) * sh[11] +
        SH_C3[3] * z * (2.0 * zz - 3.0 * xx - 3.0 * yy) * sh[12] +
        SH_C3[4] * x * (4.0 * zz - xx - yy) * sh[13] +
        SH_C3[5] * z * (xx - yy) * sh[14] +
        SH_C3[6] * x * (xx - 3.0 * yy) * sh[15];

    // unconditional
    // result = (SH_C0 * sh[0]);
    result +=  0.5;

    return max(result, vec3(0.0));
}

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
    
    // pull the data out of the custom attributes:
    
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
    float sigma4 = sh11.g;
    float sigma5 = sh11.b;
    
    float scaler = 300.;  // a scale factor that helps
                          // since the points are so small
    
    vec4 cen = vec4(x,y,z,0)*scaler;              // weird scale factor
    vec4 cam = view * vec4(cen.xyz, 1.);
    
    vec4 pos2d = projection * cam;
    float clip = 1.2 * pos2d.w;
    if (pos2d.z < -clip || pos2d.x < -clip || pos2d.x > clip || pos2d.y < -clip || pos2d.y > clip) {
        gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
        return;
    }

    vec2 u1 = vec2(sigma0*1., sigma1*1.);
    vec2 u2 = vec2(sigma2*1., sigma3*1.);
    vec2 u3 = vec2(sigma4*1., sigma5*1.);
    
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
    vec2 diagonalVector = normalize(vec2(-cov2d[0][1], lambda1 - cov2d[0][0]));
    
    vec2 majorAxis = min(sqrt(2.0 * lambda1), 2048.0) * diagonalVector;
    vec2 minorAxis = min(sqrt(2.0 * lambda2), 2048.0) * vec2(diagonalVector.y, -diagonalVector.x);
    
    vec2 vCenter = vec2(pos2d) / pos2d.w;

    //vColor = vec4(color, 1.);
    // vec3 col = compute_color_from_sh(cen.xyz , vec3(cam.x, cam.y, cam.z));
    vec3 col = compute_color_from_sh(vec3(x,y,z), vec3(cam.x, cam.y, cam.z));

    // vColor = clamp(pos2d.z/pos2d.w, 0.0, 1.0)* vec4(col, a);//* vec4(r,g,b, a);
    vColor = vec4(col, a);//* vec4(r,g,b, a);
    
    vPosition = position.xy;
    
    // not sure about the 2 here but the splats looked small
    gl_Position = vec4(
        vCenter
        + position.x*scaler*2. * majorAxis / viewport
        + position.y*scaler*2. * minorAxis / viewport , 0.0, 1.0);
    
}

