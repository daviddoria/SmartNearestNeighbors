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

// This algorithm is explained in "Point Primitives for Interactive Modeling and Processing of 3D Geometry"

// Voronoi cell V_i of q_i is
// V_i = {x in ||x-q_i|| <= ||x-q_j|| all j in kNeighbors, j!=i}
// Voronoi neighbors are a subset of the kNeighbors whose Voronoi cell is adjacent to V_p, 
// the Voronoi cell of the point for which the kNeighbors were found

// Custom
#include "VoronoiNeighbors.h"

// VTK
#include <vtkIdList.h>
#include <vtkKdTree.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

// ITK
#include "itkVoronoiDiagram2DGenerator.h"
#include "itkImageFileWriter.h"
#include "itkVTKPolyDataWriter.h"

typedef itk::VoronoiDiagram2DGenerator<double> VoronoiGeneratorType;
typedef itk::VoronoiDiagram2D<double> VoronoiDiagramType;
typedef VoronoiDiagramType::PointType PointType;

struct ParallelSortObject
{
  PointType point;
  unsigned int id;
};

/* Compare point coordinates  in the y direction. */
bool pointSorter(ParallelSortObject arg1, ParallelSortObject arg2)
{
  if ( arg1.point[1] < arg2.point[1] ) { return 1; }
  else if ( arg1.point[1] > arg2.point[1] )
    {
    return 0;
    }
  else if ( arg1.point[0] < arg2.point[0] )
    {
    return 1;
    }
  else if ( arg1.point[0] > arg2.point[0] )
    {
    return 0;
    }
  else { return 1; }
}

void VoronoiNeighbors(vtkPoints* points, unsigned int centerPointId, vtkPoints* neighborPoints)
{
  // This function takes in a point cloud, 'points', and produces a point cloud, 'neighbors',
  // of the 'centerPointId's Voronoi Neighbors
  
  if(centerPointId > points->GetNumberOfPoints() - 1)
    {
    std::cerr << "You have requested the neighbors of point " << centerPointId
	      << " but the input only has " << points->GetNumberOfPoints() << " points!" << std::endl;
    exit(-1);
    }
  
  typedef VoronoiDiagramType::CellType CellType;
  typedef VoronoiDiagramType::CellAutoPointer CellAutoPointer;
  typedef CellType::PointIdIterator PointIdIterator;
  typedef VoronoiDiagramType::NeighborIdIterator NeighborIdIterator;

  VoronoiDiagramType::Pointer voronoiDiagram = VoronoiDiagramType::New();
  VoronoiGeneratorType::Pointer voronoiGenerator = VoronoiGeneratorType::New();

  double bounds[6];
  points->GetBounds(bounds);
  
  PointType boudingSize;
  boudingSize[0] = bounds[1];
  boudingSize[1] = bounds[3];
  //boudingSize[0] = width;
  //boudingSize[1] = height;
  voronoiGenerator->SetBoundary(boudingSize);
  std::cout << "Boundary size set to " << boudingSize << std::endl;
  
  PointType origin;
  origin[0] = bounds[0];
  origin[1] = bounds[2];
  voronoiGenerator->SetOrigin(origin);
  std::cout << "Origin set to " << origin << std::endl;

  // Create a list of seeds
  std::vector<ParallelSortObject> seedSortObjects;
  
  std::cout << "There are " << points->GetNumberOfPoints() << " points" << std::endl;
  
  
  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
    {
    double p[3];
    points->GetPoint(i,p);
  
    PointType seed;
    seed[0] = p[0];
    seed[1] = p[1];
    std::cout << "Adding seed " << seed << std::endl;
    ParallelSortObject parallelSortObject;
    parallelSortObject.point = seed;
    parallelSortObject.id = i;
    seedSortObjects.push_back(parallelSortObject);
    
    }
    
  std::sort(seedSortObjects.begin(), seedSortObjects.end(), pointSorter);
  
  std::vector<unsigned int> newIds; // this will be a map from new id -> old id
  for(unsigned int i = 0; i < seedSortObjects.size(); ++i)
    {
    voronoiGenerator->AddOneSeed(seedSortObjects[i].point);
    newIds.push_back(seedSortObjects[i].id);
    }
  
  voronoiGenerator->Update();
  voronoiDiagram = voronoiGenerator->GetOutput();
  
  // Find the new id of the center point
  unsigned int newCenterPointId = 0;
  for(unsigned int i = 0; i < seedSortObjects.size(); ++i)
    {
    if(seedSortObjects[i].id == centerPointId)
      {
      newCenterPointId = i;
      break;
      }
    }
  std::cout << "New center point id: " << newCenterPointId << std::endl;
  
  // Construct the output points
  for(NeighborIdIterator neighbors = voronoiDiagram->NeighborIdsBegin(newCenterPointId); neighbors != voronoiDiagram->NeighborIdsEnd(newCenterPointId); ++neighbors)
    {
    double p[3];
    points->GetPoint(newIds[*neighbors], p);
    neighborPoints->InsertNextPoint(p);
  
    // This also works
    /*
    PointType seed = voronoiGenerator->GetOutput()->GetSeed(*neighbors);
    double p[3];
    p[0] = seed[0];
    p[1] = seed[1];
    p[2] = 0;
    neighborPoints->InsertNextPoint(p);
    */
    }

  {
  // Create a mesh of the Voronoi diagram
  VoronoiDiagramType::VertexIterator allVerts;
  int j = 0;
  for(allVerts = voronoiDiagram->VertexBegin(); allVerts != voronoiDiagram->VertexEnd(); ++allVerts)
    {
    voronoiDiagram->SetPoint(j, *allVerts);
    j++;
    }

  // Write the resulting mesh
  typedef itk::VTKPolyDataWriter<VoronoiDiagramType::Superclass> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(voronoiDiagram);
  writer->SetFileName("voronoi.vtk");
  writer->Update();
  }
}
