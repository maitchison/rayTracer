#include "TGAWriter.h"

bool write_truecolor_tga( const string& filename, uint32_t* data, unsigned width, unsigned height )
{
  ofstream tgafile( filename.c_str(), ios::binary );
  if (!tgafile) return false;

  // The image header
  uint8_t header[ 18 ] = { 0 };
  header[  2 ] = 2;  // truecolor
  header[ 12 ] =  width        & 0xFF;
  header[ 13 ] = (width  >> 8) & 0xFF;
  header[ 14 ] =  height       & 0xFF;
  header[ 15 ] = (height >> 8) & 0xFF;
  header[ 16 ] = 24;  // bits per pixel

  tgafile.write( (const char*)header, 18 );

  // The image data is stored bottom-to-top, left-to-right
  for (int y = 0; y < height; y++)
  for (int x = 0; x < width; x++)
    {
    tgafile.put( (char)(data[ (y * width) + x ] >> 16 & 0xFF));
    tgafile.put( (char)(data[ (y * width) + x ] >> 8 & 0xFF));
    tgafile.put( (char)(data[ (y * width) + x ] >> 0 & 0xFF));
    }

  // The file footer. This part is totally optional.
  static const char footer[ 26 ] =
    "\0\0\0\0"  // no extension area
    "\0\0\0\0"  // no developer directory
    "TRUEVISION-XFILE"  // yep, this is a TGA file
    ".";
  tgafile.write( footer, 26 );

  tgafile.close();
  return true;
}