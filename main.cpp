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

#include <iostream>
#include <string>

#include <fbxsdk.h>

#include "FbxFileReader.h"
#include "VertexAttributes.h"

// Entrypoint
int main(void) 
{
	Mesh mesh;
    std::string filename;
	std::cout << "Enter path to fbx file to read: ";
	std::cin >> filename;

	//File reading is done in this scope so the FbxFileReader automatically releases internal resources when we're done reading
	{
		std::cout << "Opening FBX file... ";
		FbxFileReader fbxFile(filename);
	
		mesh = fbxFile.GetMesh();
		
		std::cout << "Done." << std::endl << "Reading mesh data... ";
		mesh.ExtractData();

		std::cout << "Done." << std::endl << "Optimizing... ";
		mesh.Optimize();
		std::cout << "Done." << std::endl;
	}
	
	//Process mesh data here

	system("pause");
    return 0;
}
