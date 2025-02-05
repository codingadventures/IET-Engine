#version 330

in vec3 position_eye;
in vec3 normal_eye; 

uniform mat4 view; // view matrix

// uniform float ratio;
uniform float ratioR;
uniform float ratioG;
uniform float ratioB;

uniform float reflectFactor;

out vec4 FragColor;

void main () 
{
	float F = ((1.0 - ratioG) * (1.0 - ratioG)) / ((1.0 + ratioG) * (1.0 + ratioG)); 
	/* reflect ray around normal from eye to surface */
	vec3 incident_eye = normalize (position_eye);
	vec3 normal = normalize (normal_eye);

    vec3 refractedVecR = refract(incident_eye, normal, ratioR);
	refractedVecR = vec3 (inverse(view) * vec4(refractedVecR, 0.0));

	vec3 refractedVecG = refract(incident_eye, normal, ratioG);
	refractedVecG = vec3 (inverse(view) * vec4(refractedVecG, 0.0));

	vec3 refractedVecB = refract(incident_eye, normal, ratioB);
	refractedVecB = vec3 (inverse(view) * vec4(refractedVecB, 0.0));

	vec3 refractColor;

	refractColor.r = vec3(texture (cSampler, refractedVecR)).r;
	refractColor.g = vec3(texture (cSampler, refractedVecG)).g;
	refractColor.b = vec3(texture (cSampler, refractedVecB)).b;

	float Ratio = F + (1.0 - F) * pow((1.0 - dot(-incident_eye, normal)), 5);

	vec3 reflected = reflect (incident_eye, normal);
	reflected = vec3 (inverse (view) * vec4 (reflected, 0.0));
    vec3 reflectColor = texture(cSampler, reflected);

    vec3 color = mix(refractColor, reflectColor, Ratio);
	FragColor = vec4(color,1.0);
}
