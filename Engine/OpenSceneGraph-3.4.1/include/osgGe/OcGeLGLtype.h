#ifndef __LGLTYPE_H__
#define __LGLTYPE_H__

/******  DEFINE IN BORLANDC++ 3.0 MATH.H  *******/
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401

/****** by L.C.G ***********************************/
#define M_DEG_TO_RAD  0.0174532925199432950   // ( M_PI  / 180.0 )
#define M_RAD_TO_DEG  57.2957795130823230	  // ( 180.0 / M_PI	 )

/****** by L.C.G **************************/  
#define RAD(Degree) ((Degree) * M_DEG_TO_RAD)
#define DEG(Rad)    ((Rad   ) * M_RAD_TO_DEG) 

/**************************************************
@@	From VC++ FLOAT.H
@@	Smallest value such that 1.0+LDBL_EPSILON != 1.0  
@@  Same as define macro DBL_EPSILON in FLOAT.H
**/
#include "Float.h"
#define LGLDBL_EPSILON              DBL_EPSILON * 100
//#define LGLDBL_EPSILON              1.0E-9
#define LGLDBL_EPSILON_FOR_DISPLAY         1.0E-7
#define LGLDBL_EPSILON_FOR_DISPLAY_OFFSET  1.0E-6
#define LGLDBL_MBR_PRECISION        1.0E-9
//#define LGL_LINE_INTERSECT_EPSILON   10E-9 // Line Intersect 조건 검사때의 정도 
#define LGL_LINE_INTERSECT_EPSILON LGLDBL_EPSILON 
#define LGL_ROUND_UP_INT     0.5 // LDOUBLE에서 LINT로의 형변환을 위해서 더해지는 값
#define LGL_ROUND_UP_DOUBLE  0.5 // LDOUBLE값의 반올림을 위해 더해지는 값
#define LGLDBL_AREA_MIN           -DBL_MAX / 10.0
#define LGLDBL_AREA_MAX            DBL_MAX / 10.0

//{
//extern double  V_LGLDBL_EPSILON              ;//1.0E-9
//extern double  V_LGLDBL_EPSILON_FOR_DISPLAY  ;//LGLDBL_EPSILON * 100
//extern double  V_LGLDBL_MBR_PRECISION        ;//1.0E-9
////#define LGL_LINE_INTERSECT_EPSILON   10E-9 // Line Intersect 조건 검사때의 정도 
//extern double  V_LGL_LINE_INTERSECT_EPSILON  ;//LGLDBL_EPSILON 
//extern double  V_LGL_ROUND_UP_INT            ;//0.5 // LDOUBLE에서 LINT로의 형변환을 위해서 더해지는 값
//extern double  V_LGL_ROUND_UP_DOUBLE         ;//0.5 // LDOUBLE값의 반올림을 위해 더해지는 값
//extern double  V_LGLDBL_AREA_MIN             ;//-DBL_MAX / 10.0
//extern double  V_LGLDBL_AREA_MAX             ;//DBL_MAX / 10.0
//}

//{
//double  V_LGLDBL_EPSILON              = 1.0E-15;
#define  V_LGLDBL_EPSILON               LGLDBL_EPSILON      
#define  V_LGLDBL_EPSILON_FOR_DISPLAY   (LGLDBL_EPSILON * 100)
#define  V_LGLDBL_MBR_PRECISION         (V_LGLDBL_EPSILON * 10)
//#define LGL_LINE_INTERSECT_EPSILON   10E-9 // Line Intersect 조건 검사때의 정도 
#define  V_LGL_LINE_INTERSECT_EPSILON   (LGLDBL_EPSILON * 10000)
//double  V_LGL_LINE_INTERSECT_EPSILON  = 1.0E-7;
#define  V_LGL_ROUND_UP_INT             0.5 // LDOUBLE에서 LINT로의 형변환을 위해서 더해지는 값
#define  V_LGL_ROUND_UP_DOUBLE          0.5 // LDOUBLE값의 반올림을 위해 더해지는 값
#define  V_LGLDBL_AREA_MIN              (-DBL_MAX / 10.0)
#define  V_LGLDBL_AREA_MAX              (DBL_MAX / 10.0)

#define DEG2RAD 0.017453277777777777777777777777778 //(ONE_PI / 180.0f)

//}



#define LFLOAT                float
#define LDOUBLE               double
#define LLONG_DOUBLE          long double 

#define LSHORT                short int
#define LINT                  int
#define LUINT                 T_KEY 
#define LLONG                 long INT_PTR
#define LUNSIGNED_LONG        unsigned long INT_PTR 

#define LBYTE                 BYTE
#define LCHAR                 char 
#define LUNSIGNED_CHAR        unsigned char
#define LBOOL                 BOOL
#define LPOSITION             POSITION

#define LLOGPEN               LOGPEN
#define LLOGBRUSH             LOGBRUSH
#define LLOGFONT              LOGFONT
#ifndef REM
	#define REM(param)
#endif

#endif


/*  DEFINE IN VC++ FLOAT.H
#define DBL_DIG 	15								// # of decimal digits of precision  
#define DBL_EPSILON	2.2204460492503131e-016			// smallest such that 1.0+DBL_EPSILON != 1.0  
#define DBL_MANT_DIG	53							// # of bits in mantissa  
#define DBL_MAX 	1.7976931348623158e+308			// max value  
#define DBL_MAX_10_EXP	308							// max decimal exponent  
#define DBL_MAX_EXP	1024							// max binary exponent  
#define DBL_MIN 	2.2250738585072014e-308			// min positive value  
#define DBL_MIN_10_EXP	(-307)						// min decimal exponent  
#define DBL_MIN_EXP	(-1021) 						// min binary exponent  
#define _DBL_RADIX	2								// exponent radix  
#define _DBL_ROUNDS	1								// addition rounding: near  

#define FLT_DIG 	6								// # of decimal digits of precision  
#define FLT_EPSILON	1.192092896e-07F				// smallest such that 1.0+FLT_EPSILON != 1.0  
#define FLT_GUARD	0
#define FLT_MANT_DIG	24							// # of bits in mantissa  
#define FLT_MAX 	3.402823466e+38F				// max value  
#define FLT_MAX_10_EXP	38							// max decimal exponent  
#define FLT_MAX_EXP	128								// max binary exponent  
#define FLT_MIN 	1.175494351e-38F				// min positive value  
#define FLT_MIN_10_EXP	(-37)						// min decimal exponent  
#define FLT_MIN_EXP	(-125)							// min binary exponent  
#define FLT_NORMALIZE	0
#define FLT_RADIX	2								// exponent radix  
#define FLT_ROUNDS	1								// addition rounding: near  

#ifndef _M_M68K
#define LDBL_DIG	DBL_DIG 						// # of decimal digits of precision  
#define LDBL_EPSILON	DBL_EPSILON					// smallest such that 1.0+LDBL_EPSILON != 1.0  
#define LDBL_MANT_DIG	DBL_MANT_DIG				// # of bits in mantissa  
#define LDBL_MAX	DBL_MAX 						// max value  
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP				// max decimal exponent  
#define LDBL_MAX_EXP	DBL_MAX_EXP					// max binary exponent  
#define LDBL_MIN	DBL_MIN 						// min positive value  
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP				// min decimal exponent  
#define LDBL_MIN_EXP	DBL_MIN_EXP					// min binary exponent  
#define _LDBL_RADIX	DBL_RADIX						// exponent radix  
#define _LDBL_ROUNDS	DBL_ROUNDS					// addition rounding: near  
#else
#define LDBL_DIG	18								// # of decimal digits of precision  
#define LDBL_EPSILON	1.08420217248550443412e-019L// smallest such that 1.0+LDBL_EPSILON != 1.0  
#define LDBL_MANT_DIG	64							// # of bits in mantissa  
#define LDBL_MAX	1.189731495357231765e+4932L  	// max value  
#define LDBL_MAX_10_EXP 4932						// max decimal exponent  
#define LDBL_MAX_EXP	16384						// max binary exponent  
#define LDBL_MIN	3.3621031431120935063e-4932L 	// min positive value  
#define LDBL_MIN_10_EXP (-4931) 					// min decimal exponent  
#define LDBL_MIN_EXP	(-16381)					// min binary exponent  
#define _LDBL_RADIX	2								// exponent radix  
#define _LDBL_ROUNDS	1							// addition rounding: near  
#endif
*/