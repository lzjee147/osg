
#include "Common/Type.h"
namespace cn
{


//定义误差的最小值
#define _MIN_ERROR_THRE	0.00001f
#define _MIN_ERROR_THRE_D 0.00000001

//表示矩阵计算相关预定义
#define _GEN_SUB_DETERMINANT_2(obj,i1,i2,j1,j2) \
	(obj._##i1##j1*obj._##i2##j2 - obj._##i1##j2*obj._##i2##j1)

#define _GEN_SUB_DETERMINANT_3(obj,i1,i2,i3,j1,j2,j3) \
	(obj._##i1##j1*_GEN_SUB_DETERMINANT_2(obj,i2,i3,j2,j3) - \
	obj._##i1##j2*_GEN_SUB_DETERMINANT_2(obj,i2,i3,j1,j3) + \
	obj._##i1##j3*_GEN_SUB_DETERMINANT_2(obj,i2,i3,j1,j2))

#define _GEN_SUB_DETERMINANT_4(obj,i1,i2,i3,i4,j1,j2,j3,j4) \
	(obj._##i1##j1*_GEN_SUB_DETERMINANT_3(obj,i2,i3,i4,j2,j3,j4) - \
	obj._##i1##j2*_GEN_SUB_DETERMINANT_3(obj,i2,i3,i4,j1,j3,j4) + \
	obj._##i1##j3*_GEN_SUB_DETERMINANT_3(obj,i2,i3,i4,j1,j2,j4) - \
	obj._##i1##j4*_GEN_SUB_DETERMINANT_3(obj,i2,i3,i4,j1,j2,j3))

#define _GEN_SUB_DETERMINANT_5(obj,i1,i2,i3,i4,i5,j1,j2,j3,j4,j5) \
	(obj._##i1##j1*_GEN_SUB_DETERMINANT_4(obj,i2,i3,i4,i5,j2,j3,j4,j5) - \
	obj._##i1##j2*_GEN_SUB_DETERMINANT_4(obj,i2,i3,i4,i5,j1,j3,j4,j5) + \
	obj._##i1##j3*_GEN_SUB_DETERMINANT_4(obj,i2,i3,i4,i5,j1,j2,j4,j5) - \
	obj._##i1##j4*_GEN_SUB_DETERMINANT_4(obj,i2,i3,i4,i5,j1,j2,j3,j5) + \
	obj._##i1##j5*_GEN_SUB_DETERMINANT_4(obj,i2,i3,i4,i5,j1,j2,j3,j4))



u128 u128::m_uZero;


u128::u128()
{
	u_0_63 =0; u_64_127 =0;
}


u128::u128(const u128 &src)
{
	u_0_63 =src.u_0_63; u_64_127 =src.u_64_127;
}


u128 &u128::operator = (const u128 &src)
{
	u_0_63 =src.u_0_63; u_64_127 =src.u_64_127;
	return (*this);
}


u128 &u128::operator () (const u128 &src)
{
	u_0_63 =src.u_0_63; u_64_127 =src.u_64_127;
	return (*this);
}


bool operator == (const u128 &src1, const u128 &src2)
{
	return (src1.u_0_63==src2.u_0_63) && (src1.u_64_127==src2.u_64_127);
}


bool operator != (const u128 &src1, const u128 &src2)
{
	return (src1.u_0_63!=src2.u_0_63) || (src1.u_64_127!=src2.u_64_127);
}


bool operator <= (const u128 &src1, const u128 &src2)
{
	if(src1.u_64_127 != src2.u_64_127){return (src1.u_64_127 <= src2.u_64_127);}else{return (src1.u_0_63 <= src2.u_0_63);}
}


bool operator >= (const u128 &src1, const u128 &src2)
{
	if(src1.u_64_127 != src2.u_64_127){return (src1.u_64_127 >= src2.u_64_127);}else{return (src1.u_0_63 >= src2.u_0_63);}
}


bool operator < (const u128 &src1, const u128 &src2)
{
	if(src1.u_64_127 != src2.u_64_127){return (src1.u_64_127 < src2.u_64_127);}else{return (src1.u_0_63 < src2.u_0_63);}
}


bool operator > (const u128 &src1, const u128 &src2)
{
	if(src1.u_64_127 != src2.u_64_127){return (src1.u_64_127 > src2.u_64_127);}else{return (src1.u_0_63 > src2.u_0_63);}
}


/************************/
/*以下定义各种自定义类型*/
/************************/


Vec2 Vec2::m_vZero(0.0f, 0.0f);
Vec2 Vec2::m_vOne(1.0f, 1.0f);


//表示二维向量
Vec2::Vec2()
{
	x =y =0.0f;
}

Vec2::Vec2(float fX, float fY): x(fX),
                                y(fY)

{
}

Vec2 Vec2::operator + (const Vec2 &src) const
{
	return (Vec2 &)((D3DXVECTOR2 &)(*this) + (D3DXVECTOR2 &)src);
}

Vec2 Vec2::operator - (const Vec2 &src) const
{
	return (Vec2 &)((D3DXVECTOR2 &)(*this) - (D3DXVECTOR2 &)src);
}

Vec2 Vec2::operator * (float src) const
{
	return (Vec2 &)((D3DXVECTOR2 &)(*this) * src);
}

Vec2 Vec2::operator / (float src) const
{
	return (Vec2 &)((D3DXVECTOR2 &)(*this) / src);
}

Vec2 Vec2::operator * (const Vec2 &src) const
{
	float pTemp[2];
	pTemp[0] =x*src.x;
	pTemp[1] =y*src.y;
	return (Vec2 &)(*pTemp);
}

Vec2 Vec2::operator / (const Vec2 &src) const
{
	float pTemp[2];
	pTemp[0] =x/src.x;
	pTemp[1] =y/src.y;
	return (Vec2 &)(*pTemp);
}

bool Vec2::operator == (const Vec2 &src) const
{
	return ::memcmp(this, &src, sizeof(Vec2)) == 0;
}

bool Vec2::operator != (const Vec2 &src) const
{
	return ::memcmp(this, &src, sizeof(Vec2)) != 0;
}

Vec2 &Vec2::operator () (float fX, float fY)
{
	x =fX;
	y =fY;
	return *this;
}

Vec2 &Vec2::operator += (const Vec2 &src)
{
	*this =*this + src;
	return *this;
}

Vec2 &Vec2::operator -= (const Vec2 &src)
{
	*this =*this - src;
	return *this;
}

Vec2 &Vec2::operator *= (float src)
{
	*this =*this * src;
	return *this;
}

Vec2 &Vec2::operator /= (float src)
{
	*this =*this / src;
	return *this;
}

Vec2 &Vec2::operator *= (const Vec2 &src)
{
	x =x*src.x;
	y =y*src.y;
	return *this;
}

Vec2 &Vec2::operator /= (const Vec2 &src)
{
	x =x/src.x;
	y =y/src.y;
	return *this;
}

Vec2 operator * (float src1, const Vec2 &src2)
{
	return (Vec2 &)(src1 * (D3DXVECTOR2 &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Vec2 &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y;
	ostr<<strstream.str();
	return ostr;
}

float Vec2::Length() const
{
	return ::D3DXVec2Length((const D3DXVECTOR2 *)(this));
}

float Vec2::LengthSq() const
{
	return ::D3DXVec2LengthSq((const D3DXVECTOR2 *)(this));
}

void Vec2::Normalize(Vec2 &ret) const
{
	::D3DXVec2Normalize((D3DXVECTOR2 *)(&ret), (const D3DXVECTOR2 *)(this));
}

Vec2 &Vec2::NormalizeInPlace()
{
	::D3DXVec2Normalize((D3DXVECTOR2 *)(this), (const D3DXVECTOR2 *)(this));
	return *this;
}

Vec2D Vec2D::m_vZero(0.0, 0.0);
Vec2D Vec2D::m_vOne(1.0, 1.0);

Vec2D::Vec2D()
{
	x =y =0.0;
}

Vec2D::Vec2D(double dX, double dY)
{
	x =dX; y =dY;
}

Vec2D::Vec2D(const Vec2D &src)
{
	x =src.x; y =src.y;
}

Vec2D::Vec2D(const Vec2 &src)
{
	x =double(src.x); y =double(src.y);
}

Vec2D &Vec2D::operator = (const Vec2D &src)
{
	x =src.x; y =src.y;
	return (*this);
}

Vec2D &Vec2D::operator = (const Vec2 &src)
{
	x =double(src.x); y =double(src.y);
	return (*this);
}

Vec2D Vec2D::operator + (const Vec2D &src) const
{
	Vec2D cTemp(x+src.x, y+src.y);
	return cTemp;
}

Vec2D Vec2D::operator - (const Vec2D &src) const
{
	Vec2D cTemp(x-src.x, y-src.y);
	return cTemp;
}

Vec2D Vec2D::operator * (double src) const
{
	Vec2D cTemp(x*src, y*src);
	return cTemp;
}

Vec2D Vec2D::operator / (double src) const
{
	Vec2D cTemp(x/src, y/src);
	return cTemp;
}

Vec2D Vec2D::operator * (const Vec2D &src) const
{
	Vec2D cTemp(x*src.x, y*src.y);
	return cTemp;
}

Vec2D Vec2D::operator / (const Vec2D &src) const
{
	Vec2D cTemp(x/src.x, y/src.y);
	return cTemp;
}

bool Vec2D::operator == (const Vec2D &src) const
{
	return (x==src.x)&&(y==src.y);
}

bool Vec2D::operator != (const Vec2D &src) const
{
	return (x!=src.x)||(y!=src.y);
}

Vec2D &Vec2D::operator () (double dX, double dY)
{
	x =dX; y =dY;
	return (*this);
}

Vec2D &Vec2D::operator += (const Vec2D &src)
{
	x +=src.x; y +=src.y;
	return (*this);
}

Vec2D &Vec2D::operator -= (const Vec2D &src)
{
	x -=src.x; y -=src.y;
	return (*this);
}

Vec2D &Vec2D::operator *= (double src)
{
	x *=src; y *=src;
	return (*this);
}

Vec2D &Vec2D::operator /= (double src)
{
	x /=src; y /=src;
	return (*this);
}

Vec2D &Vec2D::operator *= (const Vec2D &src)
{
	x *=src.x; y *=src.y;
	return (*this);
}

Vec2D &Vec2D::operator /= (const Vec2D &src)
{
	x /=src.x; y /=src.y;
	return (*this);
}

Vec2D operator * (double src1, const Vec2D &src2)
{
	Vec2D cTemp(src1*src2.x, src1*src2.y);
	return cTemp;
}

std::ostream &operator << (std::ostream &ostr, const Vec2D &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y;
	ostr<<strstream.str();
	return ostr;
}

Vec2 Vec2D::ToVec2() const
{
	Vec2 vTemp;
	vTemp(float(x), float(y));
	return vTemp;
}

void Vec2D::ToVec2(Vec2 &ret) const
{
	ret(float(x), float(y));
}

double Vec2D::Length() const
{
	return ::sqrt(x*x+y*y);
}

double Vec2D::LengthSq() const
{
	return x*x+y*y;
}

void Vec2D::Normalize(Vec2D &ret) const
{
	double dLen =::sqrt(x*x+y*y);
	if(dLen > 0.0){
		ret(x/dLen, y/dLen);
	}else{
		ret(x, y);
	}
}

Vec2D &Vec2D::NormalizeInPlace()
{
	double dLen =::sqrt(x*x+y*y);
	if(dLen > 0.0){
		x /=dLen; y /=dLen;
	}
	return (*this);
}

Vec3 Vec3::m_vZero(0.0f, 0.0f, 0.0f);
Vec3 Vec3::m_vOne(1.0f, 1.0f, 1.0f);

// 三维向量
Vec3::Vec3()
{
	x =y =z =0.0f;
}

Vec3::Vec3(float fX, float fY, float fZ):x(fX),
                                         y(fY),
                                         z(fZ)
{

}

Vec3::Vec3(const Vec3 &src)
{
	x =src.x; y =src.y; z =src.z;
}

//Vec3::Vec3(const Vec3D &src)
//{
//	x =float(src.x); y =float(src.y); z =float(src.z);
//}

Vec3 &Vec3::operator = (const Vec3 &src)
{
	x =src.x; y =src.y; z =src.z;
	return *this;
}

//Vec3 &Vec3::operator = (const Vec3D &src)
//{
//	x =float(src.x); y =float(src.y); z =float(src.z);
//	return *this;
//}

Vec3 Vec3::operator + (const Vec3 &src) const
{
	return (Vec3 &)((D3DXVECTOR3 &)(*this) + (D3DXVECTOR3 &)src);
}

Vec3 Vec3::operator - (const Vec3 &src) const
{
	return (Vec3 &)((D3DXVECTOR3 &)(*this) - (D3DXVECTOR3 &)src);
}

Vec3 Vec3::operator *(float src) const
{
	return (Vec3 &)((D3DXVECTOR3 &)(*this) * src);
}

Vec3 Vec3::operator /(float src) const
{
	return (Vec3 &)((D3DXVECTOR3 &)(*this) / src);
}

Vec3 Vec3::operator *(const Vec3 &src) const
{
	float pTemp[3];
	pTemp[0] =x*src.x;
	pTemp[1] =y*src.y;
	pTemp[2] =z*src.z;
	return (Vec3 &)(*pTemp);
}

Vec3 Vec3::operator /(const Vec3 &src) const
{
	float pTemp[3];
	pTemp[0] =x/src.x;
	pTemp[1] =y/src.y;
	pTemp[2] =z/src.z;
	return (Vec3 &)(*pTemp);
}

bool Vec3::operator ==(const Vec3 &src) const
{
	return ::memcmp(this,&src,sizeof(Vec3)) == 0;
}

bool Vec3::operator !=(const Vec3 &src) const
{
	return ::memcmp(this,&src,sizeof(Vec3)) != 0;
}

Vec3 &Vec3::operator ()(float fX, float fY, float fZ)
{
	x = fX;
	y = fY;
	z = fZ;
	return *this;
}

Vec3 &Vec3::operator +=(const Vec3 &src)
{
	*this = *this + src;
	return *this;
}

Vec3 &Vec3::operator -=(const Vec3 &src)
{
	*this = *this - src;
	return *this;
}

Vec3 &Vec3::operator *=(float src)
{
	*this = *this * src;
	return *this;
}

Vec3 &Vec3::operator /=(float src)
{
	*this = *this / src;
	return *this;
}
Vec3 &Vec3::operator *=(const Vec3 &src)
{
	x = x * src.x;
	y = y * src.y;
	z = z * src.z;
	return *this;
}

Vec3 &Vec3::operator /=(const Vec3 &src)
{
	x = x / src.x;
	y = y / src.y;
	z = z / src.z;
	return *this;
}

Vec3 operator * (float src1, const Vec3 &src2)
{
	return (Vec3 &)(src1 * (D3DXVECTOR3 &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Vec3 &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y<<" "<<src.z;
	ostr<<strstream.str();
	return ostr;
}

float Vec3::Length() const
{
	return ::D3DXVec3Length((const D3DXVECTOR3 *)(this));
}

float Vec3::LengthSq() const
{
	return ::D3DXVec3LengthSq((const D3DXVECTOR3 *)(this));
}

void Vec3::Normalize(Vec3 &ret) const
{
	::D3DXVec3Normalize((D3DXVECTOR3 *)(&ret),(const D3DXVECTOR3 *)(this));
}

Vec3 &Vec3::NormalizeInPlace()
{
	::D3DXVec3Normalize((D3DXVECTOR3 *)(this),(const D3DXVECTOR3 *)(this));
	return *this;
}

void Vec3::Unproject(Vec3 &ret, 
					 const Viewport &vpSrc, 
					 const Mat4x4 *lpProj, 
					 const Mat4x4 *lpView, 
					 const Mat4x4 *lpWorld) const
{
	::D3DXVec3Unproject((D3DXVECTOR3 *)(&ret),
		(const D3DXVECTOR3 *)(this),
		(const D3DVIEWPORT9 *)(&vpSrc),
		(const D3DXMATRIX *)(lpProj),
		(const D3DXMATRIX *)(lpView),
		(const D3DXMATRIX *)(lpWorld));
}

Vec3 &Vec3::UnprojectInPlace(const Viewport &vpSrc, 
							 const Mat4x4 *lpProj, 
							 const Mat4x4 *lpView, 
							 const Mat4x4 *lpWorld) 
{
	::D3DXVec3Unproject((D3DXVECTOR3 *)(this),
		(const D3DXVECTOR3 *)(this),
		(const D3DVIEWPORT9 *)(&vpSrc),
		(const D3DXMATRIX *)(lpProj),
		(const D3DXMATRIX *)(lpView),
		(const D3DXMATRIX *)(lpWorld));
	return *this;
}

void Vec3::TransformCoord(const Mat4x4 &matTrans, Vec3 &ret) const
{
	::D3DXVec3TransformCoord((D3DXVECTOR3 *)(&ret),(const D3DXVECTOR3 *)(this),(const D3DXMATRIX *)(&matTrans));		                    							 
}

Vec3 &Vec3::TransformCoordInPlace(const Mat4x4 &matTrans)
{
	::D3DXVec3TransformCoord((D3DXVECTOR3 *)(this),(const D3DXVECTOR3 *)(this),(const D3DXMATRIX *)(&matTrans));						 
	return *this;
}

void Vec3::TransformCoord(const Quaternion &qRotate, Vec3 &ret) const
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Vec3 &vUV1 =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp));
	Vec3 &vUV2 =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp2));
	Vec3 &vAxis =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp3));

	vAxis(qRotate.x, qRotate.y, qRotate.z);
	Math::Vec3Cross(vAxis, *this, vUV1);
	Math::Vec3Cross(vAxis, vUV1, vUV2);
	vUV1 *=(2.0f*qRotate.w);
	vUV2 *=2.0f;

	ret =*this+vUV1+vUV2;
}

Vec3 &Vec3::TransformCoordInPlace(const Quaternion &qRotate)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Vec3 &vUV1 =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp));
	Vec3 &vUV2 =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp2));
	Vec3 &vAxis =(Vec3 &)(*(lpDLLTLSData->m_pVecTemp3));

	vAxis(qRotate.x, qRotate.y, qRotate.z);
	Math::Vec3Cross(vAxis, *this, vUV1);
	Math::Vec3Cross(vAxis, vUV1, vUV2);
	vUV1 *=(2.0f*qRotate.w);
	vUV2 *=2.0f;

	*this +=(vUV1+vUV2);
	return *this;
}

void Vec3::TransformNormal(const Mat4x4 &matTrans, Vec3 &ret) const
{
	::D3DXVec3TransformNormal((D3DXVECTOR3 *)(&ret),(const D3DXVECTOR3 *)(this),(const D3DXMATRIX *)(&matTrans));
}

Vec3 &Vec3::TransformNormalInPlace(const Mat4x4 &matTrans)
{
	::D3DXVec3TransformNormal((D3DXVECTOR3 *)(this),(const D3DXVECTOR3 *)(this),(const D3DXMATRIX *)(&matTrans));
	return *this;
}

//获取与当前向量垂直的一个向量
void Vec3::Orthogonal(Vec3 &ret) const
{
	float fXAbs =::abs(x); float fYAbs =::abs(y); float fZAbs =::abs(z);
	if(fZAbs>=fXAbs && fZAbs>=fYAbs){
		ret.x =0.0f; ret.y =-z; ret.z =y;
	}else{
		ret.x =-y; ret.y =x; ret.z =0.0f;
	}
}


Vec3D Vec3D::m_vZero(0.0, 0.0, 0.0);
Vec3D Vec3D::m_vOne(1.0, 1.0, 1.0);


Vec3D::Vec3D()
{
	x =y =z =0.0;
}


Vec3D::Vec3D(double dX, double dY, double dZ)
{
	x =dX; y =dY; z =dZ;
}


Vec3D::Vec3D(const Vec3D &src)
{
	x =src.x; y =src.y; z =src.z;
}


Vec3D::Vec3D(const Vec3 &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z);
}


Vec3D &Vec3D::operator = (const Vec3D &src)
{
	x =src.x; y =src.y; z =src.z;
	return (*this);
}


Vec3D &Vec3D::operator = (const Vec3 &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z);
	return (*this);
}


Vec3D Vec3D::operator + (const Vec3D &src) const
{
	Vec3D cTemp(x+src.x, y+src.y, z+src.z);
	return cTemp;
}


Vec3D Vec3D::operator - (const Vec3D &src) const
{
	Vec3D cTemp(x-src.x, y-src.y, z-src.z);
	return cTemp;
}


Vec3D Vec3D::operator * (double src) const
{
	Vec3D cTemp(x*src, y*src, z*src);
	return cTemp;
}


Vec3D Vec3D::operator / (double src) const
{
	Vec3D cTemp(x/src, y/src, z/src);
	return cTemp;
}


Vec3D Vec3D::operator * (const Vec3D &src) const
{
	Vec3D cTemp(x*src.x, y*src.y, z*src.z);
	return cTemp;
}


Vec3D Vec3D::operator / (const Vec3D &src) const
{
	Vec3D cTemp(x/src.x, y/src.y, z/src.z);
	return cTemp;
}


bool Vec3D::operator == (const Vec3D &src) const
{
	return (x==src.x)&&(y==src.y)&&(z==src.z);
}


bool Vec3D::operator != (const Vec3D &src) const
{
	return (x!=src.x)||(y!=src.y)||(z!=src.z);
}


Vec3D &Vec3D::operator () (double dX, double dY, double dZ)
{
	x =dX; y =dY; z =dZ;
	return (*this);
}


Vec3D &Vec3D::operator += (const Vec3D &src)
{
	x +=src.x; y +=src.y; z +=src.z;
	return (*this);
}


Vec3D &Vec3D::operator -= (const Vec3D &src)
{
	x -=src.x; y -=src.y; z -=src.z;
	return (*this);
}


Vec3D &Vec3D::operator *= (double src)
{
	x *=src; y *=src; z *=src;
	return (*this);
}


Vec3D &Vec3D::operator /= (double src)
{
	x /=src; y /=src; z /=src;
	return (*this);
}


Vec3D &Vec3D::operator *= (const Vec3D &src)
{
	x *=src.x; y *=src.y; z *=src.z;
	return (*this);
}


Vec3D &Vec3D::operator /= (const Vec3D &src)
{
	x /=src.x; y /=src.y; z /=src.z;
	return (*this);
}


Vec3D operator * (double src1, const Vec3D &src2)
{
	Vec3D cTemp(src1*src2.x, src1*src2.y, src1*src2.z);
	return cTemp;
}


std::ostream &operator << (std::ostream &ostr, const Vec3D &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y<<" "<<src.z;
	ostr<<strstream.str();
	return ostr;
}


Vec3 Vec3D::ToVec3() const
{
	Vec3 vTemp;
	vTemp(float(x), float(y), float(z));
	return vTemp;
}


void Vec3D::ToVec3(Vec3 &ret) const
{
	ret(float(x), float(y), float(z));
}


double Vec3D::Length() const
{
	return ::sqrt(x*x+y*y+z*z);
}


double Vec3D::LengthSq() const
{
	return x*x+y*y+z*z;
}


void Vec3D::Normalize(Vec3D &ret) const
{
	double dLen =::sqrt(x*x+y*y+z*z);
	if(dLen > 0.0){
		ret(x/dLen, y/dLen, z/dLen);
	}else{
		ret(x, y, z);
	}
}


Vec3D &Vec3D::NormalizeInPlace()
{
	double dLen =::sqrt(x*x+y*y+z*z);
	if(dLen > 0.0){
		x /=dLen; y /=dLen; z /=dLen;
	}
	return (*this);
}


void Vec3D::Transform(const Mat3x3D &matTrans, Vec3D &ret) const
{
	ret.x =x*matTrans._11+y*matTrans._21+z*matTrans._31;
	ret.y =x*matTrans._12+y*matTrans._22+z*matTrans._32;
	ret.z =x*matTrans._13+y*matTrans._23+z*matTrans._33;
}


Vec3D &Vec3D::TransformInPlace(const Mat3x3D &matTrans)
{
	double dX =x*matTrans._11+y*matTrans._21+z*matTrans._31;
	double dY =x*matTrans._12+y*matTrans._22+z*matTrans._32;
	double dZ =x*matTrans._13+y*matTrans._23+z*matTrans._33;
	return *this;
}


void Vec3D::TransformCoord(const Mat4x4D &matTrans, Vec3D &ret) const
{
	ret.x =x*matTrans._11+y*matTrans._21+z*matTrans._31+matTrans._41;
	ret.y =x*matTrans._12+y*matTrans._22+z*matTrans._32+matTrans._42;
	ret.z =x*matTrans._13+y*matTrans._23+z*matTrans._33+matTrans._43;
	double dW =x*matTrans._14+y*matTrans._24+z*matTrans._34+matTrans._44;
	if(dW != 0.0){
		ret.x /=dW; ret.y /=dW; ret.z /=dW; 
	}
}


Vec3D &Vec3D::TransformCoordInPlace(const Mat4x4D &matTrans)
{
	double dX =x*matTrans._11+y*matTrans._21+z*matTrans._31+matTrans._41;
	double dY =x*matTrans._12+y*matTrans._22+z*matTrans._32+matTrans._42;
	double dZ =x*matTrans._13+y*matTrans._23+z*matTrans._33+matTrans._43;
	double dW =x*matTrans._14+y*matTrans._24+z*matTrans._34+matTrans._44;
	if(dW != 0.0){
		x =dX/dW; y =dY/dW; z =dZ/dW; 
	}else{
		x =dX; y =dY; z =dZ; 
	}
	return *this;
}


void Vec3D::TransformCoord(const QuaternionD &qRotate, Vec3D &ret) const
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Vec3D &vUV1 =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp));
	Vec3D &vUV2 =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp2));
	Vec3D &vAxis =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp3));

	vAxis(qRotate.x, qRotate.y, qRotate.z);
	Math::Vec3DCross(vAxis, *this, vUV1);
	Math::Vec3DCross(vAxis, vUV1, vUV2);
	vUV1 *=2.0*qRotate.w;
	vUV2 *=2.0;

	ret =*this+vUV1+vUV2;
}


Vec3D &Vec3D::TransformCoordInPlace(const QuaternionD &qRotate)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Vec3D &vUV1 =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp));
	Vec3D &vUV2 =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp2));
	Vec3D &vAxis =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp3));

	vAxis(qRotate.x, qRotate.y, qRotate.z);
	Math::Vec3DCross(vAxis, *this, vUV1);
	Math::Vec3DCross(vAxis, vUV1, vUV2);
	vUV1 *=2.0*qRotate.w;
	vUV2 *=2.0;

	*this +=(vUV1+vUV2);
	return (*this);
}


void Vec3D::TransformNormal(const Mat4x4D &matTrans, Vec3D &ret) const
{
	ret.x =x*matTrans._11+y*matTrans._21+z*matTrans._31;
	ret.y =x*matTrans._12+y*matTrans._22+z*matTrans._32;
	ret.z =x*matTrans._13+y*matTrans._23+z*matTrans._33;
}


Vec3D &Vec3D::TransformNormalInPlace(const Mat4x4D &matTrans)
{
	double dX =x*matTrans._11+y*matTrans._21+z*matTrans._31;
	double dY =x*matTrans._12+y*matTrans._22+z*matTrans._32;
	double dZ =x*matTrans._13+y*matTrans._23+z*matTrans._33;
	x =dX; y =dY; z =dZ; 
	return *this;
}


//获取与当前向量垂直的一个向量
void Vec3D::Orthogonal(Vec3D &ret) const
{
	double dXAbs =::abs(x); double dYAbs =::abs(y); double dZAbs =::abs(z);
	if(dZAbs>=dXAbs && dZAbs>=dYAbs){
		ret.x =0.0; ret.y =-z; ret.z =y;
	}else{
		ret.x =-y; ret.y =x; ret.z =0.0;
	}
}


//四维向量
Vec4 Vec4::m_qZero(0.0f, 0.0f, 0.0f, 0.0f);
Vec4 Vec4::m_qOne(1.0f, 1.0f, 1.0f, 1.0f);

Vec4::Vec4()
{
	x = y = z = w = 0.0f;
}

Vec4::Vec4(float fX, float fY, float fZ, float fW):x(fX),
                                                   y(fY),
                                                   z(fZ),
                                                   w(fW)

{

}

Vec4 Vec4::operator +(const Vec4 &src) const
{
	return (Vec4 &)((D3DXVECTOR4 &)(*this) + (D3DXVECTOR4 &)src);
}

Vec4 Vec4::operator -(const Vec4 &src) const
{
	return (Vec4 &)((D3DXVECTOR4 &)(*this) - (D3DXVECTOR4 &)src);
}

Vec4 Vec4::operator *(float src) const
{
	return (Vec4 &)((D3DXVECTOR4 &)(*this) * src);
}

Vec4 Vec4::operator /(float src) const
{
	return (Vec4 &)((D3DXVECTOR4 &)(*this) / src);
}

Vec4 Vec4::operator *(const Vec4 &src) const
{
	float pTemp[4];
	pTemp[0] =x*src.x;
	pTemp[1] =y*src.y;
	pTemp[2] =z*src.z;
	pTemp[3] =w*src.w;
	return (Vec4 &)(*pTemp);
}

Vec4 Vec4::operator /(const Vec4 &src) const
{
	float pTemp[4];
	pTemp[0] =x/src.x;
	pTemp[1] =y/src.y;
	pTemp[2] =z/src.z;
	pTemp[3] =w/src.w;
	return (Vec4 &)(*pTemp);
}

bool Vec4::operator ==(const Vec4 &src) const
{
	return ::memcmp(this,&src,sizeof(Vec4)) == 0;
}

bool Vec4::operator !=(const Vec4 &src) const
{
	return ::memcmp(this,&src,sizeof(Vec4)) != 0;
}

Vec4 &Vec4::operator ()(float fX, float fY, float fZ,float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
	return *this;
}

Vec4 &Vec4::operator +=(const Vec4 &src)
{
	*this = *this + src;
	return *this;
}

Vec4 &Vec4::operator -=(const Vec4 &src)
{
	*this = *this - src;
	return *this;
}

Vec4 &Vec4::operator *=(float src)
{
	*this = *this * src;
	return *this;
}

Vec4 &Vec4::operator /=(float src)
{
	*this = *this / src;
	return *this;
}

Vec4 &Vec4::operator *=(const Vec4 &src)
{
	x = x * src.x;
	y = y * src.y;
	z = z * src.z;
	w = w * src.w;
	return *this;
}

Vec4 &Vec4::operator /=(const Vec4 &src)
{
	x = x / src.x;
	y = y / src.y;
	z = z / src.z;
	w = w / src.w;
	return *this;
}

Vec4 operator * (float src1, const Vec4 &src2)
{
	return (Vec4 &)(src1 * (D3DXVECTOR4 &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Vec4 &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y<<" "<<src.z<<" "<<src.w;
	ostr<<strstream.str();
	return ostr;
}

float Vec4::Length() const
{
	return ::D3DXVec4Length((const D3DXVECTOR4 *)(this));
}

float Vec4::LengthSq() const
{
	return ::D3DXVec4LengthSq((const D3DXVECTOR4 *)(this));
}

void Vec4::Normalize(Vec4 &ret) const
{
	::D3DXVec4Normalize((D3DXVECTOR4 *)(&ret),(const D3DXVECTOR4 *)(this));
}

Vec4 &Vec4::NormalizeInPlace()
{
	::D3DXVec4Normalize((D3DXVECTOR4 *)(this),(const D3DXVECTOR4 *)(this));
	return *this;
}

void Vec4::Transform(const Mat4x4 &matTrans, Vec4 &ret) const
{
	::D3DXVec4Transform((D3DXVECTOR4 *)(&ret),(const D3DXVECTOR4 *)(this),(const D3DXMATRIX *)(&matTrans));
}

Vec4 &Vec4::TransformInPlace(const Mat4x4 &matTrans)
{
	::D3DXVec4Transform((D3DXVECTOR4 *)(this),(const D3DXVECTOR4 *)(this),(const D3DXMATRIX *)(&matTrans));
	return *this;
}

Vec4D Vec4D::m_qZero(0.0, 0.0, 0.0, 0.0);
Vec4D Vec4D::m_qOne(1.0, 1.0, 1.0, 1.0);

Vec4D::Vec4D()
{
	x =y =z =w =0.0;
}

Vec4D::Vec4D(double dX, double dY, double dZ, double dW)
{
	x =dX; y =dY; z =dZ; w =dW;
}

Vec4D::Vec4D(const Vec4D &src)
{
	x =src.x; y =src.y; z =src.z; w =src.w;
}

Vec4D::Vec4D(const Vec4 &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z); w =double(src.w);
}

Vec4D &Vec4D::operator = (const Vec4D &src)
{
	x =src.x; y =src.y; z =src.z; w =src.w;
	return (*this);
}

Vec4D &Vec4D::operator = (const Vec4 &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z); w =double(src.w);
	return (*this);
}

Vec4D Vec4D::operator + (const Vec4D &src) const
{
	Vec4D cTemp(x+src.x, y+src.y, z+src.z, w+src.w);
	return cTemp;
}

Vec4D Vec4D::operator - (const Vec4D &src) const
{
	Vec4D cTemp(x-src.x, y-src.y, z-src.z, w-src.w);
	return cTemp;
}

Vec4D Vec4D::operator * (double src) const
{
	Vec4D cTemp(x*src, y*src, z*src, w*src);
	return cTemp;
}

Vec4D Vec4D::operator / (double src) const
{
	Vec4D cTemp(x/src, y/src, z/src, w/src);
	return cTemp;
}

Vec4D Vec4D::operator * (const Vec4D &src) const
{
	Vec4D cTemp(x*src.x, y*src.y, z*src.z, w*src.w);
	return cTemp;
}

Vec4D Vec4D::operator / (const Vec4D &src) const
{
	Vec4D cTemp(x/src.x, y/src.y, z/src.z, w/src.w);
	return cTemp;
}

bool Vec4D::operator == (const Vec4D &src) const
{
	return (x==src.x)&&(y==src.y)&&(z==src.z)&&(w==src.w);
}

bool Vec4D::operator != (const Vec4D &src) const
{
	return (x!=src.x)||(y!=src.y)||(z!=src.z)||(w!=src.w);
}

Vec4D &Vec4D::operator () (double dX, double dY, double dZ, double dW)
{
	x =dX; y =dY; z =dZ; w =dW;
	return (*this);
}

Vec4D &Vec4D::operator += (const Vec4D &src)
{
	x +=src.x; y +=src.y; z +=src.z; w +=src.w;
	return (*this);
}

Vec4D &Vec4D::operator -= (const Vec4D &src)
{
	x -=src.x; y -=src.y; z -=src.z; w -=src.w;
	return (*this);
}

Vec4D &Vec4D::operator *= (double src)
{
	x *=src; y *=src; z *=src; w *=src;
	return (*this);
}

Vec4D &Vec4D::operator /= (double src)
{
	x /=src; y /=src; z /=src; w /=src;
	return (*this);
}

Vec4D &Vec4D::operator *= (const Vec4D &src)
{
	x *=src.x; y *=src.y; z *=src.z; w *=src.w;
	return (*this);
}

Vec4D &Vec4D::operator /= (const Vec4D &src)
{
	x /=src.x; y /=src.y; z /=src.z; w /=src.w;
	return (*this);
}

Vec4D operator * (double src1, const Vec4D &src2)
{
	Vec4D cTemp(src1*src2.x, src1*src2.y, src1*src2.z, src1*src2.w);
	return cTemp;
}

std::ostream &operator << (std::ostream &ostr, const Vec4D &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y<<" "<<src.z<<" "<<src.w;
	ostr<<strstream.str();
	return ostr;
}

Vec4 Vec4D::ToVec4() const
{
	Vec4 qTemp;
	qTemp(float(x), float(y), float(z), float(w));
	return qTemp;
}

void Vec4D::ToVec4(Vec4 &ret) const
{
	ret(float(x), float(y), float(z), float(w));
}

double Vec4D::Length() const
{
	return ::sqrt(x*x+y*y+z*z+w*w);
}

double Vec4D::LengthSq() const
{
	return x*x+y*y+z*z+w*w;
}

void Vec4D::Normalize(Vec4D &ret) const
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		ret(x/dLen, y/dLen, z/dLen, w/dLen);
	}else{
		ret(x, y, z, w);
	}
}

Vec4D &Vec4D::NormalizeInPlace()
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		x /=dLen; y /=dLen; z /=dLen; w /=dLen;
	}
	return (*this);
}

void Vec4D::Transform(const Mat4x4 &matTrans, Vec4D &ret) const
{
	ret.x =x*matTrans._11+y*matTrans._21+z*matTrans._31+w*matTrans._41;
	ret.y =x*matTrans._12+y*matTrans._22+z*matTrans._32+w*matTrans._42;
	ret.z =x*matTrans._13+y*matTrans._23+z*matTrans._33+w*matTrans._43;
	ret.w =x*matTrans._14+y*matTrans._24+z*matTrans._34+w*matTrans._44;
}

Vec4D &Vec4D::TransformInPlace(const Mat4x4 &matTrans)
{
	double dX =x*matTrans._11+y*matTrans._21+z*matTrans._31+w*matTrans._41;
	double dY =x*matTrans._12+y*matTrans._22+z*matTrans._32+w*matTrans._42;
	double dZ =x*matTrans._13+y*matTrans._23+z*matTrans._33+w*matTrans._43;
	double dW =x*matTrans._14+y*matTrans._24+z*matTrans._34+w*matTrans._44;
	x =dX; y =dY; z =dZ; w =dW;
	return *this;
}


Vec5D Vec5D::m_vZero(0.0, 0.0, 0.0, 0.0, 0.0);
Vec5D Vec5D::m_vOne(1.0, 1.0, 1.0, 1.0, 1.0);

Vec5D::Vec5D()
{
	v1 =v2 =v3 =v4 =v5 =0.0;
}

Vec5D::Vec5D(double dV1, double dV2, double dV3, double dV4, double dV5)
{
	v1 =dV1; v2 =dV2; v3 =dV3; v4 =dV4; v5 =dV5; 
}

Vec5D::Vec5D(const Vec5D &src)
{
	v1 =src.v1; v2 =src.v2; v3 =src.v3; v4 =src.v4; v5 =src.v5; 
}

Vec5D &Vec5D::operator = (const Vec5D &src)
{
	v1 =src.v1; v2 =src.v2; v3 =src.v3; v4 =src.v4; v5 =src.v5; 
	return (*this);
}

Vec5D Vec5D::operator + (const Vec5D &src) const
{
	Vec5D cTemp(v1+src.v1, v2+src.v2, v3+src.v3, v4+src.v4, v5+src.v5);
	return cTemp;
}

Vec5D Vec5D::operator - (const Vec5D &src) const
{
	Vec5D cTemp(v1-src.v1, v2-src.v2, v3-src.v3, v4-src.v4, v5-src.v5);
	return cTemp;
}

Vec5D Vec5D::operator * (double src) const
{
	Vec5D cTemp(v1*src, v2*src, v3*src, v4*src, v5*src);
	return cTemp;
}

Vec5D Vec5D::operator / (double src) const
{
	Vec5D cTemp(v1/src, v2/src, v3/src, v4/src, v5/src);
	return cTemp;
}

Vec5D Vec5D::operator * (const Vec5D &src) const
{
	Vec5D cTemp(v1*src.v1, v2*src.v2, v3*src.v3, v4*src.v4, v5*src.v5);
	return cTemp;
}

Vec5D Vec5D::operator / (const Vec5D &src) const
{
	Vec5D cTemp(v1/src.v1, v2/src.v2, v3/src.v3, v4/src.v4, v5/src.v5);
	return cTemp;
}

bool Vec5D::operator == (const Vec5D &src) const
{
	return (v1==src.v1)&&(v2==src.v2)&&(v3==src.v3)&&(v4==src.v4)&&(v5==src.v5);
}

bool Vec5D::operator != (const Vec5D &src) const
{
	return (v1!=src.v1)||(v2!=src.v2)||(v3!=src.v3)||(v4!=src.v4)||(v5!=src.v5);
}

Vec5D &Vec5D::operator () (double dV1, double dV2, double dV3, double dV4, double dV5)
{
	v1 =dV1; v2 =dV2; v3 =dV3; v4 =dV4; v5 =dV5; 
	return (*this);
}

Vec5D &Vec5D::operator += (const Vec5D &src)
{
	v1 +=src.v1; v2 +=src.v2; v3 +=src.v3; v4 +=src.v4; v5 +=src.v5; 
	return (*this);
}

Vec5D &Vec5D::operator -= (const Vec5D &src)
{
	v1 -=src.v1; v2 -=src.v2; v3 -=src.v3; v4 -=src.v4; v5 -=src.v5; 
	return (*this);
}

Vec5D &Vec5D::operator *= (double src)
{
	v1 *=src; v2 *=src; v3 *=src; v4 *=src; v5 *=src; 
	return (*this);
}

Vec5D &Vec5D::operator /= (double src)
{
	v1 /=src; v2 /=src; v3 /=src; v4 /=src; v5 /=src; 
	return (*this);
}

Vec5D &Vec5D::operator *= (const Vec5D &src)
{
	v1 *=src.v1; v2 *=src.v2; v3 *=src.v3; v4 *=src.v4; v5 *=src.v5; 
	return (*this);
}

Vec5D &Vec5D::operator /= (const Vec5D &src)
{
	v1 /=src.v1; v2 /=src.v2; v3 /=src.v3; v4 /=src.v4; v5 /=src.v5; 
	return (*this);
}

Vec5D operator * (double src1, const Vec5D &src2)
{
	Vec5D cTemp(src1*src2.v1, src1*src2.v2, src1*src2.v3, src1*src2.v4, src1*src2.v5);
	return cTemp;
}

double Vec5D::Length() const
{
	return ::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5);
}

double Vec5D::LengthSq() const
{
	return v1*v1+v2*v2+v3*v3+v4*v4+v5*v5;
}

void Vec5D::Normalize(Vec5D &ret) const
{
	double dLen =::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5);
	if(dLen > 0.0){
		ret(v1/dLen, v2/dLen, v3/dLen, v4/dLen, v5/dLen);
	}else{
		ret(v1, v2, v3, v4, v5);
	}
}

Vec5D &Vec5D::NormalizeInPlace()
{
	double dLen =::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5);
	if(dLen > 0.0){
		v1 /=dLen; v2 /=dLen; v3 /=dLen; v4 /=dLen; v5 /=dLen; 
	}
	return (*this);
}

void Vec5D::Transform(const Mat5x5D &matTrans, Vec5D &ret) const
{
	ret.v1 =v1*matTrans._11+v2*matTrans._21+v3*matTrans._31+v4*matTrans._41+v5*matTrans._51;
	ret.v2 =v1*matTrans._12+v2*matTrans._22+v3*matTrans._32+v4*matTrans._42+v5*matTrans._52;
	ret.v3 =v1*matTrans._13+v2*matTrans._23+v3*matTrans._33+v4*matTrans._43+v5*matTrans._53;
	ret.v4 =v1*matTrans._14+v2*matTrans._24+v3*matTrans._34+v4*matTrans._44+v5*matTrans._54;
	ret.v5 =v1*matTrans._15+v2*matTrans._25+v3*matTrans._35+v4*matTrans._45+v5*matTrans._55;
}

Vec5D &Vec5D::TransformInPlace(const Mat5x5D &matTrans)
{
	double dV1 =v1*matTrans._11+v2*matTrans._21+v3*matTrans._31+v4*matTrans._41+v5*matTrans._51;
	double dV2 =v1*matTrans._12+v2*matTrans._22+v3*matTrans._32+v4*matTrans._42+v5*matTrans._52;
	double dV3 =v1*matTrans._13+v2*matTrans._23+v3*matTrans._33+v4*matTrans._43+v5*matTrans._53;
	double dV4 =v1*matTrans._14+v2*matTrans._24+v3*matTrans._34+v4*matTrans._44+v5*matTrans._54;
	double dV5 =v1*matTrans._15+v2*matTrans._25+v3*matTrans._35+v4*matTrans._45+v5*matTrans._55;
	v1 =dV1; v2 =dV2; v3 =dV3; v4 =dV4; v5 =dV5;
	return *this;
}


Vec8D Vec8D::m_vZero(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
Vec8D Vec8D::m_vOne(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);

Vec8D::Vec8D()
{
	v1 =v2 =v3 =v4 =v5 =v6 =v7 =v8 =0.0;
}

Vec8D::Vec8D(double dV1, double dV2, double dV3, double dV4, double dV5, double dV6, double dV7, double dV8)
{
	v1 =dV1; v2 =dV2; v3 =dV3; v4 =dV4; v5 =dV5; v6 =dV6; v7 =dV7; v8 =dV8; 
}

Vec8D::Vec8D(const Vec8D &src)
{
	v1 =src.v1; v2 =src.v2; v3 =src.v3; v4 =src.v4; v5 =src.v5; v6 =src.v6; v7 =src.v7; v8 =src.v8; 
}

Vec8D &Vec8D::operator = (const Vec8D &src)
{
	v1 =src.v1; v2 =src.v2; v3 =src.v3; v4 =src.v4; v5 =src.v5; v6 =src.v6; v7 =src.v7; v8 =src.v8; 
	return (*this);
}

Vec8D Vec8D::operator + (const Vec8D &src) const
{
	Vec8D cTemp(v1+src.v1, v2+src.v2, v3+src.v3, v4+src.v4, v5+src.v5, v6+src.v6, v7+src.v7, v8+src.v8);
	return cTemp;
}

Vec8D Vec8D::operator - (const Vec8D &src) const
{
	Vec8D cTemp(v1-src.v1, v2-src.v2, v3-src.v3, v4-src.v4, v5-src.v5, v6-src.v6, v7-src.v7, v8-src.v8);
	return cTemp;
}

Vec8D Vec8D::operator * (double src) const
{
	Vec8D cTemp(v1*src, v2*src, v3*src, v4*src, v5*src, v6*src, v7*src, v8*src);
	return cTemp;
}

Vec8D Vec8D::operator / (double src) const
{
	Vec8D cTemp(v1/src, v2/src, v3/src, v4/src, v5/src, v6/src, v7/src, v8/src);
	return cTemp;
}

Vec8D Vec8D::operator * (const Vec8D &src) const
{
	Vec8D cTemp(v1*src.v1, v2*src.v2, v3*src.v3, v4*src.v4, v5*src.v5, v6*src.v6, v7*src.v7, v8*src.v8);
	return cTemp;
}

Vec8D Vec8D::operator / (const Vec8D &src) const
{
	Vec8D cTemp(v1/src.v1, v2/src.v2, v3/src.v3, v4/src.v4, v5/src.v5, v6/src.v6, v7/src.v7, v8/src.v8);
	return cTemp;
}

bool Vec8D::operator == (const Vec8D &src) const
{
	return (v1==src.v1)&&(v2==src.v2)&&(v3==src.v3)&&(v4==src.v4)&&(v5==src.v5)&&(v6==src.v6)&&(v7==src.v7)&&(v8==src.v8);
}

bool Vec8D::operator != (const Vec8D &src) const
{
	return (v1!=src.v1)||(v2!=src.v2)||(v3!=src.v3)||(v4!=src.v4)||(v5!=src.v5)||(v6!=src.v6)||(v7!=src.v7)||(v8!=src.v8);
}

Vec8D &Vec8D::operator () (double dV1, double dV2, double dV3, double dV4, double dV5, double dV6, double dV7, double dV8)
{
	v1 =dV1; v2 =dV2; v3 =dV3; v4 =dV4; v5 =dV5; v6 =dV6; v7 =dV7; v8 =dV8; 
	return (*this);
}

Vec8D &Vec8D::operator += (const Vec8D &src)
{
	v1 +=src.v1; v2 +=src.v2; v3 +=src.v3; v4 +=src.v4; v5 +=src.v5; v6 +=src.v6; v7 +=src.v7; v8 +=src.v8;
	return (*this);
}

Vec8D &Vec8D::operator -= (const Vec8D &src)
{
	v1 -=src.v1; v2 -=src.v2; v3 -=src.v3; v4 -=src.v4; v5 -=src.v5; v6 -=src.v6; v7 -=src.v7; v8 -=src.v8;
	return (*this);
}

Vec8D &Vec8D::operator *= (double src)
{
	v1 *=src; v2 *=src; v3 *=src; v4 *=src; v5 *=src; v6 *=src; v7 *=src; v8 *=src;
	return (*this);
}

Vec8D &Vec8D::operator /= (double src)
{
	v1 /=src; v2 /=src; v3 /=src; v4 /=src; v5 /=src; v6 /=src; v7 /=src; v8 /=src; 
	return (*this);
}

Vec8D &Vec8D::operator *= (const Vec8D &src)
{
	v1 *=src.v1; v2 *=src.v2; v3 *=src.v3; v4 *=src.v4; v5 *=src.v5; v6 *=src.v6; v7 *=src.v7; v8 *=src.v8;
	return (*this);
}

Vec8D &Vec8D::operator /= (const Vec8D &src)
{
	v1 /=src.v1; v2 /=src.v2; v3 /=src.v3; v4 /=src.v4; v5 /=src.v5; v6 /=src.v6; v7 /=src.v7; v8 /=src.v8; 
	return (*this);
}

Vec8D operator * (double src1, const Vec8D &src2)
{
	Vec8D cTemp(src1*src2.v1, src1*src2.v2, src1*src2.v3, src1*src2.v4, src1*src2.v5, src1*src2.v6, src1*src2.v7, src1*src2.v8);
	return cTemp;
}

double Vec8D::Length() const
{
	return ::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5+v6*v6+v7*v7+v8*v8);
}

double Vec8D::LengthSq() const
{
	return v1*v1+v2*v2+v3*v3+v4*v4+v5*v5+v6*v6+v7*v7+v8*v8;
}

void Vec8D::Normalize(Vec8D &ret) const
{
	double dLen =::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5+v6*v6+v7*v7+v8*v8);
	if(dLen > 0.0){
		ret(v1/dLen, v2/dLen, v3/dLen, v4/dLen, v5/dLen, v6/dLen, v7/dLen, v8/dLen);
	}else{
		ret(v1, v2, v3, v4, v5, v6, v7, v8);
	}
}

Vec8D &Vec8D::NormalizeInPlace()
{
	double dLen =::sqrt(v1*v1+v2*v2+v3*v3+v4*v4+v5*v5+v6*v6+v7*v7+v8*v8);
	if(dLen > 0.0){
		v1 /=dLen; v2 /=dLen; v3 /=dLen; v4 /=dLen; v5 /=dLen; v6 /=dLen; v7 /=dLen; v8 /=dLen; 
	}
	return (*this);
}


//表示一个平面

Plane Plane::m_qZero(0.0f, 0.0f, 0.0f, 0.0f);
Plane Plane::m_qOne(1.0f, 1.0f, 1.0f, 1.0f);

Plane::Plane()
{
	a = b = c = d = 0.0f;
}

Plane::Plane(float fA, float fB, float fC, float fD):a(fA),
                                                     b(fB),
													 c(fC),
                                                     d(fD)
{

}

Plane &Plane::operator ()(float fA,float fB,float fC,float fD)
{
	a = fA;
	b = fB;
	c = fC;
	d = fD;
	return *this;
}

Plane Plane::operator *(float src) const
{
	return (Plane &)((D3DXPLANE &)(*this) * src);
}

Plane Plane::operator /(float src) const
{
	return (Plane &)((D3DXPLANE &)(*this) / src);
}

Plane &Plane::operator *=(float src)
{
	*this = *this * src;
	return *this;
}

Plane &Plane::operator /=(float src)
{
	*this = *this / src;
	return *this;
}

Plane operator * (float src1, const Plane &src2)
{
	return (Plane &)(src1 * (D3DXPLANE &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Plane &src)
{
	std::stringstream strstream;
	strstream<<src.a<<" "<<src.b<<" "<<src.c<<" "<<src.d;
	ostr<<strstream.str();
	return ostr;
}

void Plane::Normalize(Plane &ret) const
{
	::D3DXPlaneNormalize((D3DXPLANE *)(&ret),(const D3DXPLANE *)(this));
}

Plane &Plane::NormalizeInPlace()
{
	::D3DXPlaneNormalize((D3DXPLANE *)(this),(const D3DXPLANE *)(this));
	return *this;
}

void Plane::Transform(const Mat4x4 &matTransInvT, Plane &ret) const
{
	::D3DXPlaneTransform((D3DXPLANE *)(&ret),(const D3DXPLANE *)(this),(const D3DXMATRIX *)(&matTransInvT));
}

Plane &Plane::TransformInPlace(const Mat4x4 &matTransInvT)
{
	::D3DXPlaneTransform((D3DXPLANE *)(this),(const D3DXPLANE *)(this),(const D3DXMATRIX *)(&matTransInvT));
	return *this;
}

float Plane::DotCoord(const Vec3 &src) const
{
	return ::D3DXPlaneDotCoord((const D3DXPLANE *)(this),(const D3DXVECTOR3 *)(&src));
}

float Plane::DotNormal(const Vec3 &src) const
{
	return ::D3DXPlaneDotNormal((const D3DXPLANE *)(this),(const D3DXVECTOR3 *)(&src));

}

PlaneD PlaneD::m_qZero(0.0, 0.0, 0.0, 0.0);

PlaneD PlaneD::m_qOne(1.0, 1.0, 1.0, 1.0);

PlaneD::PlaneD()
{
	a =b =c =d =0.0;
}

PlaneD::PlaneD(double dA, double dB, double dC, double dD)
{
	a =dA; b =dB; c =dC; d =dD;
}

PlaneD::PlaneD(const Plane &src)
{
	a =double(src.a); b =double(src.b); c =double(src.c); d =double(src.d);
}

PlaneD &PlaneD::operator = (const Plane &src)
{
	a =double(src.a); b =double(src.b); c =double(src.c); d =double(src.d);
	return *this;
}

PlaneD &PlaneD::operator () (double dA, double dB, double dC, double dD)
{
	a =dA; b =dB; c =dC; d =dD;
	return *this;
}

PlaneD PlaneD::operator * (double src) const
{
	return PlaneD(a*src, b*src, c*src, d*src);
}

PlaneD PlaneD::operator / (double src) const
{
	return PlaneD(a/src, b/src, c/src, d/src);
}

PlaneD &PlaneD::operator *= (double src)
{
	a *=src; b *=src; c *=src; d *=src;
	return *this;
}

PlaneD &PlaneD::operator /= (double src)
{
	a /=src; b /=src; c /=src; d /=src;
	return *this;
}

PlaneD operator * (double src1, const PlaneD &src2)
{
	return PlaneD(src2.a*src1, src2.b*src1, src2.c*src1, src2.d*src1);
}

Plane PlaneD::ToPlane() const
{
	return Plane(float(a), float(b), float(c), float(d));
}

void PlaneD::ToPlane(Plane &ret) const
{
	ret.a =float(a); ret.b =float(b); ret.c =float(c); ret.d =float(d);
}

void PlaneD::Normalize(PlaneD &ret) const
{
	double dLen =::sqrt(a*a+b*b+c*c);
	if(dLen > 0.0){
		ret.a =a/dLen; ret.b =b/dLen; ret.c =c/dLen; ret.d =d/dLen; 
	}else{
		ret.a =a; ret.b =b; ret.c =c; ret.d =d; 
	}
}

PlaneD &PlaneD::NormalizeInPlace()
{
	double dLen =::sqrt(a*a+b*b+c*c);
	if(dLen > 0.0){
		a /=dLen; b /=dLen; c /=dLen; d /=dLen; 
	}
	return *this;
}

void PlaneD::Transform(const Mat4x4D &matTransInvT, PlaneD &ret) const
{
	ret.a =matTransInvT._11*a+matTransInvT._21*b+matTransInvT._31*c+matTransInvT._41*d;
	ret.b =matTransInvT._12*a+matTransInvT._22*b+matTransInvT._32*c+matTransInvT._42*d;
	ret.c =matTransInvT._13*a+matTransInvT._23*b+matTransInvT._33*c+matTransInvT._43*d;
	ret.d =matTransInvT._14*a+matTransInvT._24*b+matTransInvT._34*c+matTransInvT._44*d;
}

PlaneD &PlaneD::TransformInPlace(const Mat4x4D &matTransInvT)
{
	double dA =matTransInvT._11*a+matTransInvT._21*b+matTransInvT._31*c+matTransInvT._41*d;
	double dB =matTransInvT._12*a+matTransInvT._22*b+matTransInvT._32*c+matTransInvT._42*d;
	double dC =matTransInvT._13*a+matTransInvT._23*b+matTransInvT._33*c+matTransInvT._43*d;
	double dD =matTransInvT._14*a+matTransInvT._24*b+matTransInvT._34*c+matTransInvT._44*d;
	a =dA; b =dB; c =dC; d =dD;
	return *this;
}

double PlaneD::DotCoord(const Vec3D &src) const
{
	return src.x*a+src.y*b+src.z*c+d;
}

double PlaneD::DotNormal(const Vec3D &src) const
{
	return src.x*a+src.y*b+src.z*c;
}


//表示四元数

Quaternion Quaternion::m_qIdentity(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion()
{
	x = y =z = 0.0f;
	w = 1.0f;
}
Quaternion::Quaternion(float fX, float fY, float fZ, float fW):x(fX),
                                                               y(fY),
                                                               z(fZ),
                                                               w(fW)
{

}

Quaternion::Quaternion(float fYaw, float fPitch, float fRoll)
{
	::D3DXQuaternionRotationYawPitchRoll((D3DXQUATERNION *)(this),fYaw,fPitch,fRoll);
}

Quaternion Quaternion::operator +(const Quaternion &src) const
{
	return (Quaternion &)((D3DXVECTOR4 &)(*this) + (D3DXVECTOR4 &)src);
}

Quaternion Quaternion::operator -(const Quaternion &src) const
{
	return (Quaternion &)((D3DXVECTOR4 &)(*this) - (D3DXVECTOR4 &)src);
}

Quaternion Quaternion::operator *(float src) const
{
	return (Quaternion &)((D3DXQUATERNION &)(*this) * src);
}

Quaternion Quaternion::operator /(float src) const
{
	return (Quaternion &)((D3DXQUATERNION &)(*this) / src);
}

Quaternion Quaternion::operator *(const Quaternion &src) const
{
	return (Quaternion &)((D3DXQUATERNION &)(*this) * (D3DXQUATERNION &)src);
}

bool Quaternion::operator ==(const Quaternion &src) const
{
	return ::memcmp(this,&src,sizeof(Quaternion)) == 0;
}

bool Quaternion::operator !=(const Quaternion &src) const
{
	return ::memcmp(this,&src,sizeof(Quaternion)) != 0;
}

Quaternion &Quaternion::operator ()(float fX, float fY, float fZ,float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
	return *this;
}

Quaternion &Quaternion::operator +=(const Quaternion &src)
{
	*this = *this + src;
	return *this;
}

Quaternion &Quaternion::operator -=(const Quaternion &src)
{
	*this = *this - src;
	return *this;
}

Quaternion &Quaternion::operator *=(float src)
{
	*this = *this * src;
	return *this;
}

Quaternion &Quaternion::operator /=(float src)
{
	*this = *this / src;
	return *this;
}

Quaternion &Quaternion::operator *=(const Quaternion &src)
{
	*this = *this * src;
	return *this;
}

Quaternion operator * (float src1, const Quaternion &src2)
{
	return (Quaternion &)(src1 * (D3DXQUATERNION &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Quaternion &src)
{
	std::stringstream strstream;
	strstream<<src.x<<" "<<src.y<<" "<<src.z<<" "<<src.w;
	ostr<<strstream.str();
	return ostr;
}

float Quaternion::Length() const
{
	return ::D3DXQuaternionLength((const D3DXQUATERNION *)(this));
}

float Quaternion::LengthSq() const
{
	return ::D3DXQuaternionLengthSq((const D3DXQUATERNION *)(this));
}

void Quaternion::Conjugate(Quaternion &ret) const
{
	::D3DXQuaternionConjugate((D3DXQUATERNION *)(&ret),(const D3DXQUATERNION *)(this));
}

Quaternion &Quaternion::ConjugateInPlace()
{
	::D3DXQuaternionConjugate((D3DXQUATERNION *)(this),(const D3DXQUATERNION *)(this));
	return *this;
}

void Quaternion::Normalize(Quaternion &ret) const
{
	::D3DXQuaternionNormalize((D3DXQUATERNION *)(&ret),(const D3DXQUATERNION *)(this));
}
Quaternion &Quaternion::NormalizeInPlace()
{
	::D3DXQuaternionNormalize((D3DXQUATERNION *)(this),(const D3DXQUATERNION *)(this));
	return *this;
}

void Quaternion::Inverse(Quaternion &ret) const
{
	::D3DXQuaternionInverse((D3DXQUATERNION *)(&ret),(const D3DXQUATERNION *)(this));
}

Quaternion &Quaternion::InverseInPlace()
{
	::D3DXQuaternionInverse((D3DXQUATERNION *)(this),(const D3DXQUATERNION *)(this));
	return *this;
}

Quaternion &Quaternion::RotationYawPitchRoll(float fYaw, float fPitch, float fRoll)
{
	::D3DXQuaternionRotationYawPitchRoll((D3DXQUATERNION *)(this),fYaw,fPitch,fRoll);
	return *this;
}

Quaternion &Quaternion::RotationAxis(const cn::Vec3 &vDir, float fAngle)
{
	::D3DXQuaternionRotationAxis((D3DXQUATERNION *)(this),(const D3DXVECTOR3 *)(&vDir), fAngle);
	return *this;
}

Quaternion &Quaternion::Identity()
{
	::D3DXQuaternionIdentity((D3DXQUATERNION *)(this));
	return *this;
}

void Quaternion::Decompose(float &fYaw, float &fPitch, float &fRoll) const
{
	float fSP =-2.0f*(y*z-w*x);
	if(::abs(fSP) > 0.9999f){ //若万向锁发生的话
		fPitch =1.570796f*fSP;
		fYaw =::atan2(-x*z+w*y, 0.5f-y*y-z*z);
		fRoll =0.0f;
	}else{
		fPitch =::asin(fSP);
		fYaw =::atan2(x*z+w*y, 0.5f-x*x-y*y);
		fRoll =::atan2(x*y+w*z, 0.5f-x*x-z*z);
	}
}


QuaternionD QuaternionD::m_qIdentity(0.0, 0.0, 0.0, 1.0);

QuaternionD::QuaternionD()
{
	x =y =z =0.0; w =1.0;
}

QuaternionD::QuaternionD(double dX, double dY, double dZ, double dW)
: x(dX)
, y(dY)
, z(dZ)
, w(dW)
{

}

QuaternionD::QuaternionD(const QuaternionD &src)
{
	x =src.x; y =src.y; z =src.z; w =src.w;
}

QuaternionD::QuaternionD(const Quaternion &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z); w =double(src.w);
}

QuaternionD &QuaternionD::operator = (const QuaternionD &src)
{
	x =src.x; y =src.y; z =src.z; w =src.w;
	return *this;
}

QuaternionD &QuaternionD::operator = (const Quaternion &src)
{
	x =double(src.x); y =double(src.y); z =double(src.z); w =double(src.w);
	return *this;
}

QuaternionD QuaternionD::operator * (const QuaternionD &src) const
{
	QuaternionD qTemp;
	qTemp.x =   x*src.w + z*src.y - y*src.z + w*src.x;
	qTemp.y =   y*src.w + x*src.z - z*src.x + w*src.y;
	qTemp.z =   z*src.w + y*src.x - x*src.y + w*src.z;
	qTemp.w = - x*src.x - y*src.y - z*src.z + w*src.w;
	return qTemp;
}

bool QuaternionD::operator == (const QuaternionD &src) const
{
	return ::memcmp(this,&src,sizeof(QuaternionD)) == 0;
}

bool QuaternionD::operator != (const QuaternionD &src) const
{
	return ::memcmp(this,&src,sizeof(QuaternionD)) != 0;
}

QuaternionD &QuaternionD::operator () (double dX, double dY, double dZ, double dW)
{
	x =dX; y =dY; z =dZ; w =dW;
	return *this;
}

QuaternionD &QuaternionD::operator () (const Quaternion &qSrc)
{
	x =double(qSrc.x); y =double(qSrc.y); z =double(qSrc.z); w =double(qSrc.w);
	return *this;
}

QuaternionD &QuaternionD::operator *= (const QuaternionD &src)
{
	*this = *this * src;
	return *this;
}

Quaternion QuaternionD::ToQuaternion() const
{
	Quaternion qTemp;
	qTemp(float(x), float(y), float(z), float(w));
	return qTemp;
}

void QuaternionD::ToQuaternion(Quaternion &ret) const
{
	ret(float(x), float(y), float(z), float(w));
}

double QuaternionD::Length() const
{
	return ::sqrt(x*x+y*y+z*z+w*w);
}

double QuaternionD::LengthSq() const
{
	return x*x+y*y+z*z+w*w;
}

void QuaternionD::Conjugate(QuaternionD &ret) const
{
	ret.x =-x; ret.y =-y; ret.z =-z; ret.w =w;
}

QuaternionD &QuaternionD::ConjugateInPlace()
{
	x =-x; y =-y; z =-z;
	return *this;
}

void QuaternionD::Normalize(QuaternionD &ret) const
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		double dDiv =1.0/dLen;
		ret.x =x*dDiv; ret.y =y*dDiv; ret.z =z*dDiv; ret.w =w*dDiv;
	}else{
		ret.x =ret.y =ret.z =0.0; ret.w =1.0;
	}
}

QuaternionD &QuaternionD::NormalizeInPlace()
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		double dDiv =1.0/dLen;
		x *=dDiv; y *=dDiv; z *=dDiv; w *=dDiv;
	}else{
		x =y =z =0.0; w =1.0;
	}
	return *this;
}

void QuaternionD::Inverse(QuaternionD &ret) const
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		double dDiv =1.0/dLen;
		ret.x =-x*dDiv; ret.y =-y*dDiv; ret.z =-z*dDiv; ret.w =w*dDiv;
	}else{
		ret.x =ret.y =ret.z =0.0; ret.w =1.0;
	}
}

QuaternionD &QuaternionD::InverseInPlace()
{
	double dLen =::sqrt(x*x+y*y+z*z+w*w);
	if(dLen > 0.0){
		double dDiv =1.0/dLen;
		x *=-dDiv; y *=-dDiv; z *=-dDiv; w *=dDiv;
	}else{
		x =y =z =0.0; w =1.0;
	}
	return *this;
}

QuaternionD &QuaternionD::Identity()
{
	x =y =z =0.0; w =1.0;
	return *this;
}

//表示一个4x4矩阵

Mat4x4 Mat4x4::m_matIdentity;

Mat4x4::Mat4x4()
{
	_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; _14 = 0.0f;
	_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; _24 = 0.0f;
	_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; _34 = 0.0f;
	_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; _44 = 1.0f;
}

Mat4x4::Mat4x4(float f11, float f12, float f13, float f14, 
			   float f21, float f22, float f23, float f24, 
			   float f31, float f32, float f33, float f34, 
			   float f41, float f42, float f43, float f44)
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

Mat4x4 Mat4x4::operator +(const Mat4x4 &src) const
{
	return (Mat4x4 &)((D3DXMATRIX &)(*this) + (D3DXMATRIX &)src); 
}

Mat4x4 Mat4x4::operator -(const Mat4x4 &src) const
{
	return (Mat4x4 &)((D3DXMATRIX &)(*this) - (D3DXMATRIX &)src); 
}

Mat4x4 Mat4x4::operator *(float src) const
{
	return (Mat4x4 &)((D3DXMATRIX &)(*this) * src);
}

Mat4x4 Mat4x4::operator /(float src) const
{
	return (Mat4x4 &)((D3DXMATRIX &)(*this) / src);
}

Mat4x4 Mat4x4::operator *(const Mat4x4 &src) const
{
	return (Mat4x4 &)((D3DXMATRIX &)(*this) * (D3DXMATRIX &)src);
}

bool Mat4x4::operator ==(const Mat4x4 &src) const
{
	return ::memcmp(this,&src,sizeof(Mat4x4)) == 0;
}

bool Mat4x4::operator !=(const Mat4x4 &src) const
{
	return ::memcmp(this,&src,sizeof(Mat4x4)) != 0;
}

Mat4x4 &Mat4x4::operator +=(const Mat4x4 &src)
{
	*this = *this + src;
	return *this;
}

Mat4x4 &Mat4x4::operator -=(const Mat4x4 &src)
{
	*this = *this - src;
	return *this;
}

Mat4x4 &Mat4x4::operator *=(float src)
{
	*this = *this * src;
	return *this;
}

Mat4x4 &Mat4x4::operator /=(float src)
{
	*this = *this / src;
	return *this;
}

Mat4x4 &Mat4x4::operator *=(const Mat4x4 &src)
{
	*this = *this * src;
	return *this;
}

Mat4x4 operator * (float src1, const Mat4x4 &src2)
{
	return (Mat4x4 &)(src1 * (D3DXMATRIX &)(src2));
}

std::ostream &operator << (std::ostream &ostr, const Mat4x4 &src)
{
	std::stringstream strstream;
	strstream<<src._11<<" "<<src._12<<" "<<src._13<<" "<<src._14<<" "
		     <<src._21<<" "<<src._22<<" "<<src._23<<" "<<src._24<<" " 
		     <<src._31<<" "<<src._32<<" "<<src._33<<" "<<src._34<<" "
		     <<src._41<<" "<<src._42<<" "<<src._43<<" "<<src._44;
	ostr<<strstream.str();
	return ostr;
}

//将缩放、旋转、平移几个分量组合为一个矩阵
Mat4x4 &Mat4x4::AffineTransform(float fScale, const Quaternion *lpRotate, const Vec3 *lpTranslation)
{
	::D3DXMatrixAffineTransformation((D3DXMATRIX *)(this),fScale,NULL,
		                             (const D3DXQUATERNION *)lpRotate,(const D3DXVECTOR3 *)lpTranslation);
	return *this;
}

Mat4x4 &Mat4x4::Transform(const Vec3 *lpScale, const Quaternion *lpRotate, const Vec3 *lpTranslation)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	float *pScale =lpDLLTLSData->m_pMatrix1;
	float *pRotate =lpDLLTLSData->m_pMatrix2;
	float *pTranslation =lpDLLTLSData->m_pMatrix3;

	::D3DXMatrixIdentity((D3DXMATRIX *)this);
	if(lpScale){
		(D3DXMATRIX &)(*this) *=*::D3DXMatrixScaling((D3DXMATRIX *)(pScale), lpScale->x, lpScale->y, lpScale->z);
	}
	if(lpRotate){
		(D3DXMATRIX &)(*this) *=*::D3DXMatrixRotationQuaternion((D3DXMATRIX *)(pRotate), (const D3DXQUATERNION *)(lpRotate));
	}
	if(lpTranslation){
		(D3DXMATRIX &)(*this) *=*::D3DXMatrixTranslation((D3DXMATRIX *)(pTranslation), lpTranslation->x, lpTranslation->y, lpTranslation->z);
	}

	return *this;
}

Mat4x4 &Mat4x4::Scale(const Vec3 &vScale)
{
	::D3DXMatrixScaling((D3DXMATRIX *)(this), vScale.x, vScale.y, vScale.z);
	return *this;
}

Mat4x4 &Mat4x4::Rotate(const Quaternion &qRotate)
{
	::D3DXMatrixRotationQuaternion((D3DXMATRIX *)(this), (const D3DXQUATERNION *)(&qRotate));
	return *this;
}

Mat4x4 &Mat4x4::Rotate(float fYaw, float fPitch, float fRoll)
{
	::D3DXMatrixRotationYawPitchRoll((D3DXMATRIX *)(this), fYaw, fPitch, fRoll);
	return *this;
}

Mat4x4 &Mat4x4::RotationAxis(const cn::Vec3 &vDir, float fAngle)
{
	::D3DXMatrixRotationAxis((D3DXMATRIX *)(this), (const D3DXVECTOR3 *)(&vDir), fAngle);
	return *this;
}

Mat4x4 &Mat4x4::Translation(const Vec3 &vTranslation)
{
	::D3DXMatrixTranslation((D3DXMATRIX *)(this), vTranslation.x, vTranslation.y, vTranslation.z);
	return *this;
}

Mat4x4 &Mat4x4::LookAt(const Vec3 &vEyePos, const Vec3 &vAt, const Vec3 &vUp)
{
	::D3DXMatrixLookAtLH((D3DXMATRIX *)(this), (const D3DXVECTOR3 *)(&vEyePos), (const D3DXVECTOR3 *)(&vAt), (const D3DXVECTOR3 *)(&vUp));
	return *this;
}

Mat4x4 &Mat4x4::PerspectiveFov(float fFovY, float fAspect, float fZNear, float fZFar)
{
	::D3DXMatrixPerspectiveFovLH((D3DXMATRIX *)(this), fFovY, fAspect, fZNear, fZFar);
	return *this;
}

Mat4x4 &Mat4x4::PerspectiveOffCenter(float fXMin, float fXMax, float fYMin, float fYMax, float fZMin, float fZMax)
{
	::D3DXMatrixPerspectiveOffCenterLH((D3DXMATRIX *)(this), fXMin, fXMax, fYMin, fYMax, fZMin, fZMax);
	return *this;
}

Mat4x4 &Mat4x4::Ortho(float fWidth, float fHeight, float fZNear, float fZFar)
{
	::D3DXMatrixOrthoLH((D3DXMATRIX *)(this), fWidth, fHeight, fZNear, fZFar);
	return *this;
}

Mat4x4 &Mat4x4::OrthoOffCenter(float fXMin, float fXMax, float fYMin, float fYMax, float fZMin, float fZMax)
{
	::D3DXMatrixOrthoOffCenterLH((D3DXMATRIX *)(this), fXMin, fXMax, fYMin, fYMax, fZMin, fZMax);
	return *this;
}

Mat4x4 &Mat4x4::UI2D(float fRTWidth, float fRTHeight, const cn::Vec2 &vCenterScale, const cn::Vec2 &vCenterTrans)
{
	_11 = vCenterScale.x*2.0f/fRTWidth;	_12 = 0.0f;				_13 = 0.0f; _14 = 0.0f;
	_21 = 0.0f;				_22 = vCenterScale.y*2.0f/fRTHeight;	_23 = 0.0f; _24 = 0.0f;
	_31 = 0.0f;				_32 = 0.0f;				_33 = 0.0f; _34 = 0.0f;
	_41 = vCenterTrans.x;	_42 = vCenterTrans.y;	_43 = 0.0f; _44 = 1.0f;
	return *this;
}

Mat4x4 &Mat4x4::Reflect(const Plane &qPlane)
{
	::D3DXMatrixReflect((D3DXMATRIX *)(this), (const D3DXPLANE *)(&qPlane));
	return *this;
}


//将一个组合矩阵拆分为缩放、旋转、平移几个分量
void Mat4x4::Decompose(Vec3 &vScale, Quaternion &qRotate, Vec3 &vTranslation) const
{
	::D3DXMatrixDecompose((D3DXVECTOR3 *)(&vScale),(D3DXQUATERNION *)(&qRotate),
		                  (D3DXVECTOR3 *)(&vTranslation),(const D3DXMATRIX *)this);
}
//将矩阵转化为行列式
float Mat4x4::Determinant() const
{
	return ::D3DXMatrixDeterminant((const D3DXMATRIX *)this);
}

Mat4x4 &Mat4x4::Identity()
{
	::D3DXMatrixIdentity((D3DXMATRIX *)this);
	return *this;
}
//求矩阵的逆矩阵
void Mat4x4::Inverse(Mat4x4 &ret) const
{
	::D3DXMatrixInverse((D3DXMATRIX *)(&ret),NULL,(const D3DXMATRIX *)this);
}

Mat4x4 &Mat4x4::InverseInPlace()
{
	::D3DXMatrixInverse((D3DXMATRIX *)this,NULL,(const D3DXMATRIX *)this);
	return *this;
}
//求矩阵的转置矩阵
void Mat4x4::Transpose(Mat4x4 &ret) const
{
	::D3DXMatrixTranspose((D3DXMATRIX *)(&ret),(const D3DXMATRIX *)this);
}

Mat4x4 &Mat4x4::TransposeInPlace()
{
	::D3DXMatrixTranspose((D3DXMATRIX *)this,(const D3DXMATRIX *)this);
	return *this;
}

void Mat4x4::GetFrustumPlanes(Plane lpFrustumPlanes[6]) const
{
   lpFrustumPlanes[0].a =m[0][3] + m[0][0];
   lpFrustumPlanes[0].b =m[1][3] + m[1][0];
   lpFrustumPlanes[0].c =m[2][3] + m[2][0];
   lpFrustumPlanes[0].d =m[3][3] + m[3][0];

   lpFrustumPlanes[1].a =m[0][3] - m[0][0];
   lpFrustumPlanes[1].b =m[1][3] - m[1][0];
   lpFrustumPlanes[1].c =m[2][3] - m[2][0];
   lpFrustumPlanes[1].d =m[3][3] - m[3][0];

   lpFrustumPlanes[2].a =m[0][3] - m[0][1];
   lpFrustumPlanes[2].b =m[1][3] - m[1][1];
   lpFrustumPlanes[2].c =m[2][3] - m[2][1];
   lpFrustumPlanes[2].d =m[3][3] - m[3][1];

   lpFrustumPlanes[3].a =m[0][3] + m[0][1];
   lpFrustumPlanes[3].b =m[1][3] + m[1][1];
   lpFrustumPlanes[3].c =m[2][3] + m[2][1];
   lpFrustumPlanes[3].d =m[3][3] + m[3][1];

   lpFrustumPlanes[4].a =m[0][2];
   lpFrustumPlanes[4].b =m[1][2];
   lpFrustumPlanes[4].c =m[2][2];
   lpFrustumPlanes[4].d =m[3][2];

   lpFrustumPlanes[5].a =m[0][3] - m[0][2];
   lpFrustumPlanes[5].b =m[1][3] - m[1][2];
   lpFrustumPlanes[5].c =m[2][3] - m[2][2];
   lpFrustumPlanes[5].d =m[3][3] - m[3][2];

   for(u32 i =0; i<6; i++){ 
      lpFrustumPlanes[i].NormalizeInPlace();
   }
}

Mat4x4D Mat4x4D::m_matIdentity;

Mat4x4D::Mat4x4D()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0;
}

Mat4x4D::Mat4x4D(double d11, double d12, double d13, double d14, 
			   double d21, double d22, double d23, double d24, 
			   double d31, double d32, double d33, double d34, 
			   double d41, double d42, double d43, double d44)
{
	_11 = d11; _12 = d12; _13 = d13; _14 = d14;
	_21 = d21; _22 = d22; _23 = d23; _24 = d24;
	_31 = d31; _32 = d32; _33 = d33; _34 = d34;
	_41 = d41; _42 = d42; _43 = d43; _44 = d44;
}

Mat4x4D::Mat4x4D(const Mat4x4 &src)
{
	_11 = double(src._11); _12 = double(src._12); _13 = double(src._13); _14 = double(src._14);
	_21 = double(src._21); _22 = double(src._22); _23 = double(src._23); _24 = double(src._24);
	_31 = double(src._31); _32 = double(src._32); _33 = double(src._33); _34 = double(src._34);
	_41 = double(src._41); _42 = double(src._42); _43 = double(src._43); _44 = double(src._44);
}

Mat4x4D &Mat4x4D::operator = (const Mat4x4 &src)
{
	_11 = double(src._11); _12 = double(src._12); _13 = double(src._13); _14 = double(src._14);
	_21 = double(src._21); _22 = double(src._22); _23 = double(src._23); _24 = double(src._24);
	_31 = double(src._31); _32 = double(src._32); _33 = double(src._33); _34 = double(src._34);
	_41 = double(src._41); _42 = double(src._42); _43 = double(src._43); _44 = double(src._44);
	return *this;
}

Mat4x4D Mat4x4D::operator +(const Mat4x4D &src) const
{
	return Mat4x4D(
		_11+src._11, _12+src._12, _13+src._13, _14+src._14,
		_21+src._21, _22+src._22, _23+src._23, _24+src._24,
		_31+src._31, _32+src._32, _33+src._33, _34+src._34,
		_41+src._41, _42+src._42, _43+src._43, _44+src._44);
}

Mat4x4D Mat4x4D::operator -(const Mat4x4D &src) const
{
	return Mat4x4D(
		_11-src._11, _12-src._12, _13-src._13, _14-src._14,
		_21-src._21, _22-src._22, _23-src._23, _24-src._24,
		_31-src._31, _32-src._32, _33-src._33, _34-src._34,
		_41-src._41, _42-src._42, _43-src._43, _44-src._44);
}

Mat4x4D Mat4x4D::operator *(double src) const
{
	return Mat4x4D(
		_11*src, _12*src, _13*src, _14*src,
		_21*src, _22*src, _23*src, _24*src,
		_31*src, _32*src, _33*src, _34*src,
		_41*src, _42*src, _43*src, _44*src);
}

Mat4x4D Mat4x4D::operator /(double src) const
{
	return Mat4x4D(
		_11/src, _12/src, _13/src, _14/src,
		_21/src, _22/src, _23/src, _24/src,
		_31/src, _32/src, _33/src, _34/src,
		_41/src, _42/src, _43/src, _44/src);
}

Mat4x4D Mat4x4D::operator *(const Mat4x4D &src) const
{
	return Mat4x4D(
		_11*src._11+_12*src._21+_13*src._31+_14*src._41, _11*src._12+_12*src._22+_13*src._32+_14*src._42, _11*src._13+_12*src._23+_13*src._33+_14*src._43, _11*src._14+_12*src._24+_13*src._34+_14*src._44,
		_21*src._11+_22*src._21+_23*src._31+_24*src._41, _21*src._12+_22*src._22+_23*src._32+_24*src._42, _21*src._13+_22*src._23+_23*src._33+_24*src._43, _21*src._14+_22*src._24+_23*src._34+_24*src._44,
		_31*src._11+_32*src._21+_33*src._31+_34*src._41, _31*src._12+_32*src._22+_33*src._32+_34*src._42, _31*src._13+_32*src._23+_33*src._33+_34*src._43, _31*src._14+_32*src._24+_33*src._34+_34*src._44,
		_41*src._11+_42*src._21+_43*src._31+_44*src._41, _41*src._12+_42*src._22+_43*src._32+_44*src._42, _41*src._13+_42*src._23+_43*src._33+_44*src._43, _41*src._14+_42*src._24+_43*src._34+_44*src._44);
}

bool Mat4x4D::operator ==(const Mat4x4D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat4x4D)) == 0;
}

bool Mat4x4D::operator !=(const Mat4x4D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat4x4D)) != 0;
}

Mat4x4D &Mat4x4D::operator +=(const Mat4x4D &src)
{
	*this = *this + src;
	return *this;
}

Mat4x4D &Mat4x4D::operator -=(const Mat4x4D &src)
{
	*this = *this - src;
	return *this;
}

Mat4x4D &Mat4x4D::operator *=(double src)
{
	*this = *this * src;
	return *this;
}

Mat4x4D &Mat4x4D::operator /=(double src)
{
	*this = *this / src;
	return *this;
}

Mat4x4D &Mat4x4D::operator *=(const Mat4x4D &src)
{
	*this = *this * src;
	return *this;
}

Mat4x4D operator * (double src1, const Mat4x4D &src2)
{
	return Mat4x4D(
		src2._11*src1, src2._12*src1, src2._13*src1, src2._14*src1,
		src2._21*src1, src2._22*src1, src2._23*src1, src2._24*src1,
		src2._31*src1, src2._32*src1, src2._33*src1, src2._34*src1,
		src2._41*src1, src2._42*src1, src2._43*src1, src2._44*src1);
}

Mat4x4 Mat4x4D::ToMat4x4() const
{
	return Mat4x4(
		float(_11), float(_12), float(_13), float(_14),
		float(_21), float(_22), float(_23), float(_24),
		float(_31), float(_32), float(_33), float(_34),
		float(_41), float(_42), float(_43), float(_44));
}

void Mat4x4D::ToMat4x4(Mat4x4 &ret) const
{
	ret._11 = float(_11); ret._12 = float(_12); ret._13 = float(_13); ret._14 = float(_14);
	ret._21 = float(_21); ret._22 = float(_22); ret._23 = float(_23); ret._24 = float(_24);
	ret._31 = float(_31); ret._32 = float(_32); ret._33 = float(_33); ret._34 = float(_34);
	ret._41 = float(_41); ret._42 = float(_42); ret._43 = float(_43); ret._44 = float(_44);
}

Mat4x4D &Mat4x4D::AffineTransform(double dScale, const QuaternionD *lpRotate, const Vec3D *lpTranslation)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	//Mat4x4D &matScale =(Mat4x4D &)(*(lpDLLTLSData->m_pScale_d));
	Mat4x4D &matRotate =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix2_d));
	Mat4x4D &matTranslation =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix3_d));

	Scale(cn::Vec3D(dScale, dScale, dScale));
	if(lpRotate){
		(*this) *=(matRotate.Rotate(*lpRotate));
	}
	if(lpTranslation){
		(*this) *=(matTranslation.Translation(*lpTranslation));
	}

	return *this;
}

Mat4x4D &Mat4x4D::Transform(const Vec3D *lpScale, const QuaternionD *lpRotate, const Vec3D *lpTranslation)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Mat4x4D &matScale =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix1_d));
	Mat4x4D &matRotate =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix2_d));
	Mat4x4D &matTranslation =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix3_d));

	Identity();
	if(lpScale){
		(*this) *=(matScale.Scale(*lpScale));
	}
	if(lpRotate){
		(*this) *=(matRotate.Rotate(*lpRotate));
	}
	if(lpTranslation){
		(*this) *=(matTranslation.Translation(*lpTranslation));
	}

	return *this;
}

Mat4x4D &Mat4x4D::Scale(const Vec3D &vScale)
{
	_11 = vScale.x; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = vScale.y; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = vScale.z; _34 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0;
	
	return *this;
}

Mat4x4D &Mat4x4D::Rotate(const QuaternionD &qRotate)
{
	double dR_X =(qRotate.x != -0.0) ? qRotate.x : 0.0;
	double dR_Y =(qRotate.y != -0.0) ? qRotate.y : 0.0;
	double dR_Z =(qRotate.z != -0.0) ? qRotate.z : 0.0;
	double dR_W =(qRotate.w != -0.0) ? qRotate.w : 0.0;
	if((dR_X==0.0 && dR_Y==0.0 && dR_Z==-_SQRT_2_DIV_2 && dR_W==_SQRT_2_DIV_2) ||
		(dR_X==0.0 && dR_Y==0.0 && dR_Z==_SQRT_2_DIV_2 && dR_W==-_SQRT_2_DIV_2))
	{
		_11 = 0.0;	_12 = -1.0;	_13 = 0.0;	_14 = 0.0;
		_21 = 1.0;	_22 = 0.0;	_23 = 0.0;	_24 = 0.0;
		_31 = 0.0;	_32 = 0.0;	_33 = 1.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else if((dR_X==0.0 && dR_Y==0.0 && dR_Z==_SQRT_2_DIV_2 && dR_W==_SQRT_2_DIV_2) ||
		(dR_X==0.0 && dR_Y==0.0 && dR_Z==-_SQRT_2_DIV_2 && dR_W==-_SQRT_2_DIV_2))
	{
		_11 = 0.0;	_12 = 1.0;	_13 = 0.0;	_14 = 0.0;
		_21 = -1.0;	_22 = 0.0;	_23 = 0.0;	_24 = 0.0;
		_31 = 0.0;	_32 = 0.0;	_33 = 1.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else if((dR_X==0.0 && dR_Y==0.0 && dR_Z==0.0 && dR_W==1.0) ||
		(dR_X==0.0 && dR_Y==0.0 && dR_Z==0.0 && dR_W==-1.0))
	{
		_11 = 1.0;	_12 = 0.0;	_13 = 0.0;	_14 = 0.0;
		_21 = 0.0;	_22 = 1.0;	_23 = 0.0;	_24 = 0.0;
		_31 = 0.0;	_32 = 0.0;	_33 = 1.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else if((dR_X==0.0 && dR_Y==0.0 && dR_Z==1.0 && dR_W==0.0) ||
		(dR_X==0.0 && dR_Y==0.0 && dR_Z==-1.0 && dR_W==0.0))
	{
		_11 = -1.0;	_12 = 0.0;	_13 = 0.0;	_14 = 0.0;
		_21 = 0.0;	_22 = -1.0;	_23 = 0.0;	_24 = 0.0;
		_31 = 0.0;	_32 = 0.0;	_33 = 1.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else if((dR_X==_SQRT_2_DIV_2 && dR_Y==0.0 && dR_Z==0.0 && dR_W==_SQRT_2_DIV_2) ||
		(dR_X==-_SQRT_2_DIV_2 && dR_Y==0.0 && dR_Z==0.0 && dR_W==-_SQRT_2_DIV_2))
	{
		_11 = 1.0;	_12 = 0.0;	_13 = 0.0;	_14 = 0.0;
		_21 = 0.0;	_22 = 0.0;	_23 = 1.0;	_24 = 0.0;
		_31 = 0.0;	_32 = -1.0;	_33 = 0.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else if((dR_X==-_SQRT_2_DIV_2 && dR_Y==0.0 && dR_Z==0.0 && dR_W==_SQRT_2_DIV_2) ||
		(dR_X==_SQRT_2_DIV_2 && dR_Y==0.0 && dR_Z==0.0 && dR_W==-_SQRT_2_DIV_2))
	{
		_11 = 1.0;	_12 = 0.0;	_13 = 0.0;	_14 = 0.0;
		_21 = 0.0;	_22 = 0.0;	_23 = -1.0;	_24 = 0.0;
		_31 = 0.0;	_32 = 1.0;	_33 = 0.0;	_34 = 0.0;
		_41 = 0.0;	_42 = 0.0;	_43 = 0.0;	_44 = 1.0;
	}else{
		double dX_2 =2.0*qRotate.x*qRotate.x;
		double dY_2 =2.0*qRotate.y*qRotate.y;
		double dZ_2 =2.0*qRotate.z*qRotate.z;
		double dXY =2.0*qRotate.x*qRotate.y;
		double dYZ =2.0*qRotate.y*qRotate.z;
		double dXZ =2.0*qRotate.x*qRotate.z;
		double dWX =2.0*qRotate.w*qRotate.x;
		double dWY =2.0*qRotate.w*qRotate.y;
		double dWZ =2.0*qRotate.w*qRotate.z;
		_11 = 1.0-dY_2-dZ_2;	_12 = dXY+dWZ;			_13 = dXZ-dWY;			_14 = 0.0;
		_21 = dXY-dWZ;			_22 = 1.0-dX_2-dZ_2;	_23 = dYZ+dWX;			_24 = 0.0;
		_31 = dXZ+dWY;			_32 = dYZ-dWX;			_33 = 1.0-dX_2-dY_2;	_34 = 0.0;
		_41 = 0.0;				_42 = 0.0;				_43 = 0.0;				_44 = 1.0;
	}
	
	return *this;
}

Mat4x4D &Mat4x4D::Rotate(double dYaw, double dPitch, double dRoll)
{
	double dSinH =::sin(dYaw); double dCosH =::cos(dYaw);
	double dSinP =::sin(dPitch); double dCosP =::cos(dPitch);
	double dSinB =::sin(dRoll); double dCosB =::cos(dRoll);
	_11 = dCosH*dCosB+dSinH*dSinP*dSinB;	_12 = dSinB*dCosP;	_13 = -dSinH*dCosB+dCosH*dSinP*dSinB;	_14 = 0.0;
	_21 = -dCosH*dSinB+dSinH*dSinP*dCosB;	_22 = dCosB*dCosP;	_23 = dSinH*dSinB+dCosH*dSinP*dCosB;	_24 = 0.0;
	_31 = dSinH*dCosP;						_32 = -dSinP;		_33 = dCosH*dCosP;						_34 = 0.0;
	_41 = 0.0;								_42 = 0.0;			_43 = 0.0;								_44 = 1.0;
	
	return *this;
}

Mat4x4D &Mat4x4D::RotationAxis(const cn::Vec3D &vDir, double dAngle)
{
	double dSinA =::sin(dAngle/2.0); double dCosA =::cos(dAngle/2.0);
	QuaternionD qRotate(dSinA*vDir.x, dSinA*vDir.y, dSinA*vDir.z, dCosA);
	return Rotate(qRotate);
}

Mat4x4D &Mat4x4D::Translation(const Vec3D &vTranslation)
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0;
	_41 = vTranslation.x; _42 = vTranslation.y; _43 = vTranslation.z; _44 = 1.0;
	
	return *this;
}

Mat4x4D &Mat4x4D::LookAt(const Vec3D &vEyePos, const Vec3D &vAt, const Vec3D &vUp)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	Vec3D &vAxisX =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp));
	Vec3D &vAxisY =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp2));
	Vec3D &vAxisZ =(Vec3D &)(*(lpDLLTLSData->m_pVecDTemp3));
	vAxisZ =vAt-vEyePos; vAxisZ.NormalizeInPlace();
	cn::Math::Vec3DCross(vUp, vAxisZ, vAxisX); vAxisX.NormalizeInPlace();
	cn::Math::Vec3DCross(vAxisZ, vAxisX, vAxisY);

	_11 = vAxisX.x; _12 = vAxisY.x; _13 = vAxisZ.x; _14 = 0.0;
	_21 = vAxisX.y; _22 = vAxisY.y; _23 = vAxisZ.y; _24 = 0.0;
	_31 = vAxisX.z; _32 = vAxisY.z; _33 = vAxisZ.z; _34 = 0.0;
	_41 = -cn::Math::Vec3DDot(vAxisX, vEyePos); _42 = -cn::Math::Vec3DDot(vAxisY, vEyePos); _43 = -cn::Math::Vec3DDot(vAxisZ, vEyePos); _44 = 1.0;

	return *this;
}

Mat4x4D &Mat4x4D::PerspectiveFov(double dFovY, double dAspect, double dZNear, double dZFar)
{
	double dYScale =1.0/::tan(dFovY/2.0);
	double dXScale =dYScale/dAspect;

	_11 = dXScale; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = dYScale; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = dZFar/(dZFar-dZNear); _34 = 1.0;
	_41 = 0.0; _42 = 0.0; _43 = dZNear*dZFar/(dZNear-dZFar); _44 = 0.0;

	return *this;
}

Mat4x4D &Mat4x4D::PerspectiveOffCenter(double dXMin, double dXMax, double dYMin, double dYMax, double dZMin, double dZMax)
{
	_11 = 2.0*dZMin/(dXMax-dXMin); _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 2.0*dZMin/(dYMax-dYMin); _23 = 0.0; _24 = 0.0;
	_31 = (dXMin+dXMax)/(dXMin-dXMax); _32 = (dYMin+dYMax)/(dYMin-dYMax); _33 = dZMax/(dZMax-dZMin); _34 = 1.0;
	_41 = 0.0; _42 = 0.0; _43 = dZMin*dZMax/(dZMin-dZMax); _44 = 0.0;

	return *this;
}

Mat4x4D &Mat4x4D::Ortho(double dWidth, double dHeight, double dZNear, double dZFar)
{
	_11 = 2.0/dWidth; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 2.0/dHeight; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0/(dZFar-dZNear); _34 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = dZNear/(dZNear-dZFar); _44 = 1.0;

	return *this;
}

Mat4x4D &Mat4x4D::OrthoOffCenter(double dXMin, double dXMax, double dYMin, double dYMax, double dZMin, double dZMax)
{
	_11 = 2.0/(dXMax-dXMin); _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 2.0/(dYMax-dYMin); _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0/(dZMax-dZMin); _34 = 0.0;
	_41 = (dXMin+dXMax)/(dXMin-dXMax); _42 = (dYMin+dYMax)/(dYMin-dYMax); _43 = dZMin/(dZMin-dZMax); _44 = 1.0;

	return *this;
}

Mat4x4D &Mat4x4D::Reflect(const PlaneD &qPlane)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	PlaneD &qT =(PlaneD &)(*(lpDLLTLSData->m_pPlaneDTemp));
	qPlane.Normalize(qT);

	_11 = -2.0*qT.a*qT.a+1.0;	_12 = -2.0*qT.b*qT.a;		_13 = -2.0*qT.c*qT.a;		_14 = 0.0;
	_21 = -2.0*qT.a*qT.b;		_22 = -2.0*qT.b*qT.b+1.0;	_23 = -2.0*qT.c*qT.b;		_24 = 0.0;
	_31 = -2.0*qT.a*qT.c;		_32 = -2.0*qT.b*qT.c;		_33 = -2.0*qT.c*qT.c+1.0;	_34 = 0.0;
	_41 = -2.0*qT.a*qT.d;		_42 = -2.0*qT.b*qT.d;		_43 = -2.0*qT.c*qT.d;		_44 = 1.0;

	return *this;
}

void Mat4x4D::Decompose(Vec3D &vScale, QuaternionD &qRotate, Vec3D &vTranslation) const
{
	vTranslation(_41, _42, _43);
	vScale.x =::sqrt(_11*_11+_12*_12+_13*_13);
	vScale.y =::sqrt(_21*_21+_22*_22+_23*_23);
	vScale.z =::sqrt(_31*_31+_32*_32+_33*_33);
	double dM11, dM12, dM13;
	double dM21, dM22, dM23;
	double dM31, dM32, dM33;
	if(vScale.x != 0.0){dM11 =_11/vScale.x; dM12 =_12/vScale.x; dM13 =_13/vScale.x;}else{dM11 =_11; dM12 =_12; dM13 =_13;}
	if(vScale.y != 0.0){dM21 =_21/vScale.y; dM22 =_22/vScale.y; dM23 =_23/vScale.y;}else{dM21 =_21; dM22 =_22; dM23 =_23;}
	if(vScale.z != 0.0){dM31 =_31/vScale.z; dM32 =_32/vScale.z; dM33 =_33/vScale.z;}else{dM31 =_31; dM32 =_32; dM33 =_33;}
	double dWBase =dM11+dM22+dM33;
	double dXBase =dM11-dM22-dM33;
	double dYBase =dM22-dM11-dM33;
	double dZBase =dM33-dM11-dM22;
	cn::u32 uSel =0; double dMaxBase =dWBase;
	if(dXBase > dMaxBase){uSel =1; dMaxBase =dXBase;}
	if(dYBase > dMaxBase){uSel =2; dMaxBase =dYBase;}
	if(dZBase > dMaxBase){uSel =3; dMaxBase =dZBase;}
	double dMaxVal =::sqrt(dMaxBase+1.0)*0.5;
	double dMult =0.25/dMaxVal;
	switch(uSel)
	{
	case 0:
		{
			qRotate((dM23-dM32)*dMult, (dM31-dM13)*dMult, (dM12-dM21)*dMult, dMaxVal);
		}
		break;
	case 1:
		{
			qRotate(dMaxVal, (dM12+dM21)*dMult, (dM31+dM13)*dMult, (dM23-dM32)*dMult);
		}
		break;
	case 2:
		{
			qRotate((dM12+dM21)*dMult, dMaxVal, (dM23+dM32)*dMult, (dM31-dM13)*dMult);
		}
		break;
	case 3:
		{
			qRotate((dM31+dM13)*dMult, (dM23+dM32)*dMult, dMaxVal, (dM12-dM21)*dMult);
		}
		break;
	default:
		{
			qRotate =cn::QuaternionD::m_qIdentity;
		}
		break;
	}
}

double Mat4x4D::Determinant() const
{
	return _GEN_SUB_DETERMINANT_4((*this),1,2,3,4,1,2,3,4);
}

Mat4x4D &Mat4x4D::Identity()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0;
	
	return *this;
}

void Mat4x4D::Inverse(Mat4x4D &ret) const
{
	double dDeterminant =Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		ret._11 =_GEN_SUB_DETERMINANT_3((*this),2,3,4,2,3,4)*dDiv;
		ret._21 =-_GEN_SUB_DETERMINANT_3((*this),2,3,4,1,3,4)*dDiv;
		ret._31 =_GEN_SUB_DETERMINANT_3((*this),2,3,4,1,2,4)*dDiv;
		ret._41 =-_GEN_SUB_DETERMINANT_3((*this),2,3,4,1,2,3)*dDiv;

		ret._12 =-_GEN_SUB_DETERMINANT_3((*this),1,3,4,2,3,4)*dDiv;
		ret._22 =_GEN_SUB_DETERMINANT_3((*this),1,3,4,1,3,4)*dDiv;
		ret._32 =-_GEN_SUB_DETERMINANT_3((*this),1,3,4,1,2,4)*dDiv;
		ret._42 =_GEN_SUB_DETERMINANT_3((*this),1,3,4,1,2,3)*dDiv;

		ret._13 =_GEN_SUB_DETERMINANT_3((*this),1,2,4,2,3,4)*dDiv;
		ret._23 =-_GEN_SUB_DETERMINANT_3((*this),1,2,4,1,3,4)*dDiv;
		ret._33 =_GEN_SUB_DETERMINANT_3((*this),1,2,4,1,2,4)*dDiv;
		ret._43 =-_GEN_SUB_DETERMINANT_3((*this),1,2,4,1,2,3)*dDiv;

		ret._14 =-_GEN_SUB_DETERMINANT_3((*this),1,2,3,2,3,4)*dDiv;
		ret._24 =_GEN_SUB_DETERMINANT_3((*this),1,2,3,1,3,4)*dDiv;
		ret._34 =-_GEN_SUB_DETERMINANT_3((*this),1,2,3,1,2,4)*dDiv;
		ret._44 =_GEN_SUB_DETERMINANT_3((*this),1,2,3,1,2,3)*dDiv;
	}
}

Mat4x4D &Mat4x4D::InverseInPlace()
{
	double dDeterminant =Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		Mat4x4D &matTemp =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix1_d));
		matTemp =*this;

		_11 =_GEN_SUB_DETERMINANT_3(matTemp,2,3,4,2,3,4)*dDiv;
		_21 =-_GEN_SUB_DETERMINANT_3(matTemp,2,3,4,1,3,4)*dDiv;
		_31 =_GEN_SUB_DETERMINANT_3(matTemp,2,3,4,1,2,4)*dDiv;
		_41 =-_GEN_SUB_DETERMINANT_3(matTemp,2,3,4,1,2,3)*dDiv;

		_12 =-_GEN_SUB_DETERMINANT_3(matTemp,1,3,4,2,3,4)*dDiv;
		_22 =_GEN_SUB_DETERMINANT_3(matTemp,1,3,4,1,3,4)*dDiv;
		_32 =-_GEN_SUB_DETERMINANT_3(matTemp,1,3,4,1,2,4)*dDiv;
		_42 =_GEN_SUB_DETERMINANT_3(matTemp,1,3,4,1,2,3)*dDiv;

		_13 =_GEN_SUB_DETERMINANT_3(matTemp,1,2,4,2,3,4)*dDiv;
		_23 =-_GEN_SUB_DETERMINANT_3(matTemp,1,2,4,1,3,4)*dDiv;
		_33 =_GEN_SUB_DETERMINANT_3(matTemp,1,2,4,1,2,4)*dDiv;
		_43 =-_GEN_SUB_DETERMINANT_3(matTemp,1,2,4,1,2,3)*dDiv;

		_14 =-_GEN_SUB_DETERMINANT_3(matTemp,1,2,3,2,3,4)*dDiv;
		_24 =_GEN_SUB_DETERMINANT_3(matTemp,1,2,3,1,3,4)*dDiv;
		_34 =-_GEN_SUB_DETERMINANT_3(matTemp,1,2,3,1,2,4)*dDiv;
		_44 =_GEN_SUB_DETERMINANT_3(matTemp,1,2,3,1,2,3)*dDiv;
	}

	return *this;
}

void Mat4x4D::Transpose(Mat4x4D &ret) const
{
	ret._11 =_11; ret._12 =_21; ret._13 =_31; ret._14 =_41;
	ret._21 =_12; ret._22 =_22; ret._23 =_32; ret._24 =_42;
	ret._31 =_13; ret._32 =_23; ret._33 =_33; ret._34 =_43;
	ret._41 =_14; ret._42 =_24; ret._43 =_34; ret._44 =_44;
}

Mat4x4D &Mat4x4D::TransposeInPlace()
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	Mat4x4D &matTemp =(Mat4x4D &)(*(lpDLLTLSData->m_pMatrix1_d));
	matTemp =*this;

	_11 =matTemp._11; _12 =matTemp._21; _13 =matTemp._31; _14 =matTemp._41;
	_21 =matTemp._12; _22 =matTemp._22; _23 =matTemp._32; _24 =matTemp._42;
	_31 =matTemp._13; _32 =matTemp._23; _33 =matTemp._33; _34 =matTemp._43;
	_41 =matTemp._14; _42 =matTemp._24; _43 =matTemp._34; _44 =matTemp._44;

	return *this;
}

void Mat4x4D::GetFrustumPlanes(PlaneD lpFrustumPlanes[6]) const
{
   lpFrustumPlanes[0].a =m[0][3] + m[0][0];
   lpFrustumPlanes[0].b =m[1][3] + m[1][0];
   lpFrustumPlanes[0].c =m[2][3] + m[2][0];
   lpFrustumPlanes[0].d =m[3][3] + m[3][0];

   lpFrustumPlanes[1].a =m[0][3] - m[0][0];
   lpFrustumPlanes[1].b =m[1][3] - m[1][0];
   lpFrustumPlanes[1].c =m[2][3] - m[2][0];
   lpFrustumPlanes[1].d =m[3][3] - m[3][0];

   lpFrustumPlanes[2].a =m[0][3] - m[0][1];
   lpFrustumPlanes[2].b =m[1][3] - m[1][1];
   lpFrustumPlanes[2].c =m[2][3] - m[2][1];
   lpFrustumPlanes[2].d =m[3][3] - m[3][1];

   lpFrustumPlanes[3].a =m[0][3] + m[0][1];
   lpFrustumPlanes[3].b =m[1][3] + m[1][1];
   lpFrustumPlanes[3].c =m[2][3] + m[2][1];
   lpFrustumPlanes[3].d =m[3][3] + m[3][1];

   lpFrustumPlanes[4].a =m[0][2];
   lpFrustumPlanes[4].b =m[1][2];
   lpFrustumPlanes[4].c =m[2][2];
   lpFrustumPlanes[4].d =m[3][2];

   lpFrustumPlanes[5].a =m[0][3] - m[0][2];
   lpFrustumPlanes[5].b =m[1][3] - m[1][2];
   lpFrustumPlanes[5].c =m[2][3] - m[2][2];
   lpFrustumPlanes[5].d =m[3][3] - m[3][2];

   for(u32 i =0; i<6; i++){ 
      lpFrustumPlanes[i].NormalizeInPlace();
   }
}

Mat5x5D Mat5x5D::m_matIdentity;

Mat5x5D::Mat5x5D()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0; _15 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0; _25 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0; _35 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0; _45 = 0.0;
	_51 = 0.0; _52 = 0.0; _53 = 0.0; _54 = 0.0; _55 = 1.0;
}

Mat5x5D::Mat5x5D(double d11, double d12, double d13, double d14, double d15,
			double d21, double d22, double d23, double d24, double d25,
			double d31, double d32, double d33, double d34, double d35,
			double d41, double d42, double d43, double d44, double d45,
			double d51, double d52, double d53, double d54, double d55)
{
	_11 = d11; _12 = d12; _13 = d13; _14 = d14; _15 = d15;
	_21 = d21; _22 = d22; _23 = d23; _24 = d24; _25 = d25;
	_31 = d31; _32 = d32; _33 = d33; _34 = d34; _35 = d35;
	_41 = d41; _42 = d42; _43 = d43; _44 = d44; _45 = d45;
	_51 = d51; _52 = d52; _53 = d53; _54 = d54; _55 = d55;
}

Mat5x5D Mat5x5D::operator +(const Mat5x5D &src) const
{
	return Mat5x5D(
		_11+src._11, _12+src._12, _13+src._13, _14+src._14, _15+src._15,
		_21+src._21, _22+src._22, _23+src._23, _24+src._24, _25+src._25,
		_31+src._31, _32+src._32, _33+src._33, _34+src._34, _35+src._35,
		_41+src._41, _42+src._42, _43+src._43, _44+src._44, _45+src._45,
		_51+src._51, _52+src._52, _53+src._53, _54+src._54, _55+src._55);
}

Mat5x5D Mat5x5D::operator -(const Mat5x5D &src) const
{
	return Mat5x5D(
		_11-src._11, _12-src._12, _13-src._13, _14-src._14, _15-src._15,
		_21-src._21, _22-src._22, _23-src._23, _24-src._24, _25-src._25,
		_31-src._31, _32-src._32, _33-src._33, _34-src._34, _35-src._35,
		_41-src._41, _42-src._42, _43-src._43, _44-src._44, _45-src._45,
		_51-src._51, _52-src._52, _53-src._53, _54-src._54, _55-src._55);
}

Mat5x5D Mat5x5D::operator *(double src) const
{
	return Mat5x5D(
		_11*src, _12*src, _13*src, _14*src, _15*src,
		_21*src, _22*src, _23*src, _24*src, _25*src,
		_31*src, _32*src, _33*src, _34*src, _35*src,
		_41*src, _42*src, _43*src, _44*src, _45*src,
		_51*src, _52*src, _53*src, _54*src, _55*src);
}

Mat5x5D Mat5x5D::operator /(double src) const
{
	return Mat5x5D(
		_11/src, _12/src, _13/src, _14/src, _15/src,
		_21/src, _22/src, _23/src, _24/src, _25/src,
		_31/src, _32/src, _33/src, _34/src, _35/src,
		_41/src, _42/src, _43/src, _44/src, _45/src,
		_51/src, _52/src, _53/src, _54/src, _55/src);
}

Mat5x5D Mat5x5D::operator *(const Mat5x5D &src) const
{
	return Mat5x5D(
		_11*src._11+_12*src._21+_13*src._31+_14*src._41+_15*src._51, _11*src._12+_12*src._22+_13*src._32+_14*src._42+_15*src._52, _11*src._13+_12*src._23+_13*src._33+_14*src._43+_15*src._53, _11*src._14+_12*src._24+_13*src._34+_14*src._44+_15*src._54, _11*src._15+_12*src._25+_13*src._35+_14*src._45+_15*src._55,
		_21*src._11+_22*src._21+_23*src._31+_24*src._41+_25*src._51, _21*src._12+_22*src._22+_23*src._32+_24*src._42+_25*src._52, _21*src._13+_22*src._23+_23*src._33+_24*src._43+_25*src._53, _21*src._14+_22*src._24+_23*src._34+_24*src._44+_25*src._54, _21*src._15+_22*src._25+_23*src._35+_24*src._45+_25*src._55,
		_31*src._11+_32*src._21+_33*src._31+_34*src._41+_35*src._51, _31*src._12+_32*src._22+_33*src._32+_34*src._42+_35*src._52, _31*src._13+_32*src._23+_33*src._33+_34*src._43+_35*src._53, _31*src._14+_32*src._24+_33*src._34+_34*src._44+_35*src._54, _31*src._15+_32*src._25+_33*src._35+_34*src._45+_35*src._55,
		_41*src._11+_42*src._21+_43*src._31+_44*src._41+_45*src._51, _41*src._12+_42*src._22+_43*src._32+_44*src._42+_45*src._52, _41*src._13+_42*src._23+_43*src._33+_44*src._43+_45*src._53, _41*src._14+_42*src._24+_43*src._34+_44*src._44+_45*src._54, _41*src._15+_42*src._25+_43*src._35+_44*src._45+_45*src._55,
		_51*src._11+_52*src._21+_53*src._31+_54*src._41+_55*src._51, _51*src._12+_52*src._22+_53*src._32+_54*src._42+_55*src._52, _51*src._13+_52*src._23+_53*src._33+_54*src._43+_55*src._53, _51*src._14+_52*src._24+_53*src._34+_54*src._44+_55*src._54, _51*src._15+_52*src._25+_53*src._35+_54*src._45+_55*src._55
		);
}

bool Mat5x5D::operator ==(const Mat5x5D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat5x5D)) == 0;
}

bool Mat5x5D::operator !=(const Mat5x5D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat5x5D)) != 0;
}

Mat5x5D &Mat5x5D::operator +=(const Mat5x5D &src)
{
	*this = *this + src;
	return *this;
}

Mat5x5D &Mat5x5D::operator -=(const Mat5x5D &src)
{
	*this = *this - src;
	return *this;
}

Mat5x5D &Mat5x5D::operator *=(double src)
{
	*this = *this * src;
	return *this;
}

Mat5x5D &Mat5x5D::operator /=(double src)
{
	*this = *this / src;
	return *this;
}

Mat5x5D &Mat5x5D::operator *=(const Mat5x5D &src)
{
	*this = *this * src;
	return *this;
}

Mat5x5D &Mat5x5D::operator () (double d11, double d12, double d13, double d14, double d15,
					double d21, double d22, double d23, double d24, double d25,
					double d31, double d32, double d33, double d34, double d35,
					double d41, double d42, double d43, double d44, double d45,
					double d51, double d52, double d53, double d54, double d55
					)
{
	_11 = d11; _12 = d12; _13 = d13; _14 = d14; _15 = d15;
	_21 = d21; _22 = d22; _23 = d23; _24 = d24; _25 = d25;
	_31 = d31; _32 = d32; _33 = d33; _34 = d34; _35 = d35;
	_41 = d41; _42 = d42; _43 = d43; _44 = d44; _45 = d45;
	_51 = d51; _52 = d52; _53 = d53; _54 = d54; _55 = d55;

	return (*this);
}

Mat5x5D operator * (double src1, const Mat5x5D &src2)
{
	return Mat5x5D(
		src2._11*src1, src2._12*src1, src2._13*src1, src2._14*src1, src2._15*src1,
		src2._21*src1, src2._22*src1, src2._23*src1, src2._24*src1, src2._25*src1,
		src2._31*src1, src2._32*src1, src2._33*src1, src2._34*src1, src2._35*src1,
		src2._41*src1, src2._42*src1, src2._43*src1, src2._44*src1, src2._45*src1,
		src2._51*src1, src2._52*src1, src2._53*src1, src2._54*src1, src2._55*src1);
}

double Mat5x5D::Determinant() const
{
	return _GEN_SUB_DETERMINANT_5((*this),1,2,3,4,5,1,2,3,4,5);
}

Mat5x5D &Mat5x5D::Identity()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0; _14 = 0.0; _15 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0; _24 = 0.0; _25 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0; _34 = 0.0; _35 = 0.0;
	_41 = 0.0; _42 = 0.0; _43 = 0.0; _44 = 1.0; _45 = 0.0;
	_51 = 0.0; _52 = 0.0; _53 = 0.0; _54 = 0.0; _55 = 1.0;
	
	return *this;
}

void Mat5x5D::Inverse(Mat5x5D &ret, const double *pDet) const
{
	double dDeterminant =pDet ? (*pDet) : Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		ret._11 =_GEN_SUB_DETERMINANT_4((*this),2,3,4,5,2,3,4,5)*dDiv;
		ret._21 =-_GEN_SUB_DETERMINANT_4((*this),2,3,4,5,1,3,4,5)*dDiv;
		ret._31 =_GEN_SUB_DETERMINANT_4((*this),2,3,4,5,1,2,4,5)*dDiv;
		ret._41 =-_GEN_SUB_DETERMINANT_4((*this),2,3,4,5,1,2,3,5)*dDiv;
		ret._51 =_GEN_SUB_DETERMINANT_4((*this),2,3,4,5,1,2,3,4)*dDiv;

		ret._12 =-_GEN_SUB_DETERMINANT_4((*this),1,3,4,5,2,3,4,5)*dDiv;
		ret._22 =_GEN_SUB_DETERMINANT_4((*this),1,3,4,5,1,3,4,5)*dDiv;
		ret._32 =-_GEN_SUB_DETERMINANT_4((*this),1,3,4,5,1,2,4,5)*dDiv;
		ret._42 =_GEN_SUB_DETERMINANT_4((*this),1,3,4,5,1,2,3,5)*dDiv;
		ret._52 =-_GEN_SUB_DETERMINANT_4((*this),1,3,4,5,1,2,3,4)*dDiv;

		ret._13 =_GEN_SUB_DETERMINANT_4((*this),1,2,4,5,2,3,4,5)*dDiv;
		ret._23 =-_GEN_SUB_DETERMINANT_4((*this),1,2,4,5,1,3,4,5)*dDiv;
		ret._33 =_GEN_SUB_DETERMINANT_4((*this),1,2,4,5,1,2,4,5)*dDiv;
		ret._43 =-_GEN_SUB_DETERMINANT_4((*this),1,2,4,5,1,2,3,5)*dDiv;
		ret._53 =_GEN_SUB_DETERMINANT_4((*this),1,2,4,5,1,2,3,4)*dDiv;

		ret._14 =-_GEN_SUB_DETERMINANT_4((*this),1,2,3,5,2,3,4,5)*dDiv;
		ret._24 =_GEN_SUB_DETERMINANT_4((*this),1,2,3,5,1,3,4,5)*dDiv;
		ret._34 =-_GEN_SUB_DETERMINANT_4((*this),1,2,3,5,1,2,4,5)*dDiv;
		ret._44 =_GEN_SUB_DETERMINANT_4((*this),1,2,3,5,1,2,3,5)*dDiv;
		ret._54 =-_GEN_SUB_DETERMINANT_4((*this),1,2,3,5,1,2,3,4)*dDiv;

		ret._15 =_GEN_SUB_DETERMINANT_4((*this),1,2,3,4,2,3,4,5)*dDiv;
		ret._25 =-_GEN_SUB_DETERMINANT_4((*this),1,2,3,4,1,3,4,5)*dDiv;
		ret._35 =_GEN_SUB_DETERMINANT_4((*this),1,2,3,4,1,2,4,5)*dDiv;
		ret._45 =-_GEN_SUB_DETERMINANT_4((*this),1,2,3,4,1,2,3,5)*dDiv;
		ret._55 =_GEN_SUB_DETERMINANT_4((*this),1,2,3,4,1,2,3,4)*dDiv;
	}
}

Mat5x5D &Mat5x5D::InverseInPlace(const double *pDet)
{
	double dDeterminant =pDet ? (*pDet) : Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		Mat5x5D &matTemp =(Mat5x5D &)(*(lpDLLTLSData->m_pMatrix1_d));
		matTemp =*this;

		_11 =_GEN_SUB_DETERMINANT_4(matTemp,2,3,4,5,2,3,4,5)*dDiv;
		_21 =-_GEN_SUB_DETERMINANT_4(matTemp,2,3,4,5,1,3,4,5)*dDiv;
		_31 =_GEN_SUB_DETERMINANT_4(matTemp,2,3,4,5,1,2,4,5)*dDiv;
		_41 =-_GEN_SUB_DETERMINANT_4(matTemp,2,3,4,5,1,2,3,5)*dDiv;
		_51 =_GEN_SUB_DETERMINANT_4(matTemp,2,3,4,5,1,2,3,4)*dDiv;

		_12 =-_GEN_SUB_DETERMINANT_4(matTemp,1,3,4,5,2,3,4,5)*dDiv;
		_22 =_GEN_SUB_DETERMINANT_4(matTemp,1,3,4,5,1,3,4,5)*dDiv;
		_32 =-_GEN_SUB_DETERMINANT_4(matTemp,1,3,4,5,1,2,4,5)*dDiv;
		_42 =_GEN_SUB_DETERMINANT_4(matTemp,1,3,4,5,1,2,3,5)*dDiv;
		_52 =-_GEN_SUB_DETERMINANT_4(matTemp,1,3,4,5,1,2,3,4)*dDiv;

		_13 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,4,5,2,3,4,5)*dDiv;
		_23 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,4,5,1,3,4,5)*dDiv;
		_33 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,4,5,1,2,4,5)*dDiv;
		_43 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,4,5,1,2,3,5)*dDiv;
		_53 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,4,5,1,2,3,4)*dDiv;

		_14 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,5,2,3,4,5)*dDiv;
		_24 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,5,1,3,4,5)*dDiv;
		_34 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,5,1,2,4,5)*dDiv;
		_44 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,5,1,2,3,5)*dDiv;
		_54 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,5,1,2,3,4)*dDiv;

		_15 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,4,2,3,4,5)*dDiv;
		_25 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,4,1,3,4,5)*dDiv;
		_35 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,4,1,2,4,5)*dDiv;
		_45 =-_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,4,1,2,3,5)*dDiv;
		_55 =_GEN_SUB_DETERMINANT_4(matTemp,1,2,3,4,1,2,3,4)*dDiv;
	}

	return *this;
}

void Mat5x5D::Transpose(Mat5x5D &ret) const
{
	ret._11 =_11; ret._12 =_21; ret._13 =_31; ret._14 =_41; ret._15 =_51;
	ret._21 =_12; ret._22 =_22; ret._23 =_32; ret._24 =_42; ret._25 =_52;
	ret._31 =_13; ret._32 =_23; ret._33 =_33; ret._34 =_43; ret._35 =_53;
	ret._41 =_14; ret._42 =_24; ret._43 =_34; ret._44 =_44; ret._45 =_54;
	ret._51 =_15; ret._52 =_25; ret._53 =_35; ret._54 =_45; ret._55 =_55;
}

Mat5x5D &Mat5x5D::TransposeInPlace()
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	Mat5x5D &matTemp =(Mat5x5D &)(*(lpDLLTLSData->m_pMatrix1_d));
	matTemp =*this;

	_11 =matTemp._11; _12 =matTemp._21; _13 =matTemp._31; _14 =matTemp._41; _15 =matTemp._51;
	_21 =matTemp._12; _22 =matTemp._22; _23 =matTemp._32; _24 =matTemp._42; _25 =matTemp._52;
	_31 =matTemp._13; _32 =matTemp._23; _33 =matTemp._33; _34 =matTemp._43; _35 =matTemp._53;
	_41 =matTemp._14; _42 =matTemp._24; _43 =matTemp._34; _44 =matTemp._44; _45 =matTemp._54;
	_51 =matTemp._15; _52 =matTemp._25; _53 =matTemp._35; _54 =matTemp._45; _55 =matTemp._55;

	return *this;
}

//表示4x3矩阵

Mat4x3 Mat4x3::m_matIdentity;

Mat4x3::Mat4x3()
{
	_11 = 1.0f; _12 = 0.0f; _13 = 0.0f; 
	_21 = 0.0f; _22 = 1.0f; _23 = 0.0f; 
	_31 = 0.0f; _32 = 0.0f; _33 = 1.0f; 
	_41 = 0.0f; _42 = 0.0f; _43 = 0.0f; 
}

Mat4x3::Mat4x3(const Mat4x4 &src)
{
	_11 = src._11; _12 = src._12; _13 = src._13; 
	_21 = src._21; _22 = src._22; _23 = src._23; 
	_31 = src._31; _32 = src._32; _33 = src._33; 
	_41 = src._41; _42 = src._42; _43 = src._43; 

}

Mat4x3 &Mat4x3::operator =(const Mat4x4 &src)
{
	_11 = src._11; _12 = src._12; _13 = src._13; 
	_21 = src._21; _22 = src._22; _23 = src._23; 
	_31 = src._31; _32 = src._32; _33 = src._33; 
	_41 = src._41; _42 = src._42; _43 = src._43;
	return *this;
}
//将4x3矩阵强制类型换换为4x4矩阵,()强制类型转换符
Mat4x3::operator Mat4x4 () const
{
	Mat4x4 matTemp;
	matTemp._11 = (*this)._11;matTemp._12 = (*this)._12;matTemp._13 = (*this)._13;matTemp._14 = 0.0f;
	matTemp._21 = (*this)._21;matTemp._22 = (*this)._22;matTemp._23 = (*this)._23;matTemp._24 = 0.0f;
	matTemp._31 = (*this)._31;matTemp._32 = (*this)._32;matTemp._33 = (*this)._33;matTemp._34 = 0.0f;
	matTemp._41 = (*this)._41;matTemp._42 = (*this)._42;matTemp._43 = (*this)._43;matTemp._44 = 1.0f;
	return matTemp;
	
}
std::ostream &operator << (std::ostream &ostr, const Mat4x3 &src)
{
	std::stringstream strstream;
	strstream<<src._11<<" "<<src._12<<" "<<src._13<<" "
		     <<src._21<<" "<<src._22<<" "<<src._23<<" "
		     <<src._31<<" "<<src._32<<" "<<src._33<<" "
		     <<src._41<<" "<<src._42<<" "<<src._43;
	ostr<<strstream.str();
	return ostr;
}

Mat3x3D Mat3x3D::m_matIdentity;

Mat3x3D::Mat3x3D()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0;
}

Mat3x3D::Mat3x3D(double d11, double d12, double d13,
			   double d21, double d22, double d23,
			   double d31, double d32, double d33)
{
	_11 = d11; _12 = d12; _13 = d13;
	_21 = d21; _22 = d22; _23 = d23;
	_31 = d31; _32 = d32; _33 = d33;
}

Mat3x3D Mat3x3D::operator +(const Mat3x3D &src) const
{
	return Mat3x3D(
		_11+src._11, _12+src._12, _13+src._13,
		_21+src._21, _22+src._22, _23+src._23,
		_31+src._31, _32+src._32, _33+src._33);
}

Mat3x3D Mat3x3D::operator -(const Mat3x3D &src) const
{
	return Mat3x3D(
		_11-src._11, _12-src._12, _13-src._13,
		_21-src._21, _22-src._22, _23-src._23,
		_31-src._31, _32-src._32, _33-src._33);
}

Mat3x3D Mat3x3D::operator *(double src) const
{
	return Mat3x3D(
		_11*src, _12*src, _13*src,
		_21*src, _22*src, _23*src,
		_31*src, _32*src, _33*src);
}

Mat3x3D Mat3x3D::operator /(double src) const
{
	return Mat3x3D(
		_11/src, _12/src, _13/src,
		_21/src, _22/src, _23/src,
		_31/src, _32/src, _33/src);
}

Mat3x3D Mat3x3D::operator *(const Mat3x3D &src) const
{
	return Mat3x3D(
		_11*src._11+_12*src._21+_13*src._31, _11*src._12+_12*src._22+_13*src._32, _11*src._13+_12*src._23+_13*src._33,
		_21*src._11+_22*src._21+_23*src._31, _21*src._12+_22*src._22+_23*src._32, _21*src._13+_22*src._23+_23*src._33,
		_31*src._11+_32*src._21+_33*src._31, _31*src._12+_32*src._22+_33*src._32, _31*src._13+_32*src._23+_33*src._33);
}

bool Mat3x3D::operator ==(const Mat3x3D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat3x3D)) == 0;
}

bool Mat3x3D::operator !=(const Mat3x3D &src) const
{
	return ::memcmp(this,&src,sizeof(Mat3x3D)) != 0;
}

Mat3x3D &Mat3x3D::operator +=(const Mat3x3D &src)
{
	*this = *this + src;
	return *this;
}

Mat3x3D &Mat3x3D::operator -=(const Mat3x3D &src)
{
	*this = *this - src;
	return *this;
}

Mat3x3D &Mat3x3D::operator *=(double src)
{
	*this = *this * src;
	return *this;
}

Mat3x3D &Mat3x3D::operator /=(double src)
{
	*this = *this / src;
	return *this;
}

Mat3x3D &Mat3x3D::operator *=(const Mat3x3D &src)
{
	*this = *this * src;
	return *this;
}

Mat3x3D operator * (double src1, const Mat3x3D &src2)
{
	return Mat3x3D(
		src2._11*src1, src2._12*src1, src2._13*src1,
		src2._21*src1, src2._22*src1, src2._23*src1,
		src2._31*src1, src2._32*src1, src2._33*src1);
}

double Mat3x3D::Determinant() const
{
	return _GEN_SUB_DETERMINANT_3((*this),1,2,3,1,2,3);
}

Mat3x3D &Mat3x3D::Identity()
{
	_11 = 1.0; _12 = 0.0; _13 = 0.0;
	_21 = 0.0; _22 = 1.0; _23 = 0.0;
	_31 = 0.0; _32 = 0.0; _33 = 1.0;
	
	return *this;
}

void Mat3x3D::Inverse(Mat3x3D &ret) const
{
	double dDeterminant =Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		ret._11 =_GEN_SUB_DETERMINANT_2((*this),2,3,2,3)*dDiv;
		ret._21 =-_GEN_SUB_DETERMINANT_2((*this),2,3,1,3)*dDiv;
		ret._31 =_GEN_SUB_DETERMINANT_2((*this),2,3,1,2)*dDiv;

		ret._12 =-_GEN_SUB_DETERMINANT_2((*this),1,3,2,3)*dDiv;
		ret._22 =_GEN_SUB_DETERMINANT_2((*this),1,3,1,3)*dDiv;
		ret._32 =-_GEN_SUB_DETERMINANT_2((*this),1,3,1,2)*dDiv;

		ret._13 =_GEN_SUB_DETERMINANT_2((*this),1,2,2,3)*dDiv;
		ret._23 =-_GEN_SUB_DETERMINANT_2((*this),1,2,1,3)*dDiv;
		ret._33 =_GEN_SUB_DETERMINANT_2((*this),1,2,1,2)*dDiv;
	}
}

Mat3x3D &Mat3x3D::InverseInPlace()
{
	double dDeterminant =Determinant();
	if(dDeterminant != 0.0){
		double dDiv =1.0/dDeterminant;

		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		Mat3x3D &matTemp =(Mat3x3D &)(*(lpDLLTLSData->m_pMatrix1_d));
		matTemp =*this;

		_11 =_GEN_SUB_DETERMINANT_2(matTemp,2,3,2,3)*dDiv;
		_21 =-_GEN_SUB_DETERMINANT_2(matTemp,2,3,1,3)*dDiv;
		_31 =_GEN_SUB_DETERMINANT_2(matTemp,2,3,1,2)*dDiv;

		_12 =-_GEN_SUB_DETERMINANT_2(matTemp,1,3,2,3)*dDiv;
		_22 =_GEN_SUB_DETERMINANT_2(matTemp,1,3,1,3)*dDiv;
		_32 =-_GEN_SUB_DETERMINANT_2(matTemp,1,3,1,2)*dDiv;

		_13 =_GEN_SUB_DETERMINANT_2(matTemp,1,2,2,3)*dDiv;
		_23 =-_GEN_SUB_DETERMINANT_2(matTemp,1,2,1,3)*dDiv;
		_33 =_GEN_SUB_DETERMINANT_2(matTemp,1,2,1,2)*dDiv;
	}

	return *this;
}

void Mat3x3D::Transpose(Mat3x3D &ret) const
{
	ret._11 =_11; ret._12 =_21; ret._13 =_31;
	ret._21 =_12; ret._22 =_22; ret._23 =_32;
	ret._31 =_13; ret._32 =_23; ret._33 =_33;
}

Mat3x3D &Mat3x3D::TransposeInPlace()
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	Mat3x3D &matTemp =(Mat3x3D &)(*(lpDLLTLSData->m_pMatrix1_d));
	matTemp =*this;

	_11 =matTemp._11; _12 =matTemp._21; _13 =matTemp._31;
	_21 =matTemp._12; _22 =matTemp._22; _23 =matTemp._32;
	_31 =matTemp._13; _32 =matTemp._23; _33 =matTemp._33;
	return *this;
}

Mat4x3_Col Mat4x3_Col::m_matIdentity;

Mat4x3_Col::Mat4x3_Col()
{
	_11 =1.0f; _21 =0.0f; _31 =0.0f; _41 =0.0f; 
	_12 =0.0f; _22 =1.0f; _32 =0.0f; _42 =0.0f; 
	_13 =0.0f; _23 =0.0f; _33 =1.0f; _43 =0.0f; 
}

Mat4x3_Col::Mat4x3_Col(const Mat4x4 &src)
{
	_11 =src._11; _21 =src._21; _31 =src._31; _41 =src._41; 
	_12 =src._12; _22 =src._22; _32 =src._32; _42 =src._42; 
	_13 =src._13; _23 =src._23; _33 =src._33; _43 =src._43; 
}

Mat4x3_Col &Mat4x3_Col::operator () (const Mat4x4 &src)
{
	_11 =src._11; _21 =src._21; _31 =src._31; _41 =src._41; 
	_12 =src._12; _22 =src._22; _32 =src._32; _42 =src._42; 
	_13 =src._13; _23 =src._23; _33 =src._33; _43 =src._43; 
	return *this;
}

Mat4x3_Col &Mat4x3_Col::Identity()
{
	_11 =1.0f; _21 =0.0f; _31 =0.0f; _41 =0.0f; 
	_12 =0.0f; _22 =1.0f; _32 =0.0f; _42 =0.0f; 
	_13 =0.0f; _23 =0.0f; _33 =1.0f; _43 =0.0f; 
	return *this;
}

AABB AABB::m_bbZero;

void AABB::Transform(const Mat4x4 &matTransform, AABB &ret) const
{
	float pTemp[3];

	ret.m_vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	ret.m_vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(u32 i =0; i<8; i++){
		pTemp[0] =i&1 ? m_vMin.x : m_vMax.x;
		pTemp[1] =i&2 ? m_vMin.y : m_vMax.y;
		pTemp[2] =i&4 ? m_vMax.z : m_vMin.z;

		((Vec3 *)(pTemp))->TransformCoordInPlace(matTransform);

		if(pTemp[0] > ret.m_vMax.x){ret.m_vMax.x =pTemp[0];} if(pTemp[0] < ret.m_vMin.x){ret.m_vMin.x =pTemp[0];}
		if(pTemp[1] > ret.m_vMax.y){ret.m_vMax.y =pTemp[1];} if(pTemp[1] < ret.m_vMin.y){ret.m_vMin.y =pTemp[1];}
		if(pTemp[2] > ret.m_vMax.z){ret.m_vMax.z =pTemp[2];} if(pTemp[2] < ret.m_vMin.z){ret.m_vMin.z =pTemp[2];}
	}
}

AABB &AABB::TransformInPlace(const Mat4x4 &matTransform)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	float *pTemp =lpDLLTLSData->m_pVecTemp;
	float *pTemp2 =lpDLLTLSData->m_pVecTemp2;
	float *pTemp3 =lpDLLTLSData->m_pVecTemp3;

	pTemp2[0] =pTemp2[1] =pTemp2[2] =FLT_MAX;
	pTemp3[0] =pTemp3[1] =pTemp3[2] =-FLT_MAX;

	for(u32 i =0; i<8; i++){
		pTemp[0] =i&1 ? m_vMin.x : m_vMax.x;
		pTemp[1] =i&2 ? m_vMin.y : m_vMax.y;
		pTemp[2] =i&4 ? m_vMax.z : m_vMin.z;

		((Vec3 *)(pTemp))->TransformCoordInPlace(matTransform);

		if(pTemp[0] > pTemp3[0]){pTemp3[0] =pTemp[0];} if(pTemp[0] < pTemp2[0]){pTemp2[0] =pTemp[0];}
		if(pTemp[1] > pTemp3[1]){pTemp3[1] =pTemp[1];} if(pTemp[1] < pTemp2[1]){pTemp2[1] =pTemp[1];}
		if(pTemp[2] > pTemp3[2]){pTemp3[2] =pTemp[2];} if(pTemp[2] < pTemp2[2]){pTemp2[2] =pTemp[2];}
	}

	m_vMin(pTemp2[0], pTemp2[1], pTemp2[2]);
	m_vMax(pTemp3[0], pTemp3[1], pTemp3[2]);

	return *this;
}

//生成轴对称包围盒的8个角点坐标
//lpCornerPotBuf：用于存储8个角点的缓冲区首地址
void AABB::GenCornerPots(cn::Vec3 *lpCornerPotBuf) const
{
	for(u32 i =0; i<8; i++){
		lpCornerPotBuf[i].x =i&1 ? m_vMin.x : m_vMax.x;
		lpCornerPotBuf[i].y =i&2 ? m_vMin.y : m_vMax.y;
		lpCornerPotBuf[i].z =i&4 ? m_vMax.z : m_vMin.z;
	}
}

//检测包围盒是否有效(即m_vMax>=m_vMin)
bool AABB::IsValid() const
{
	return (m_vMin.x<=m_vMax.x)&&(m_vMin.y<=m_vMax.y)&&(m_vMin.z<=m_vMax.z);
}

AABB_D AABB_D::m_bbZero;

//生成轴对称包围盒的8个角点坐标
//lpCornerPotBuf：用于存储8个角点的缓冲区首地址
void AABB_D::GenCornerPots(cn::Vec3D *lpCornerPotBuf) const
{
	for(u32 i =0; i<8; i++){
		lpCornerPotBuf[i].x =i&1 ? m_vMin.x : m_vMax.x;
		lpCornerPotBuf[i].y =i&2 ? m_vMin.y : m_vMax.y;
		lpCornerPotBuf[i].z =i&4 ? m_vMax.z : m_vMin.z;
	}
}

Ray Ray::m_rayZero;

RayD RayD::m_rayZero;

RayD::RayD()
: m_vPos(0.0, 0.0, 0.0)
, m_vDir(0.0, 0.0, 0.0)
{

}

RayD::RayD(const Ray &src)
{
	m_vPos =src.m_vPos; m_vDir =src.m_vDir;
}

RayD &RayD::operator = (const Ray &src)
{
	m_vPos =src.m_vPos; m_vDir =src.m_vDir;
	return *this;
}

Sphere Sphere::m_sphZero;

//表示一个球体
Sphere::Sphere():m_vCenter(0.0f,0.0f,0.0f),
                 m_fRadius(0.0f)
{

}

void Sphere::Transform(float fScale, const Quaternion &qRotate, const Vec3 &vTranslation, Sphere &ret) const
{
	ret.m_fRadius =m_fRadius*::abs(fScale);
	(m_vCenter*fScale).TransformCoord(qRotate, ret.m_vCenter);
	ret.m_vCenter +=vTranslation;
}

Sphere &Sphere::TransformInPlace(float fScale, const Quaternion &qRotate, const Vec3 &vTranslation)
{
	m_fRadius *=::abs(fScale);
	(m_vCenter*fScale).TransformCoord(qRotate, m_vCenter);
	m_vCenter +=vTranslation;

	return *this;
}

Rect Rect::m_rcZero;

//表示一个矩形区域
Rect::Rect():m_fLeft(0.0f),
             m_fTop(0.0f),
			 m_fRight(0.0f),
			 m_fBottom(0.0f)
{

}


Rect::Rect(float l,float t,float r,float b):
m_fLeft(l),
m_fTop(t),
m_fRight(r),
m_fBottom(b)
{

}

//获取给定的坐标点是否在区域内(由底向顶，坐标值递增)
bool Rect::IsInside_B2T(float fX, float fY) const
{
	return (fX>=m_fLeft)&&(fX<=m_fRight)&&(fY>=m_fBottom)&&(fY<=m_fTop);
}

//获取给定的坐标点是否在区域内(由底向顶，坐标值递减)
bool Rect::IsInside_T2B(float fX, float fY) const
{
	return (fX>=m_fLeft)&&(fX<=m_fRight)&&(fY>=m_fTop)&&(fY<=m_fBottom);
}

//平移
void Rect::Offset(float fX, float fY, Rect &rcRet) const
{
	rcRet.m_fLeft +=fX; rcRet.m_fRight +=fX; rcRet.m_fBottom +=fY; rcRet.m_fTop +=fY;
}

//平移
void Rect::OffsetInPlace(float fX, float fY)
{
	m_fLeft +=fX; m_fRight +=fX; m_fBottom +=fY; m_fTop +=fY;
}

Rect &Rect::operator()(float l,float t,float r,float b)
{
	m_fLeft =l; m_fTop =t; m_fRight =r; m_fBottom =b;
	return *this;
}

RectInt RectInt::m_rcZero;

//表示一个矩形区域
RectInt::RectInt():m_sLeft(0),
             m_sTop(0),
			 m_sRight(0),
			 m_sBottom(0)
{

}


RectInt::RectInt(cn::s32 l,cn::s32 t,cn::s32 r,cn::s32 b)
:m_sLeft(l),
m_sTop(t),
m_sRight(r),
m_sBottom(b)
{

}

//获取给定的坐标点是否在区域内(由底向顶，坐标值递增)
bool RectInt::IsInside_B2T(cn::s32 sX, cn::s32 sY) const
{
	return (sX>=m_sLeft)&&(sX<m_sRight)&&(sY>=m_sBottom)&&(sY<m_sTop);
}

//获取给定的坐标点是否在区域内(由底向顶，坐标值递减)
bool RectInt::IsInside_T2B(cn::s32 sX, cn::s32 sY) const
{
	return (sX>=m_sLeft)&&(sX<m_sRight)&&(sY>=m_sTop)&&(sY<m_sBottom);
}

//平移
void RectInt::Offset(cn::s32 sX, cn::s32 sY, RectInt &rcRet) const
{
	rcRet.m_sLeft +=sX; rcRet.m_sRight +=sX; rcRet.m_sBottom +=sY; rcRet.m_sTop +=sY;
}

//平移
void RectInt::OffsetInPlace(cn::s32 sX, cn::s32 sY)
{
	m_sLeft +=sX; m_sRight +=sX; m_sBottom +=sY; m_sTop +=sY;
}

RectInt &RectInt::operator()(cn::s32 l,cn::s32 t,cn::s32 r,cn::s32 b)
{
	m_sLeft =l; m_sTop =t; m_sRight =r; m_sBottom =b;
	return *this;
}

Viewport Viewport::m_vpZero;

//表示一个视口
Viewport::Viewport():m_uX(0),
                     m_uY(0),
					 m_uWidth(0),
					 m_uHeight(0),
					 m_fMinZ(0.0f),
					 m_fMaxZ(0.0f)										 
{

}


//构造函数
Buffer::Buffer()
: m_pData(NULL)
, m_uLen(0)
, m_uCapacity(0)
, m_uStatCount(0)
, m_dStatTime(0.0)
{

}

//构造函数，填充pData指向的uLen个字节的数据
Buffer::Buffer(const void* pData, u32 uLen)
: m_pData(NULL)
, m_uLen(0)
, m_uCapacity(0)
, m_uStatCount(0)
, m_dStatTime(0.0)
{
	(*this)(pData, uLen);
}

//构造函数，创建一个uLen字节的缓冲区
Buffer::Buffer(u32 uLen)
: m_pData(NULL)
, m_uLen(0)
, m_uCapacity(0)
, m_uStatCount(0)
, m_dStatTime(0.0)
{
	(*this)(uLen);
}

//拷贝构造函数
Buffer::Buffer(const Buffer &src)
: m_pData(NULL)
, m_uLen(0)
, m_uCapacity(0)
, m_uStatCount(0)
, m_dStatTime(0.0)
{
	*this =src;
}

//析构函数
Buffer::~Buffer()
{
	if(m_pData){
		delete []m_pData;
	}
}

//赋值运算符重载
Buffer & Buffer::operator = (const Buffer &src)
{
	return (*this)(src.m_pData, src.m_uLen);
}

//运算符重载，填充pData指向的uLen个字节的数据
Buffer & Buffer::operator () (const void* pData, u32 uLen)
{
	cn::u32 uForceCapacity =0xffffffff;
	if(uLen < m_uCapacity/2){
		m_uStatCount++;
		if(m_uStatCount > _MIN_MEM_RELEASE_COUNT){
			if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
					cn::Literal::GetFormattedStr("Buffer::operator () (const void* pData, u32 uLen) = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
			}
			uForceCapacity =m_uCapacity/2; m_uStatCount =0;
		}else if(m_uStatCount > _MIN_MEM_RELEASE_TIMESTAT_COUNT){
			double dCurTime =cn::Time::GetAbsTime(); double dInterval =dCurTime-m_dStatTime;
			if(dInterval > _MIN_MEM_RELEASE_TIMEOUT){
				if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
					cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
						cn::Literal::GetFormattedStr("Buffer::operator () (const void* pData, u32 uLen) = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
				}
				uForceCapacity =m_uCapacity/2; m_uStatCount =0;
			}			
		}else if(m_uStatCount == _MIN_MEM_RELEASE_TIMESTAT_COUNT){
			m_dStatTime =cn::Time::GetAbsTime();
		}
	}else{
		m_uStatCount =0;
	}
	if(uForceCapacity!=0xffffffff || uLen>m_uCapacity){
		SAFE_DELETE_ARRAY(m_pData); m_uCapacity =0;
	}
	m_uLen =uLen;
	if(m_uLen > 0){
		if(m_uCapacity == 0){
			m_uCapacity =(uForceCapacity != 0xffffffff) ? uForceCapacity : m_uLen;
			m_pData =new u8[m_uCapacity];
		}
		::memcpy(m_pData, pData, m_uLen);
	}
	return *this;
}

//运算符重载，创建一个uLen字节的缓冲区
Buffer & Buffer::operator () (u32 uLen)
{
	cn::u32 uForceCapacity =0xffffffff;
	if(uLen < m_uCapacity/2){
		m_uStatCount++;
		if(m_uStatCount > _MIN_MEM_RELEASE_COUNT){
			if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
					cn::Literal::GetFormattedStr("Buffer::operator () (u32 uLen) = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
			}
			uForceCapacity =m_uCapacity/2; m_uStatCount =0;
		}else if(m_uStatCount > _MIN_MEM_RELEASE_TIMESTAT_COUNT){
			double dCurTime =cn::Time::GetAbsTime(); double dInterval =dCurTime-m_dStatTime;
			if(dInterval > _MIN_MEM_RELEASE_TIMEOUT){
				if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
					cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
						cn::Literal::GetFormattedStr("Buffer::operator () (u32 uLen) = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
				}
				uForceCapacity =m_uCapacity/2; m_uStatCount =0;
			}			
		}else if(m_uStatCount == _MIN_MEM_RELEASE_TIMESTAT_COUNT){
			m_dStatTime =cn::Time::GetAbsTime();
		}
	}else{
		m_uStatCount =0;
	}
	if(uForceCapacity!=0xffffffff || uLen>m_uCapacity){
		SAFE_DELETE_ARRAY(m_pData); m_uCapacity =0;
	}
	m_uLen =uLen;
	if(m_uLen > 0){
		if(m_uCapacity == 0){
			m_uCapacity =(uForceCapacity != 0xffffffff) ? uForceCapacity : m_uLen;
			m_pData =new u8[m_uCapacity];
		}
	}
	return *this;
}

//获得缓冲区首地址
void *Buffer::GetData() const
{
	return m_pData;
}

//获得缓冲区字节数
u32 Buffer::GetLen() const
{
	return m_uLen;
}


//重置缓冲区的大小，缓冲区位置有效的原有数据将予以保留
void Buffer::Resize(u32 uLen)
{
	if(m_uLen != uLen){
		cn::u32 uForceCapacity =0xffffffff;
		if(uLen < m_uCapacity/2){
			m_uStatCount++;
			if(m_uStatCount > _MIN_MEM_RELEASE_COUNT){
				if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
					cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
						cn::Literal::GetFormattedStr("Buffer::Resize = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
				}
				uForceCapacity =m_uCapacity/2; m_uStatCount =0;
			}else if(m_uStatCount > _MIN_MEM_RELEASE_TIMESTAT_COUNT){
				double dCurTime =cn::Time::GetAbsTime(); double dInterval =dCurTime-m_dStatTime;
				if(dInterval > _MIN_MEM_RELEASE_TIMEOUT){
					if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
						cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
							cn::Literal::GetFormattedStr("Buffer::Resize = force resize Capacity[%u] StatCount[%u]", m_uCapacity, m_uStatCount));
					}
					uForceCapacity =m_uCapacity/2; m_uStatCount =0;
				}			
			}else if(m_uStatCount == _MIN_MEM_RELEASE_TIMESTAT_COUNT){
				m_dStatTime =cn::Time::GetAbsTime();
			}
		}else{
			m_uStatCount =0;
		}
		if(uForceCapacity!=0xffffffff || uLen>m_uCapacity){
			void *pData =NULL;
			m_uCapacity =(uForceCapacity != 0xffffffff) ? uForceCapacity : uLen;
			if(m_uCapacity > 0){pData =new u8[m_uCapacity];}
			if(m_uLen>0 && uLen>0){
				::memcpy(pData, m_pData, min(m_uLen, uLen));
			}
			SAFE_DELETE_ARRAY(m_pData); m_pData =pData;
		}
		m_uLen =uLen;
	}
}


//创建一个二维ID区域对象
Ref/*ID_REGION2D*/ ID_REGION2D::Create()
{
	return cn::Ref(new ID_REGION2D_IMP());
}


//构造函数
ID_REGION2D::ID_REGION2D()
{

}


//析构函数
ID_REGION2D::~ID_REGION2D()
{

}


//构造函数
ID_REGION2D_IMP::ID_REGION2D_IMP()
: ID_REGION2D()
{
	m_sColNum =0;
	m_lpColInfos =NULL;
	m_sSegNum =NULL;
	m_lpSegInfos =NULL;
	m_bAdding =false;
}


//析构函数
ID_REGION2D_IMP::~ID_REGION2D_IMP()
{
	SAFE_DELETE_ARRAY(m_lpColInfos);
	SAFE_DELETE_ARRAY(m_lpSegInfos);
}


//开始添加新的列区段
//返回值：true->可以开始添加，false->不允许添加
bool ID_REGION2D_IMP::BeginAddColSegs()
{
	if(m_bAdding){
		return false;
	}
	ConvertFormalMemToTempMem();
	m_bAdding =true;

	return true;
}


//添加一个列区段(必须位于BeginAddColSegs-EndAddColSegs对内)
bool ID_REGION2D_IMP::AddColSeg(const ID_REGION2D::COL_SEG &cSeg)
{
	if(m_bAdding==false || cSeg.m_sZEnd<=cSeg.m_sZStart){
		return false;
	}
	std::map<cn::s32, cn::s32> &mapColSegs =m_mapTempColInfos[cSeg.m_sX];
	std::pair<std::map<cn::s32, cn::s32>::iterator, bool> pairRet =mapColSegs.insert(std::make_pair(cSeg.m_sZStart, cSeg.m_sZEnd));
	if(pairRet.second == false){
		cn::s32 &sFinalZEnd =pairRet.first->second;
		sFinalZEnd =max(sFinalZEnd, cSeg.m_sZEnd);
	}
	return true;
}


//将一个二维ID区域添加融合到自身(必须位于BeginAddColSegs-EndAddColSegs对内)
bool ID_REGION2D_IMP::AddIDRegion2D(const ID_REGION2D *lpRegion)
{
	cn::s32 i, j;

	if(m_bAdding == false){
		return false;
	}
	ID_REGION2D_IMP *lpRegionImp =(ID_REGION2D_IMP *)(lpRegion);
	if(lpRegionImp==NULL || lpRegionImp->m_bAdding){
		return false;
	}
	for(i =0; i<lpRegionImp->m_sColNum; i++){
		COL_EXT &cSrcCol =lpRegionImp->m_lpColInfos[i];
		std::map<cn::s32, cn::s32> &mapColSegs =m_mapTempColInfos[cSrcCol.m_sX];
		for(j =cSrcCol.m_sSegStart; j<cSrcCol.m_sSegEnd; j++){
			COL_SEG_EXT &cSrcColSeg =lpRegionImp->m_lpSegInfos[j];
			if(cSrcColSeg.m_sZStart < cSrcColSeg.m_sZEnd){
				std::pair<std::map<cn::s32, cn::s32>::iterator, bool> pairRet =mapColSegs.insert(std::make_pair(cSrcColSeg.m_sZStart, cSrcColSeg.m_sZEnd));
				if(pairRet.second == false){
					cn::s32 &sFinalZEnd =pairRet.first->second;
					sFinalZEnd =max(sFinalZEnd, cSrcColSeg.m_sZEnd);
				}
			}
		}
	}
	return true;
}


//结束添加新的列区段
void ID_REGION2D_IMP::EndAddColSegs()
{
	if(m_bAdding == false){
		return;
	}
	ConvertTempMemToFormalMem();
	m_bAdding =false;
}


//获取ID区域包含的所有区段信息
const ID_REGION2D::COL_SEG *ID_REGION2D_IMP::GetAllColSegs(cn::u32 &uSegNum)
{
	cn::s32 i, j;

	if(m_bAdding || m_sSegNum==0){
		uSegNum =0; return NULL;
	}
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	std::vector<BYTE> &arrRet =lpDLLTLSData->m_arrTempColSegs;
	arrRet.resize(m_sSegNum*sizeof(ID_REGION2D::COL_SEG));
	ID_REGION2D::COL_SEG *lpRet =(ID_REGION2D::COL_SEG *)(&(arrRet[0]));
	uSegNum =0;
	for(i =0; i<m_sColNum; i++){
		COL_EXT &cCurCol =m_lpColInfos[i];
		for(j =cCurCol.m_sSegStart; j<cCurCol.m_sSegEnd; j++){
			COL_SEG_EXT &cCurSeg =m_lpSegInfos[j];
			ID_REGION2D::COL_SEG &cDestSeg =lpRet[uSegNum++];
			cDestSeg.m_sX =cCurCol.m_sX; cDestSeg.m_sZStart =cCurSeg.m_sZStart; cDestSeg.m_sZEnd =cCurSeg.m_sZEnd;
		}
	}
	return lpRet;
}


//删除ID区域包含的所有区段信息
void ID_REGION2D_IMP::DelAllColSegs()
{
	if(m_bAdding){
		return;
	}
	m_sColNum =0;
	SAFE_DELETE_ARRAY(m_lpColInfos);
	m_sSegNum =NULL;
	SAFE_DELETE_ARRAY(m_lpSegInfos);
}


//将自身区域复制一份新的返回
Ref/*ID_REGION2D*/ ID_REGION2D_IMP::Clone()
{
	if(m_bAdding){
		return cn::Ref(NULL);
	}

	cn::Ref/*ID_REGION2D_IMP*/ refRet =ID_REGION2D::Create();
	ID_REGION2D_IMP *lpRet =(ID_REGION2D_IMP *)(refRet.Get());
	if(lpRet){
		lpRet->m_sColNum =m_sColNum;
		lpRet->m_lpColInfos =new COL_EXT[lpRet->m_sColNum];
		::memcpy(lpRet->m_lpColInfos, m_lpColInfos, sizeof(COL_EXT)*lpRet->m_sColNum);
		lpRet->m_sSegNum =m_sSegNum;
		lpRet->m_lpSegInfos =new COL_SEG_EXT[lpRet->m_sSegNum];
		::memcpy(lpRet->m_lpSegInfos, m_lpSegInfos, sizeof(COL_SEG_EXT)*lpRet->m_sSegNum);
	}
	return refRet;
}


//将区域信息粗略化
//sMult：表示粗略倍数
void ID_REGION2D_IMP::Coarsen(cn::s32 sMult)
{
	cn::s32 i, j;

	if(m_bAdding || sMult<=1){
		return;
	}
	m_mapTempColInfos.clear();
	for(i =0; i<m_sColNum; i++){
		COL_EXT &cSrcCol =m_lpColInfos[i];
		std::map<cn::s32, cn::s32> &mapColSegs =m_mapTempColInfos[cSrcCol.m_sX/sMult];
		for(j =cSrcCol.m_sSegStart; j<cSrcCol.m_sSegEnd; j++){
			COL_SEG_EXT &cSrcColSeg =m_lpSegInfos[j];
			cn::s32 sNewZStart =cSrcColSeg.m_sZStart/sMult; cn::s32 sNewZEnd =(cSrcColSeg.m_sZEnd-1)/sMult+1;
			if(sNewZStart < sNewZEnd){
				std::pair<std::map<cn::s32, cn::s32>::iterator, bool> pairRet =mapColSegs.insert(std::make_pair(sNewZStart, sNewZEnd));
				if(pairRet.second == false){
					cn::s32 &sFinalZEnd =pairRet.first->second;
					sFinalZEnd =max(sFinalZEnd, sNewZEnd);
				}
			}
		}
	}
	ConvertTempMemToFormalMem();
}


//将区域信息精细化
//sMult：表示精细倍数
void ID_REGION2D_IMP::Detail(cn::s32 sMult)
{
	cn::s32 i, j;

	if(m_bAdding || sMult<=1){
		return;
	}
	cn::s32 sNewColNum =m_sColNum*sMult;
	COL_EXT *lpNewColInfos =new COL_EXT[sNewColNum];
	cn::s32 sNewSegNum =m_sSegNum*sMult;
	COL_SEG_EXT *lpNewSegInfos =new COL_SEG_EXT[sNewSegNum];
	for(i =0; i<sNewColNum; i++){
		COL_EXT &cNewCol =lpNewColInfos[i]; COL_EXT &cOldCol =m_lpColInfos[i/sMult];
		cNewCol.m_sX =cOldCol.m_sX*sMult+i%sMult; cn::s32 sSegInnerNum =cOldCol.m_sSegEnd-cOldCol.m_sSegStart;
		cNewCol.m_sSegStart =cOldCol.m_sSegStart*sMult+sSegInnerNum*(i%sMult);
		cNewCol.m_sSegEnd =cNewCol.m_sSegStart+sSegInnerNum;
		for(j =0; j<sSegInnerNum; j++){
			COL_SEG_EXT &cNewSeg =lpNewSegInfos[cNewCol.m_sSegStart+j];
			COL_SEG_EXT &cOldSeg =m_lpSegInfos[cOldCol.m_sSegStart+j];
			cNewSeg.m_sZStart =cOldSeg.m_sZStart*sMult;
			cNewSeg.m_sZEnd =cOldSeg.m_sZEnd*sMult;
		}
	}
	SAFE_DELETE_ARRAY(m_lpColInfos);
	SAFE_DELETE_ARRAY(m_lpSegInfos);
	m_sColNum =sNewColNum;
	m_sSegNum =sNewSegNum;
	m_lpColInfos =lpNewColInfos;
	m_lpSegInfos =lpNewSegInfos;
}


//判断一个ID位置是否在区域内
bool ID_REGION2D_IMP::IsInside(cn::s32 sIDX, cn::s32 sIDZ)
{
	if(m_bAdding){
		return false;
	}
	cn::s32 sCol_0 =0; cn::s32 sCol_1 =m_sColNum;
	while(sCol_0 < sCol_1){
		cn::s32 sColMid =(sCol_0+sCol_1)/2;
		COL_EXT &cCurCol =m_lpColInfos[sColMid];
		if(cCurCol.m_sX == sIDX){
			cn::s32 sSeg_0 =cCurCol.m_sSegStart; cn::s32 sSeg_1 =cCurCol.m_sSegEnd;
			while(sSeg_0+1 < sSeg_1){
				cn::s32 sSegMid =(sSeg_0+sSeg_1)/2;
				COL_SEG_EXT &cCurSeg =m_lpSegInfos[sSegMid];
				if(cCurSeg.m_sZStart > sIDZ){
					sSeg_1 =sSegMid;
				}else{
					sSeg_0 =sSegMid;
				}
			}
			if(sSeg_0 < sSeg_1){
				COL_SEG_EXT &cCurSeg =m_lpSegInfos[sSeg_0];
				if(sIDZ>=cCurSeg.m_sZStart && sIDZ<cCurSeg.m_sZEnd){
					return true;
				}else{
					return false;
				}
			}
			break;
		}else if(cCurCol.m_sX > sIDX){
			sCol_1 =sColMid;
		}else{
			sCol_0 =sColMid+1;
		}
	}
	return false;
}


//将区域的正式存储信息转换为临时存储信息
void ID_REGION2D_IMP::ConvertFormalMemToTempMem()
{
	cn::s32 i, j;

	m_mapTempColInfos.clear();
	for(i =0; i<m_sColNum; i++){
		COL_EXT &cCurCol =m_lpColInfos[i];
		std::map<cn::s32, cn::s32> &mapColSegs =m_mapTempColInfos[cCurCol.m_sX];
		for(j =cCurCol.m_sSegStart; j<cCurCol.m_sSegEnd; j++){
			COL_SEG_EXT &cCurColSeg =m_lpSegInfos[j];
			mapColSegs.insert(std::make_pair(cCurColSeg.m_sZStart, cCurColSeg.m_sZEnd));
		}
	}
	m_sColNum =0;
	SAFE_DELETE_ARRAY(m_lpColInfos);
	m_sSegNum =0;
	SAFE_DELETE_ARRAY(m_lpSegInfos);
}


//将区域的临时存储信息转换为正式存储信息
void ID_REGION2D_IMP::ConvertTempMemToFormalMem()
{
	COL_EXT cCurCol;
	COL_SEG_EXT cCurSeg;

	std::vector<COL_EXT> arrColInfos;
	std::vector<COL_SEG_EXT> arrSegInfos;
	m_sColNum =0;
	SAFE_DELETE_ARRAY(m_lpColInfos);
	m_sSegNum =0;
	SAFE_DELETE_ARRAY(m_lpSegInfos);
	for(std::map<cn::s32, std::map<cn::s32, cn::s32>>::iterator iter1 =m_mapTempColInfos.begin(); iter1 != m_mapTempColInfos.end(); iter1++){
		cCurCol.m_sX =iter1->first; cCurCol.m_sSegStart =m_sSegNum;
		std::map<cn::s32, cn::s32> &mapColSegs =iter1->second;
		for(std::map<cn::s32, cn::s32>::iterator iter2 =mapColSegs.begin(); iter2 != mapColSegs.end(); iter2++){
			cCurSeg.m_sZStart =iter2->first; cCurSeg.m_sZEnd =iter2->second;
			if(cCurCol.m_sSegStart != m_sSegNum){
				COL_SEG_EXT &cTailSeg =arrSegInfos[m_sSegNum-1];
				if(cCurSeg.m_sZStart > cTailSeg.m_sZEnd){
					arrSegInfos.push_back(cCurSeg); m_sSegNum++;
				}else{
					cTailSeg.m_sZEnd =max(cTailSeg.m_sZEnd, cCurSeg.m_sZEnd);
				}
			}else{
				arrSegInfos.push_back(cCurSeg); m_sSegNum++;
			}
		}
		cCurCol.m_sSegEnd =m_sSegNum;
		if(cCurCol.m_sSegStart < cCurCol.m_sSegEnd){
			arrColInfos.push_back(cCurCol); m_sColNum++;
		}
	}
	if(m_sColNum > 0){
		m_lpColInfos =new COL_EXT[m_sColNum];
		::memcpy(m_lpColInfos, &(arrColInfos[0]), sizeof(COL_EXT)*m_sColNum);
	}
	if(m_sSegNum > 0){
		m_lpSegInfos =new COL_SEG_EXT[m_sSegNum];
		::memcpy(m_lpSegInfos, &(arrSegInfos[0]), sizeof(COL_SEG_EXT)*m_sSegNum);
	}
	std::map<cn::s32, std::map<cn::s32, cn::s32>> mapTemp;
	m_mapTempColInfos.swap(mapTemp);
}


//获取顶点元素的字节大小
cn::u32 GetVerElemSize(cn::VERTEX_ELEMENT::DECLTYPE eType)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	bool &bInited =lpDLLTLSData->m_b_GetVerElemSize_Inited;
	cn::u32 *pSizeBuf =(cn::u32 *)(lpDLLTLSData->m_p_GetVerElemSize_SizeBuf);

	if(bInited == false){
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT1] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT2] =8;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT3] =12;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT4] =16;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_D3DCOLOR] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_UBYTE4] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_SHORT2] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_SHORT4] =8;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_UBYTE4N] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_SHORT2N] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_SHORT4N] =8;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_USHORT2N] =4,
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_USHORT4N] =8,
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_UDEC3] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_DEC3N] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT16_2] =4;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_FLOAT16_4] =8;
		pSizeBuf[cn::VERTEX_ELEMENT::DECLTYPE_UNUSED] =0;

		bInited =true;
	}

	return pSizeBuf[eType];
}


//获取一个像素格式对应的字节数
u32 GetPixelFormatSize(PIXEL_FORMAT eFmt)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();

	bool &bInited =lpDLLTLSData->m_b_GetPixelFormatSize_Inited;
	cn::u32 *pSizeBuf =(cn::u32 *)(lpDLLTLSData->m_p_GetPixelFormatSize_SizeBuf);

	if(bInited == false){
		pSizeBuf[PIXEL_FORMAT_UNKNOWN] =0;
		pSizeBuf[PIXEL_FORMAT_R8G8B8] =3;
		pSizeBuf[PIXEL_FORMAT_A8R8G8B8] =4;
		pSizeBuf[PIXEL_FORMAT_X8R8G8B8] =4;
		pSizeBuf[PIXEL_FORMAT_R5G6B5] =2;
		pSizeBuf[PIXEL_FORMAT_X1R5G5B5] =2;
		pSizeBuf[PIXEL_FORMAT_A1R5G5B5] =2;
		pSizeBuf[PIXEL_FORMAT_A4R4G4B4] =2;
		pSizeBuf[PIXEL_FORMAT_R3G3B2] =1;
		pSizeBuf[PIXEL_FORMAT_A8] =1;
		pSizeBuf[PIXEL_FORMAT_A8R3G3B2] =2;
		pSizeBuf[PIXEL_FORMAT_X4R4G4B4] =2;
		pSizeBuf[PIXEL_FORMAT_A2B10G10R10] =4;
		pSizeBuf[PIXEL_FORMAT_A8B8G8R8] =4;
		pSizeBuf[PIXEL_FORMAT_X8B8G8R8] =4;
		pSizeBuf[PIXEL_FORMAT_G16R16] =4;
		pSizeBuf[PIXEL_FORMAT_A2R10G10B10] =4;
		pSizeBuf[PIXEL_FORMAT_A16B16G16R16] =8;
		pSizeBuf[PIXEL_FORMAT_A8P8] =2;
		pSizeBuf[PIXEL_FORMAT_P8] =1;
		pSizeBuf[PIXEL_FORMAT_L8] =1;
		pSizeBuf[PIXEL_FORMAT_A8L8] =2;
		pSizeBuf[PIXEL_FORMAT_A4L4] =1;
		pSizeBuf[PIXEL_FORMAT_V8U8] =2;
		pSizeBuf[PIXEL_FORMAT_L6V5U5] =2;
		pSizeBuf[PIXEL_FORMAT_X8L8V8U8] =4;
		pSizeBuf[PIXEL_FORMAT_Q8W8V8U8] =4;
		pSizeBuf[PIXEL_FORMAT_V16U16] =4;
		pSizeBuf[PIXEL_FORMAT_A2W10V10U10] =4;

		pSizeBuf[PIXEL_FORMAT_D16_LOCKABLE] =2;
		pSizeBuf[PIXEL_FORMAT_D32] =4;
		pSizeBuf[PIXEL_FORMAT_D15S1] =2;
		pSizeBuf[PIXEL_FORMAT_D24S8] =4;
		pSizeBuf[PIXEL_FORMAT_D24X8] =4;
		pSizeBuf[PIXEL_FORMAT_D24X4S4] =4;
		pSizeBuf[PIXEL_FORMAT_D16] =2;
		pSizeBuf[PIXEL_FORMAT_D32F_LOCKABLE] =4;
		pSizeBuf[PIXEL_FORMAT_D24FS8] =4;
		pSizeBuf[PIXEL_FORMAT_L16] =2;
		pSizeBuf[PIXEL_FORMAT_Q16W16V16U16] =8;

		pSizeBuf[PIXEL_FORMAT_R16F] =2;
		pSizeBuf[PIXEL_FORMAT_G16R16F] =4;
		pSizeBuf[PIXEL_FORMAT_A16B16G16R16F] =8;
		pSizeBuf[PIXEL_FORMAT_R32F] =4;
		pSizeBuf[PIXEL_FORMAT_G32R32F] =8;
		pSizeBuf[PIXEL_FORMAT_A32B32G32R32F] =16;
		pSizeBuf[PIXEL_FORMAT_CxV8U8] =4;

		bInited =true;
	}

	if(u32(eFmt) < 120){
		return pSizeBuf[eFmt];
	}else{
		return 1;
	}
}


//转换数据格式(法线转换基于sterographic projection)
inline cn::u32 ConvertNorMatID_Orig2New(const cn::Vec3 &vOrigNor, cn::u32 uOrigMatID)
{
	cn::u32 uRet =(0x7fff & uOrigMatID);
	if(vOrigNor.y >= 0.0f){
		uRet |=(1<<15); 
		float fX =(vOrigNor.x/(1.0f+vOrigNor.y)*0.5f+0.5f)*255.0f; float fZ =(vOrigNor.z/(1.0f+vOrigNor.y)*0.5f+0.5f)*255.0f;
		uRet |=(cn::u32(max(0.0f, min(255.0f, fX)))<<16); uRet |=(cn::u32(max(0.0f, min(255.0f, fZ)))<<24);
	}else{
		float fX =(vOrigNor.x/(1.0f-vOrigNor.y)*0.5f+0.5f)*255.0f; float fZ =(vOrigNor.z/(1.0f-vOrigNor.y)*0.5f+0.5f)*255.0f;
		uRet |=(cn::u32(max(0.0f, min(255.0f, fX)))<<16); uRet |=(cn::u32(max(0.0f, min(255.0f, fZ)))<<24);
	}
	return uRet;
}


//转换数据格式
inline cn::u32 ConvertNorMatID_Old2New(cn::u8 uOldNorX, cn::u8 uOldNorY, cn::u8 uOldNorZ, cn::u8 uOldMatID)
{
	cn::u32 uRet =cn::u32(uOldMatID);
	cn::Vec3 vTempNor(float(uOldNorX)/255.0f*2.0f-1.0f, float(uOldNorY)/255.0f*2.0f-1.0f, float(uOldNorZ)/255.0f*2.0f-1.0f); vTempNor.NormalizeInPlace();
	if(vTempNor.y >= 0.0f){
		uRet |=(1<<15); 
		float fX =(vTempNor.x/(1.0f+vTempNor.y)*0.5f+0.5f)*255.0f; float fZ =(vTempNor.z/(1.0f+vTempNor.y)*0.5f+0.5f)*255.0f;
		uRet |=(cn::u32(max(0.0f, min(255.0f, fX)))<<16); uRet |=(cn::u32(max(0.0f, min(255.0f, fZ)))<<24);
	}else{
		float fX =(vTempNor.x/(1.0f-vTempNor.y)*0.5f+0.5f)*255.0f; float fZ =(vTempNor.z/(1.0f-vTempNor.y)*0.5f+0.5f)*255.0f;
		uRet |=(cn::u32(max(0.0f, min(255.0f, fX)))<<16); uRet |=(cn::u32(max(0.0f, min(255.0f, fZ)))<<24);
	}
	return uRet;
}


//转换数据格式
inline void ConvertNorMatID_New2Orig(cn::u32 uNewNorMatID, cn::Vec3 *lpOrigNor, cn::u32 *pOrigMatID)
{
	if(lpOrigNor){
		float fX =float(0xff & (uNewNorMatID>>16))/255.0f*2.0f-1.0f; float fZ =float(0xff & (uNewNorMatID>>24))/255.0f*2.0f-1.0f; 
		float fTemp =fX*fX+fZ*fZ; (*lpOrigNor)(2.0f*fX/(1.0f+fTemp), (uNewNorMatID&0x8000) ? (1.0f-fTemp)/(1.0f+fTemp) : (fTemp-1.0f)/(1.0f+fTemp), 2.0f*fZ/(1.0f+fTemp));
		lpOrigNor->NormalizeInPlace();
	}
	if(pOrigMatID){
		(*pOrigMatID) =(uNewNorMatID & 0x7fff);
	}
}


//转换数据格式
inline cn::u32 ConvertNorMatID_OrigBit2NewBit(cn::u32 uOrigNorBit, cn::u32 uOrigMatID)
{
	return uOrigNorBit | (0x7fff & uOrigMatID);
}


//转换数据格式
inline void ConvertNorMatID_NewBit2OrigBit(cn::u32 uNewNorMatID, cn::u32 *pOrigNorBit, cn::u32 *pOrigMatID)
{
	if(pOrigNorBit){(*pOrigNorBit) =(uNewNorMatID & 0xffff8000);}
	if(pOrigMatID){(*pOrigMatID) =(uNewNorMatID & 0x7fff);}
}























}







namespace RealEngine
{





//创建一个强引用数组
cn::Ref/*ArrayRef*/ ArrayRef::Create()
{
	return cn::Ref(new ArrayRef());
}


cn::Ref/*ArrayRef*/ ArrayRef::Create(const cn::Ref *lpData, cn::u32 uLen)
{
	return cn::Ref(new ArrayRef(lpData, uLen));
}


cn::Ref/*ArrayRef*/ ArrayRef::Create(cn::u32 uLen)
{
	return cn::Ref(new ArrayRef(uLen));
}


//赋值运算符重载
ArrayRef & ArrayRef::operator = (const ArrayRef &src)
{
	return (*this)(src.GetData(), src.GetLen());
}

//运算符重载，填充lpData指向的uLen个字节的数据
ArrayRef & ArrayRef::operator () (const cn::Ref *lpData, cn::u32 uLen)
{
	cn::u32 i;

	std::vector<cn::Ref> &arrTemp =*((std::vector<cn::Ref> *)(m_pData));
	arrTemp.resize(uLen);
	if(uLen > 0){
		cn::Ref *lpDest =&(arrTemp[0]);
		for(i =0; i<uLen; i++){
			lpDest[i] =lpData[i];
		}
	}

	return *this;
}

//运算符重载，创建一个uLen字节的缓冲区
ArrayRef & ArrayRef::operator () (cn::u32 uLen)
{
	std::vector<cn::Ref> &arrTemp =*((std::vector<cn::Ref> *)(m_pData));
	arrTemp.clear();
	arrTemp.resize(uLen);

	return *this;
}

//获得缓冲区首地址
cn::Ref *ArrayRef::GetData() const
{
	std::vector<cn::Ref> &arrTemp =*((std::vector<cn::Ref> *)(m_pData));
	if(arrTemp.size()>0){
		return &(arrTemp[0]);
	}else{
		return NULL;
	}
}

//获得缓冲区元素数
cn::u32 ArrayRef::GetLen() const
{
	std::vector<cn::Ref> &arrTemp =*((std::vector<cn::Ref> *)(m_pData));
	return arrTemp.size();
}


//重置缓冲区的大小，缓冲区位置有效的原有数据将予以保留
void ArrayRef::Resize(cn::u32 uLen)
{
	std::vector<cn::Ref> &arrTemp =*((std::vector<cn::Ref> *)(m_pData));
	arrTemp.resize(uLen);
}


//构造函数
ArrayRef::ArrayRef()
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::Ref>);
}


//构造函数，填充lpData指向的uLen个字节的数据
ArrayRef::ArrayRef(const cn::Ref *lpData, cn::u32 uLen)
: m_pData(NULL)
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::Ref>);

	(*this)(lpData, uLen);
}


//构造函数，创建一个uLen字节的缓冲区
ArrayRef::ArrayRef(cn::u32 uLen)
: m_pData(NULL)
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::Ref>);

	(*this)(uLen);
}


//析构函数
ArrayRef::~ArrayRef()
{
	delete ((std::vector<cn::Ref> *)(m_pData));
}


//创建一个弱引用数组
cn::Ref/*ArrayWRef*/ ArrayWRef::Create()
{
	return cn::Ref(new ArrayWRef());
}


cn::Ref/*ArrayWRef*/ ArrayWRef::Create(const cn::WRef *lpData, cn::u32 uLen)
{
	return cn::Ref(new ArrayWRef(lpData, uLen));
}


cn::Ref/*ArrayWRef*/ ArrayWRef::Create(cn::u32 uLen)
{
	return cn::Ref(new ArrayWRef(uLen));
}


//赋值运算符重载
ArrayWRef & ArrayWRef::operator = (const ArrayWRef &src)
{
	return (*this)(src.GetData(), src.GetLen());
}

//运算符重载，填充lpData指向的uLen个字节的数据
ArrayWRef & ArrayWRef::operator () (const cn::WRef *lpData, cn::u32 uLen)
{
	cn::u32 i;

	std::vector<cn::WRef> &arrTemp =*((std::vector<cn::WRef> *)(m_pData));
	arrTemp.resize(uLen);
	if(uLen > 0){
		cn::WRef *lpDest =&(arrTemp[0]);
		for(i =0; i<uLen; i++){
			lpDest[i] =lpData[i];
		}
	}

	return *this;
}

//运算符重载，创建一个uLen字节的缓冲区
ArrayWRef & ArrayWRef::operator () (cn::u32 uLen)
{
	std::vector<cn::WRef> &arrTemp =*((std::vector<cn::WRef> *)(m_pData));
	arrTemp.clear();
	arrTemp.resize(uLen);

	return *this;
}

//获得缓冲区首地址
cn::WRef *ArrayWRef::GetData() const
{
	std::vector<cn::WRef> &arrTemp =*((std::vector<cn::WRef> *)(m_pData));
	if(arrTemp.size()>0){
		return &(arrTemp[0]);
	}else{
		return NULL;
	}
}

//获得缓冲区元素数
cn::u32 ArrayWRef::GetLen() const
{
	std::vector<cn::WRef> &arrTemp =*((std::vector<cn::WRef> *)(m_pData));
	return arrTemp.size();
}


//重置缓冲区的大小，缓冲区位置有效的原有数据将予以保留
void ArrayWRef::Resize(cn::u32 uLen)
{
	std::vector<cn::WRef> &arrTemp =*((std::vector<cn::WRef> *)(m_pData));
	arrTemp.resize(uLen);
}


//构造函数
ArrayWRef::ArrayWRef()
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayWRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::WRef>);
}


//构造函数，填充lpData指向的uLen个字节的数据
ArrayWRef::ArrayWRef(const cn::WRef *lpData, cn::u32 uLen)
: m_pData(NULL)
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayWRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::WRef>);

	(*this)(lpData, uLen);
}


//构造函数，创建一个uLen字节的缓冲区
ArrayWRef::ArrayWRef(cn::u32 uLen)
: m_pData(NULL)
{
	//设置RTTI信息
	m_pClassIDs[++m_uCurClassLevel] =_GET_CLASS_ID(ArrayWRef);

	//初始化成员
	m_pData =(void *)(new std::vector<cn::WRef>);

	(*this)(uLen);
}


//析构函数
ArrayWRef::~ArrayWRef()
{
	delete ((std::vector<cn::WRef> *)(m_pData));
}































}