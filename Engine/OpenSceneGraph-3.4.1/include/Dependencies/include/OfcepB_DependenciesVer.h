#ifndef __OfcepB_DependencieVER_H__
#define __OfcepB_DependencieVER_H__

#if !defined(_AFXDLL)
	#error you must make this DLL as  MFC Lib£¡(in OfcepB_DependencieVer.h)
#endif

#if defined(_DEBUG)
  #if defined(_AFXEXT)
    #define AUTOLIBNAMEzlib "zlib_Ofcepd.lib"
    #define AUTOLIBNAMEzziplib "zziplibd.lib"
    #define AUTOLIBNAMEFreetype "freetyped.lib"
    //#define AUTOLIBNAMELibJPEG		"LibJPEGd.lib"
    //#define AUTOLIBNAMELibMNG		"LibMNGd.lib"
    //#define AUTOLIBNAMELibOpenJPEG  "LibOpenJPEGd.lib"
   // #define AUTOLIBNAMELibPNG       "LibPNGd.lib"
    //#define AUTOLIBNAMELibTIFF		 "LibTIFFd.lib"
    //#define AUTOLIBNAMEOpenEXR		 "OpenEXRd.lib"
    #define AUTOLIBNAMEFreeImage "FreeImaged.lib"

  #else
    #define AUTOLIBNAMEzlib "zlib_Ofcep.lib"
    #define AUTOLIBNAMEzziplib "zziplib.lib"
    #define AUTOLIBNAMEFreetype "freetype.lib"
   /* #define AUTOLIBNAMELibJPEG		"LibJPEG.lib"
    #define AUTOLIBNAMELibMNG		"LibMNG.lib"
    #define AUTOLIBNAMELibOpenJPEG  "LibOpenJPEG.lib"
    #define AUTOLIBNAMELibPNG       "LibPNG.lib"
    #define AUTOLIBNAMELibTIFF		 "LibTIFF.lib"
    #define AUTOLIBNAMEOpenEXR		 "OpenEXR.lib"*/
    #define AUTOLIBNAMEFreeImage "FreeImage.lib"
  #endif
#else
	#if defined(_AFXEXT)
    #define AUTOLIBNAMEzlib "zlib_Ofcep.lib"
    #define AUTOLIBNAMEzziplib "zziplib.lib"
    #define AUTOLIBNAMEFreetype "freetype.lib"
  /*  #define AUTOLIBNAMELibJPEG		"LibJPEG.lib"
    #define AUTOLIBNAMELibMNG		"LibMNG.lib"
    #define AUTOLIBNAMELibOpenJPEG  "LibOpenJPEG.lib"
    #define AUTOLIBNAMELibPNG       "LibPNG.lib"
    #define AUTOLIBNAMELibTIFF		 "LibTIFF.lib"
    #define AUTOLIBNAMEOpenEXR		 "OpenEXR.lib"*/
    #define AUTOLIBNAMEFreeImage "FreeImage.lib"    
	#else   
    #define AUTOLIBNAMEzlib "zlib_Ofcep.lib"
    #define AUTOLIBNAMEzziplib "zziplib.lib"
    #define AUTOLIBNAMEFreetype "freetype.lib"
   /* #define AUTOLIBNAMELibJPEG		"LibJPEG.lib"
    #define AUTOLIBNAMELibMNG		"LibMNG.lib"
    #define AUTOLIBNAMELibOpenJPEG  "LibOpenJPEG.lib"
    #define AUTOLIBNAMELibPNG       "LibPNG.lib"
    #define AUTOLIBNAMELibTIFF		 "LibTIFF.lib"
    #define AUTOLIBNAMEOpenEXR		 "OpenEXR.lib"*/
    #define AUTOLIBNAMEFreeImage "FreeImage.lib"
	#endif
#endif

// Perform autolink here:
#pragma message( "automatically link with (" AUTOLIBNAMEzlib ")")
#pragma comment(lib, AUTOLIBNAMEzlib)

#pragma message( "automatically link with (" AUTOLIBNAMEzziplib ")")
#pragma comment(lib, AUTOLIBNAMEzziplib)

#pragma message( "automatically link with (" AUTOLIBNAMEFreetype ")")
#pragma comment(lib, AUTOLIBNAMEFreetype)

//#pragma message( "automatically link with (" AUTOLIBNAMELibJPEG ")")
//#pragma comment(lib, AUTOLIBNAMELibJPEG)

//#pragma message( "automatically link with (" AUTOLIBNAMELibMNG ")")
//#pragma comment(lib, AUTOLIBNAMELibMNG)


//#pragma message( "automatically link with (" AUTOLIBNAMELibOpenJPEG ")")
//#pragma comment(lib,AUTOLIBNAMELibOpenJPEG )


//#pragma message( "automatically link with (" AUTOLIBNAMELibPNG ")")
//#pragma comment(lib, AUTOLIBNAMELibPNG)


//#pragma message( "automatically link with (" AUTOLIBNAMELibTIFF ")")
//#pragma comment(lib, AUTOLIBNAMELibTIFF)


//#pragma message( "automatically link with (" AUTOLIBNAMEOpenEXR ")")
//#pragma comment(lib, AUTOLIBNAMEOpenEXR)

#pragma message( "automatically link with (" AUTOLIBNAMEFreeImage ")")
#pragma comment(lib, AUTOLIBNAMEFreeImage)

#undef AUTOLIBNAMEzlib
#undef AUTOLIBNAMEzziplib
#undef AUTOLIBNAMEFreeImage
#undef AUTOLIBNAMEFreetype
//#undef AUTOLIBNAMELibJPEG		
//#undef AUTOLIBNAMELibMNG		
//#undef AUTOLIBNAMELibOpenJPEG  
/*#undef AUTOLIBNAMELibPNG  */     
//#undef AUTOLIBNAMELibTIFF		
//#undef AUTOLIBNAMEOpenEXR		
#endif // End of __OfcepB_DependencieVER_H__



