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
    
    vec4 cen = vec4(x,y,z,1)*100.;              // weird scale factor
    vec4 cam = view * vec4(cen.xyz, 1.);
    vec4 pos2d = projection * cam;
    float clip = 1.2 * pos2d.w;
    if (pos2d.z < -clip || pos2d.x < -clip || pos2d.x > clip || pos2d.y < -clip || pos2d.y > clip) {
        gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
        return;
    }
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
    vec2 majorAxis = min(sqrt(2.0 * lambda1), 1024.0) * diagonalVector;
    vec2 minorAxis = min(sqrt(2.0 * lambda2), 1024.0) * vec2(diagonalVector.y, -diagonalVector.x);
    
    vec2 vCenter = vec2(pos2d) / pos2d.w;

    vColor = vec4(color, 1.);
    
    vColor = clamp(pos2d.z/pos2d.w+1.0, 0.0, 1.0) * vec4(r,g,b,a);
    //vec4((cov.w) & 0xffu, (cov.w >> 8) & 0xffu, (cov.w >> 16) & 0xffu, (cov.w >> 24) & 0xffu) / 255.0;
        
    
    vPosition = position.xy;

    // 0.1 is a scale factor I wasn't sure
    
    gl_Position = vec4(
        vCenter
        + position.x*0.1 * majorAxis / viewport
        + position.y*0.1 * minorAxis / viewport, 0.0, 1.0);
    
      
}

