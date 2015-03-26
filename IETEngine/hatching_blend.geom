#version 440 core
layout (triangles_adjacency) in; 
layout(triangle_strip, max_vertices = 12) out;

struct Light {
    vec3 position;
};

struct Triangle
{
	vec3 L;
	vec3 vertices[3];
	vec3 T;
	vec3 G;
	vec3 B;
	vec3 N;
};

Triangle triangles[4];
vec3 texture_coordinates[12];

out vec3 light_direction;

const int c 	= 0;
const int alpha = 1;
const int beta  = 2;
const int gamma = 3;

 
in vec3 out_normal[];

uniform Light light; 
uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position;
uniform mat4 mvp;
  
out vec2 tex_coord_geom;


out vec3 normalized_normal;
out vec3 eye_direction;
out vec3 vertex_view_space;


void init()
{
	triangles[c].vertices[0] = gl_in[0].gl_Position.xyz;
	triangles[c].vertices[1] = gl_in[2].gl_Position.xyz;
	triangles[c].vertices[2] = gl_in[4].gl_Position.xyz;

	triangles[alpha].vertices[0] = gl_in[0].gl_Position.xyz;
	triangles[alpha].vertices[1] = gl_in[4].gl_Position.xyz;
	triangles[alpha].vertices[2] = gl_in[5].gl_Position.xyz;

	triangles[beta].vertices[0] = gl_in[2].gl_Position.xyz;
	triangles[beta].vertices[1] = gl_in[3].gl_Position.xyz;
	triangles[beta].vertices[2] = gl_in[4].gl_Position.xyz;

	triangles[gamma].vertices[0] = gl_in[0].gl_Position.xyz;
	triangles[gamma].vertices[1] = gl_in[1].gl_Position.xyz;
	triangles[gamma].vertices[2] = gl_in[2].gl_Position.xyz;
}

void calculateTB(int triangleIndex)
{
	triangles[triangleIndex].B = normalize(triangles[triangleIndex].L - triangles[triangleIndex].N * dot(triangles[triangleIndex].L, triangles[triangleIndex].N) );
    
    triangles[triangleIndex].T  = cross(triangles[triangleIndex].B , triangles[triangleIndex].N );

}

bool isVertexInTriangle(int triangleIndex, vec3 vertex)
{
	vec3 vertices[3] = triangles[triangleIndex].vertices;

	return vertices[0] == vertex || vertices[1] == vertex || vertices[2] == vertex;
}

void calculateNormal(int triangleIndex)
{
	vec3 v1 = triangles[triangleIndex].vertices[0];
	vec3 v2 = triangles[triangleIndex].vertices[1];
	vec3 v3 = triangles[triangleIndex].vertices[2];

	vec3 ab = v2 - v1;
	vec3 ac = v3 - v1;
	vec3 N = normalize(cross(ab, ac));

	triangles[triangleIndex].N = N;
}

void calculateLG(int triangleIndex)
{
	vec3 v1 = triangles[triangleIndex].vertices[0];
	vec3 v2 = triangles[triangleIndex].vertices[1];
	vec3 v3 = triangles[triangleIndex].vertices[2];

    vec3 G = ( v1 + v2 + v3 ) / 3.0;

    vec3 L = light.position - G;

    triangles[triangleIndex].G = G;
    triangles[triangleIndex].L = L;
}

bool areTrianglesEqual(int triangleIndex1,int triangleIndex2)
{
	vec3 vertices1[3] = triangles[triangleIndex1].vertices;
	vec3 vertices2[3] = triangles[triangleIndex2].vertices;
	
	return vertices1[0] == vertices2[0] && vertices1[1] == vertices2[1] && vertices1[2] == vertices2[2];
}

void main()
{
	init();

	for(int i = 0; i < triangles.length(); i++)
	{
		calculateNormal(i);
		calculateLG(i);
	}

	for(int i = 0; i < 3; i++) //Iteration over vertices of triangle C
	{
		for (int t = 0; t < triangles.length(); t++)
		{
			// formula is i * 4 + t --> same formula for y * width + x
			vec3 ViL = triangles[t].L - triangles[c].vertices[i];
			//light_direction[i * 4 + t] = ViL;
			if (areTrianglesEqual(t,c))
			{
				calculateTB(c);
				texture_coordinates[i * 4 + c].s = dot(triangles[c].vertices[i], triangles[c].T ); 
				texture_coordinates[i * 4 + c].t = dot(triangles[c].vertices[i], triangles[c].B ); 
				texture_coordinates[i * 4 + c].p = 1.0;
			}
			else if(isVertexInTriangle(t,triangles[c].vertices[i]))
			{
				vec3 projectedVi = triangles[c].vertices[i] - triangles[t].vertices[0];
				float distance = dot(projectedVi, triangles[t].N);
				vec3 vip = triangles[c].vertices[i] - distance * triangles[t].N;
				calculateTB(t);
				texture_coordinates[i * 4 + t].s = dot(vip, triangles[t].T ); 
				texture_coordinates[i * 4 + t].t = dot(vip, triangles[t].B ); 
				texture_coordinates[i * 4 + t].p = -1.0;
			}
			else
			{
				calculateTB(t);
				texture_coordinates[i * 4 + t].s = dot(triangles[t].vertices[i], triangles[t].T ); 
				texture_coordinates[i * 4 + t].t = dot(triangles[t].vertices[i], triangles[t].B ); 
				texture_coordinates[i * 4 + t].p = dot(triangles[c].N, triangles[t].N);
			}
		}
	}

	for(int i = 0; i < 3; i++)
	{
	
		tex_coord_geom = 
			texture_coordinates[i * 4].st * texture_coordinates[i * 4].p + 
			texture_coordinates[i * 4 + 1].st * texture_coordinates[i * 4 + 1].p + 
			texture_coordinates[i * 4 + 2].st * texture_coordinates[i * 4 + 2].p + 
			texture_coordinates[i * 4 + 3].st * texture_coordinates[i * 4 + 3].p;

		gl_Position = mvp * gl_in[i].gl_Position;
   	 
  		light_direction = normalize(light.position - triangles[c].vertices[i]);
  		normalized_normal = normalize(mat3(model_transpose_inverse) * out_normal[i]);
  		eye_direction 	=  normalize(eye_position - triangles[c].vertices[i]);
    	vertex_view_space = triangles[c].vertices[i];
    	EmitVertex();	
	}	

	for (int i = 1; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			gl_Position = mvp * vec4(triangles[i].vertices[j],1.0);
			tex_coord_geom = 
			texture_coordinates[j * 4 + i].st * texture_coordinates[j * 4 + i].p;

			light_direction = normalize(light.position - triangles[i].vertices[j]);
	  		//normalized_normal = normalize(mat3(model_transpose_inverse) * out_normal[i]);
	  		eye_direction 	=  normalize(eye_position - triangles[i].vertices[j]);
	    	vertex_view_space = triangles[i].vertices[j];
			EmitVertex();
		}
	}

	EndPrimitive();
}
