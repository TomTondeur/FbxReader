// This file is part of FbxReader.
// 
// FbxReader is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// FbxReader is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with FbxReader.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#ifndef VERTEXATTRIBUTES_H
#define VERTEXATTRIBUTES_H

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <cassert>

template<typename T>
struct VertexAttribute
{
	std::vector<T> data;
	std::vector<unsigned int> indices;
	
	//Vertex attributes are optimized using an index array to indirectly access the unique entries in the data array
	const T& GetRefAt(unsigned int vertexIndex)
	{
		return data.at(indices.at(vertexIndex));
	}

	//Get attributes from the specified layer element
	void ExtractData(FbxMesh* pMesh, FbxLayerElementTemplate<T>* pLayerElement)
	{
		if(!pLayerElement)
			return;

		if(!data.empty())
			data.clear();

		if(!pMesh->IsTriangleMesh())
			throw std::exception( (std::string("Mesh ") + pMesh->GetName() + " contains N-gons. Only triangle meshes are supported.").c_str() );
		
		//Gather necessary data to loop over all vertices and retrieve the attributes

		const auto mappingMode = pLayerElement->GetMappingMode();
		const auto referenceMode = pLayerElement->GetReferenceMode();

		const unsigned int nrOfTriangles = pMesh->GetPolygonCount();
		int index=0;
		
		//For every vertex in every triangle
		for(unsigned int iTri=0; iTri<nrOfTriangles; ++iTri){
			for(unsigned int iVert=0; iVert<3; ++iVert){

				switch(mappingMode){
					//Attributes are stored per control point
					case FbxLayerElement::eByControlPoint:
						switch(referenceMode){
							//Direct access
							case FbxLayerElement::eDirect:
								index = pMesh->GetPolygonVertex(iTri,iVert);
								break;
							//Indirect access
							case FbxLayerElement::eIndexToDirect:
								index = pLayerElement->GetIndexArray().GetAt( pMesh->GetPolygonVertex(iTri,iVert) );
								break;
							default:
								throw std::exception("Invalid reference mode");
						}
						break;
					//Attributes are stored per polygon vertex
					case FbxLayerElement::eByPolygonVertex: 
						switch(referenceMode){
							//Direct access
							case FbxLayerElement::eDirect:
								index = 3*iTri + iVert;
								break;
							//Indirect access
							case FbxLayerElement::eIndexToDirect:
								index = pLayerElement->GetIndexArray().GetAt( 3*iTri + iVert );
								break;
							default:
								throw std::exception("Invalid reference mode");
						}
						break;
					default:
						throw std::exception("Invalid mapping mode");
				}
				//Get the attribute from the direct array
				data.push_back( pLayerElement->GetDirectArray().GetAt(index) );
			}
		}
	}

	//Optimize the data by eliminating duplicates and using an index array
	void Optimize(void)
	{
		if(data.empty()) //Nothing to optimize
			return;

		const unsigned int nrOfIndices = data.size();
		std::vector<T> uniqueData;
		
		//Ignore previous indices
		indices.clear();
		indices.resize(nrOfIndices);

		for(unsigned int i=0; i<nrOfIndices; ++i){
			auto it = find(uniqueData.begin(), uniqueData.end(), data[i]);
		
			//Current data is not a duplicate => add it to uniqueData
			if(it == uniqueData.end()){
				uniqueData.push_back(data[i]);
				it = uniqueData.end() - 1;
			}
			
			//Store index to this element in our index array
			indices[i] = it - uniqueData.begin();
		}
		//Replace current data with unique data
		data = std::move(uniqueData);
	}
};

struct Mesh
{
	Mesh(FbxMesh* _pMesh);
	Mesh(void);
	
	//Get the name from the mesh node
	std::string GetName(void);
	
	//Store vertex attributes for later use
	void ExtractData(void);

	//Optimize stored vertex attributes
	void Optimize(void);

	VertexAttribute<FbxVector4> Positions;
	VertexAttribute<FbxVector2> TexCoords;
	VertexAttribute<FbxVector4> Normals;
	VertexAttribute<FbxVector4> Tangents;
	VertexAttribute<FbxVector4> Binormals;
	VertexAttribute<FbxColor>	Colors;

private:
	FbxMesh* pMesh;
};

#endif