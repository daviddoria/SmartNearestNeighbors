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
  /*
  double centerPoint[3];
  points->GetPoint(centerPointId, centerPoint);
  */
  double bounds[6];
  points->GetBounds(bounds);
  const double height = bounds[3] - bounds[2];
  const double width = bounds[1] - bounds[0];
  
  typedef itk::VoronoiDiagram2D<double> VoronoiDiagramType;
  typedef itk::VoronoiDiagram2DGenerator<double> VoronoiGeneratorType;

  typedef VoronoiDiagramType::PointType PointType;
  typedef VoronoiDiagramType::CellType CellType;
  typedef VoronoiDiagramType::CellAutoPointer CellAutoPointer;
  typedef CellType::PointIdIterator PointIdIterator;
  typedef VoronoiDiagramType::NeighborIdIterator NeighborIdIterator;

  VoronoiDiagramType::Pointer voronoiDiagram = VoronoiDiagramType::New();
  VoronoiGeneratorType::Pointer voronoiGenerator = VoronoiGeneratorType::New();

  PointType boudingSize;
  boudingSize[0] = width;
  boudingSize[1] = height;
  voronoiGenerator->SetBoundary(boudingSize);
  std::cout << "Boundary size set to " << boudingSize << std::endl;
  
  PointType origin;
  origin[0] = bounds[0];
  origin[1] = bounds[2];
  voronoiGenerator->SetOrigin(origin);
  std::cout << "Origin set to " << origin << std::endl;

  // Create a list of seeds
  std::vector<PointType> seeds;
  
  std::cout << "There are " << points->GetNumberOfPoints() << " points" << std::endl;
  
  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
    {
    double p[3];
    points->GetPoint(i,p);
  
    PointType seed;
    seed[0] = p[0];
    seed[1] = p[1];
    //std::cout << "Adding seed " << seed << std::endl;
    seeds.push_back(seed);
    voronoiGenerator->AddOneSeed(seeds[i]);
    }
  
  voronoiGenerator->Update();
  voronoiDiagram = voronoiGenerator->GetOutput();
  
//  PointType centerPoint = voronoiDiagram->GetSeed(centerPointId);
//  CellAutoPointer centerCell;
//  voronoiDiagram->GetCellId(centerPointId, centerCell);

  for(NeighborIdIterator neighbors = voronoiDiagram->NeighborIdsBegin(centerPointId); neighbors != voronoiDiagram->NeighborIdsEnd(centerPointId); ++neighbors)
    {
    double p[3];
    points->GetPoint(*neighbors, p);
  
    neighborPoints->InsertNextPoint(p);
    }

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
