// VTK
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

// Custom
#include "BSPNeighbors.h"

int main(int argc, char *argv[])
{
  // Verify arguments
  if(argc < 3)
    {
    std::cerr << "Required arguments: input.vtp output.vtp" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse arguments
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];

  vtkSmartPointer<vtkXMLPolyDataReader> reader =
    vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->Update();
    
  // Find the nearest neighbors of the 9th point
  unsigned int centerPointId = 9;
  
  vtkSmartPointer<vtkPoints> bspNeighborPoints = 
    vtkSmartPointer<vtkPoints>::New();
    
  BSPNeighbors(reader->GetOutput()->GetPoints(), centerPointId, bspNeighborPoints);
    
  vtkSmartPointer<vtkPolyData> bspNeighborPolydata = 
    vtkSmartPointer<vtkPolyData>::New();
  bspNeighborPolydata->SetPoints(bspNeighborPoints);

  {
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(bspNeighborPolydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("bspNeighbors.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  }
  
  return EXIT_SUCCESS;
}
