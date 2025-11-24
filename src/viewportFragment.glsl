#version 460 core
out vec4 fragColor;

uniform int viewportWidth;
uniform int viewportHeight;
uniform int numSamples;
uniform float yaw;
uniform float pitch;
uniform vec3 cameraPosition;

layout(std430, binding=0) buffer accumulationBuffer {
    vec4 accumulation[];
};

const float PI = 3.1415926535;
const float INFINITY = 1.0 / 0.0;
const int diffuseBounces = 4;

float focalLength = 2.0;

struct Ray{
    vec3 origin;
    vec3 direction;
};

struct HitRecord{
    vec3 p;
    vec3 normal;
    float t;
    bool front_face;
    int material;
};

struct Sphere{
    vec3 origin;
    float radius;
    int material;
};

struct Material{
    vec3 albedo;
    vec3 emissionColor;
    float emissionStrength;
};

Sphere worldSpheres[3] = Sphere[3](Sphere(vec3(0.0, -100.5, -1.0), 100.0, 0), 
                                   Sphere(vec3(0.0,  0.0,   -1.0), 0.5,   2),
                                   Sphere(vec3(0.0,  5.0,    0.0), 1.0,   2));

Material materials[3] = Material[3](Material(vec3(1.0, 0.0, 0.0), vec3(0.0), 0.0),
                                    Material(vec3(1.0), vec3(1.0), 20.0),
                                    Material(vec3(1.0, 1.0, 1.0), vec3(0.0), 0.0));

vec3 acesApprox(vec3 v){
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

uvec3 pcg3d(inout uvec3 v){
    v = v * 1664525u + 1013904223u;
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    v = v ^ (v >> 16u);
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    return v;
}

vec3 hash(inout uvec3 v){
    return vec3( pcg3d(v) ) / vec3( float( 0xffffffffU) ); 
}

vec3 hash13(uint n) {
	  n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    uvec3 k = n * uvec3(n,n*16807U,n*48271U);
    return vec3( k & uvec3(0x7fffffffU))/float(0x7fffffff);
}

vec3 randUnitVec(inout uvec3 seed) {
    float t = 2*PI*hash(seed).x;
    float z = 2*hash(seed).y-1;
    float factor = sqrt(1-z*z);
    return vec3(factor*cos(t),factor*sin(t),z);
}

vec3 randomOnHemisphere(vec3 normal, inout uvec3 seed){
    vec3 onUnitSphere = randUnitVec(seed);
    if(dot(onUnitSphere, normal) > 0.0)
        return  onUnitSphere;
    else
        return -onUnitSphere;
}

mat2 rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

void setFaceNormal(inout HitRecord rec, inout Ray ray, inout vec3 outwardNormal){
    bool frontFace = dot(ray.direction, outwardNormal) < 0.0;
    rec.normal = frontFace ? outwardNormal : -outwardNormal;
}

vec3 rayAt(Ray ray, float t){
    return (ray.origin + t*ray.direction);
}

bool sphereHit(Sphere sphere, inout Ray ray, float rayTmin, float rayTmax, inout HitRecord rec){
    vec3 oc = sphere.origin - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float h = dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.radius*sphere.radius;
    
    float discriminant = h*h - a*c;
    if(discriminant < 0.0){
        return false;
    }
    float sqrtD = sqrt(discriminant);
    
    float root = (h - sqrtD) / a;
    if(root <= rayTmin || root >= rayTmax){
        root = (h + sqrtD) / a;
        if(root <= rayTmin || root >= rayTmin){
            return false;
        }
    }

    rec.t = root;
    rec.p = rayAt(ray, rec.t);
    rec.material = sphere.material;
    vec3 outwardNormal = (rec.p - sphere.origin) / sphere.radius;
    setFaceNormal(rec, ray, outwardNormal);

    return true;
}


bool worldHit(Ray ray, float rayTmin, float rayTmax, inout HitRecord rec){
    HitRecord tempRec;
    bool hitAnything = false;
    float closestSoFar = rayTmax;
   
    for(int i = 0; i < 3; i++){
        if(sphereHit(worldSpheres[i], ray, rayTmin, closestSoFar, tempRec)){
            hitAnything = true;
            if(tempRec.t < closestSoFar){  
                closestSoFar = tempRec.t;  
                rec = tempRec;  
            }
        }
    }

    return hitAnything;
}

vec3 rayColor(inout Ray ray, inout uvec3 seed){
    vec3 rayColor = vec3(1.0);
    vec3 brightnessScore = vec3(0.0);
    
    for(int bounce = 0; bounce < diffuseBounces; bounce++){
        HitRecord rec;
        vec3 sunDir = normalize(vec3(0.0, 0.5, 1.0));
        if(worldHit(ray, 0.001, INFINITY, rec)){
            ray.origin = rec.p;
            ray.direction = rec.normal + randUnitVec(seed);
            
            vec3 emittedLight = materials[rec.material].emissionColor * 
                                materials[rec.material].emissionStrength;

            brightnessScore += emittedLight * rayColor;

            brightnessScore /= PI;
            
            rayColor *= materials[rec.material].albedo;

            Ray shadowRay = Ray(rec.p + rec.normal * 0.001, sunDir);
            HitRecord shadowRec;

            if(!worldHit(shadowRay, 0.001, 10000.0, shadowRec)){
                float sunIntensity = max(dot(rec.normal, sunDir), 0.0);
                brightnessScore += vec3(0.8) * sunIntensity * rayColor;
            }

        }
        else{
            vec3 unitDirection = normalize(ray.direction);
            float a = 0.5*(unitDirection.y + 1.0);
            float sun = dot(sunDir, unitDirection) > 0.999 ? 1.0 : 0.0;
            vec3 skyColor = a * vec3(0.5, 0.7, 1.0) + (1.0-a) * vec3(1.0);
            skyColor = mix(skyColor, vec3(1.0), sun);
            brightnessScore += skyColor * rayColor;
            return brightnessScore;
        }
    } 
    return brightnessScore;
}

vec2 sampleSquare(inout uvec3 seed){
    return vec2(hash(seed).yz);
}

Ray getRay(vec2 normalized, float aspectRatio, inout uvec3 seed){
    vec2 offset = sampleSquare(seed);
    
    vec3 rayDirection = vec3(normalized + offset / viewportHeight, focalLength);

    rayDirection.yz *= rot(pitch);
    rayDirection.xz *= rot(yaw + PI/2);
    
    normalize(rayDirection);

    return Ray(cameraPosition, rayDirection);
}

vec3 render(){
    float aspectRatio = float(viewportWidth) / viewportHeight;
    vec2 normalized = ( vec2( gl_FragCoord.x / viewportWidth,
                              gl_FragCoord.y / viewportHeight) * 2.0 - vec2(1.0) );
    normalized.x *= aspectRatio;
    uvec3 seed = uvec4(gl_FragCoord).xyz + uvec3(hash13(numSamples) * 676767);

    vec3 pixelColor = vec3(0.0);
    Ray ray = getRay(normalized, aspectRatio, seed);
    pixelColor += rayColor(ray, seed);
    
    return pixelColor;
}

void main(){
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    int index = pixelCoord.y * viewportWidth + pixelCoord.x;

    vec3 newSample = render();
  
    if(numSamples == 1){
        accumulation[index] = vec4(newSample, 1.0);
    }
    else{
        vec3 oldColor = accumulation[index].rgb;
        vec3 accumulatedColor = (oldColor * float(numSamples - 1) + newSample) / float(numSamples);
        accumulation[index] = vec4(accumulatedColor, 1.0);
    }

    vec3 finalColor = acesApprox(pow(accumulation[index].rgb, vec3(1/2.2)));
    fragColor = vec4(finalColor, 1.0); 
}
