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

#include <vtkIdList.h>
#include <vtkKdTree.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

void BSPNeighbors(vtkPoints* inputPoints, unsigned int centerPointId, vtkPoints* bspNeighbors, unsigned int k)
{
  double centerPoint[3];
  inputPoints->GetPoint(centerPointId, centerPoint);
  
  // Create a vtkPoints of all points except the center point
  vtkSmartPointer<vtkPoints> points = 
    vtkSmartPointer<vtkPoints>::New();
  for(vtkIdType i = 0; i < inputPoints->GetNumberOfPoints(); ++i)
    {
    if(i == centerPointId)
      {
      continue;
      }
    double p[3];
    inputPoints->GetPoint(i, p);
    points->InsertNextPoint(p);
    }
  
  // Create the tree
  vtkSmartPointer<vtkKdTree> pointTree = 
    vtkSmartPointer<vtkKdTree>::New();
  pointTree->BuildLocatorFromPoints(points);
  
  vtkSmartPointer<vtkIdList> result = 
    vtkSmartPointer<vtkIdList>::New();
     
  // Find the k nearest neighbors
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

  // For demonstration only, we output the K nearest neighbors
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
      bspNeighbors->InsertNextPoint(neighborPoint);
      }
    } // end kNeighbors loop

}
