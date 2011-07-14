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

// VTK
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPointSource.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

int main(int argc, char *argv[])
{
  // This program generates a 2D point cloud and finds the Voronoi Neighbors
  // of a particular point. The input point cloud, selected point, and
  // output neighbors are written to "input.vtp", "point.vtp", and
  // "neighbors.vtp", respectively.

  //Create a 3D point cloud
  vtkSmartPointer<vtkPointSource> pointSource =
    vtkSmartPointer<vtkPointSource>::New();
  pointSource->SetCenter(0.0, 0.0, 0.0);
  pointSource->SetNumberOfPoints(50);
  pointSource->SetRadius(5.0);
  //pointSource->SetS
  pointSource->Update();

  vtkSmartPointer<vtkPoints> points2D =
    vtkSmartPointer<vtkPoints>::New();
  for(unsigned int i = 0; i < pointSource->GetNumberOfPoints(); ++i)
    {
    double p3D[3];
    pointSource->GetOutput()->GetPoint(i, p3D);
    double origin[3] = {0.0, 0.0, 0.0};
    double normal[3] = {0.0, 0.0, 1.0};
    double p2D[3];
  
    vtkPlane::ProjectPoint(p3D, origin, normal, p2D);
    points2D->InsertNextPoint(p2D);
    }

  vtkSmartPointer<vtkPolyData> points2Dpolydata =
    vtkSmartPointer<vtkPolyData>::New();
  points2Dpolydata->SetPoints(points2D);

  // Write the input to a file
  {
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(points2Dpolydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("input.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  }
  
  unsigned int centerPointId = 25; // Find the Voronoi Neighbors of the point with id = 25
  
  // Write the center point to a file
  {
  vtkSmartPointer<vtkPoints> centerPoint = vtkSmartPointer<vtkPoints>::New();
  double p[3];
  points2D->GetPoint(centerPointId, p);
  centerPoint->InsertNextPoint(p);
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(centerPoint);
  
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(polydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("centerPoint.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  }
  
  // Find the Voronoi Neighbors of the center point
  vtkSmartPointer<vtkPoints> neighbors = vtkSmartPointer<vtkPoints>::New();
  VoronoiNeighbors(points2D, centerPointId, neighbors);
  
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
