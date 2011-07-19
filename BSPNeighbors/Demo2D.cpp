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

// VTK
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPointSource.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

// Custom
#include "BSPNeighbors.h"

void Project2D(vtkPoints* points3D, vtkPoints* points2D);

int main(int argc, char *argv[])
{
  //Create a 3D point cloud
  vtkSmartPointer<vtkPointSource> pointSource =
    vtkSmartPointer<vtkPointSource>::New();
  pointSource->SetCenter(0.0, 0.0, 0.0);
  pointSource->SetNumberOfPoints(50);
  pointSource->SetRadius(5.0);
  pointSource->Update();
    
  vtkSmartPointer<vtkPoints> points2D = vtkSmartPointer<vtkPoints>::New();
  Project2D(pointSource->GetOutput()->GetPoints(), points2D);
  
  {
  // Write the input
  vtkSmartPointer<vtkPolyData> points2Dpolydata = vtkSmartPointer<vtkPolyData>::New();
  points2Dpolydata->SetPoints(points2D);
  
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
  
  // Find the nearest neighbors of the 9th point
  unsigned int queryPointId = 0;
  
  // Write the center point to a file
  {
  vtkSmartPointer<vtkPoints> queryPoint = vtkSmartPointer<vtkPoints>::New();
  double p[3];
  points2D->GetPoint(queryPointId, p);
  queryPoint->InsertNextPoint(p);
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(queryPoint);
  
  vtkSmartPointer<vtkVertexGlyphFilter> vertexGlyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexGlyphFilter->SetInputConnection(polydata->GetProducerPort());
  vertexGlyphFilter->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("queryPoint.vtp");
  writer->SetInputConnection(vertexGlyphFilter->GetOutputPort());
  writer->Write();
  }
  
  vtkSmartPointer<vtkPoints> bspNeighborPoints = 
    vtkSmartPointer<vtkPoints>::New();
    
  BSPNeighbors(points2D, queryPointId, bspNeighborPoints);
    
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

void Project2D(vtkPoints* points3D, vtkPoints* points2D)
{
  for(unsigned int i = 0; i < points3D->GetNumberOfPoints(); ++i)
    {
    double p3D[3];
    points3D->GetPoint(i, p3D);
    double origin[3] = {0.0, 0.0, 0.0};
    double normal[3] = {0.0, 0.0, 1.0};
    double p2D[3];
  
    vtkPlane::ProjectPoint(p3D, origin, normal, p2D);
    points2D->InsertNextPoint(p2D);
    }
}
