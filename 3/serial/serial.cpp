#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>

#define BLACK 0
#define WHITE 255
#define RED 6
#define BLUE 7
#define GREEN 8


using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std;

#pragma pack(1)
#pragma once

typedef unsigned char UCHAR;
typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;


class Pixel{
  public:
    UCHAR red, green, blue;
    Pixel(int r, int g, int b){
      red = UCHAR(r);
      blue = UCHAR(b);
      green = UCHAR(g);
    }
    Pixel(){
      red = UCHAR(0);
      green = UCHAR(0);
      blue = UCHAR(0);
    }
};
typedef vector<vector<Pixel>> PixelTable;

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize, PixelTable &pixelt){
  ifstream file(fileName);

  if (file){
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else{
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer, PixelTable &pixelt){
  PixelTable vec(rows, vector<Pixel> (cols));
  pixelt = vec;
  int count = 0;
  int extra = cols % 4;
  for (int i = rows - 1 ; i >= 0; i--){
    count += extra;
    for (int j = cols - 1; j >= 0; j--){
      for (int k = 0; k < 3; k++){
        switch (k){
        case 0:
          // fileReadBuffer[end - count] is the red value
          pixelt[i][j].red = fileReadBuffer[end - count];
          count++;
          break;
        case 1:
          // fileReadBuffer[end - count] is the green value
          pixelt[i][j].green = fileReadBuffer[end - count];
          count++;
          break;
        case 2:
          // fileReadBuffer[end - count] is the blue value
          pixelt[i][j].blue = fileReadBuffer[end - count];
          count++;
          break;
          // go to the next position in the buffer
        }
      }
    }
  }
}


void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, PixelTable pixelt){
  std :: ofstream write(nameOfFileToCreate);
  if (!write) {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 0;
  int extra = cols % 4;
  for (int i = rows - 1 ; i >= 0; i--){
    count += extra;
    for (int j = cols - 1; j >= 0; j--){
      for (int k = 0; k < 3; k++){
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = pixelt[i][j].red;
          count++;
          break;
        case 1:
          fileBuffer[bufferSize - count] = pixelt[i][j].green;
          count++;
          break;
        case 2:
          fileBuffer[bufferSize - count] = pixelt[i][j].blue;
          count++;
          break;
        }
      }
    }
  }
  write.write(fileBuffer, bufferSize);
}

PixelTable horizontalmirrorfilter(PixelTable pixelt){
  for (int i = 0; i < pixelt.size(); i ++){
    for (int j = 0; j < cols / 2; j ++){
      swap(pixelt[i][j], pixelt[i][cols - j - 1]);
    }
  }
  return pixelt;
}

PixelTable verticalmirrorfilter(PixelTable pixelt){
  for (int j = 0; j < cols; j ++){
    for (int i = 0; i < rows / 2; i ++){
      swap(pixelt[i][j], pixelt[rows - i - 1][j]);
    }
  }
  return pixelt;
}

int convolutionhandler(int i, int j, PixelTable &pixelt, int color){
  vector<vector<int>> matrix {{0, -1, 0}, {-1 , 5, -1}, {0, -1, 0}};
  if (color == BLUE){
    int blue = 0;
    blue += (pixelt[i][j].blue * matrix[1][1]);
    blue += (pixelt[i - 1][j].blue * matrix[0][1]);
    blue += (pixelt[i - 1][j - 1].blue * matrix[0][0]);
    blue += (pixelt[i - 1][j + 1].blue * matrix[0][2]);
    blue += (pixelt[i][j - 1].blue * matrix[1][0]);
    blue += (pixelt[i + 1][j - 1].blue * matrix[2][0]);
    blue += (pixelt[i + 1][j].blue * matrix[2][1]);
    blue += (pixelt[i + 1][j + 1].blue * matrix[2][2]);
    blue += (pixelt[i][j + 1].blue * matrix[1][2]);
    if (blue > 255){
      return 255;
    }
    if (blue < 0){
      return 0;
    }
    return blue;  
  }
  if (color == GREEN){
    int green = 0;
    green += (pixelt[i][j].green * matrix[1][1]);
    green += (pixelt[i - 1][j].green * matrix[0][1]);
    green += (pixelt[i - 1][j - 1].green * matrix[0][0]);
    green += (pixelt[i - 1][j + 1].green * matrix[0][2]);
    green += (pixelt[i][j - 1].green * matrix[1][0]);
    green += (pixelt[i + 1][j - 1].green * matrix[2][0]);
    green += (pixelt[i + 1][j].green * matrix[2][1]);
    green += (pixelt[i + 1][j + 1].green * matrix[2][2]);
    green += (pixelt[i][j + 1].green * matrix[1][2]);
    if (green > 255){
      return 255;
    }
    if (green < 0){
      return 0;
    }
    return green;  
  }
  int red = 0;
  red += (pixelt[i][j].red * matrix[1][1]);
  red += (pixelt[i - 1][j].red * matrix[0][1]);
  red += (pixelt[i - 1][j - 1].red * matrix[0][0]);
  red += (pixelt[i - 1][j + 1].red * matrix[0][2]);
  red += (pixelt[i][j - 1].red * matrix[1][0]);
  red += (pixelt[i + 1][j - 1].red * matrix[2][0]);
  red += (pixelt[i + 1][j].red * matrix[2][1]);
  red += (pixelt[i + 1][j + 1].red * matrix[2][2]);
  red += (pixelt[i][j + 1].red * matrix[1][2]);
  if (red > 255){
    return 255;
  }
  if (red < 0){
    return 0;
  }
  return red;  
}

PixelTable sharpenfilter(PixelTable pixelt){
  PixelTable temp = pixelt;
  for (int i = 0; i < rows; i ++){
    for (int j = 0; j < cols; j ++){
      if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1){
        continue;
      }
      pixelt[i][j].red = convolutionhandler(i, j, temp, RED);      
      pixelt[i][j].blue = convolutionhandler(i, j, temp, BLUE);      
      pixelt[i][j].green = convolutionhandler(i, j, temp, GREEN);      
    }
  }
  return pixelt;
}

PixelTable sepiafilter(PixelTable pixelt){
  for (int i = 0; i < rows; i ++){
    for (int j = 0; j < cols; j ++){
      int red = pixelt[i][j].red;
      int green = pixelt[i][j].green;
      int blue = pixelt[i][j].blue;
      int sepiaRed = int(0.393 * red + 0.769 * green + 0.189 * blue);
      int sepiaGreen = int(0.349 * red + 0.686 * green + 0.168 * blue);
      int sepiaBlue = int(0.272 * red + 0.534 * green + 0.131 * blue);
      pixelt[i][j].red = (sepiaRed > 255 ? 255 : sepiaRed);
      pixelt[i][j].green = (sepiaGreen > 255 ? 255 : sepiaGreen);
      pixelt[i][j].blue = (sepiaBlue > 255 ? 255 : sepiaBlue);
    }
  }
  return pixelt;
}

PixelTable Xfilter(PixelTable pixelt){
  float rc = (float)rows / cols;
  for (int i = 0; i < rows; i ++){
    for (int j = 0; j < cols; j ++){
      if (i == int(j * (rc))){
        pixelt[i][j].red = WHITE;
        pixelt[i][j].green = WHITE;
        pixelt[i][j].blue = WHITE;
      }
      if (i == int(j * (-rc) + rows)){
        pixelt[i][j].red = WHITE;
        pixelt[i][j].green = WHITE;
        pixelt[i][j].blue = WHITE;
      }
    }
  }
  return pixelt;
}


int main(int argc, char *argv[])
{
  auto start = chrono :: high_resolution_clock::now();
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  PixelTable pixelt;
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize, pixelt))
  {
    cout << "File read error" << endl;
    return 1;
  }

  // read input file
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer, pixelt);
  // apply filters
  pixelt = horizontalmirrorfilter(pixelt);
  pixelt = verticalmirrorfilter(pixelt);
  pixelt = sharpenfilter(pixelt);
  pixelt = sepiafilter(pixelt);
  pixelt = Xfilter(pixelt);
  // write output file
  char* outname = new char[20];
  strcpy(outname, "outputserial.bmp");
  writeOutBmp24(fileBuffer, outname, bufferSize, pixelt);
  auto end = chrono :: high_resolution_clock::now();
	auto duration = chrono :: duration_cast<chrono :: microseconds>(end - start);
  cout << "Serial Execution Time: " << duration.count() / double(1e6) << endl;

  return 0;
}