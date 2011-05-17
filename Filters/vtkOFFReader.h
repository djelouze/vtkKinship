#ifndef __vtkOFFReader_h
#define __vtkOFFReader_h

#include "vtkPolyDataAlgorithm.h"

class VTK_EXPORT vtkOFFReader : public vtkPolyDataAlgorithm 
{
public:
  vtkTypeMacro(vtkOFFReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with merging set to true.
  static vtkOFFReader *New();

  // Description:
  // Specify file name of stereo lithography file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // A simple, non-exhaustive check to see if a file is a valid ply file.
  static int CanReadFile(const char *filename);

protected:
  vtkOFFReader();
  ~vtkOFFReader();

  char *FileName;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
  vtkOFFReader(const vtkOFFReader&);  // Not implemented.
  void operator=(const vtkOFFReader&);  // Not implemented.
};

#endif
