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
#ifndef FBXFiLEREADER_H
#define FBXFiLEREADER_H

#include <string>
#include <memory>
#include <fbxsdk.h>
#include "VertexAttributes.h"

//Custom deleter for FbxManager pointer
template<>
struct std::default_delete<FbxManager>{
	void operator()(FbxManager* ptr) { ptr->Destroy(); }
};

class FbxFileReader
{
public:
	// * Imports a .fbx file and creates a scene to hold its contents.
	FbxFileReader(const std::string& filename);
	virtual ~FbxFileReader(void);

	// * Retrieves the first MeshNode found in the scene. 
	// * Throws exception if no meshes are found.
	Mesh& GetMesh(void);
	
	// * Retrieves the MeshNode with the given name. 
	// * Throws exception when mesh is not found.
	Mesh& GetMesh(const std::string& filename);
	
private:
	static std::unique_ptr<FbxManager> s_pSdkManager;
	FbxScene* m_pScene;	
	
	std::vector<Mesh> m_Meshes;

	//Store meshes for later use
	void ReadMeshes(void);

	//Disabling default copy constructor & assignment operator
	FbxFileReader(const FbxFileReader& src);
	FbxFileReader& operator=(const FbxFileReader& src);
};

#endif