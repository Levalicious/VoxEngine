#version 460
layout (local_size_x = 32, local_size_y = 32) in;

const float fov = 90;
const float PI = 3.1415926535897932384626433832795;

uniform layout(r32f) image2D outTex;

uniform float time;
uniform uint frame;
uniform vec3 cameraPos;
uniform vec3 cameraOrient;

struct BlockType {
    vec4 color;
    vec4 light;
};

struct Ray {
    vec3 origin;
    vec3 direction;
    float maxLen;
    bool hit;
    vec3 collisionNormal;
    vec3 collisionPos;
};

layout(std430, binding = 0) buffer voxelChunk {
    int chunk[];
};

layout(std430, binding = 1) buffer blockTypes {
    BlockType blocks[];
};

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

uint expandBits(uint x) {
    x = (x | (x << 16)) & uint(0x030000FF);
    x = (x | (x <<  8)) & uint(0x0300F00F);
    x = (x | (x <<  4)) & uint(0x030C30C3);
    x = (x | (x <<  2)) & uint(0x09249249);
    return x;
}

uint morton(uint x, uint y, uint z) {

    uint xx = expandBits(uint(x % 64));
    uint yy = expandBits(uint(y % 64));
    uint zz = expandBits(uint(z % 64));

    return (xx | (yy << 1) | (zz << 2));
}

uint traceRay(inout Ray ray) {
    vec4 color = vec4(vec3(0.0), 1.0);

    ivec3 mapPos = ivec3(floor(ray.origin + 0.f));
    vec3 deltaDist = abs(vec3(length(ray.direction)) / ray.direction);
    ivec3 rayStep = ivec3(sign(ray.direction));
    vec3 sideDist = (sign(ray.direction) * (vec3(mapPos) - ray.origin) + (sign(ray.direction) * 0.5) + 0.5) * deltaDist;

    bvec3 mask;
    uint index;

    while (length(vec3(mapPos) - ray.origin) < ray.maxLen) {
        index = morton(mapPos.x, mapPos.y, mapPos.z);
        if (chunk[index] != 0) {
            ray.hit = true;
            vec3 temp = vec3(mask) * sideDist;
            ray.collisionPos = ray.direction * max(temp.x, max(temp.y, temp.z)) + ray.origin;
            if (mask.x) {
                if (rayStep.x > 0) {
                    ray.collisionNormal = vec3(-1.f, 0.f, 0.f);
                } else {
                    ray.collisionNormal = vec3(1.f, 0.f, 0.f);
                }
            }
            if (mask.y) {
                if (rayStep.y > 0) {
                    ray.collisionNormal = vec3(0.f, -1.f, 0.f);
                } else {
                    ray.collisionNormal = vec3(0.f, 1.f, 0.f);
                }
            }
            if (mask.z) {
                if (rayStep.z > 0) {
                    ray.collisionNormal = vec3(0.f, 0.f, -1.f);
                } else {
                    ray.collisionNormal = vec3(0.f, 0.f, 1.f);
                }
            }
            return index;
        }
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(mask) * rayStep;
    }

    ray.hit = false;
    return index;
}

const float PRIMARY_RAY_LEN = 128.f;

const vec3 backgroundColor = vec3(0.f, 1.f, 0.f);

const uint maxBounces = 16;

vec4 mmix(vec4 a, vec4 b, float c) {
    return a * (1.f - c) + b * c;
}

void main() {
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    vec4 pix = imageLoad(outTex, texCoord);
    pix += vec4(0.5);
    imageStore(outTex, texCoord, pix);
    /*
    ivec2 dims = imageSize(outTex);
    uint threadID = gl_GlobalInvocationID.x * dims.y + gl_GlobalInvocationID.y;
    seed = (threadID + frame * 998979899) * 916686619;

    vec2 transformCoord = vec2(Xor32() * 2.f - 1.f, Xor32() * 2.f - 1.f);


    uvec2 texCoord = toTexCoord(transformCoord);
    */
    //vec4 new_pixel = vec4(color, 1.f);
   // new_pixel = vec4(((imageLoad(outTex, ivec2(texCoord)).xyz * 9.f) + color) / 10.f, 1.f);
    /*
    vec4 new_pixel = imageLoad(outTex, ivec2(texCoord)) + vec4(0.5f);
    imageStore(outTex, ivec2(texCoord), new_pixel);
    */
}


