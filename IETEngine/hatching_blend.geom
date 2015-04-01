#version 330 core
layout (triangles_adjacency) in; 
layout(triangle_strip, max_vertices = 3) out;

struct Light {
    vec3 position;
};

struct Triangle
{
	vec3 L;
	vec3 vertices[3];
	vec3 T; 
	vec3 B;
	vec3 N;
	vec3 calculated_normal[3];
};

Triangle triangles[4];
vec3 texture_coordinates[12];
vec3 vertex_light_direction[12];

out vec3 light_direction;

const int c 	= 0;
const int alpha = 1;
const int beta  = 2;
const int gamma = 3;

int triangle_indexes[12];

//this array contains the indexes of the vertices on the farest edges of the whole triangle passed to 
//the geometry shader. -1 means the initial triangle which is set to be the central reference triangle.
int triangleverticesIndexesOnEdge[4];
 
in vec3 out_normal[];

uniform Light light; 
uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position;
uniform mat4 mvp;
  
out vec3 tex_coord_geom[4];


out vec3 normalized_normal;
out vec3 eye_direction;
out vec3 vertex_view_space; 

vec3 getVertexOnEdge(int triangleIndex)
{
	vec3 vertex = gl_in[triangleverticesIndexesOnEdge[triangleIndex]].gl_Position.xyz;
	return vertex;
}

void init()
{
	triangle_indexes[0] = 0;// c
	triangle_indexes[1] = 2;
	triangle_indexes[2] = 4;

	triangle_indexes[3] = 0;// alpha
	triangle_indexes[4] = 4;
	triangle_indexes[5] = 5;

	triangle_indexes[6] = 2;// beta
	triangle_indexes[7] = 3;
	triangle_indexes[8] = 4;
	
	triangle_indexes[9] =  0;// gamma
	triangle_indexes[10] = 1;
	triangle_indexes[11] = 2;
 
	triangleverticesIndexesOnEdge[0] = -1;
 	triangleverticesIndexesOnEdge[1] =  5;
 	triangleverticesIndexesOnEdge[2] =  3;
 	triangleverticesIndexesOnEdge[3] =  1; 

	// Initialize triangles in local/object space!
	for(int i = 0; i < 4; i++)
	{
		 

		vec3 v1 = vec3(gl_in[triangle_indexes[i * 3 + 0]].gl_Position);
		vec3 v2 = vec3(gl_in[triangle_indexes[i * 3 + 1]].gl_Position);
		vec3 v3 = vec3(gl_in[triangle_indexes[i * 3 + 2]].gl_Position);
		vec3 ab = v2 - v1;
		vec3 ac = v3 - v1;
		vec3 N = normalize(cross(ab, ac));

		vec3 G = ( v1 + v2 + v3 ) / 3.0;

    	vec3 L = mat3(inverse(model_matrix)) * light.position - G; // light position in local space
 		
 		float NdotL =  dot( L, N);

 		vec3 B = normalize( L -  N * NdotL ); //plane projection
    
    	vec3 T = cross( B , N); 
 
		triangles[i].N = N;
		triangles[i].T = T;
    	triangles[i].L = L;
		triangles[i].B = B;
		triangles[i].vertices[0] = v1;
		triangles[i].vertices[1] = v2;
		triangles[i].vertices[2] = v3;
    }
}
 

bool isVertexOfCInTriangleT(int triangleIndex, int vertexIndex)
{
 	int offset = triangleIndex * 3;
 	bool isV1Shared = triangle_indexes[offset + 0] == triangle_indexes[vertexIndex];
 	bool isV2Shared = triangle_indexes[offset + 1] == triangle_indexes[vertexIndex];
 	bool isV3Shared = triangle_indexes[offset + 2] == triangle_indexes[vertexIndex];

 	return isV1Shared || isV2Shared || isV3Shared;
} 

void RenderGeometry()
{
	//----------------------
	gl_Position = mvp * gl_in[0].gl_Position;

	tex_coord_geom[0] = texture_coordinates[0];
	tex_coord_geom[1] = texture_coordinates[1];
	tex_coord_geom[2] = texture_coordinates[2];
	tex_coord_geom[3] = texture_coordinates[3]; 
	
	light_direction = normalize(light.position -  vec3(model_matrix * gl_in[0].gl_Position));
	normalized_normal  = normalize(mat3(model_transpose_inverse) * out_normal[0]); 
	eye_direction 	=  normalize(eye_position - vec3(model_matrix * gl_in[0].gl_Position));
	//vertex_view_space = triangles[c].vertices[0];
	//L =  triangles[c].L;
	EmitVertex();

	gl_Position = mvp * gl_in[2].gl_Position; 
	tex_coord_geom[0] = texture_coordinates[4];
	tex_coord_geom[1] = texture_coordinates[5];
	tex_coord_geom[2] = texture_coordinates[6];
	tex_coord_geom[3] = texture_coordinates[7];
 	light_direction = normalize(light.position -  vec3(model_matrix * gl_in[2].gl_Position));
	normalized_normal  = normalize(mat3(model_transpose_inverse) * out_normal[2]); 
	eye_direction 	=  normalize(eye_position - vec3(model_matrix * gl_in[2].gl_Position));
	//vertex_view_space = triangles[c].vertices[1];
	//L =  triangles[c].L;

	EmitVertex();

	gl_Position = mvp * gl_in[4].gl_Position;

	tex_coord_geom[0] = texture_coordinates[8];
	tex_coord_geom[1] = texture_coordinates[9];
	tex_coord_geom[2] = texture_coordinates[10];
	tex_coord_geom[3] = texture_coordinates[11];

	light_direction = normalize(light.position -  vec3(model_matrix * gl_in[4].gl_Position));
	normalized_normal  = normalize(mat3(model_transpose_inverse) * out_normal[4]); 
	eye_direction 	=  normalize(eye_position - vec3(model_matrix * gl_in[4].gl_Position));
	//vertex_view_space = triangles[c].vertices[2];
	//L =  triangles[c].L;

	EmitVertex();

	EndPrimitive();


}

void main()
{
	init();
 

	for(int i = 0; i < 3; i++) //Iteration over vertices of triangle C
	{
		for (int t = 0; t < triangles.length(); t++) //Iteration over every triangles available in this shader stage (4)
		{
			// formula is i * 4 + t --> same formula for y * width + x
			vec3 ViL = triangles[t].L - triangles[c].vertices[i];
			vertex_light_direction[i * 4 + t] = ViL;
			if (t == c) //means triangle t is actually triangle c
			{ 
				texture_coordinates[i * 4 + c].s = dot(triangles[c].vertices[i], triangles[c].T ); 
				texture_coordinates[i * 4 + c].t = dot(triangles[c].vertices[i], triangles[c].B ); 
				texture_coordinates[i * 4 + c].p = 1.0;
			}
			else if(!isVertexOfCInTriangleT(t, i))
			{
				vec3 vertexOnEdge = getVertexOnEdge(t);
				vec3 projectedVi = triangles[c].vertices[i] - vertexOnEdge;
				float distance = dot(projectedVi, triangles[t].N);
				vec3 vip = triangles[c].vertices[i] - distance * triangles[t].N;
				 
				texture_coordinates[i * 4 + t].s = dot(vip, triangles[t].T ); 
				texture_coordinates[i * 4 + t].t = dot(vip, triangles[t].B ); 
				texture_coordinates[i * 4 + t].p = -1.0;
			}
			else
			{ 
				texture_coordinates[i * 4 + t].s = dot(triangles[c].vertices[i], triangles[c].T ); 
				texture_coordinates[i * 4 + t].t = dot(triangles[c].vertices[i], triangles[c].B ); 
				texture_coordinates[i * 4 + t].p = dot(triangles[c].N, triangles[t].N);
			}
		}
	}

	 
	RenderGeometry();

}
