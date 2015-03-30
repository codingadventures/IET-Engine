#version 440 core
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
	vec3 G;
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
out vec3 L;

void init()
{
	triangles[c].vertices[0] = vec3( model_matrix * gl_in[0].gl_Position);
	triangles[c].vertices[1] = vec3( model_matrix * gl_in[2].gl_Position);
	triangles[c].vertices[2] = vec3( model_matrix * gl_in[4].gl_Position);
	triangles[c].calculated_normal[0] =  normalize(mat3(model_transpose_inverse) * out_normal[0]) ;
	triangles[c].calculated_normal[1] =  normalize(mat3(model_transpose_inverse) * out_normal[2]) ;
	triangles[c].calculated_normal[2] =  normalize(mat3(model_transpose_inverse) * out_normal[4]) ;


	triangles[alpha].vertices[0] = vec3( model_matrix * gl_in[0].gl_Position);
	triangles[alpha].vertices[1] = vec3( model_matrix * gl_in[4].gl_Position);
	triangles[alpha].vertices[2] = vec3( model_matrix * gl_in[5].gl_Position);
	triangles[alpha].calculated_normal[0] =  normalize(mat3(model_transpose_inverse) * out_normal[0]) ;
	triangles[alpha].calculated_normal[1] =  normalize(mat3(model_transpose_inverse) * out_normal[4]) ;
	triangles[alpha].calculated_normal[2] =  normalize(mat3(model_transpose_inverse) * out_normal[5]) ;


	triangles[beta].vertices[0] =  vec3( model_matrix * gl_in[2].gl_Position);
	triangles[beta].vertices[1] =  vec3( model_matrix * gl_in[3].gl_Position);
	triangles[beta].vertices[2] =  vec3( model_matrix * gl_in[4].gl_Position);
	triangles[beta].calculated_normal[0] =  normalize(mat3(model_transpose_inverse) * out_normal[2]) ;
	triangles[beta].calculated_normal[1] =  normalize(mat3(model_transpose_inverse) * out_normal[3]) ;
	triangles[beta].calculated_normal[2] =  normalize(mat3(model_transpose_inverse) * out_normal[4]) ;


	triangles[gamma].vertices[0] = vec3( model_matrix * gl_in[0].gl_Position);
	triangles[gamma].vertices[1] = vec3( model_matrix * gl_in[1].gl_Position);
	triangles[gamma].vertices[2] = vec3( model_matrix * gl_in[2].gl_Position);
	triangles[gamma].calculated_normal[0] =  normalize(mat3(model_transpose_inverse) * out_normal[0]) ;
	triangles[gamma].calculated_normal[1] =  normalize(mat3(model_transpose_inverse) * out_normal[1]) ;
	triangles[gamma].calculated_normal[2] =  normalize(mat3(model_transpose_inverse) * out_normal[2]) ;
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

void RenderGeometry()
{
	//----------------------
	gl_Position = mvp * gl_in[0].gl_Position;
	tex_coord_geom[0] = texture_coordinates[0];
	tex_coord_geom[1] = texture_coordinates[1];
	tex_coord_geom[2] = texture_coordinates[2];
	tex_coord_geom[3] = texture_coordinates[3]; 
	light_direction = normalize(light.position - triangles[c].vertices[0]);
	normalized_normal  = triangles[c].calculated_normal[0]; 
	eye_direction 	=  normalize(eye_position - triangles[c].vertices[0]);
	vertex_view_space = triangles[c].vertices[0];
	L =  triangles[c].L;
	EmitVertex();

	gl_Position = mvp * gl_in[2].gl_Position; 
	tex_coord_geom[0] = texture_coordinates[4];
	tex_coord_geom[1] = texture_coordinates[5];
	tex_coord_geom[2] = texture_coordinates[6];
	tex_coord_geom[3] = texture_coordinates[7];
 	light_direction = normalize(light.position - triangles[c].vertices[1]);
	normalized_normal  = triangles[c].calculated_normal[0]; 
	eye_direction 	=  normalize(eye_position - triangles[c].vertices[1]);
	vertex_view_space = triangles[c].vertices[1];
	L =  triangles[c].L;

	EmitVertex();

	gl_Position = mvp * gl_in[4].gl_Position; 
	tex_coord_geom[0] = texture_coordinates[8];
	tex_coord_geom[1] = texture_coordinates[9];
	tex_coord_geom[2] = texture_coordinates[10];
	tex_coord_geom[3] = texture_coordinates[11];

	light_direction = normalize(light.position - triangles[c].vertices[2]);
	normalized_normal  = triangles[c].calculated_normal[0]; 
	eye_direction 	=  normalize(eye_position - triangles[c].vertices[2]);
	vertex_view_space = triangles[c].vertices[2];
	L =  triangles[c].L;

	EmitVertex();

	EndPrimitive();


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
			vertex_light_direction[i * 4 + t] = ViL;
			if (areTrianglesEqual(t,c))
			{
				calculateTB(c);
				texture_coordinates[i * 4 + c].s = dot(triangles[c].vertices[i], triangles[c].T ); 
				texture_coordinates[i * 4 + c].t = dot(triangles[c].vertices[i], triangles[c].B ); 
				texture_coordinates[i * 4 + c].p = 1.0;
			}
			else if(!isVertexInTriangle(t,triangles[c].vertices[i]))
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
				calculateTB(c);
				texture_coordinates[i * 4 + t].s = dot(triangles[c].vertices[i], triangles[c].T ); 
				texture_coordinates[i * 4 + t].t = dot(triangles[c].vertices[i], triangles[c].B ); 
				texture_coordinates[i * 4 + t].p = dot(triangles[c].N, triangles[t].N);
			}
		}
	}

	 
	RenderGeometry();

}
