// Custom
#include "VoronoiNeighbors.h"

// STL
#include <iostream>
#include <sstream>

// VTK
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmartPointer.h>

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
  
  // Read the input
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();
  
  return 0;
}
