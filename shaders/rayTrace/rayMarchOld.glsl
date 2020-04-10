#version 460
layout (local_size_x = 64, local_size_y = 24) in;

const int MAX_RAY_STEP = 128;

const float fov = 90;
const float PI = 3.1415926535897932384626433832795;

uniform writeonly image2D outTex;

struct Camera {
    vec3 pos;
    vec3 target;
    vec3 up;
};

layout(std430, binding = 0) buffer voxelChunk {
    int chunk[];
};

layout(std430, binding = 1) buffer lightBuf {
    vec3 lights[];
};

uniform float time;
uniform uint frame;

uniform vec3 cameraPos;
uniform vec3 cameraAngles;

uint expandBits(uint x) {
    x = (x | (x << 16)) & uint(0x030000FF);
    x = (x | (x <<  8)) & uint(0x0300F00F);
    x = (x | (x <<  4)) & uint(0x030C30C3);
    x = (x | (x <<  2)) & uint(0x09249249);
    return x;
}

uint morton(uint x, uint y, uint z) {
    uint xx = expandBits(uint(x));
    uint yy = expandBits(uint(y));
    uint zz = expandBits(uint(z));

    return xx | (yy << 1) | (zz << 2);
}

bool getVoxel(ivec3 c) {
    if (c.x < 0 || c.x > 64) return false;
    if (c.y < 0 || c.y > 64) return false;
    if (c.z < 0 || c.z > 64) return false;
    return chunk[morton(c.x, c.y, c.z)] != 0;
}

vec4 traceRay(vec3 origin, vec3 target) {
    vec3 direc = normalize(target - origin);

    ivec3 mapPos = ivec3(floor(origin + 0.));
    vec3 deltaDist = abs(vec3(length(direc)) / direc);
    ivec3 rayStep = ivec3(sign(direc));
    vec3 sideDist = (sign(direc) * (vec3(mapPos) - origin) + (sign(direc) * 0.5) + 0.5) * deltaDist;

    bvec3 mask;

    bool hit = false;
    for (int i = 0; i < MAX_RAY_STEP; i++) {
        if (getVoxel(mapPos)) {
            hit = true;
            break;
        }
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(mask) * rayStep;
    }
    vec4 color = vec4(vec3(0.0), 1.0);
    if (!hit) return color;
    if (mask.x) {
        if (rayStep.x > 0) {
            color = vec4(1.0, 1.0, 0.0, 1.0);
        } else {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }
    if (mask.y) {
        if (rayStep.y > 0) {
            color = vec4(0.0, 1.0, 1.0, 1.0);
        } else {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        }
    }
    if (mask.z) {
        if (rayStep.z > 0) {
            color = vec4(1.0, 0.0, 1.0, 1.0);
        } else {
            color = vec4(0.0, 0.0, 1.0, 1.0);
        }
    }
    return color;
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
    0.0,                                0.0,                                0.0,                                1.0);
}

uint seed;

float Xor32() {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed * 2.3283064365387e-10f;
}

ivec2 toTexCoord(vec2 coord) {
    ivec2 dims = imageSize(outTex);
    return ivec2((coord * vec2(dims) + vec2(dims)) / 2);
}

void main() {
    vec4 pixel = vec4(0.0);
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(outTex);
    uint threadID = (gl_GlobalInvocationID.x * dims.y + gl_GlobalInvocationID.y);
    seed = (threadID + frame * 998979899) * 916686619;

    if (Xor32() > 0.95f) {
        vec2 transformCoord;
        transformCoord = (texCoord * 2.0 - dims) / dims;

        vec3 rayPos = cameraPos;
        vec3 rayTarget = vec3((transformCoord.x) * tan(fov / 2 * PI / 180)  * (float(dims.x) / float(dims.y)), transformCoord.y * tan(fov / 2 * PI / 180), 1.0f) + rayPos;// cameraDir + transformCoord.x * cameraPlaneU + transformCoord.y * cameraPlaneV;


        pixel = traceRay(rayPos, rayTarget);
    }

    if (pixel.w >= 0.95) imageStore(outTex, texCoord, pixel);

}
// pixel = vec4(abs(transformCoord), 0.0, 1.0);