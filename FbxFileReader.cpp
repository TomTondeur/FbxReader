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

#include "FbxFileReader.h"
#include <algorithm>

using namespace std;

unique_ptr<FbxManager> FbxFileReader::s_pSdkManager = nullptr;

FbxFileReader::FbxFileReader(const string& filename) : m_pScene(nullptr)
{
    // Create an SDK manager
	if(!s_pSdkManager)
	    s_pSdkManager = unique_ptr<FbxManager>( FbxManager::Create() );
    
	//Configure I/O settings
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(s_pSdkManager.get(), IOSROOT);
    s_pSdkManager->SetIOSettings(pIOSettings);
	
	// Create an importer
	FbxImporter* pImporter = FbxImporter::Create(s_pSdkManager.get(), "");
    if( !pImporter->Initialize(filename.c_str(), -1, pIOSettings) )
        throw exception("Failed to initialize FbxImporter.");
    
    // Add fbx data to scene using the importer
    m_pScene = FbxScene::Create( s_pSdkManager.get(), filename.c_str() );
    pImporter->Import(m_pScene);

    // Done importing
    pImporter->Destroy();

	//Check if the scene contains a root node
    if( !m_pScene->GetRootNode() )
        throw std::exception("No RootNode could be found.");
}

FbxFileReader::~FbxFileReader(void)
{
	m_pScene->Destroy();
}

//Returns the first mesh encountered in the fbx file
Mesh& FbxFileReader::GetMesh(void)
{
	if(m_Meshes.empty())
		ReadMeshes();

	return m_Meshes[0];
}
	
//Returns a mesh with a specific name
Mesh& FbxFileReader::GetMesh(const std::string& nodeName)
{
	if(m_Meshes.empty())
		ReadMeshes();

	auto it = find_if(m_Meshes.begin(), m_Meshes.end(), [&](Mesh& meshRef){
		return meshRef.GetName() == nodeName;
	});

	if(it == m_Meshes.end())
		throw exception( ("Failed to find a mesh with the name " + nodeName).c_str() );

	return *it;
}

void FbxFileReader::ReadMeshes(void)
{
	//Get the scene's rootnode
    FbxNode* pRootNode = m_pScene->GetRootNode();
	
	// Loop through all of the child nodes in the scene
	int nrOfChildren = pRootNode->GetChildCount();
	for(int i=0; i<nrOfChildren; ++i){
		FbxNode* pChild = pRootNode->GetChild(i);
		FbxNodeAttribute* pAttribute = pChild->GetNodeAttribute();

		//If this node is a mesh, add it to our collection
		if(pAttribute && pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			m_Meshes.push_back( Mesh( pChild->GetMesh() ) );
	}

	//Throw when no meshes are found
	if(m_Meshes.empty())
		throw exception("Failed to find mesh data.");
}
