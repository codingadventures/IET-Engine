#ifndef Mesh_h__
#define Mesh_h__


// Std. Includes
#include <sstream>
#include <vector>
#include "Vertex.h"


// GL Includes
#ifndef NO_OPENGL
#include <GL/glew.h> // Contains all the necessary OpenGL includes
#include "Shader.h"
#include "Texture.h"

#include "BoundingBox.h" 
#include "BoundingSphere.h"
#include "Material.h"
#include "Cube.h"
#endif // !NO_OPENGL

#include "glm/gtc/constants.hpp"
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>

#include <glm/detail/type_vec3.hpp>
#include "FEMMesh.h"
#include "octree.h" 



namespace Rendering
{

	using namespace std;
#ifndef NO_OPENGL
	using namespace Physics;
#else
	//typedef unsigned int GLuint;
#endif // !NO_OPENGL


	class Mesh {
	public:
		/*  Mesh Data  */
		//vector<Vertex>				m_vertices;
		TVecCoord					m_vertices;
		TVecCoord					m_normals;
		vector<glm::vec2>			m_texCoords;
		vector<GLuint>				m_indices;
		vector<GLuint>				m_adjacent_indices;
		#ifndef NO_OPENGL
		vector<Texture>				m_textures; 
		vector<VertexWeight>		m_boneWeights;
		#endif // !NO_OPENGL
		glm::vec3					m_center_of_mass; 
		glm::vec3					m_polyhedral_center_of_mass;

	private:						
		//BoundingBox					d_bounding_box;
		//BoundingSphere				d_bounding_sphere;

		/*  Render data  */			
#ifndef NO_OPENGL

		GLuint						d_VAO;
		GLuint						d_VBO;
		GLuint						d_VBO_textures;
		GLuint						d_VBO_normals;
		GLuint						d_EBO;
		GLuint						d_bone_VBO;
		map<string, Bone>			d_bone_mapping;
		Material					d_material;
#endif
		MyVector(TTetra)			d_map_i;
		MyVector(TCoord4)			d_map_f;
		float						d_area;
		
	public:
		/*  Functions  */
		// Constructor
#ifndef NO_OPENGL
		Mesh(TVecCoord vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexWeight> boneWeights, vector<GLuint> adjacent_indices, Material material, vector<glm::vec2> textCoords, TVecCoord normals);
		void Draw(Shader& shader, bool withAdjecencies = false);
		float Area() const { return d_area; } 
#else
		Mesh(TVecCoord vertices, vector<GLuint> indices);
		
#endif // !NO_OPENGL
		 
		void updatePositions(FEMMesh* inputMesh);
		void updateNormals(FEMMesh*);
		void init(FEMMesh* inputMesh);
		// Render the mesh
		

		
		/*	BoundingBox  const& Bounding_box() const { 
		return d_bounding_box; 
		} 
		BoundingSphere const&   Bounding_sphere() const {
		return  d_bounding_sphere; 
		} 
		*/
	private:
		
		#ifndef NO_OPENGL
bool hasBones() const{
			return m_boneWeights.size() >0;
		}
		/*  Functions    */
		// Initializes all the buffer objects/arrays
		void setupMesh();
#endif

		
		// Calculation of the center of mass based on paul bourke's website
		// http://paulbourke.net/geometry/polygonmesh/
		void calculateCenterOfMass();

		void calculateArea();

		void calculateBoundingBox();

		void calculateTexCoord();
	};
#ifndef NO_OPENGL

	inline Mesh::Mesh(TVecCoord vertices, vector<GLuint> indices, vector<Texture> textures, vector<VertexWeight> boneWeights, vector<GLuint> adjacent_indices, Material material, vector<glm::vec2> textCoords, TVecCoord normals): 
	m_adjacent_indices(adjacent_indices),
		m_normals(normals),
		//d_bounding_box(BoundingBox(nullptr)),
		//d_bounding_sphere(BoundingSphere(NULL)),
		d_material(material),
		d_area(0.0f)
	{ 
		this->m_vertices = vertices;
		this->m_indices = indices;
		this->m_textures = textures; 
		this->m_boneWeights = boneWeights; 
		this->m_texCoords = textCoords;
		//this->calculate_center_of_mass();
		//this->calculateArea();
		//this->calculateBoundingBox(); 
		//this->calculate_tex_coord();
		this->setupMesh();

	}
#else
	inline Mesh::Mesh(TVecCoord vertices, vector<unsigned int> indices){
		this->m_vertices = vertices;
		this->m_indices = indices;
	}
#endif

	void Mesh::init(FEMMesh* inputMesh)
	{ 
		if (inputMesh)
		{
			std::cout << "Creating mapping between simulation mesh \"" << inputMesh->filename << "\" and surface mesh \" "  << std::endl;
			static std::string input_filename;
			static sofa::helper::vector<Mat3x3d> bases;
			static sofa::helper::vector<Vec3d> centers;
			static Octree<Vec3d> octree;
			const TVecTetra& tetras = inputMesh->tetrahedra;
			const TVecCoord& in = inputMesh->positions;
			const TVecCoord& out = m_vertices;
			d_map_i.resize(out.size());
			d_map_f.resize(out.size());
			if (input_filename != inputMesh->filename || bases.size() != tetras.size()) // we have to recompute the octree and bases
			{
				input_filename = inputMesh->filename;
				sofa::helper::vector< BBox<Vec3d> > bbox;
				bases.resize(tetras.size());
				centers.resize(tetras.size());
				bbox.resize(tetras.size());
				std::cout << "  Preparing tetrahedra" << std::endl;
				for (unsigned int t=0; t<tetras.size(); ++t)
				{
					Mat3x3d m, mt;
					m[0] = in[tetras[t][1]]-in[tetras[t][0]];
					m[1] = in[tetras[t][2]]-in[tetras[t][0]];
					m[2] = in[tetras[t][3]]-in[tetras[t][0]];
					mt.transpose(m);
					bases[t].invert(mt);
					centers[t] = (in[tetras[t][0]]+in[tetras[t][1]]+in[tetras[t][2]]+in[tetras[t][3]])*0.25;
					bbox[t].add(tetras[t].begin(), tetras[t].end(), in);
				}
				std::cout << "  Building octree" << std::endl;
				octree.init(bbox,8,8);
			}
			std::cout << "  Processing vertices" << std::endl;
			int outside = 0;
			sofa::helper::vector<Octree<Vec3d>*> cells;
			for (unsigned int i=0;i<out.size();i++)
			{
				Vec3d pos = out[i];
				Vec3d coefs;
				int index = -1;
				double distance = 1e10;
				Octree<Vec3d>* cell = octree.findNear(pos);
				if (cell)
				{
					const sofa::helper::vector<int>& elems = cell->elems();
					for (unsigned int e = 0; e < elems.size(); e++)
					{
						unsigned int t = elems[e];
						Vec3d v = bases[t] * (pos - in[tetras[t][0]]);
						double d = std::max(std::max(-v[0],-v[1]),std::max(-v[2],v[0]+v[1]+v[2]-1));
						if (d>0) d = (pos-centers[t]).norm2();
						if (d<distance) { coefs = v; distance = d; index = t; }
					}
				}
				if (distance > 0)
				{ // pos is outside of the fem mesh, find the nearest tetra

					// first let's find at least one tetra that is close, if not already found
					if (index >= 0) // we already have a close tetra, we need to look only for closer ones
					{
						cells.clear();
						octree.findAllAround(cells, pos, sqrt(distance)*1.5);
						for (unsigned int ci = 0; ci < cells.size(); ++ci)
						{
							if (cells[ci] == cell) continue; // already processed this cell
							const sofa::helper::vector<int>& elems = cells[ci]->elems();
							for (unsigned int e = 0; e < elems.size(); e++)
							{
								unsigned int t = elems[e];
								double d = (pos-centers[t]).norm2();
								if (d<distance)
								{
									coefs = bases[t] * (pos - in[tetras[t][0]]);
									distance = d; index = t;
								}
							}
						}
					}
					else
					{
						// failsafe case (should not happen...), to be sure we do a brute-force search
						for (unsigned int t = 0; t < tetras.size(); t++)
						{
							double d = (pos-centers[t]).norm2();
							if (d<distance)
							{
								coefs = bases[t] * (pos - in[tetras[t][0]]);
								distance = d; index = t;
							}
						}
					}
					if (index >= 0)
					{
						//if (verbose >= 1) std::cout << "Surface vertex " << i << " mapped outside of tetra " << index << " with coefs " << coefs << std::endl;
						++outside;
					}
				}
				if (index >= 0)
				{
					//std::cout << "Surface vertex " << i << " mapped from tetra " << index << " with coefs " << coefs << std::endl;
					d_map_i[i][0] = tetras[index][0]; d_map_f[i][0] = (float)(1-coefs[0]-coefs[1]-coefs[2]);
					d_map_i[i][1] = tetras[index][1]; d_map_f[i][1] = (float)(coefs[0]);
					d_map_i[i][2] = tetras[index][2]; d_map_f[i][2] = (float)(coefs[1]);
					d_map_i[i][3] = tetras[index][3]; d_map_f[i][3] = (float)(coefs[2]);
				}
			}
			std::cout << "Mapping done: " << outside << " / " << out.size() << " vertices outside of simulation mesh" << std::endl;
		}
	}
#ifndef NO_OPENGL
	inline void Mesh::Draw(Shader& shader, bool withAdjecencies)
	{


		shader.Use();
		if ( this->m_textures.size()>0)
		{
			for(GLuint i = 0; i < this->m_textures.size(); i++)
			{
				GLuint textureType = GL_TEXTURE_2D;
				glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
				// Retrieve texture number (the N in diffuse_textureN)
				stringstream ss;
				string number;
				auto uniform_name = m_textures[i].Get_Uniform_Name("1");   
				//if(name == "material.texture_diffuse")
				//	ss << diffuseNr++; // Transfer GLuint to stream
				//else if(name == "material.texture_specular")
				//	ss << specularNr++; // Transfer GLuint to stream
				//number = ss.str(); 
				// Now set the sampler to the correct texture unit
				GLuint shader_location = glGetUniformLocation(shader.m_program,  uniform_name.c_str());
				glUniform1i(shader_location, i);
				// And finally bind the texture
				glBindTexture(textureType, this->m_textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.
		}

		d_material.SetShader(shader);

		// Draw mesh
		glBindVertexArray(this->d_VAO);

		GLuint drawMode = withAdjecencies ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		GLuint indices_size = withAdjecencies ? m_adjacent_indices.size() : m_indices.size();
		/*	if (withAdjecencies)
		{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->d_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_adjacent_indices.size() * sizeof(GLuint), &this->m_adjacent_indices[0], GL_STATIC_DRAW);

		}*/
		glDrawElements(drawMode, indices_size, GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);
 
	}

	inline void Mesh::setupMesh()
	{
		 
// Create buffers/arrays
		glGenVertexArrays(1, &this->d_VAO);

		glGenBuffers(1, &this->d_VBO);
		glGenBuffers(1, &this->d_EBO);
		glGenBuffers(1,&this->d_VBO_textures);
		glGenBuffers(1,&this->d_VBO_normals);

		glBindVertexArray(this->d_VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO);
		// A great thing about struct is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(TCoord), &this->m_vertices[0], GL_STREAM_DRAW); 



		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->d_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(GLuint), &this->m_indices[0], GL_STATIC_DRAW);



		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TCoord), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);	
		glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO_normals);
		glBufferData(GL_ARRAY_BUFFER, this->m_normals.size() * sizeof(TCoord), &this->m_normals[0], GL_STATIC_DRAW); 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,(GLvoid*)0);
		// Vertex Texture Coordinates
		glEnableVertexAttribArray(2);	
		glBindBuffer(GL_ARRAY_BUFFER, this->d_VBO_textures);
		glBufferData(GL_ARRAY_BUFFER, this->m_texCoords.size() * sizeof(glm::vec2), &this->m_texCoords[0], GL_STATIC_DRAW); 
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);


		/*glEnableVertexAttribArray(3);	
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));*/

		//
		glEnableVertexAttribArray(4);	
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

		if (hasBones())
		{
			glGenBuffers(1, &this->d_bone_VBO);


			glBindBuffer(GL_ARRAY_BUFFER, d_bone_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_boneWeights[0]) * m_boneWeights.size(), &m_boneWeights[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexWeight), (const GLvoid*)0);

			glEnableVertexAttribArray(6);    
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexWeight), (const GLvoid*)offsetof(VertexWeight, Weights)); 
		}



		glBindVertexArray(0);
	}

#endif
 

	void Mesh::updatePositions(FEMMesh* inputMesh)
	{
		const TVecCoord& in = inputMesh->positions;
		TVecCoord& out = m_vertices;
		if (d_map_f.size() != out.size() || d_map_i.size() != out.size()) return;

		DEVICE_METHOD(TetraMapper3f_apply)( out.size(), d_map_i.deviceRead(), d_map_f.deviceRead(), out.deviceWrite(), in.deviceRead() );

		const void * pointer = NULL;

		pointer = this->m_vertices.hostRead();
#ifndef NO_OPENGL


		glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
		glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(TCoord), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, this->m_vertices.size() * sizeof(TCoord), pointer );
#endif

		/*for (unsigned int i=0;i<out.size();++i)
		{
		out[i] = 
		in[map_i[i][0]] * map_f[i][0] +
		in[map_i[i][1]] * map_f[i][1] +
		in[map_i[i][2]] * map_f[i][2] +
		in[map_i[i][3]] * map_f[i][3];
		}*/
	}
	 
	//
	void Mesh::updateNormals(FEMMesh* inputMesh)
	{
	//	bool computeTangents = false;
	//    m_normals.recreate(inputMesh->positions.size());
	// /*   if (computeTangents)
	//        tangents.recreate(positions.size());*/
	//#ifdef SOFA_DEVICE_CUDA
	//    if (!inputMesh->velems.empty())
	//    { // use GPU
	//        if (!computeTangents)
	//        {
	//            fnormals.recreate(inputMesh->triangles.size());
	//            CudaVisualModel3f_calcTNormals(inputMesh->triangles.size(), inputMesh->positions.size(), inputMesh->triangles.deviceRead(), fnormals.deviceWrite(), inputMesh->positions.deviceRead());
	//            CudaVisualModel3f_calcVNormals(inputMesh->triangles.size(), inputMesh->positions.size(), nbElemPerVertex, velems.deviceRead(), normals.deviceWrite(), fnormals.deviceRead(), positions.deviceRead());
	//        }
	//        else
	//        {
	//            fnormals.recreate(inputMesh->triangles.size());
	//            ftangents.recreate(inputMesh->triangles.size());
	//            CudaVisualModel3f_calcTNormalsAndTangents(triangles.size(), positions.size(), triangles.deviceRead(), fnormals.deviceWrite(), ftangents.deviceWrite(), positions.deviceRead(), texcoords.deviceRead());
	//            CudaVisualModel3f_calcVNormalsAndTangents(triangles.size(), positions.size(), nbElemPerVertex, velems.deviceRead(), normals.deviceWrite(), tangents.deviceWrite(), fnormals.deviceRead(), ftangents.deviceRead(), positions.deviceRead(), texcoords.deviceRead());
	//        }
	//    }
	//    else
	//#endif
	//    { // use CPU
	//        if (!computeTangents)
	//        {
	//            for (unsigned int i=0;i<m_normals.size();++i)
	//                m_normals[i].clear();
	//            for (unsigned int i=0;i<inputMesh->triangles.size();++i)
	//            {
	//                TCoord n = cross(inputMesh->positions[inputMesh->triangles[i][1]]-inputMesh->positions[inputMesh->triangles[i][0]], 
	//                                 inputMesh->positions[inputMesh->triangles[i][2]]-inputMesh->positions[inputMesh->triangles[i][0]]);
	//                n.normalize();
	//                for (unsigned int j=0;j<3;++j)
	//                    m_normals[inputMesh->triangles[i][j]] += n;
	//            }
	//            for (unsigned int i=0;i<m_normals.size();++i)
	//                m_normals[i].normalize();
	//        }
	//        else
	//        {
	//            for (unsigned int i=0;i<m_normals.size();++i)
	//            {
	//                m_normals[i].clear();
	//               // tangents[i].clear();
	//            }
	//            for (unsigned int i=0;i<inputMesh->triangles.size();++i)
	//            {
	//                TCoord A = inputMesh->positions[inputMesh->triangles[i][0]];
	//                TCoord B = inputMesh->positions[inputMesh->triangles[i][1]];
	//                TCoord C = inputMesh->positions[inputMesh->triangles[i][2]];
	//                B -= A;
	//                C -= A;
	//                TCoord n = cross(B,C);
	//                n.normalize();
	//                TReal Au = m_texCoords[inputMesh->triangles[i][0]][0];
	//                TReal Bu = m_texCoords[inputMesh->triangles[i][1]][0];
	//                TReal Cu = m_texCoords[inputMesh->triangles[i][2]][0];
	//                Bu -= Au;
	//                Cu -= Au;
	//                TCoord t = B * Cu - C * Bu;
	//                t.normalize();
	//                for (unsigned int j=0;j<3;++j)
	//                {
	//                    m_normals[inputMesh->triangles[i][j]] += n;
	//                   // tangents[inputMesh->triangles[i][j]] += t;
	//                }
	//            }
	//            for (unsigned int i=0;i<m_normals.size();++i)
	//            {
	//                //tangents[i] = cross(normals[i],tangents[i]);
	//                m_normals[i].normalize();
	//               // tangents[i].normalize();
	//            }
	//        }
	//    }
	}

	//inline void Mesh::calculateCenterOfMass()
	//{
	//}

	//inline void Mesh::calculateArea()
	//{	 
	//	size_t N = m_vertices.size();
	//	if (N % 3 != 0) return;

	//	for (int i = 0; i < N; i = i + 3)
	//	{
	//		glm::vec3 v1 = m_vertices[i].Position;
	//		glm::vec3 v2 = m_vertices[i+1].Position;
	//		glm::vec3 v3 = m_vertices[i+2].Position;
	//		d_area += glm::length(glm::cross(v2 - v1,v3 - v1)) * 0.5f;
	//	}
	//}

	//inline void Mesh::calculateBoundingBox()
	//{
	//	d_bounding_box = BoundingBox(m_vertices);

	//}

	//inline void Mesh::calculateTexCoord()
	//{
	//	size_t N = m_vertices.size();
	//	if (N % 3 != 0) return;

	//	for (int i = 0; i < N; i = i + 3)
	//	{
	//		auto v1 = m_vertices[i].Position;
	//		auto v2 = m_vertices[i+1].Position;
	//		auto v3 = m_vertices[i+2].Position;

	//		auto mod1 = fmod( atan2( v1.z, v1.x )  +  glm::pi<float>() , glm::pi<float>());
	//		auto mod2 = fmod( atan2( v2.z, v2.x )  +  glm::pi<float>() , glm::pi<float>());
	//		auto mod3 = fmod( atan2( v3.z, v3.x )  +  glm::pi<float>() , glm::pi<float>());

	//		m_vertices[i].TexCoords		= glm::vec2(1.0f - ( mod1 / glm::pi<float>() * 0.5f ), 0.5f - v1.y / 2.0f );
	//		m_vertices[i+1].TexCoords	= glm::vec2( 1.0f - ( mod2  / glm::pi<float>() * 0.5 ), 0.5 - v2.y / 2.0f );
	//		m_vertices[i+2]	.TexCoords	=  glm::vec2( 1.0f - ( mod3 / glm::pi<float>() * 0.5 ), 0.5 - v3.y / 2.0f );
	//	}
	//}
}

#endif // Mesh_h__

