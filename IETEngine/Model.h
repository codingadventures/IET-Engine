#ifndef Model_h__
#define Model_h__


// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "Skeleton.h"
// GL Includes
#include <GL/glew.h> // Contains all the necessary OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL.h>
#include "Mesh.h"
#include "IAnimation.h"
#include "KeyFrameAnimator.h"
#include "Inertia.h"
#include "AntTweakBar.h"

namespace Rendering
{

	using namespace std;
	using namespace Physics::Tensor;
	 
	class Model 
	{
	public:

		glm::mat4*		m_animation_matrix;
		Skeleton*		m_skeleton;
		glm::vec3		m_Direction;

	private:  
		vector<Mesh>	d_meshes;

		vector<Texture> d_textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

		string			d_directory;

		GLuint*			d_bone_location;
		int				d_numberOfBone;

		glm::mat4		d_model_matrix;
		glm::mat4		d_scale;
		glm::mat4		d_position;


		glm::quat		d_rotation;



	public:
		/*  Functions   */
		// Constructor, expects a filepath to a 3D model.
		Model(GLchar* path) :
			d_numberOfBone(0)
		{ 
			assert(path);

			m_skeleton = new Skeleton(); 



			this->loadModel(path); 

			if (m_skeleton->getNumberOfBones()>0)
			{
				m_animation_matrix = (glm::mat4*) malloc(m_skeleton->getNumberOfBones() * sizeof(glm::mat4));

				CleanAnimationMatrix();

				m_skeleton->updateSkeleton();
				m_skeleton->updateAnimationMatrix(m_animation_matrix); 
			}


		} 


		glm::vec3		Position() const { return decomposeT(d_position); } 

		bool			Has_Texture() {return d_textures_loaded.size()>0;}

		~Model()
		{
			delete m_animation_matrix;
			delete d_bone_location;
			delete m_skeleton;

		}
		// Draws the model, and thus all its meshes
		void Draw(Shader& shader)
		{
			shader.Use();

			this->d_model_matrix = GetModelMatrix();


			if (m_skeleton->getNumberOfBones()>0)
				glUniformMatrix4fv (d_bone_location[0], m_skeleton->getNumberOfBones(), GL_FALSE, glm::value_ptr(m_animation_matrix[0]));

			shader.SetUniform("hasTexture",Has_Texture());

			for(GLuint i = 0; i < this->d_meshes.size(); i++)
				this->d_meshes[i].Draw(shader);
		}

		void Scale(glm::vec3 scale_vector)
		{
			this->d_scale = glm::scale(this->d_scale,scale_vector);
		}
		void Translate(glm::vec3 translation_vector)
		{
			this->d_position = glm::translate(this->d_position,translation_vector);
		}

		void TranslateFromOrigin(glm::vec3 translation_vector)
		{
			this->d_position = glm::translate(glm::mat4(),translation_vector);


		}
		void Rotate(glm::vec3 rotation_vector, float radians)
		{

			this->d_rotation =  glm::rotate(d_rotation,radians,rotation_vector);
			;
		}

		glm::quat Rotation() const { return d_rotation; } 

		vector<Mesh>* Meshes() { 
			return &d_meshes; 
		}

		void addTextures(Texture texture)
		{
			for (auto& mesh : d_meshes)
			{
				mesh.m_textures.push_back(texture);
			} 
		}

		void Rotate(glm::quat rotation)
		{
			this->d_rotation =  rotation;
		}


		glm::vec3 GetPositionVec()
		{
			return decomposeT(d_position);
		}	
		glm::mat4 GetPosition()
		{
			return d_position;
		}

		glm::mat4 GetModelMatrix() const
		{
			return d_position * glm::toMat4(d_rotation) * d_scale;
		}

		void Animate(IAnimation* animationInvoker, glm::vec3 target, string boneEffector, int numParent = 4)
		{
			assert(animationInvoker);

			Bone *effector = m_skeleton->GetBone(boneEffector.c_str());

			assert(effector);

			animationInvoker->Animate(this->d_model_matrix, effector ,target,numParent);

			m_skeleton->updateAnimationMatrix(m_animation_matrix);
		}

		vector<glm::vec3> getBonesOrientation( )
		{
			return m_skeleton->getBonePositions(this->d_model_matrix);
		}

		glm::vec3 getBoneOrientation(const char* name)
		{
			return m_skeleton->GetBone(name)->getWorldSpacePosition(this->d_model_matrix);
		}

		void CleanAnimationMatrix()
		{
			//Initialize bones in the shader for the uniform 
			for (unsigned int i = 0 ; i < m_skeleton->getNumberOfBones(); i++) 
				m_animation_matrix[i] = glm::mat4(1); 
		}

		void resetAnimation()
		{
			CleanAnimationMatrix();
		}

		void setJointLimit(string boneName, AngleRestriction angleRestriction )
		{ 
			m_skeleton->GetBone(boneName.c_str())->angleRestriction = angleRestriction;
		}



		void ClearJointsLimit(){
			m_skeleton->ResetAllJointLimits();
		}


	private:
		/*  Model Data  */

		/*  Functions   */
		// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(string path)
		{
			// Read file via ASSIMP
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate |  aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
			// Check for errors
			if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}
			/*const auto materialCount = scene->mNumMaterials;

			for (auto i = 0; i < materialCount; ++i)
			{
				int mapping = aiTextureMapping_PLANE;
				scene->mMaterials[i]->Get(AI_MATKEY_MAPPING_DIFFUSE(0), mapping);
				scene->mMaterials[i]->AddProperty(&mapping, 1, AI_MATKEY_MAPPING_DIFFUSE(0));
			}
			scene = importer.ApplyPostProcessing(aiProcess_CalcTangentSpace              |  \
				aiProcess_GenNormals                    |  \
				aiProcess_JoinIdenticalVertices |  \
				aiProcess_GenSmoothNormals | \
				aiProcess_Triangulate                   |  \
				aiProcess_GenUVCoords           |  \
				aiProcess_TransformUVCoords | \
				aiProcess_SortByPType     );*/

			// Retrieve the directory path of the filepath
			this->d_directory = path.substr(0, path.find_last_of('\\'));

			// Process ASSIMP's root node recursively
			this->processNode(scene->mRootNode, scene);

			// there should always be a 'root node', even if no skeleton exists
			if (!m_skeleton->importSkeletonBone ( scene->mRootNode)) {
				fprintf (stderr, "ERROR: Model %s - could not import node tree from mesh\n",path.c_str());
			} // endif 
			m_skeleton->inverseGlobal =  aiMatrix4x4ToGlm(&scene->mRootNode->mTransformation);
			int numOfBones = m_skeleton->getNumberOfBones();
			if (numOfBones > 0)
			{ 
				d_bone_location = (GLuint*) malloc( m_skeleton->getNumberOfBones()* sizeof(GLuint));

				for (unsigned int i = 0 ; i < numOfBones; i++) {

					char Name[128];
					//IKMatrices[i] = glm::mat4(1); 

					memset(Name, 0, sizeof(Name));
					sprintf_s(Name, sizeof(Name), "bones[%d]", i);
					//	GLint location = glGetUniformLocation(d_shader->Program, Name);
					//					if (location == INVALID_UNIFORM_LOCATION) {
					//fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", Name);
					//}

					//d_bone_location[i] = location;
				}
			}

		}

		// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode* node, const aiScene* scene)
		{ 

			// Process each mesh located at the current node
			for(GLuint i = 0; i < node->mNumMeshes; i++)
			{
				// The node object only contains indices to index the actual objects in the scene. 
				// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]]; 
				Mesh mesh = this->processMesh(ai_mesh, scene );

				this->d_meshes.push_back(mesh);			
			}
			// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for(GLuint i = 0; i < node->mNumChildren; i++)
			{
				this->processNode(node->mChildren[i], scene);
			}

		}


		Mesh processMesh(aiMesh* ai_mesh, const aiScene* scene )
		{
			// Data to fill
			vector<Vertex> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;
			vector<VertexWeight> boneWeights;
			Material material;
			glm::uint numBones = 0; 

#pragma region [ Process Vertices ]
			// Walk through each of the mesh's vertices
			for(GLuint i = 0; i < ai_mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector; // We declare a placeholder vector since Assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				// Positions
				vector.x = ai_mesh->mVertices[i].x;
				vector.y = ai_mesh->mVertices[i].y;
				vector.z = ai_mesh->mVertices[i].z;
				vertex.Position = vector;
				// Normals
				if (ai_mesh->HasNormals())
				{
					vector.x = ai_mesh->mNormals[i].x;
					vector.y = ai_mesh->mNormals[i].y;
					vector.z = ai_mesh->mNormals[i].z;
					vertex.Normal = vector;
				}
				// Texture Coordinates
				if(ai_mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = ai_mesh->mTextureCoords[0][i].x; 
					vec.y = ai_mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				if (ai_mesh->HasTangentsAndBitangents())
				{
					glm::vec3 tangent;
					tangent.x = ai_mesh->mTangents[i].x;
					tangent.y = ai_mesh->mTangents[i].y;
					tangent.z = ai_mesh->mTangents[i].z;

					vertex.Tangent = tangent;
				}
				vertices.push_back(vertex);
			}
#pragma endregion

#pragma region [ Process Faces ]

			// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for(GLuint i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				// Retrieve all indices of the face and store them in the indices vector
				for(GLuint j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
#pragma endregion

#pragma region [ Process Materials ]
			// Process materials
			if(ai_mesh->mMaterialIndex >= 0)
			{
				aiMaterial* aiMaterial = scene->mMaterials[ai_mesh->mMaterialIndex];

				aiColor4D ambient;
				glm::vec3 glmAmbient;
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient);

				glmAmbient =   aiColor4DToGlm(ambient);

				aiColor4D diffuse;
				glm::vec3 glmDiffuse;
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
				glmDiffuse = aiColor4DToGlm(diffuse);

				aiColor4D specular;
				glm::vec3 glmSpecular;
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular);
				glmSpecular = aiColor4DToGlm(specular);

				float shininess = 0.0f;
				aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess);

				material = Material(glmAmbient,glmDiffuse,glmSpecular,32.0f);

				// We assume a convention for sampler names in the Shaders. Each diffuse texture should be named
				// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
				// Same applies to other texture as the following list summarizes:
				// Diffuse: texture_diffuseN
				// Specular: texture_specularN
				// Normal: texture_normalN

				// 1. Diffuse maps
				vector<Texture> diffuseMaps = this->loadMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, TextureType_DIFFUSE);
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				// 2. Specular maps
				vector<Texture> specularMaps = this->loadMaterialTextures(aiMaterial, aiTextureType_SPECULAR, TextureType_SPECULAR);
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

				// 3. Normal maps
				vector<Texture> normalMaps = this->loadMaterialTextures(aiMaterial, aiTextureType_NORMALS, TextureType_NORMAL);
				 textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

				// 4. Ref maps
				vector<Texture> reflMaps = this->loadMaterialTextures(aiMaterial, aiTextureType_AMBIENT, TextureType_REFLECTION);
				textures.insert(textures.end(), reflMaps.begin(), reflMaps.end());
			}
#pragma endregion  

#pragma region [ Process Bones ]

			if(ai_mesh->HasBones())
			{
				int bone_count =  ai_mesh->mNumBones;

				boneWeights.resize(ai_mesh->mNumVertices);

				for (GLuint i = 0 ; i < ai_mesh->mNumBones ; i++) {                
					int BoneIndex = 0;        
					string BoneName(ai_mesh->mBones[i]->mName.data);

					if ( m_skeleton->boneMapping.find(BoneName) == m_skeleton->boneMapping.end()) {
						// Allocate an index for a new bone
						BoneInfo info;

						BoneIndex = d_numberOfBone++; 
						info.offset = aiMatrix4x4ToGlm(&ai_mesh->mBones[i]->mOffsetMatrix);
						info.index = BoneIndex;
						m_skeleton->boneMapping[BoneName] = info;
					}
					else
						BoneIndex = m_skeleton->boneMapping[BoneName].index;

					for (GLuint j = 0 ; j < ai_mesh->mBones[i]->mNumWeights ; j++) {
						int VertexID =   ai_mesh->mBones[i]->mWeights[j].mVertexId;
						float Weight  = ai_mesh->mBones[i]->mWeights[j].mWeight;   
						for (glm::uint i = 0 ; i < 4 ; i++) {
							if (boneWeights[VertexID].Weights[i] == 0.0) {
								boneWeights[VertexID].IDs[i]     = BoneIndex;
								boneWeights[VertexID].Weights[i] = Weight;
								break;
							}        
						}
					}
				}     
			}

#pragma endregion  

			// Return a mesh object created from the extracted mesh data
			return Mesh(vertices, indices, textures, boneWeights,material);
		}

		// Checks all material textures of a given type and loads the textures if they're not loaded yet.
		// The required info is returned as a Texture struct.
		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
		{
			vector<Texture> textures; 
			for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
			{

				aiString str;
				mat->GetTexture(type, i, &str);
				string file_name(str.C_Str());
				// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				GLboolean skip = false;
				for(GLuint j = 0; j < d_textures_loaded.size(); j++)
				{
					if(d_textures_loaded[j].m_file_name == file_name)
					{
						textures.push_back(d_textures_loaded[j]);
						skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}

				if(!skip)
				{   // If texture hasn't been loaded already, load it
					Texture texture(file_name, typeName);
					if (texture.Load(this->d_directory));
					{
						textures.push_back(texture);
						this->d_textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
					}
					//texture.path = str;
				}
			}
			return textures;
		}

	};
	 
}
#endif // Model_h__