//! \class vtkUVSphereSource
//! \brief This class is a polydata source. It outputs a sphere where meridians and parallels
//! are represented through polyline. It proposes a NoiseLevel parameter that 
//! disturb the sphere regularity.
//!
//! \author Jerome Velut
//! \date 02-2007

#ifndef __vtkUVSphereSource_h
#define __vtkUVSphereSource_h

#include <vtkPolyDataAlgorithm.h>

class VTK_EXPORT vtkUVSphereSource : public vtkPolyDataAlgorithm 
{
public:
  void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeRevisionMacro(vtkUVSphereSource,vtkPolyDataAlgorithm);

 static vtkUVSphereSource *New();

  vtkSetMacro(ParallelResolution,int);
  vtkGetMacro(ParallelResolution,int);

  vtkSetMacro(MeridianResolution,int);
  vtkGetMacro(MeridianResolution,int);

  vtkSetMacro(BuildVectorData,int);
  vtkGetMacro(BuildVectorData,int);

  vtkSetMacro(Radius,double);
  vtkGetMacro(Radius,double);

  vtkSetMacro(NoiseLevel,double);
  vtkGetMacro(NoiseLevel,double);

  vtkSetMacro(ModulationAmplitude,double);
  vtkGetMacro(ModulationAmplitude,double);

  vtkSetMacro(NumberOfModulations,int);
  vtkGetMacro(NumberOfModulations,int);

  void SetCenter(double x, double y, double z)
	{Center[0] = x;Center[1] = y;Center[2] = z;Modified();};
  vtkGetVectorMacro(Center,double,3);

  void SetOutput( vtkPolyData* output );

protected:
  vtkUVSphereSource();
  ~vtkUVSphereSource() {};

    int RequestData( vtkInformation*,
                     vtkInformationVector**,
                     vtkInformationVector*);

  double Center[3];
double Radius;
double NoiseLevel;
int MeridianResolution; // the number of points on a line between north pole and south pole
int ParallelResolution; // the number of points on a parallel (like the equator)
double ModulationAmplitude;
int NumberOfModulations;
int BuildVectorData;

private:
  vtkUVSphereSource(const vtkUVSphereSource&);  // Not implemented.
  void operator=(const vtkUVSphereSource&);  // Not implemented.
};

#endif //__vtkUVSphereSource_h


