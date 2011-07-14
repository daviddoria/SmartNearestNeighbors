
  // Setup an image to visualize the input
  {
  typedef itk::Image< unsigned char, 2>  ImageType;

  ImageType::IndexType start;
  start.Fill(0);

  ImageType::SizeType size;
  size.Fill(100);

  ImageType::RegionType region(start,size);

  ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  ImageType::IndexType ind;
  ind[0] = 50;
  ind[1] = 50;
  image->SetPixel(ind, 255);

  ind[0] = 25;
  ind[1] = 25;
  image->SetPixel(ind, 255);

  ind[0] = 75;
  ind[1] = 25;
  image->SetPixel(ind, 255);

  ind[0] = 25;
  ind[1] = 75;
  image->SetPixel(ind, 255);
  
  ind[0] = 75;
  ind[1] = 75;
  image->SetPixel(ind, 255);

  typedef  itk::ImageFileWriter< ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("image.png");
  writer->SetInput(image);
  writer->Update();
  }
