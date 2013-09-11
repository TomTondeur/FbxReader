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

#include "VertexAttributes.h"
#include <map>

Mesh::Mesh(FbxMesh* _pMesh):pMesh(_pMesh){}
Mesh::Mesh(void):pMesh(nullptr){}

std::string Mesh::GetName(void)
{
	assert(pMesh != nullptr);
	return std::string(pMesh->GetName() );
}

void Mesh::ExtractData(void)
{	
	assert(pMesh != nullptr);
	
	//Only accept triangle meshes
	if(!pMesh->IsTriangleMesh())
		throw std::exception( (std::string("Mesh ") + pMesh->GetName() + " contains N-gons. Only triangle meshes are supported.").c_str() );
		
	//Loop over all vertices in all triangles and get the vertex positions
	const unsigned int triCount = pMesh->GetPolygonCount();
	for (unsigned int iTri=0; iTri<triCount; ++iTri)
		for (unsigned int iVert=0; iVert<3; ++iVert)
			Positions.data.push_back( pMesh->GetControlPointAt( pMesh->GetPolygonVertex(iTri,iVert) ) );

	//Copy vertex attribute arrays into vectors of our own
	unsigned int layerCount = pMesh->GetLayerCount();
	for (unsigned int layer=0; layer<layerCount; ++layer){
		auto pLayer = pMesh->GetLayer(layer);			
	
		TexCoords.ExtractData(	pMesh, pLayer->GetUVs() );
		Normals.ExtractData(	pMesh, pLayer->GetNormals() );
		Tangents.ExtractData(	pMesh, pLayer->GetTangents() );
		Binormals.ExtractData(	pMesh, pLayer->GetBinormals() );
		Colors.ExtractData(		pMesh, pLayer->GetVertexColors() );
	}
}

void Mesh::Optimize(void)
{
	Positions.Optimize();
	TexCoords.Optimize();
	Normals.Optimize();
	Tangents.Optimize();
	Binormals.Optimize();
	Colors.Optimize();
}
