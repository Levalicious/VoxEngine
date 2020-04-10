#version 120

void main() {

    vec3 throughput = vec3(1.f);
    vec3 radiance = vec3(0.f);

    Ray ray;

    ray.origin = cameraPos;
    ray.direction = normalize(vec3((transformCoord.x) * tan(fov / 2 * PI / 180)  * (float(dims.x) / float(dims.y)), transformCoord.y * tan(fov / 2 * PI / 180), 1.0f));
    ray.maxLen = PRIMARY_RAY_LEN;

    vec3 color = vec3(0.f);


    for (uint bounce = 0; bounce < maxBounces; bounce++) {
        uint index = traceRay(ray);
        int blockID = chunk[index];

        if (!ray.hit) {
            break;
        }


        /* Add emmision */
        radiance += blocks[blockID - 1].light.xyz;

        ray.origin = ray.collisionPos + ray.collisionNormal * 0.001;

        ray.direction = normalize(reflect(-ray.direction, ray.collisionNormal));

        /* Multiply throughput by dot product & albedo */
        //throughput *= (1.f / abs(dot(ray.collisionNormal, ray.direction))) * blocks[blockID - 1].color.xyz; // * ((maxBounces - bounce) / maxBounces));

        /* Bounce again */
    }

    /* If I'm going to add shadow rays they'd probably be here */
    color = radiance * throughput;
}
