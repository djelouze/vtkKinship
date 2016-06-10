//! \class vtkHistoryManager
//! \brief Archive inputs and select desired output 
//!

#ifndef __vtkHistoryManager_h
#define __vtkHistoryManager_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h"

class VTK_EXPORT vtkHistoryManager : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkHistoryManager,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkHistoryManager *New();

  void ResetInputs();
  void AddInput( vtkPolyData* );

  //! CurrentOutput accessors
  vtkSetMacro( CurrentOutput, int );
  //! CurrentOutput accessors
  vtkGetMacro( CurrentOutput, int );

protected:
  //! constructor
  vtkHistoryManager();
  //! destructor
  virtual ~vtkHistoryManager() {};

  //! VTK Pipeline function
  int RequestData( vtkInformation*, 
                   vtkInformationVector**, 
                   vtkInformationVector*);

private:
  vtkHistoryManager(const vtkHistoryManager&);  // Not implemented.
  void operator=(const vtkHistoryManager&);  // Not implemented.

  unsigned int CurrentOutput; //!< Actual iteration
  std::vector<vtkPolyData*> Inputs;
};

#endif
