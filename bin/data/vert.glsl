// #version 150

// uniform mat4 modelViewProjectionMatrix;
// in vec4 position;
// in vec4 instanceColor;
// in vec2 texcoord;


// uniform sampler2D tex;
// out vec4 color;
// out vec2 texCoordVarying;

// void main(){
//     texCoordVarying = texcoord;
//     color = instanceColor;
//     gl_Position = modelViewProjectionMatrix * position;

// }

// Adapted from https://github.com/antimatter15/splat
#version 430
precision mediump float;

in vec2 position;
in uint depth_index;

struct Splat {
  vec3 center;
  float alpha;
  vec3 covA;
  vec3 covB;
  vec3 sh[16];
};

layout(std430, binding=2) readonly buffer splat_buffer {
  Splat splats[];
};

uniform mat4 projection, view;
uniform vec2 focal;
uniform vec2 viewport;
uniform vec3 cam_pos;
uniform int sh_degree;

out vec4 vColor;
out vec2 vPosition;

mat3 transpose(mat3 m) {
  return mat3(
      m[0][0], m[1][0], m[2][0],
      m[0][1], m[1][1], m[2][1],
      m[0][2], m[1][2], m[2][2]
  );
}


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

vec3 get_rgb(vec3 d) {
    vec3 rgb = vec3(0.5);

    const Splat s = splats[depth_index];

    rgb += SH_C0 * s.sh[0];

    if (sh_degree >= 1) {
        rgb +=
            - SH_C1 * d.y * s.sh[1]
            + SH_C1 * d.z * s.sh[2]
            - SH_C1 * d.x * s.sh[3];
    }

    if (sh_degree >= 2) {
        float xx = d.x * d.x;
        float yy = d.y * d.y;
        float zz = d.z * d.z;
        float xy = d.x * d.y;
        float yz = d.y * d.z;
        float xz = d.x * d.z;
        rgb +=
            SH_C2[0] * xy * s.sh[4] +
            SH_C2[1] * yz * s.sh[5] +
            SH_C2[2] * (2.0 * zz - xx - yy) * s.sh[6] +
            SH_C2[3] * xz * s.sh[7] +
            SH_C2[4] * (xx - yy) * s.sh[8];

        if (sh_degree >= 3) {
            rgb +=
                SH_C3[0] * d.y * (3.0 * xx - yy) * s.sh[9] +
                SH_C3[1] * d.z * xy * s.sh[10] +
                SH_C3[2] * d.y * (4.0 * zz - xx - yy) * s.sh[11] +
                SH_C3[3] * d.z * (2.0 * zz - 3.0 * xx - 3.0 * yy) * s.sh[12] +
                SH_C3[4] * d.x * (4.0 * zz - xx - yy) * s.sh[13] +
                SH_C3[5] * d.z * (xx - yy) * s.sh[14] +
                SH_C3[6] * d.x * (xx - 3.0 * yy) * s.sh[15];
        }
    }

    return clamp(rgb, 0.0, 1.0);
}

void main () {
  const Splat s = splats[depth_index];
  vec4 camspace = view * vec4(s.center, 1);
  vec4 pos2d = projection * camspace;

  float bounds = 1.2 * pos2d.w;
  if (pos2d.z < -pos2d.w
      || pos2d.x < -bounds
      || pos2d.x > bounds
      || pos2d.y < -bounds
      || pos2d.y > bounds) {
      gl_Position = vec4(0.0, 0.0, 2.0, 1.0);
      return;
  }

  mat3 Vrk = mat3(
      s.covA.x, s.covA.y, s.covA.z,
      s.covA.y, s.covB.x, s.covB.y,
      s.covA.z, s.covB.y, s.covB.z
  );

  mat3 J = mat3(
      focal.x / camspace.z, 0., -(focal.x * camspace.x) / (camspace.z * camspace.z),
      0., -focal.y / camspace.z, (focal.y * camspace.y) / (camspace.z * camspace.z),
      0., 0., 0.
  );

  mat3 W = transpose(mat3(view));
  mat3 T = W * J;
  mat3 cov = transpose(T) * Vrk * T;

  vec2 vCenter = vec2(pos2d) / pos2d.w;

  float diagonal1 = cov[0][0] + 0.3;
  float offDiagonal = cov[0][1];
  float diagonal2 = cov[1][1] + 0.3;

  float mid = 0.5 * (diagonal1 + diagonal2);
  float radius = length(vec2((diagonal1 - diagonal2) / 2.0, offDiagonal));
  float lambda1 = mid + radius;
  float lambda2 = max(mid - radius, 0.1);
  vec2 diagonalVector = normalize(vec2(offDiagonal, lambda1 - diagonal1));
  vec2 v1 = min(sqrt(2.0 * lambda1), 1024.0) * diagonalVector;
  vec2 v2 = min(sqrt(2.0 * lambda2), 1024.0) * vec2(diagonalVector.y, -diagonalVector.x);

  vec3 ray_direction = normalize(s.center - cam_pos);
  vColor.rgb = get_rgb(ray_direction);
  vColor.a = s.alpha;
  vPosition = position;

  gl_Position = vec4(
      vCenter
          + position.x * v1 / viewport * 2.0
          + position.y * v2 / viewport * 2.0, 0.0, 1.0);

}