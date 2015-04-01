#ifndef VertexTriangleAdjacency_h__
#define VertexTriangleAdjacency_h__
 

#include "assimp/types.h"
 #include <assimp/scene.h> 
#include "assimp/ai_assert.h"
 // --------------------------------------------------------------------------------------------
/** @brief The VertexTriangleAdjacency class computes a vertex-triangle
 *  adjacency map from a given index buffer.
 *
 *  @note Although it is called #VertexTriangleAdjacency, the current version does also
 *    support arbitrary polygons. */
// --------------------------------------------------------------------------------------------
class ASSIMP_API VertexTriangleAdjacency
{
public:

	// ----------------------------------------------------------------------------
	/** @brief Construction from an existing index buffer
	 *  @param pcFaces Index buffer
	 *  @param iNumFaces Number of faces in the buffer
	 *  @param iNumVertices Number of referenced vertices. This value
	 *    is computed automatically if 0 is specified.
	 *  @param bComputeNumTriangles If you want the class to compute
	 *    a list containing the number of referenced triangles per vertex
	 *    per vertex - pass true.  */
	VertexTriangleAdjacency(aiFace* pcFaces,unsigned int iNumFaces,
		unsigned int iNumVertices = 0,
		bool bComputeNumTriangles = true);


	// ----------------------------------------------------------------------------
	/** @brief Destructor */
	~VertexTriangleAdjacency();


public:

	// ----------------------------------------------------------------------------
	/** @brief Get all triangles adjacent to a vertex
	 *  @param iVertIndex Index of the vertex
	 *  @return A pointer to the adjacency list. */
	unsigned int* GetAdjacentTriangles(unsigned int iVertIndex) const
	{
		ai_assert(iVertIndex < iNumVertices);
		return &mAdjacencyTable[ mOffsetTable[iVertIndex]];
	}


	// ----------------------------------------------------------------------------
	/** @brief Get the number of triangles that are referenced by
	 *    a vertex. This function returns a reference that can be modified
	 *  @param iVertIndex Index of the vertex
	 *  @return Number of referenced triangles */
	unsigned int& GetNumTrianglesPtr(unsigned int iVertIndex)
	{
		ai_assert(iVertIndex < iNumVertices && NULL != mLiveTriangles);
		return mLiveTriangles[iVertIndex];
	}


public:

	//! Offset table
	unsigned int* mOffsetTable;

	//! Adjacency table
	unsigned int* mAdjacencyTable;

	//! Table containing the number of referenced triangles per vertex
	unsigned int* mLiveTriangles;

	//! Debug: Number of referenced vertices
	unsigned int iNumVertices;

};


// ------------------------------------------------------------------------------------------------
VertexTriangleAdjacency::VertexTriangleAdjacency(aiFace *pcFaces,
												 unsigned int iNumFaces,
												 unsigned int iNumVertices /*= 0*/,
												 bool bComputeNumTriangles /*= false*/)
{
	// compute the number of referenced vertices if it wasn't specified by the caller
	const aiFace* const pcFaceEnd = pcFaces + iNumFaces;
	if (!iNumVertices)	{

		for (aiFace* pcFace = pcFaces; pcFace != pcFaceEnd; ++pcFace)	{
			ai_assert(3 == pcFace->mNumIndices);
			iNumVertices = std::max(iNumVertices,pcFace->mIndices[0]);
			iNumVertices = std::max(iNumVertices,pcFace->mIndices[1]);
			iNumVertices = std::max(iNumVertices,pcFace->mIndices[2]);
		}
	}

	this->iNumVertices = iNumVertices;

	unsigned int* pi;

	// allocate storage
	if (bComputeNumTriangles)	{
		pi = mLiveTriangles = new unsigned int[iNumVertices+1];
		memset(mLiveTriangles,0,sizeof(unsigned int)*(iNumVertices+1));
		mOffsetTable = new unsigned int[iNumVertices+2]+1;
	}
	else {
		pi = mOffsetTable = new unsigned int[iNumVertices+2]+1;
		memset(mOffsetTable,0,sizeof(unsigned int)*(iNumVertices+1));
		mLiveTriangles = NULL; // important, otherwise the d'tor would crash
	}

	// get a pointer to the end of the buffer
	unsigned int* piEnd = pi+iNumVertices;
	*piEnd++ = 0u;

	// first pass: compute the number of faces referencing each vertex
	for (aiFace* pcFace = pcFaces; pcFace != pcFaceEnd; ++pcFace)
	{
		pi[pcFace->mIndices[0]]++;	
		pi[pcFace->mIndices[1]]++;	
		pi[pcFace->mIndices[2]]++;	
	}

	// second pass: compute the final offset table
	unsigned int iSum = 0;
	unsigned int* piCurOut = this->mOffsetTable;
	for (unsigned int* piCur = pi; piCur != piEnd;++piCur,++piCurOut)	{

		unsigned int iLastSum = iSum;
		iSum += *piCur; 
		*piCurOut = iLastSum;
	}
	pi = this->mOffsetTable;

	// third pass: compute the final table
	this->mAdjacencyTable = new unsigned int[iSum];
	iSum = 0;
	for (aiFace* pcFace = pcFaces; pcFace != pcFaceEnd; ++pcFace,++iSum)	{

		unsigned int idx = pcFace->mIndices[0];
		mAdjacencyTable[pi[idx]++] = iSum;

		idx = pcFace->mIndices[1];
		mAdjacencyTable[pi[idx]++] = iSum;

		idx = pcFace->mIndices[2];
		mAdjacencyTable[pi[idx]++] = iSum;
	}
	// fourth pass: undo the offset computations made during the third pass
	// We could do this in a separate buffer, but this would be TIMES slower.
	--mOffsetTable;
	*mOffsetTable = 0u;
}
// ------------------------------------------------------------------------------------------------
VertexTriangleAdjacency::~VertexTriangleAdjacency()
{
	// delete allocated storage
	delete[] mOffsetTable;
	delete[] mAdjacencyTable;
	delete[] mLiveTriangles;
}



#endif // VertexTriangleAdjacency_h__
