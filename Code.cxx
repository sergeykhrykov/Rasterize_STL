#include "itkMesh.h"
#include "itkMeshFileReader.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkCastImageFilter.h"
#include "itkTriangleMeshToBinaryImageFilter.h"

int main( int argc, char* argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: "<< std::endl;
    std::cerr << argv[0];
	std::cerr << " <InputMeshName> [<InputMeshName> <InputMeshName> ... ]";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef double MeshPixelType;

  typedef itk::Mesh< MeshPixelType, Dimension > MeshType;

  typedef itk::MeshFileReader< MeshType >  MeshReaderType;
  
  
  typedef unsigned char                           InputPixelType;
  typedef itk::Image< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType >  ImageReaderType;

  //ImageReaderType::Pointer imageReader = ImageReaderType::New();
  //imageReader->SetFileName( inputImageName );

  typedef unsigned char                             OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;
    
  typedef itk::TriangleMeshToBinaryImageFilter< MeshType, OutputImageType > FilterType;
 
  
  for (int i = 1; i < argc; ++i)
  {	  
	  const char * inputMeshName = argv[i];
	  std::string outputImageName = argv[i];
	  outputImageName += ".mha";

	  MeshReaderType::Pointer meshReader = MeshReaderType::New();
	  meshReader->SetFileName(inputMeshName);

	  FilterType::Pointer filter = FilterType::New();

	  InputImageType::SizeType size;
	  size[0] = 500;
	  size[1] = 500;
	  size[2] = 500;
	  filter->SetSize(size);

	  InputImageType::SpacingType spacing;
	  spacing[0] = .2;
	  spacing[1] = .2;
	  spacing[2] = .2;
	  filter->SetSpacing(spacing);

	  InputImageType::PointType origin;
	  origin[0] = -46;
	  origin[1] = -17;
	  origin[2] = -2;
	  filter->SetOrigin(origin);
	  filter->SetInput(meshReader->GetOutput());
	  filter->SetInsideValue(itk::NumericTraits< OutputPixelType >::max());

	  std::cout << "Rasterizing file: " << inputMeshName << "...";
	  try
	  {
		  filter->Update();
	  }
	  catch (itk::ExceptionObject & error)
	  {
		  std::cerr << "Error: " << error << std::endl;
		  return EXIT_FAILURE;
	  }

	  std::cout << "Done" << std::endl;

	  std::cout << "Writing...";
	  typedef itk::ImageFileWriter< OutputImageType > WriterType;
	  WriterType::Pointer writer = WriterType::New();
	  writer->SetFileName(outputImageName.c_str());
	  writer->SetInput(filter->GetOutput());
	  try
	  {
		  writer->Update();
	  }
	  catch (itk::ExceptionObject & error)
	  {
		  std::cerr << "Error: " << error << std::endl;
		  return EXIT_FAILURE;
	  }
	  std::cout << "Done" << std::endl;
	  
  }

  return EXIT_SUCCESS;
}
