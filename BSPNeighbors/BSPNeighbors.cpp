// This algorithm is explained in "Point Primitives for Interactive Modeling and Processing of 3D Geometry"

#include <vtkIdList.h>
#include <vtkKdTree.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

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
    
  // Find the 'k' nearest neighbors
  unsigned int k = 8;
  
  unsigned int centerPointId = 9;
  
  double centerPoint[3];
  reader->GetOutput()->GetPoint(centerPointId, centerPoint);
  
  // Create a vtkPoints of all points except the center point
  vtkSmartPointer<vtkPoints> points = 
    vtkSmartPointer<vtkPoints>::New();
  for(vtkIdType i = 0; i < reader->GetOutput()->GetNumberOfPoints(); ++i)
    {
    if(i == centerPointId)
      {
      continue;
      }
    double p[3];
    reader->GetOutput()->GetPoint(i, p);
    points->InsertNextPoint(p);
    }
  
  // Create the tree
  vtkSmartPointer<vtkKdTree> pointTree = 
    vtkSmartPointer<vtkKdTree>::New();
  pointTree->BuildLocatorFromPoints(points);
  
  vtkSmartPointer<vtkIdList> result = 
    vtkSmartPointer<vtkIdList>::New();
  
  pointTree->FindClosestNPoints(k, centerPoint, result);
  
  // Create a polydata of the result
  vtkSmartPointer<vtkPoints> kNearestPoints = 
    vtkSmartPointer<vtkPoints>::New();
  
  for(vtkIdType i = 0; i < k; i++)
    {
    vtkIdType point_ind = result->GetId(i);
    double p[3];
    points->GetPoint(point_ind, p);
    kNearestPoints->InsertNextPoint(p);
    }

  vtkSmartPointer<vtkPolyData> kNearestPolydata = 
    vtkSmartPointer<vtkPolyData>::New();
  kNearestPolydata->SetPoints(kNearestPoints);

  {
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(kNearestPolydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("kNearest.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  }
  
  // Each nearest neighbor point defines a halfspace. The BSP neighbors are a subset of the KNearestNeighbors
  // which are in the intersection of all of the halfspaces induced by the kNearestNeighbor points.
  
  // Each kNeighbor defines a halfspace as:
  // (x-q_i).(p-q_i) >= 0
  // x is a test point (the collection of x that fit this criterion is exacly the halfspace)
  // q_i is the ith nearest neighbor point
  // p is the center point (for which the kNearest points were found)
  
  // Create a vtkPoints of the BSPNeighbors
  vtkSmartPointer<vtkPoints> bspNeighborPoints = 
    vtkSmartPointer<vtkPoints>::New();
    
  for(unsigned int neighborId = 0; neighborId < k; ++neighborId) // test each kNeighbor point
    {
    bool valid = true;

    double neighborPoint[3];
    kNearestPoints->GetPoint(neighborId, neighborPoint);

    for(unsigned int halfSpaceId = 0; halfSpaceId < k; ++halfSpaceId) // against each halfspace
      {
      double halfSpacePoint[3];
      kNearestPoints->GetPoint(halfSpaceId, halfSpacePoint);
      
      // (x-q_i).(p-q_i) >= 0
      // A.B >= 0
      double A[3];
      vtkMath::Subtract(neighborPoint, halfSpacePoint, A);
      
      double B[3];
      vtkMath::Subtract(centerPoint, halfSpacePoint, B);
      
      if(!(vtkMath::Dot(A,B) >= 0))
	{
	valid = false;
	break;
	}
      } // end halfspace loop
    
    // Keep the point if all of the tests passed
    if(valid)
      {
      bspNeighborPoints->InsertNextPoint(neighborPoint);
      }
    } // end kNeighbors loop

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
