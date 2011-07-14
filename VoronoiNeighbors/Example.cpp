/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

// Custom
#include "VoronoiNeighbors.h"

// STL
#include <iostream>
#include <sstream>

// VTK
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

int main(int argc, char *argv[])
{
  // This program reads in a point cloud
  
  // Verify arguments
  if(argc < 4)
    {
    std::cerr << "Required arguments: input.vtp centerPointId output.vtp" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse arguments
  std::string inputFileName = argv[1];
  
  std::stringstream ss(argv[2]);
  unsigned int centerPointId = 0;
  ss >> centerPointId;
  
  std::string outputFileName = argv[3];
  
  // Output arguments
  std::cout << "Input file: " << inputFileName << std::endl
	    << "Center point id: " << centerPointId << std::endl
	    << "Output file: " << outputFileName << std::endl;
  
  // Read the input
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();
  
  vtkSmartPointer<vtkPoints> neighbors = vtkSmartPointer<vtkPoints>::New();
  VoronoiNeighbors(reader->GetOutput()->GetPoints(), centerPointId, neighbors);
  
  // Add the resulting neighbors to a polydata
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(neighbors);
  
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(polydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("VoronoiNeighbors.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  
  return EXIT_SUCCESS;
}
