#ifndef __vtkDualPolyDataFilter_h
#define __vtkDualPolyDataFilter_h

#include"vtkPoints.h"
#include"vtkPolyDataAlgorithm.h"
#include"vtkPolyData.h"
#include"vtkCellArray.h"

#include <algorithm>

#if defined(NO_DLL)  && defined (WIN32)
#undef VTK_EXPORT
#define VTK_EXPORT
#endif

class VTK_EXPORT vtkDualPolyDataFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(vtkDualPolyDataFilter,vtkPolyDataAlgorithm);
    static vtkDualPolyDataFilter* New();

protected:
    vtkDualPolyDataFilter();
    ~vtkDualPolyDataFilter();

    virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    virtual int FillInputPortInformation(int port, vtkInformation *info);

private:
    vtkDualPolyDataFilter(const vtkDualPolyDataFilter&);  // Not implemented.
    void operator=(const vtkDualPolyDataFilter&);  // Not implemented.

    class Edge
    {
    public:
      Edge(vtkIdType a, vtkIdType b) 
      {
        p1 = std::min(a,b);
        p2 = std::max(a,b);
      }
      virtual ~Edge() {};
      bool operator< (const Edge &right) const
      {
        if( p1 < right.p1 )
          return(true);
        else if( p1 > right.p1 )
          return(false);
        else if( p1 == right.p1 )
        {
          if( p2 >= right.p2 )
            return(false);
          else
            return(true);
        }
        else
          return(false);
      }

      bool operator== (const Edge &right) const
      {
        return(p1 == right.p1 && p2 == right.p2);
      }

      vtkIdType p1;
      vtkIdType p2;
    };
};

#endif //__vtkDualPolyDataFilter_h
