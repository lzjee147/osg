#include "Common/Math.h"

namespace cn
{


//定义误差的最小值
#define _MIN_ERROR_THRE	0.00001f
#define _MIN_ERROR_THRE_D 0.00000001


//生成单位仿射变换
Math::LOC_INFO GenLocIdentity()
{
	Math::LOC_INFO cTemp; cTemp.m_vScale(1.0f, 1.0f, 1.0f); cTemp.m_qRotate(0.0f, 0.0f, 0.0f, 1.0f); cTemp.m_vTrans(0.0f, 0.0f, 0.0f);
	return cTemp;
}
Math::LOC_INFO_D GenLocIdentity_D()
{
	Math::LOC_INFO_D cTemp; cTemp.m_vScale(1.0, 1.0, 1.0); cTemp.m_qRotate(0.0, 0.0, 0.0, 1.0); cTemp.m_vTrans(0.0, 0.0, 0.0);
	return cTemp;
}


//表示单位仿射变换
Math::LOC_INFO Math::LOC_INFO::m_cIdentity =GenLocIdentity(); 


//表示单位仿射变换
Math::LOC_INFO_D Math::LOC_INFO_D::m_cIdentity =GenLocIdentity_D(); 


//生成服从正态分布的随机数
float Math::GenNormalDistributedRandom()
{
	float fU1 =float(::rand())/float(RAND_MAX);
	float fU2 =float(::rand())/float(RAND_MAX);
	if (fU1 < 1.0e-6f){
		fU1 =1.0e-6f;
	}
	return ::sqrt(-2.0f*::log(fU1))*::cos(2.0f*_PI_F*fU2);
}


//生成Halton随机序列
//uBase：Halton序列的基数
//uSeqLoc：表示要生成Halton序列的第几个元素(从1开始)
//返回值：返回元素取值(范围为[0,1])
float Math::GenHaltonSequence(cn::u32 uBase, cn::u32 uSeqLoc)
{
	uBase =max(1, uBase); uSeqLoc =max(1, uSeqLoc);
	float fF =1.0f; float fRet =0.0f; float fBase =float(uBase);
	while(uSeqLoc > 0){
		fF /=fBase; fRet +=fF*float(uSeqLoc%uBase); uSeqLoc =uSeqLoc/uBase;
	}
	return fRet;
}


//判断一个整数是否为2的幂
bool Math::IsPow2(cn::u32 uVal)
{
	if(uVal == 0){
		return false;
	}

	cn::u32 uPow =cn::u32(::log(float(uVal))/::log(2.0f)+0.5f);
	cn::u32 uNewVal =cn::u32(::pow(2.0f, float(uPow))+0.5f);
	if(uVal == uNewVal){
		return true;
	}else{
		return false;
	}
}


//2维向量点乘
float Math::Vec2Dot(const Vec2 &vSrc1, const Vec2 &vSrc2)
{
	return ::D3DXVec2Dot((const D3DXVECTOR2 *)(&vSrc1), (const D3DXVECTOR2 *)(&vSrc2));
}


//对2维向量进行线性插值
//fCoef==0：vRet==vSrc1; fCoef==1：vRet==vSrc2;
void Math::Vec2Lerp(const Vec2 &vSrc1, const Vec2 &vSrc2, float fCoef, Vec2 &vRet)
{
	::D3DXVec2Lerp((D3DXVECTOR2 *)(&vRet), (const D3DXVECTOR2 *)(&vSrc1), (const D3DXVECTOR2 *)(&vSrc2), fCoef);
}


//3维向量点乘
float Math::Vec3Dot(const Vec3 &vSrc1, const Vec3 &vSrc2)
{
	return ::D3DXVec3Dot((const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2));
}


//3维向量叉乘
void Math::Vec3Cross(const Vec3 &vSrc1, const Vec3 &vSrc2, Vec3 &vRet)
{
	::D3DXVec3Cross((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2));
}


//对3维向量进行线性插值
//fCoef==0：vRet==vSrc1; fCoef==1：vRet==vSrc2;
void Math::Vec3Lerp(const Vec3 &vSrc1, const Vec3 &vSrc2, float fCoef, Vec3 &vRet)
{
	::D3DXVec3Lerp((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2), fCoef);
}


//对3维向量进行球面插值
//fCoef==0：vRet==vSrc1; fCoef==1：vRet==vSrc2;
void Math::Vec3Slerp(const Vec3 &vSrc1, const Vec3 &vSrc2, float fCoef, Vec3 &vRet)
{
	cn::Vec3 vSrc1Nor, vSrc2Nor;
	vSrc1.Normalize(vSrc1Nor); vSrc2.Normalize(vSrc2Nor);
	float fOmega =::acos(::D3DXVec3Dot((const D3DXVECTOR3 *)(&vSrc1Nor), (const D3DXVECTOR3 *)(&vSrc2Nor)));
	if(fOmega<0.0f || fOmega>_PI){
		vRet =cn::Vec3::m_vZero;
	}else if(fOmega < 0.001f){ //进行线性插值
		::D3DXVec3Lerp((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2), fCoef);
	}else{ //进行球面插值
		float fSinO =::sin(fOmega);
		if(fSinO > 0.000001f){
			float fSin1_tO =::sin((1.0f-fCoef)*fOmega);
			float fSintO =::sin(fCoef*fOmega);
			vRet =vSrc1*fSin1_tO/fSinO+vSrc2*fSintO/fSinO;
		}else{
			vRet =vSrc2;
		}
	}
}


//3维向量点乘(64位浮点数)
double Math::Vec3DDot(const Vec3D &vSrc1, const Vec3D &vSrc2)
{
	return vSrc1.x*vSrc2.x+vSrc1.y*vSrc2.y+vSrc1.z*vSrc2.z;
}


//3维向量叉乘(64位浮点数)
void Math::Vec3DCross(const Vec3D &vSrc1, const Vec3D &vSrc2, Vec3D &vRet)
{
	vRet.x =vSrc1.y*vSrc2.z-vSrc1.z*vSrc2.y;
	vRet.y =vSrc1.z*vSrc2.x-vSrc1.x*vSrc2.z;
	vRet.z =vSrc1.x*vSrc2.y-vSrc1.y*vSrc2.x;
}


//对3维向量进行线性插值(64位浮点数)
//dCoef==0：vRet==vSrc1; dCoef==1：vRet==vSrc2;
void Math::Vec3DLerp(const Vec3D &vSrc1, const Vec3D &vSrc2, double dCoef, Vec3D &vRet)
{
	double dCoef2 =1.0-dCoef;
	vRet.x =vSrc1.x*dCoef2+vSrc2.x*dCoef;
	vRet.y =vSrc1.y*dCoef2+vSrc2.y*dCoef;
	vRet.z =vSrc1.z*dCoef2+vSrc2.z*dCoef;
}


//对3维向量进行球面插值(64位浮点数)
//dCoef==0：vRet==vSrc1; dCoef==1：vRet==vSrc2;
void Math::Vec3DSlerp(const Vec3D &vSrc1, const Vec3D &vSrc2, double dCoef, Vec3D &vRet)
{
	cn::Vec3D vSrc1Nor, vSrc2Nor;
	vSrc1.Normalize(vSrc1Nor); vSrc2.Normalize(vSrc2Nor);
	double dOmega =::acos(vSrc1Nor.x*vSrc2Nor.x+vSrc1Nor.y*vSrc2Nor.y+vSrc1Nor.z*vSrc2Nor.z);
	if(dOmega<0.0f || dOmega>_PI){
		vRet =cn::Vec3D::m_vZero;
	}else if(dOmega < 0.001f){ //进行线性插值
		Math::Vec3DLerp(vSrc1, vSrc2, dCoef, vRet);
	}else{ //进行球面插值
		double dSinO =::sin(dOmega);
		if(dSinO > 0.000000001){
			double dSin1_tO =::sin((1.0f-dCoef)*dOmega);
			double dSintO =::sin(dCoef*dOmega);
			vRet =vSrc1*dSin1_tO/dSinO+vSrc2*dSintO/dSinO;
		}else{
			vRet =vSrc2;
		}
	}
}


//对4维向量进行线性插值
//fCoef==0：qRet==qSrc1; fCoef==1：qRet==qSrc2;
void Math::Vec4Lerp(const Vec4 &qSrc1, const Vec4 &qSrc2, float fCoef, Vec4 &qRet)
{
	::D3DXVec4Lerp((D3DXVECTOR4 *)(&qRet), (const D3DXVECTOR4 *)(&qSrc1), (const D3DXVECTOR4 *)(&qSrc2), fCoef);
}


//5维向量点乘(64位浮点数)
double Math::Vec5DDot(const Vec5D &vSrc1, const Vec5D &vSrc2)
{
	return vSrc1.v1*vSrc2.v1+vSrc1.v2*vSrc2.v2+vSrc1.v3*vSrc2.v3+vSrc1.v4*vSrc2.v4+vSrc1.v5*vSrc2.v5;
}


//8维向量点乘(64位浮点数)
double Math::Vec8DDot(const Vec8D &vSrc1, const Vec8D &vSrc2)
{
	return vSrc1.v1*vSrc2.v1+vSrc1.v2*vSrc2.v2+vSrc1.v3*vSrc2.v3+vSrc1.v4*vSrc2.v4+vSrc1.v5*vSrc2.v5+vSrc1.v6*vSrc2.v6+vSrc1.v7*vSrc2.v7+vSrc1.v8*vSrc2.v8;
}


//对四元素进行球面插值
//fCoef==0：qRet==qSrc1; fCoef==1：qRet==qSrc2;
void Math::QuaternionSlerp(const Quaternion &qSrc1, const Quaternion &qSrc2, float fCoef, Quaternion &qRet)
{
	::D3DXQuaternionSlerp((D3DXQUATERNION *)(&qRet), (const D3DXQUATERNION *)(&qSrc1), (const D3DXQUATERNION *)(&qSrc2), fCoef);
}


//合并两个Rect区域
void Math::MergeRect(const Rect &rc1,const Rect &rc2,Rect &rcRet)
{
	rcRet.m_fLeft = min(rc1.m_fLeft,rc2.m_fLeft);
	rcRet.m_fBottom=min(rc1.m_fBottom,rc2.m_fBottom);
	rcRet.m_fRight= max(rc1.m_fRight,rc2.m_fRight);
	rcRet.m_fTop  = max(rc1.m_fTop,rc2.m_fTop);
}


//合并两个RectInt区域
void Math::MergeRectInt(const RectInt &rc1,const RectInt &rc2,RectInt &rcRet)
{
	rcRet.m_sLeft = min(rc1.m_sLeft,rc2.m_sLeft);
	rcRet.m_sBottom=min(rc1.m_sBottom,rc2.m_sBottom);
	rcRet.m_sRight= max(rc1.m_sRight,rc2.m_sRight);
	rcRet.m_sTop  = max(rc1.m_sTop,rc2.m_sTop);
}


//合并多个包围盒
void Math::MergeAABB(const AABB *lpSrcBB, u32 uSrcBBNum, AABB &bbOut)
{
	if(uSrcBBNum == 0){
		return;
	}
	bbOut =lpSrcBB[0];
	for(u32 i =1; i<uSrcBBNum; i++){
		bbOut.m_vMin.x =min(bbOut.m_vMin.x, lpSrcBB[i].m_vMin.x);
		bbOut.m_vMin.y =min(bbOut.m_vMin.y, lpSrcBB[i].m_vMin.y);
		bbOut.m_vMin.z =min(bbOut.m_vMin.z, lpSrcBB[i].m_vMin.z);
		bbOut.m_vMax.x =max(bbOut.m_vMax.x, lpSrcBB[i].m_vMax.x);
		bbOut.m_vMax.y =max(bbOut.m_vMax.y, lpSrcBB[i].m_vMax.y);
		bbOut.m_vMax.z =max(bbOut.m_vMax.z, lpSrcBB[i].m_vMax.z);
	}
}


//合并多个包围球
void Math::MergeSphere(const Sphere *lpSrcSph, u32 uSrcSphNum, Sphere &sphOut)
{
	if(uSrcSphNum == 0){
		return;
	}
	sphOut =lpSrcSph[0];
	for(u32 i =1; i<uSrcSphNum; i++){
		if(sphOut.m_fRadius >= lpSrcSph[i].m_fRadius){
			float fCenterDist =(sphOut.m_vCenter-lpSrcSph[i].m_vCenter).Length();
			float fRadiusInc =(fCenterDist+lpSrcSph[i].m_fRadius - sphOut.m_fRadius)/2.0f; 
			if(fCenterDist<_MIN_ERROR_THRE || fRadiusInc <= 0.0f){
				continue;
			}
			::D3DXVec3Lerp((D3DXVECTOR3 *)(&(sphOut.m_vCenter)), (const D3DXVECTOR3 *)(&(sphOut.m_vCenter)), 
				(const D3DXVECTOR3 *)(&(lpSrcSph[i].m_vCenter)), fRadiusInc/fCenterDist);
			sphOut.m_fRadius +=fRadiusInc;

		}else{
			float fCenterDist =(sphOut.m_vCenter-lpSrcSph[i].m_vCenter).Length();
			float fRadiusInc =(fCenterDist+sphOut.m_fRadius - lpSrcSph[i].m_fRadius)/2.0f; 
			if(fCenterDist<_MIN_ERROR_THRE || fRadiusInc <= 0.0f){
				sphOut =lpSrcSph[i];
				continue;
			}
			::D3DXVec3Lerp((D3DXVECTOR3 *)(&(sphOut.m_vCenter)), (const D3DXVECTOR3 *)(&(lpSrcSph[i].m_vCenter)), 
				(const D3DXVECTOR3 *)(&(sphOut.m_vCenter)), fRadiusInc/fCenterDist);
			sphOut.m_fRadius =lpSrcSph[i].m_fRadius+fRadiusInc;
		}
	}
}


//求一点与一个AABB包围体间的最小距离平方
float Math::GetMinDistSq(const Vec3 &vSrc1, const AABB &bbSrc2)
{
	float fTemp;
	float fDistSq =0.0f;

	if(vSrc1.x < bbSrc2.m_vMin.x){
		fTemp =bbSrc2.m_vMin.x-vSrc1.x;
		fDistSq +=fTemp*fTemp;
	}else if(vSrc1.x > bbSrc2.m_vMax.x){
		fTemp =bbSrc2.m_vMax.x-vSrc1.x;
		fDistSq +=fTemp*fTemp;
	}

	if(vSrc1.y < bbSrc2.m_vMin.y){
		fTemp =bbSrc2.m_vMin.y-vSrc1.y;
		fDistSq +=fTemp*fTemp;
	}else if(vSrc1.y > bbSrc2.m_vMax.y){
		fTemp =bbSrc2.m_vMax.y-vSrc1.y;
		fDistSq +=fTemp*fTemp;
	}

	if(vSrc1.z < bbSrc2.m_vMin.z){
		fTemp =bbSrc2.m_vMin.z-vSrc1.z;
		fDistSq +=fTemp*fTemp;
	}else if(vSrc1.z > bbSrc2.m_vMax.z){
		fTemp =bbSrc2.m_vMax.z-vSrc1.z;
		fDistSq +=fTemp*fTemp;
	}

	return fDistSq;
}


//求一点与一个空间矩形间的最小距离平方
//vSrcPot：一点的坐标
//vRectBase,vRectE0,vRectE1：对空间任意点P，当且仅当在矩形内时可表示为 P=vRectBase+vRectE0*i+vRectE1*j (i,j位于区间[0,1])
//lpMinDistPot：返回矩形上距离最近的点坐标
//返回值：返回最短距离平方
float Math::GetMinDistSq(const Vec3 &vSrcPot, const Vec3 &vRectBase, const Vec3 &vRectE0, const Vec3 &vRectE1, cn::Vec3 *lpMinDistPot)
{
	cn::Vec3 vD =vSrcPot-vRectBase;

	float fS =vRectE0.x*vD.x+vRectE0.y*vD.y+vRectE0.z*vD.z;
	if(fS > 0.0f){
		float fDot0 =vRectE0.x*vRectE0.x+vRectE0.y*vRectE0.y+vRectE0.z*vRectE0.z;
		if(fS < fDot0){
			vD -=vRectE0*(fS/fDot0);
		}else{
			vD -=vRectE0;
		}
	}

	float fT =vRectE1.x*vD.x+vRectE1.y*vD.y+vRectE1.z*vD.z;
	if(fT > 0.0f){
		float fDot1 =vRectE1.x*vRectE1.x+vRectE1.y*vRectE1.y+vRectE1.z*vRectE1.z;
		if(fT < fDot1){
			vD -=vRectE1*(fT/fDot1);
		}else{
			vD -=vRectE1;
		}
	}

	if(lpMinDistPot){
		*lpMinDistPot =vSrcPot-vD;
	}
	return vD.x*vD.x+vD.y*vD.y+vD.z*vD.z;
}


//求一点与一个空间矩形间的最小距离平方
//vSrcPot：一点的坐标
//vRectBase,vRectE0,vRectE1：对空间任意点P，当且仅当在矩形内时可表示为 P=vRectBase+vRectE0*i+vRectE1*j (i,j位于区间[0,1])
//lpMinDistPot：返回矩形上距离最近的点坐标
//返回值：返回最短距离平方
double Math::GetMinDistSq_D(const Vec3D &vSrcPot, const Vec3D &vRectBase, const Vec3D &vRectE0, const Vec3D &vRectE1, cn::Vec3D *lpMinDistPot)
{
	cn::Vec3D vD =vSrcPot-vRectBase;

	double dS =vRectE0.x*vD.x+vRectE0.y*vD.y+vRectE0.z*vD.z;
	if(dS > 0.0f){
		double dDot0 =vRectE0.x*vRectE0.x+vRectE0.y*vRectE0.y+vRectE0.z*vRectE0.z;
		if(dS < dDot0){
			vD -=vRectE0*(dS/dDot0);
		}else{
			vD -=vRectE0;
		}
	}

	double dT =vRectE1.x*vD.x+vRectE1.y*vD.y+vRectE1.z*vD.z;
	if(dT > 0.0f){
		double dDot1 =vRectE1.x*vRectE1.x+vRectE1.y*vRectE1.y+vRectE1.z*vRectE1.z;
		if(dT < dDot1){
			vD -=vRectE1*(dT/dDot1);
		}else{
			vD -=vRectE1;
		}
	}

	if(lpMinDistPot){
		*lpMinDistPot =vSrcPot-vD;
	}
	return vD.x*vD.x+vD.y*vD.y+vD.z*vD.z;
}


//求一点与一个AABB包围体间的最大距离平方
float Math::GetMaxDistSq(const Vec3 &vSrc1, const AABB &bbSrc2)
{
	float fTemp;
	float fDistSq =0.0f;

	if(vSrc1.x <= (bbSrc2.m_vMin.x+bbSrc2.m_vMax.x)/2.0f){
		fTemp =bbSrc2.m_vMax.x-vSrc1.x;
		fDistSq +=fTemp*fTemp;
	}else{
		fTemp =bbSrc2.m_vMin.x-vSrc1.x;
		fDistSq +=fTemp*fTemp;
	}

	if(vSrc1.y <= (bbSrc2.m_vMin.y+bbSrc2.m_vMax.y)/2.0f){
		fTemp =bbSrc2.m_vMax.y-vSrc1.y;
		fDistSq +=fTemp*fTemp;
	}else{
		fTemp =bbSrc2.m_vMin.y-vSrc1.y;
		fDistSq +=fTemp*fTemp;
	}

	if(vSrc1.z <= (bbSrc2.m_vMin.z+bbSrc2.m_vMax.z)/2.0f){
		fTemp =bbSrc2.m_vMax.z-vSrc1.z;
		fDistSq +=fTemp*fTemp;
	}else{
		fTemp =bbSrc2.m_vMin.z-vSrc1.z;
		fDistSq +=fTemp*fTemp;
	}

	return fDistSq;
}


//求一点与一个AABB包围体间的最小距离在向量vDir上的投影长度
//vDir：表示要投影的方向向量
//返回值：vDir.Length()*投影长度
float Math::GetMinDist(const Vec3 &vSrc1, const AABB &bbSrc2, const cn::Vec3 &vDir)
{
	float fTemp;
	float fDist =0.0f;

	if(vSrc1.x < bbSrc2.m_vMin.x){
		fTemp =bbSrc2.m_vMin.x-vSrc1.x;
		fDist +=fTemp*vDir.x;
	}else if(vSrc1.x > bbSrc2.m_vMax.x){
		fTemp =bbSrc2.m_vMax.x-vSrc1.x;
		fDist +=fTemp*vDir.x;
	}

	if(vSrc1.y < bbSrc2.m_vMin.y){
		fTemp =bbSrc2.m_vMin.y-vSrc1.y;
		fDist +=fTemp*vDir.y;
	}else if(vSrc1.y > bbSrc2.m_vMax.y){
		fTemp =bbSrc2.m_vMax.y-vSrc1.y;
		fDist +=fTemp*vDir.y;
	}

	if(vSrc1.z < bbSrc2.m_vMin.z){
		fTemp =bbSrc2.m_vMin.z-vSrc1.z;
		fDist +=fTemp*vDir.z;
	}else if(vSrc1.z > bbSrc2.m_vMax.z){
		fTemp =bbSrc2.m_vMax.z-vSrc1.z;
		fDist +=fTemp*vDir.z;
	}

	return fDist;
}


//求一点与三角形的最短距离(64位浮点数)
//vSrcPot：一点的坐标
//vTriA,vTriB,vTriC：表示三角形的三个顶点
//lpMinPosCoord：返回最短距离点在三角形ABC上的重心坐标
//返回值：返回最短距离
double Math::GetMinDist_D(const cn::Vec3D &vSrcPot, const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, cn::Vec3D *lpMinPosCoord)
{
	cn::Vec3D vTemp;
	double dCoef1, dCoef2, dVecLen;
	double dMinDist =(vSrcPot-vTriA).Length(); //初始化最小距离
	if(lpMinPosCoord){(*lpMinPosCoord)(1.0, 0.0, 0.0);}

	cn::Vec3D vCross;
	cn::Math::Vec3DCross(vTriB-vTriA, vTriC-vTriA, vCross); //求出AB叉乘以AC
	double dCrossLen =vCross.Length(); //求出三角形ABC的面积*2

	if(dCrossLen > _MIN_ERROR_THRE_D){
		dCoef1 =cn::Math::Vec3DDot(vCross, vTriA-vSrcPot)/(dCrossLen*dCrossLen);
		cn::Vec3D vE =vSrcPot+vCross*dCoef1; //求出点vPos在三角形面ABC上的垂足vE
		cn::Math::Vec3DCross(vE-vTriB, vE-vTriC, vTemp); double dA =vTemp.Length()/dCrossLen;
		cn::Math::Vec3DCross(vE-vTriA, vE-vTriC, vTemp); double dB =vTemp.Length()/dCrossLen;
		cn::Math::Vec3DCross(vE-vTriA, vE-vTriB, vTemp); double dC =vTemp.Length()/dCrossLen;
		if(dA+dB+dC < 1.0+_MIN_ERROR_THRE_D){ //若垂足E位于三角形ABC内，则点vSrcPot与vE的距离即为最短距离
			if(lpMinPosCoord){(*lpMinPosCoord)(dA, dB, dC);}
			return ::abs(dCoef1*dCrossLen);
		}
	}

	//在ABC第1条边上寻找最短距离点
	dVecLen =(vTriB-vTriA).Length();
	if(dVecLen > _MIN_ERROR_THRE_D){
		dCoef2 =cn::Math::Vec3DDot(vTriB-vTriA, vSrcPot-vTriA)/(dVecLen*dVecLen);
		dCoef2 =min(1.0f, max(0.0f, dCoef2));	
		dVecLen =(vSrcPot-((1-dCoef2)*vTriA+dCoef2*vTriB)).Length();
		if(dVecLen < dMinDist){
			if(lpMinPosCoord){(*lpMinPosCoord)(1-dCoef2, dCoef2, 0.0);}
			dMinDist =dVecLen;
		}
	}

	//在ABC第2条边上寻找最短距离点
	dVecLen =(vTriC-vTriA).Length();
	if(dVecLen > _MIN_ERROR_THRE_D){
		dCoef2 =cn::Math::Vec3DDot(vTriC-vTriA, vSrcPot-vTriA)/(dVecLen*dVecLen);
		dCoef2 =min(1.0f, max(0.0f, dCoef2));	
		dVecLen =(vSrcPot-((1-dCoef2)*vTriA+dCoef2*vTriC)).Length();
		if(dVecLen < dMinDist){
			if(lpMinPosCoord){(*lpMinPosCoord)(1-dCoef2, 0.0, dCoef2);}
			dMinDist =dVecLen;
		}
	}

	//在ABC第3条边上寻找最短距离点
	dVecLen =(vTriC-vTriB).Length();
	if(dVecLen > _MIN_ERROR_THRE_D){
		dCoef2 =cn::Math::Vec3DDot(vTriC-vTriB, vSrcPot-vTriB)/(dVecLen*dVecLen);
		dCoef2 =min(1.0f, max(0.0f, dCoef2));	
		dVecLen =(vSrcPot-((1-dCoef2)*vTriB+dCoef2*vTriC)).Length();
		if(dVecLen < dMinDist){
			if(lpMinPosCoord){(*lpMinPosCoord)(0.0, 1-dCoef2, dCoef2);}
			dMinDist =dVecLen;
		}
	}

	return dMinDist;
}


//求一点与一个AABB包围体间的最大距离在向量vDir上的投影长度
//vDir：表示要投影的方向向量
//返回值：vDir.Length()*投影长度
float Math::GetMaxDist(const Vec3 &vSrc1, const AABB &bbSrc2, const cn::Vec3 &vDir)
{
	float fTemp;
	float fDist =0.0f;

	if(vSrc1.x <= (bbSrc2.m_vMin.x+bbSrc2.m_vMax.x)/2.0f){
		fTemp =bbSrc2.m_vMax.x-vSrc1.x;
		fDist +=fTemp*vDir.x;
	}else{
		fTemp =bbSrc2.m_vMin.x-vSrc1.x;
		fDist +=fTemp*vDir.x;
	}

	if(vSrc1.y <= (bbSrc2.m_vMin.y+bbSrc2.m_vMax.y)/2.0f){
		fTemp =bbSrc2.m_vMax.y-vSrc1.y;
		fDist +=fTemp*vDir.y;
	}else{
		fTemp =bbSrc2.m_vMin.y-vSrc1.y;
		fDist +=fTemp*vDir.y;
	}

	if(vSrc1.z <= (bbSrc2.m_vMin.z+bbSrc2.m_vMax.z)/2.0f){
		fTemp =bbSrc2.m_vMax.z-vSrc1.z;
		fDist +=fTemp*vDir.z;
	}else{
		fTemp =bbSrc2.m_vMin.z-vSrc1.z;
		fDist +=fTemp*vDir.z;
	}

	return fDist;
}


//检测一个包围球体与AABB包围体是否相交
bool Math::CheckIntersect(const Sphere &sphSrc1, const AABB &bbSrc2)
{
	return GetMinDistSq(sphSrc1.m_vCenter, bbSrc2) <= sphSrc1.m_fRadius*sphSrc1.m_fRadius;

	//if(sphSrc1.m_vCenter.x < bbSrc2.m_vMin.x){
	//	if(bbSrc2.m_vMin.x-sphSrc1.m_vCenter.x > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}
	//
	//if(sphSrc1.m_vCenter.x > bbSrc2.m_vMax.x){
	//	if(sphSrc1.m_vCenter.x-bbSrc2.m_vMax.x > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}

	//if(sphSrc1.m_vCenter.y < bbSrc2.m_vMin.y){
	//	if(bbSrc2.m_vMin.y-sphSrc1.m_vCenter.y > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}
	//	
	//if(sphSrc1.m_vCenter.y > bbSrc2.m_vMax.y){
	//	if(sphSrc1.m_vCenter.y-bbSrc2.m_vMax.y > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}

	//if(sphSrc1.m_vCenter.z < bbSrc2.m_vMin.z){
	//	if(bbSrc2.m_vMin.z-sphSrc1.m_vCenter.z > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}	
	//
	//if(sphSrc1.m_vCenter.z > bbSrc2.m_vMax.z){
	//	if(sphSrc1.m_vCenter.z-bbSrc2.m_vMax.z > sphSrc1.m_fRadius){
	//		return false;
	//	}
	//}
	//
	//return true;
}


//检测一个包围球体与AABB包围体是否相交
bool Math::CheckIntersect(const cn::Vec3 &vSphereCenter, float fRadiusSq, const AABB &bbSrc2)
{
	return GetMinDistSq(vSphereCenter, bbSrc2) <= fRadiusSq;
}


//检测AABB包围体于视椎体是否相交
bool Math::CheckIntersect(const AABB &bbSrc, const Plane lpFrustumPlanes[6])
{
	float pRelPlaneMax[3];

	for(cn::u32 i =0; i<6; i++){
		const Plane &CurPlane =lpFrustumPlanes[i];
	
		if(CurPlane.a > 0.0f){
			pRelPlaneMax[0] =bbSrc.m_vMax.x;
		}else{
			pRelPlaneMax[0] =bbSrc.m_vMin.x;
		}
		if(CurPlane.b > 0.0f){
			pRelPlaneMax[1] =bbSrc.m_vMax.y;
		}else{
			pRelPlaneMax[1] =bbSrc.m_vMin.y;
		}
		if(CurPlane.c > 0.0f){
			pRelPlaneMax[2] =bbSrc.m_vMax.z;
		}else{
			pRelPlaneMax[2] =bbSrc.m_vMin.z;
		}

		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMax)) < 0.0f){ //若包围盒完全在视椎体某个面外的话
			return false;
		}
	}

	return true;
}


//检测AABB包围体与射线是否相交
//fMinDist,fMaxDist：若射线所在直线与包围盒相交则返回两个交点的参数位置fMinDist和fMaxDist，若不相交则fMinDist和fMaxDist无定义
bool Math::CheckIntersect(const Ray &raySrc1, const AABB &bbSrc2, float &fMinDist, float &fMaxDist)
{
	//计算射线与当前树结点的包围盒的若干面之间的交点与射线起点的有向距离
	float fXMinDist, fXMaxDist, fYMinDist, fYMaxDist, fZMinDist, fZMaxDist, fTemp;
	if(::abs(raySrc1.m_vDir.x) > _MIN_ERROR_THRE){
		fXMinDist =(bbSrc2.m_vMin.x-raySrc1.m_vPos.x)/raySrc1.m_vDir.x;
		fXMaxDist =(bbSrc2.m_vMax.x-raySrc1.m_vPos.x)/raySrc1.m_vDir.x; 
	}else if(raySrc1.m_vPos.x>bbSrc2.m_vMin.x && raySrc1.m_vPos.x<bbSrc2.m_vMax.x){
		fXMinDist =-FLT_MAX; fXMaxDist =FLT_MAX;
	}else{
		return false;
	}
	if(::abs(raySrc1.m_vDir.y) > _MIN_ERROR_THRE){
		fYMinDist =(bbSrc2.m_vMin.y-raySrc1.m_vPos.y)/raySrc1.m_vDir.y;
		fYMaxDist =(bbSrc2.m_vMax.y-raySrc1.m_vPos.y)/raySrc1.m_vDir.y;
	}else if(raySrc1.m_vPos.y>bbSrc2.m_vMin.y && raySrc1.m_vPos.y<bbSrc2.m_vMax.y){
		fYMinDist =-FLT_MAX; fYMaxDist =FLT_MAX;
	}else{
		return false;
	}
	if(::abs(raySrc1.m_vDir.z) > _MIN_ERROR_THRE){
		fZMinDist =(bbSrc2.m_vMin.z-raySrc1.m_vPos.z)/raySrc1.m_vDir.z;
		fZMaxDist =(bbSrc2.m_vMax.z-raySrc1.m_vPos.z)/raySrc1.m_vDir.z;
	}else if(raySrc1.m_vPos.z>bbSrc2.m_vMin.z && raySrc1.m_vPos.z<bbSrc2.m_vMax.z){
		fZMinDist =-FLT_MAX; fZMaxDist =FLT_MAX;
	}else{
		return false;
	}

	//判断射线与树结点的包围盒是否相交
	if(fXMinDist > fXMaxDist){fTemp =fXMinDist; fXMinDist =fXMaxDist; fXMaxDist =fTemp;}
	if(fYMinDist > fYMaxDist){fTemp =fYMinDist; fYMinDist =fYMaxDist; fYMaxDist =fTemp;}
	if(fZMinDist > fZMaxDist){fTemp =fZMinDist; fZMinDist =fZMaxDist; fZMaxDist =fTemp;}

	fMinDist =max(fXMinDist, fYMinDist); fMinDist =max(fZMinDist, fMinDist);
	fMaxDist =min(fXMaxDist, fYMaxDist); fMaxDist =min(fZMaxDist, fMaxDist);

	if(fMinDist>fMaxDist || (fMinDist<=0.0f&&fMaxDist<=0.0f)){ //如果射线不与包围盒相交的话
		return false;
	}
	return true;
}


//检测平面与射线是否相交
//fDist：若射线所属的无限延伸的直线与平面相交则返回交点在当前射线上的参数位置，若不相交则fDist==-FLT_MAX
bool Math::CheckIntersect(const Ray &raySrc1, const Plane &qSrc2, float &fDist)
{
	float fDiv =raySrc1.m_vDir.x*qSrc2.a+raySrc1.m_vDir.y*qSrc2.b+raySrc1.m_vDir.z*qSrc2.c;
	if(::abs(fDiv) < _MIN_ERROR_THRE){
		fDist =-FLT_MAX;
		return false;
	}
	fDist =(-qSrc2.d-raySrc1.m_vPos.x*qSrc2.a-raySrc1.m_vPos.y*qSrc2.b-raySrc1.m_vPos.z*qSrc2.c) / fDiv;
	return (fDist >= 0.0f);
}


//检测三角面与射线是否相交
//fDist：若射线所属的无限延伸的直线与三角面相交则返回交点在当前射线上的参数位置，若不相交则fDist==-FLT_MAX
bool Math::CheckIntersect(const cn::Vec3 &vTriA, const cn::Vec3 &vTriB, const cn::Vec3 &vTriC, const Ray &raySrc1, float &fDist)
{
	cn::Mat4x4 matTemp;

	matTemp._11 =1.0f; matTemp._12 =0.0f; matTemp._13 =0.0f; matTemp._14 =0.0f;
	matTemp._21 =0.0f; matTemp._22 =vTriC.x-vTriA.x; matTemp._23 =vTriC.x-vTriB.x; matTemp._24 =raySrc1.m_vDir.x;
	matTemp._31 =0.0f; matTemp._32 =vTriC.y-vTriA.y; matTemp._33 =vTriC.y-vTriB.y; matTemp._34 =raySrc1.m_vDir.y;
	matTemp._41 =0.0f; matTemp._42 =vTriC.z-vTriA.z; matTemp._43 =vTriC.z-vTriB.z; matTemp._44 =raySrc1.m_vDir.z;
	float fDivide =matTemp.Determinant();
	if(::abs(fDivide)<0.00001f){ //如果计算中的分母接近0，则出错返回false
		fDist =-FLT_MAX;
		return false;
	}

	matTemp._22 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._32 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._42 =vTriC.z-raySrc1.m_vPos.z;
	float fU =matTemp.Determinant()/fDivide;

	matTemp._22 =vTriC.x-vTriA.x; matTemp._23 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._32 =vTriC.y-vTriA.y; matTemp._33 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._42 =vTriC.z-vTriA.z; matTemp._43 =vTriC.z-raySrc1.m_vPos.z;
	float fV =matTemp.Determinant()/fDivide;

	matTemp._23 =vTriC.x-vTriB.x; matTemp._24 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._33 =vTriC.y-vTriB.y; matTemp._34 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._43 =vTriC.z-vTriB.z; matTemp._44 =vTriC.z-raySrc1.m_vPos.z;
	fDist =matTemp.Determinant()/fDivide;

	if(fU>=0.0f && fV>=0.0f && fU+fV<=1.0f+_MIN_ERROR_THRE){
		return (fDist >= 0.0f);
	}else{
		fDist =-FLT_MAX;
		return false;
	}
}


//检测三角面与射线是否相交(64位浮点数)
//dDist：若射线所属的无限延伸的直线与三角面相交则返回交点在当前射线上的参数位置，若不相交则fDist==-DBL_MAX
bool Math::CheckIntersect_D(const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, const RayD &raySrc1, double &dDist)
{
	cn::Mat3x3D matTemp;

	matTemp._11 =vTriC.x-vTriA.x; matTemp._12 =vTriC.x-vTriB.x; matTemp._13 =raySrc1.m_vDir.x;
	matTemp._21 =vTriC.y-vTriA.y; matTemp._22 =vTriC.y-vTriB.y; matTemp._23 =raySrc1.m_vDir.y;
	matTemp._31 =vTriC.z-vTriA.z; matTemp._32 =vTriC.z-vTriB.z; matTemp._33 =raySrc1.m_vDir.z;
	double dDivide =matTemp.Determinant();
	if(::abs(dDivide)<_MIN_ERROR_THRE_D){ //如果计算中的分母接近0，则出错返回false
		dDist =-DBL_MAX;
		return false;
	}

	matTemp._11 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._21 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._31 =vTriC.z-raySrc1.m_vPos.z;
	double dU =matTemp.Determinant()/dDivide;

	matTemp._11 =vTriC.x-vTriA.x; matTemp._12 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._21 =vTriC.y-vTriA.y; matTemp._22 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._31 =vTriC.z-vTriA.z; matTemp._32 =vTriC.z-raySrc1.m_vPos.z;
	double dV =matTemp.Determinant()/dDivide;

	matTemp._12 =vTriC.x-vTriB.x; matTemp._13 =vTriC.x-raySrc1.m_vPos.x;
	matTemp._22 =vTriC.y-vTriB.y; matTemp._23 =vTriC.y-raySrc1.m_vPos.y;
	matTemp._32 =vTriC.z-vTriB.z; matTemp._33 =vTriC.z-raySrc1.m_vPos.z;
	dDist =matTemp.Determinant()/dDivide;

	if(dU>=0.0 && dV>=0.0 && dU+dV<=1.0+_MIN_ERROR_THRE_D){
		return (dDist >= 0.0);
	}else{
		dDist =-DBL_MAX;
		return false;
	}
}


//检测一个三角面与AABB包围体是否相交(64位浮点数)
bool Math::CheckIntersect_D(const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, const AABB_D &bbSrc2)
{
	cn::u32 i;

	if((vTriA.x>=bbSrc2.m_vMin.x && vTriA.x<=bbSrc2.m_vMax.x && vTriA.y>=bbSrc2.m_vMin.y && vTriA.y<=bbSrc2.m_vMax.y && vTriA.z>=bbSrc2.m_vMin.z && vTriA.z<=bbSrc2.m_vMax.z) ||
		(vTriB.x>=bbSrc2.m_vMin.x && vTriB.x<=bbSrc2.m_vMax.x && vTriB.y>=bbSrc2.m_vMin.y && vTriB.y<=bbSrc2.m_vMax.y && vTriB.z>=bbSrc2.m_vMin.z && vTriB.z<=bbSrc2.m_vMax.z) ||
		(vTriC.x>=bbSrc2.m_vMin.x && vTriC.x<=bbSrc2.m_vMax.x && vTriC.y>=bbSrc2.m_vMin.y && vTriC.y<=bbSrc2.m_vMax.y && vTriC.z>=bbSrc2.m_vMin.z && vTriC.z<=bbSrc2.m_vMax.z))
	{ //若三角形某个顶点完全在包围盒内部，则直接返回
		return true;
	}

	double dRatio;
	cn::Vec3D vLine[3];
	cn::u32 uLineIndex;
	double dDirect1, dDirect2, dValMin, dValMax;
	const double *pA =(const double *)(&vTriA); const double *pB =(const double *)(&vTriB); const double *pC =(const double *)(&vTriC); 
	cn::u32 uCurX, uCurY, uCurZ;
	double dSplit, dCurYMin, dCurYMax, dCurZMin, dCurZMax;
	bool bFind;

	for(i =0; i<6; i++){ //遍历包围体的6个面
		switch(i)
		{
		case 0:
			{
				uCurX =0; uCurY =1; uCurZ =2; dSplit =bbSrc2.m_vMin.x; dCurYMin =bbSrc2.m_vMin.y; dCurYMax =bbSrc2.m_vMax.y; dCurZMin =bbSrc2.m_vMin.z; dCurZMax =bbSrc2.m_vMax.z;
			}
			break;
		case 1:
			{
				uCurX =0; uCurY =1; uCurZ =2; dSplit =bbSrc2.m_vMax.x; dCurYMin =bbSrc2.m_vMin.y; dCurYMax =bbSrc2.m_vMax.y; dCurZMin =bbSrc2.m_vMin.z; dCurZMax =bbSrc2.m_vMax.z;
			}
			break;
		case 2:
			{
				uCurX =1; uCurY =0; uCurZ =2; dSplit =bbSrc2.m_vMin.y; dCurYMin =bbSrc2.m_vMin.x; dCurYMax =bbSrc2.m_vMax.x; dCurZMin =bbSrc2.m_vMin.z; dCurZMax =bbSrc2.m_vMax.z;
			}
			break;
		case 3:
			{
				uCurX =1; uCurY =0; uCurZ =2; dSplit =bbSrc2.m_vMax.y; dCurYMin =bbSrc2.m_vMin.x; dCurYMax =bbSrc2.m_vMax.x; dCurZMin =bbSrc2.m_vMin.z; dCurZMax =bbSrc2.m_vMax.z;
			}
			break;
		case 4:
			{
				uCurX =2; uCurY =1; uCurZ =0; dSplit =bbSrc2.m_vMin.z; dCurYMin =bbSrc2.m_vMin.y; dCurYMax =bbSrc2.m_vMax.y; dCurZMin =bbSrc2.m_vMin.x; dCurZMax =bbSrc2.m_vMax.x;
			}
			break;
		case 5:
			{
				uCurX =2; uCurY =1; uCurZ =0; dSplit =bbSrc2.m_vMax.z; dCurYMin =bbSrc2.m_vMin.y; dCurYMax =bbSrc2.m_vMax.y; dCurZMin =bbSrc2.m_vMin.x; dCurZMax =bbSrc2.m_vMax.x;
			}
			break;
		default:
			break;
		}

		//判断一个面上是否有交点
		uLineIndex =0;
		if(::abs(pC[uCurX]-pA[uCurX]) > _MIN_ERROR_THRE_D){
			dRatio =(pC[uCurX]-dSplit)/(pC[uCurX]-pA[uCurX]);
			if(dRatio>=-0.0001 && dRatio<=1.0001){cn::Math::Vec3DLerp(vTriC, vTriA, dRatio, vLine[uLineIndex++]);}
		}
		if(::abs(pC[uCurX]-pB[uCurX]) > _MIN_ERROR_THRE_D){
			dRatio =(pC[uCurX]-dSplit)/(pC[uCurX]-pB[uCurX]);
			if(dRatio>=-0.0001 && dRatio<=1.0001){cn::Math::Vec3DLerp(vTriC, vTriB, dRatio, vLine[uLineIndex++]);}
		}
		if(::abs(pA[uCurX]-pB[uCurX]) > _MIN_ERROR_THRE_D){
			dRatio =(pA[uCurX]-dSplit)/(pA[uCurX]-pB[uCurX]);
			if(dRatio>=-0.0001 && dRatio<=1.0001){cn::Math::Vec3DLerp(vTriA, vTriB, dRatio, vLine[uLineIndex++]);}
		}
		if(uLineIndex>=2){ //说明当前面与三角形有交线
			dDirect1 =((double *)(&vLine[1]))[uCurY]-((double *)(&vLine[0]))[uCurY]; dDirect2 =((double *)(&vLine[1]))[uCurZ]-((double *)(&vLine[0]))[uCurZ];
			dValMin =0.0f; dValMax =1.0f;
			bFind =false;
			if(dDirect1 > _MIN_ERROR_THRE_D){
				dValMin =max(dValMin, (dCurYMin-((double *)(&vLine[0]))[uCurY])/dDirect1);
				dValMax =min(dValMax, (dCurYMax-((double *)(&vLine[0]))[uCurY])/dDirect1);
				bFind =true;
			}else if(dDirect1 < -_MIN_ERROR_THRE_D){
				dValMin =max(dValMin, (dCurYMax-((double *)(&vLine[0]))[uCurY])/dDirect1);
				dValMax =min(dValMax, (dCurYMin-((double *)(&vLine[0]))[uCurY])/dDirect1);
				bFind =true;
			}else if(((double *)(&vLine[0]))[uCurY]<dCurYMin || ((double *)(&vLine[0]))[uCurY]>dCurYMax){
				continue;
			}
			if(dDirect2 > _MIN_ERROR_THRE_D){
				dValMin =max(dValMin, (dCurZMin-((double *)(&vLine[0]))[uCurZ])/dDirect2);
				dValMax =min(dValMax, (dCurZMax-((double *)(&vLine[0]))[uCurZ])/dDirect2);
				bFind =true;
			}else if(dDirect2 < -_MIN_ERROR_THRE_D){
				dValMin =max(dValMin, (dCurZMax-((double *)(&vLine[0]))[uCurZ])/dDirect2);
				dValMax =min(dValMax, (dCurZMin-((double *)(&vLine[0]))[uCurZ])/dDirect2);
				bFind =true;
			}else if(((double *)(&vLine[0]))[uCurZ]<dCurZMin || ((double *)(&vLine[0]))[uCurZ]>dCurZMax){
				continue;
			}
			if(dValMin <= dValMax){
				return true;
			}
		}
	}
	return false;
}


//检测射线<vRayPos,vRayDir>与球面<vSphCenter,dSphRadius>是否相交(64位浮点数)
//dDist：若相交则返回交点在当前射线上的参数位置，若不相交则fDist==-DBL_MAX
bool Math::CheckIntersect_Ray_Sph_D(const cn::Vec3D &vRayPos, const cn::Vec3D &vRayDir, const cn::Vec3D &vSphCenter, double dSphRadius, double &dDist)
{
	dDist =-DBL_MAX;

	double dA =vRayDir.x*vRayDir.x+vRayDir.y*vRayDir.y+vRayDir.z*vRayDir.z;
	if(dA == 0.0){
		return false;
	}
	cn::Vec3D vCP =vRayPos-vSphCenter;
	double dB =2.0*(vRayDir.x*vCP.x+vRayDir.y*vCP.y+vRayDir.z*vCP.z);
	double dC =vCP.x*vCP.x+vCP.y*vCP.y+vCP.z*vCP.z-dSphRadius*dSphRadius;
	double dSqr =dB*dB-4.0*dA*dC;
	if(dSqr < 0.0){
		return false;
	}
	double dSqrt =::sqrt(dSqr);
	double dD1 =(-dB-dSqrt)/2.0/dA; double dD2 =(-dB+dSqrt)/2.0/dA; 
	if(dD1 >= 0.0){
		if(dD2>=0.0 && dD2<dD1){
			dDist =dD2; return true;
		}else{
			dDist =dD1; return true;
		}
	}else if(dD2 >= 0.0){
		dDist =dD2; return true;
	}else{
		return false;
	}
}


//检测直线<vLinePos,vLineDir>与球面<vSphCenter,dSphRadius>是否相交(64位浮点数)
//dDist1,dDist2：若相交则返回两个交点在当前直线上的参数位置，若不相交则fDist1==-DBL_MAX,fDist2==-DBL_MAX
bool Math::CheckIntersect_Line_Sph_D(const cn::Vec3D &vLinePos, const cn::Vec3D &vLineDir, const cn::Vec3D &vSphCenter, double dSphRadius, double &dDist1, double &dDist2)
{
	dDist1 =-DBL_MAX; dDist2 =-DBL_MAX;

	double dA =vLineDir.x*vLineDir.x+vLineDir.y*vLineDir.y+vLineDir.z*vLineDir.z;
	if(dA == 0.0){
		return false;
	}
	cn::Vec3D vCP =vLinePos-vSphCenter;
	double dB =2.0*(vLineDir.x*vCP.x+vLineDir.y*vCP.y+vLineDir.z*vCP.z);
	double dC =vCP.x*vCP.x+vCP.y*vCP.y+vCP.z*vCP.z-dSphRadius*dSphRadius;
	double dSqr =dB*dB-4.0*dA*dC;
	if(dSqr < 0.0){
		return false;
	}
	double dSqrt =::sqrt(dSqr);
	dDist1 =(-dB-dSqrt)/2.0/dA; dDist2 =(-dB+dSqrt)/2.0/dA; 
	return true;
}


//检测AABB包围体相对于视椎体的相对位置
Math::INTERSECT_TYPE Math::CheckRelativeLocation(const AABB &bbSrc, const Plane lpFrustumPlanes[6])
{
	float pRelPlaneMin[3];
	float pRelPlaneMax[3];

	Math::INTERSECT_TYPE eInterType =Math::INTERSECT_TYPE_INSIDE;

	for(cn::u32 i =0; i<6; i++){
		const Plane &CurPlane =lpFrustumPlanes[i];
	
		if(CurPlane.a > 0.0f){
			pRelPlaneMin[0] =bbSrc.m_vMin.x;
			pRelPlaneMax[0] =bbSrc.m_vMax.x;
		}else{
			pRelPlaneMin[0] =bbSrc.m_vMax.x;
			pRelPlaneMax[0] =bbSrc.m_vMin.x;
		}
		if(CurPlane.b > 0.0f){
			pRelPlaneMin[1] =bbSrc.m_vMin.y;
			pRelPlaneMax[1] =bbSrc.m_vMax.y;
		}else{
			pRelPlaneMin[1] =bbSrc.m_vMax.y;
			pRelPlaneMax[1] =bbSrc.m_vMin.y;
		}
		if(CurPlane.c > 0.0f){
			pRelPlaneMin[2] =bbSrc.m_vMin.z;
			pRelPlaneMax[2] =bbSrc.m_vMax.z;
		}else{
			pRelPlaneMin[2] =bbSrc.m_vMax.z;
			pRelPlaneMax[2] =bbSrc.m_vMin.z;
		}

		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMax)) < 0.0f){ //若包围盒完全在视椎体某个面外的话
			return Math::INTERSECT_TYPE_OUTSIDE;
		}
		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMin)) < 0.0f){ //若包围盒与视椎体某个面相交的话
			eInterType =Math::INTERSECT_TYPE_INTER;
		}
	}

	return eInterType;
}


//将三角形<lpTriVers[0],lpTriVers[1],lpTriVers[2]>用单位化平面qSplit进行切割
//分割后分割平面反侧的三角形数为uSplitBackNum，分割平面正侧的三角形数为uSplitFrontNum
//分割后的三角形按先反侧后正侧的顺序依次排列为<lpTriVers[0],lpTriVers[1],lpTriVers[2]>、<lpTriVers[3],lpTriVers[4],lpTriVers[5]>、<lpTriVers[6],lpTriVers[7],lpTriVers[8]>
//分割后的所有三角形的环绕方式与原三角形的环绕方式相同
void Math::SplitTriangle(const cn::PlaneD &qSplit, cn::u32 &uSplitBackNum, cn::u32 &uSplitFrontNum, cn::Vec3D *lpTriVers)
{
	cn::u32 i, j, k;
	double dDist1, dDist2, dDist3;
	cn::Vec3D vPos[5]; //存储计算中的顶点位置
	cn::u32 pSide[5]; //存储顶点所在的方位(=1表示反侧，=2表示正侧, =3表示在分割面上)
	cn::u32 uPosNum =0; //表示计算出的顶点数目

	//处理第一条射线
	vPos[uPosNum] =lpTriVers[0];
	if((dDist1 =qSplit.DotCoord(lpTriVers[0])) < -_MIN_ERROR_THRE_D){
		pSide[uPosNum] =1;
	}else if(dDist1 > _MIN_ERROR_THRE_D){
		pSide[uPosNum] =2;
	}else{
		pSide[uPosNum] =3;
	}
	if((dDist2 =qSplit.DotCoord(lpTriVers[1])) < -_MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==2){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[0]*(-dDist2)+lpTriVers[1]*dDist1)/(dDist1-dDist2);
			pSide[uPosNum] =3;
		}
	}else if(dDist2 > _MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==1){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[0]*dDist2+lpTriVers[1]*(-dDist1))/(dDist2-dDist1);
			pSide[uPosNum] =3;
		}
	}
	//处理第二条射线
	uPosNum++;
	vPos[uPosNum] =lpTriVers[1];
	if(dDist2 < -_MIN_ERROR_THRE_D){
		pSide[uPosNum] =1;
	}else if(dDist2 > _MIN_ERROR_THRE_D){
		pSide[uPosNum] =2;
	}else{
		pSide[uPosNum] =3;
	}
	if((dDist3 =qSplit.DotCoord(lpTriVers[2])) < -_MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==2){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[1]*(-dDist3)+lpTriVers[2]*dDist2)/(dDist2-dDist3);
			pSide[uPosNum] =3;
		}
	}else if(dDist3 > _MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==1){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[1]*dDist3+lpTriVers[2]*(-dDist2))/(dDist3-dDist2);
			pSide[uPosNum] =3;
		}
	}
	//处理第三条射线
	uPosNum++;
	vPos[uPosNum] =lpTriVers[2];
	if(dDist3 < -_MIN_ERROR_THRE_D){
		pSide[uPosNum] =1;
	}else if(dDist3 > _MIN_ERROR_THRE_D){
		pSide[uPosNum] =2;
	}else{
		pSide[uPosNum] =3;
	}
	if(dDist1 < -_MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==2){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[2]*(-dDist1)+lpTriVers[0]*dDist3)/(dDist3-dDist1);
			pSide[uPosNum] =3;
		}
	}else if(dDist1 > _MIN_ERROR_THRE_D){
		if(pSide[uPosNum]==1){
			uPosNum++;
			vPos[uPosNum] =(lpTriVers[2]*dDist1+lpTriVers[0]*(-dDist3))/(dDist1-dDist3);
			pSide[uPosNum] =3;
		}
	}
	uPosNum++;

	//设置反向三角形
	uSplitBackNum =0;
	for(i =0; i<uPosNum; i++){
		if(pSide[i]==1){
			//设置第一个三角形
			j =i;
			lpTriVers[uSplitBackNum*3+0] =vPos[j];
			for(j =(j+1)%uPosNum;1;j =(j+1)%uPosNum){
				if(pSide[j]==1||pSide[j]==3)
					break;
			}
			lpTriVers[uSplitBackNum*3+1] =vPos[j];
			for(j =(j+1)%uPosNum;1;j =(j+1)%uPosNum){
				if(pSide[j]==1||pSide[j]==3)
					break;
			}
			lpTriVers[uSplitBackNum*3+2] =vPos[j];
			//设置第二个三角形
			k =j;
			for(k =(k+1)%uPosNum;1;k =(k+1)%uPosNum){
				if(pSide[k]==1||pSide[k]==3)
					break;
			}
			if(k!=i){
				uSplitBackNum++;
				lpTriVers[uSplitBackNum*3+0] =vPos[j];
				lpTriVers[uSplitBackNum*3+1] =vPos[k];
				lpTriVers[uSplitBackNum*3+2] =vPos[i];
			}
			uSplitBackNum++;
			break;
		}
	}
	//设置正向三角形
	uSplitFrontNum =0;
	for(i =0; i<uPosNum; i++){
		if(pSide[i]==2){
			//设置第一个三角形
			j =i;
			lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+0] =vPos[j];
			for(j =(j+1)%uPosNum;1;j =(j+1)%uPosNum){
				if(pSide[j]==2||pSide[j]==3)
					break;
			}
			lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+1] =vPos[j];
			for(j =(j+1)%uPosNum;1;j =(j+1)%uPosNum){
				if(pSide[j]==2||pSide[j]==3)
					break;
			}
			lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+2] =vPos[j];
			//设置第二个三角形
			k =j;
			for(k =(k+1)%uPosNum;1;k =(k+1)%uPosNum){
				if(pSide[k]==2||pSide[k]==3)
					break;
			}
			if(k!=i){
				uSplitFrontNum++;
				lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+0] =vPos[j];
				lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+1] =vPos[k];
				lpTriVers[(uSplitBackNum+uSplitFrontNum)*3+2] =vPos[i];
			}
			uSplitFrontNum++;
			break;
		}
	}
}


//对UV坐标为<sIDU,sIDV>的纹素进行变换
//sUNum,sVNum：表示纹理的尺寸
//uTrans：变换方式(cn::Math::TEXEL_TRANS的位或)
void Math::TransTexel(s32 sUNum, s32 sVNum, s32 sIDU, s32 sIDV, cn::u32 uTrans, s32 &sNewIDU, s32 &sNewIDV)
{
	if(uTrans & cn::Math::TEXEL_TRANS_INV_U){
		sIDU =sUNum-1-sIDU;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_INV_V){
		sIDV =sVNum-1-sIDV;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_SWAP_UV){
		sNewIDU =sIDV; sNewIDV =sIDU;
	}else{
		sNewIDU =sIDU; sNewIDV =sIDV;
	}
}


//对纹素区域rcSrc进行变换(由底向顶，纹素坐标值递增)
void Math::TransTexels_B2T(s32 sUNum, s32 sVNum, RectInt rcSrc, cn::u32 uTrans, RectInt &rcNew)
{
	RectInt rcTemp;

	if(uTrans & cn::Math::TEXEL_TRANS_INV_U){
		rcTemp.m_sLeft =sUNum-rcSrc.m_sRight; rcTemp.m_sRight =sUNum-rcSrc.m_sLeft;
	}else{
		rcTemp.m_sLeft =rcSrc.m_sLeft; rcTemp.m_sRight =rcSrc.m_sRight;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_INV_V){
		rcTemp.m_sBottom =sVNum-rcSrc.m_sTop; rcTemp.m_sTop =sVNum-rcSrc.m_sBottom;
	}else{
		rcTemp.m_sBottom =rcSrc.m_sBottom; rcTemp.m_sTop =rcSrc.m_sTop;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_SWAP_UV){
		rcNew.m_sLeft =rcTemp.m_sBottom; rcNew.m_sRight =rcTemp.m_sTop; rcNew.m_sBottom =rcTemp.m_sLeft; rcNew.m_sTop =rcTemp.m_sRight; 
	}else{
		rcNew.m_sLeft =rcTemp.m_sLeft; rcNew.m_sRight =rcTemp.m_sRight; rcNew.m_sBottom =rcTemp.m_sBottom; rcNew.m_sTop =rcTemp.m_sTop;
	}
}


//对纹素区域rcSrc进行变换(由底向顶，纹素坐标值递减)
void Math::TransTexels_T2B(s32 sUNum, s32 sVNum, RectInt rcSrc, cn::u32 uTrans, RectInt &rcNew)
{
	RectInt rcTemp;

	if(uTrans & cn::Math::TEXEL_TRANS_INV_U){
		rcTemp.m_sLeft =sUNum-rcSrc.m_sRight; rcTemp.m_sRight =sUNum-rcSrc.m_sLeft;
	}else{
		rcTemp.m_sLeft =rcSrc.m_sLeft; rcTemp.m_sRight =rcSrc.m_sRight;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_INV_V){
		rcTemp.m_sBottom =sVNum-rcSrc.m_sTop; rcTemp.m_sTop =sVNum-rcSrc.m_sBottom;
	}else{
		rcTemp.m_sBottom =rcSrc.m_sBottom; rcTemp.m_sTop =rcSrc.m_sTop;
	}
	if(uTrans & cn::Math::TEXEL_TRANS_SWAP_UV){
		rcNew.m_sLeft =rcTemp.m_sTop; rcNew.m_sRight =rcTemp.m_sBottom; rcNew.m_sBottom =rcTemp.m_sRight; rcNew.m_sTop =rcTemp.m_sLeft; 
	}else{
		rcNew.m_sLeft =rcTemp.m_sLeft; rcNew.m_sRight =rcTemp.m_sRight; rcNew.m_sBottom =rcTemp.m_sBottom; rcNew.m_sTop =rcTemp.m_sTop;
	}
}


//表示WDYF2015(WangDongYingFan2015)坐标系的实现
class WDYF2015Imp : public WDYF2015
{
public:

	//构造函数
	WDYF2015Imp(double dEarthRadius, cn::s32 sBaseGeoEPSG, const cn::Vec2D &vBaseGeoCoord,
		const cn::Vec2D &vBaseLocalXZ, const cn::Vec2D &vLocalXGeoDiff, const cn::Vec2D &vLocalZGeoDiff, double dLocalMeterScale);
	//析构函数
	virtual ~WDYF2015Imp();

	//获取WDYF2015坐标系参数
	virtual void GetInfo(double &dEarthRadius, cn::s32 &sBaseGeoEPSG, cn::Vec2D &vBaseGeoCoord,
		cn::Vec2D &vBaseLocalXZ, cn::Vec2D &vLocalXGeoDiff, cn::Vec2D &vLocalZGeoDiff, double &dLocalMeterScale);

	//WDYF2015局部空间坐标转换到全局唯一世界空间坐标
	virtual bool TransLocalToGol(cn::Vec3D vLocalPos, cn::Vec3D &vGolPos);
	//WDYF2015局部空间方位转换到全局唯一世界空间方位
	virtual bool TransLocalToGol(cn::Vec3D vLocalPos, cn::QuaternionD vLocalRotate, cn::Vec3D &vGolPos, cn::QuaternionD &vGolRotate);
	//WDYF2015全局唯一世界空间坐标转换到局部空间坐标
	virtual bool TransGolToLocal(cn::Vec3D vGolPos, cn::Vec3D &vLocalPos);
	//WDYF2015全局唯一世界空间方位转换到局部空间方位
	virtual bool TransGolToLocal(cn::Vec3D vGolPos, cn::QuaternionD vGolRotate, cn::Vec3D &vLocalPos, cn::QuaternionD &vLocalRotate);

	//根据经纬度和海拔高度获取地球空间中的位置
	//dLon,dLat：经纬度，以角度为单位(180度,...)
	//dHeight：海拔高度
	virtual cn::Vec3D GetEarthPos(double dLon, double dLat, double dHeight);

protected:

	double m_dEarthRadius; //表示全局唯一世界空间中地球的半径(以米为单位)
	cn::s32 m_sBaseGeoEPSG; //表示基准点的地理坐标EPSG代码
	cn::Vec2D m_vBaseGeoCoord; //表示基准点的地理坐标
	cn::Vec2D m_vBaseLocalXZ; //表示基准点在WDYF2015局部空间中的XZ坐标
	cn::Vec2D m_vLocalXGeoDiff, m_vLocalZGeoDiff; //表示WDYF2015局部空间中单位X(Z)长度上地理坐标的变化量，即地理坐标在X(Z)轴上的偏导数
	double m_dLocalMeterScale; //表示WDYF2015局部空间中，逻辑1米所对应的长度

	double m_dDiffDelta; //表示进行微分计算时的步长
	double m_dLocalEarthRadius; //表示WDYF2015局部空间中的地球半径
	cn::Vec3D m_vLocalEarthCenter; //表示WDYF2015局部空间中的地球球心位置
	cn::Vec3D m_vMidScale; cn::QuaternionD m_qMidRotate; cn::QuaternionD m_qMidRotateInv; cn::Vec3D m_vMidTrans; //表示中间空间变换信息
};


//创建一个WDYF2015坐标系对象
//dEarthRadius：表示全局唯一世界空间中地球的半径(以米为单位)
//sBaseGeoEPSG：表示基准点的地理坐标EPSG代码
//vBaseGeoCoord：表示基准点的地理坐标
//vBaseLocalXZ：表示基准点在WDYF2015局部空间中的XZ坐标
//vLocalXGeoDiff,vLocalZGeoDiff：表示WDYF2015局部空间中单位X(Z)长度上地理坐标的变化量，即地理坐标在X(Z)轴上的偏导数
//dLocalMeterScale：表示WDYF2015局部空间中，逻辑1米所对应的长度
WDYF2015 *WDYF2015::Create(double dEarthRadius, cn::s32 sBaseGeoEPSG, const cn::Vec2D &vBaseGeoCoord,
	const cn::Vec2D &vBaseLocalXZ, const cn::Vec2D &vLocalXGeoDiff, const cn::Vec2D &vLocalZGeoDiff, double dLocalMeterScale)
{
	try
	{
		return new WDYF2015Imp(dEarthRadius, sBaseGeoEPSG, vBaseGeoCoord, vBaseLocalXZ, vLocalXGeoDiff, vLocalZGeoDiff, dLocalMeterScale);
	}
	catch(...)
	{
		return NULL;
	}
}


//析构函数
WDYF2015::~WDYF2015()
{

}


//构造函数
WDYF2015::WDYF2015()
{

}


//构造函数
WDYF2015Imp::WDYF2015Imp(double dEarthRadius, cn::s32 sBaseGeoEPSG, const cn::Vec2D &vBaseGeoCoord,
	const cn::Vec2D &vBaseLocalXZ, const cn::Vec2D &vLocalXGeoDiff, const cn::Vec2D &vLocalZGeoDiff, double dLocalMeterScale)
: WDYF2015()
{
	m_dEarthRadius =dEarthRadius;
	m_sBaseGeoEPSG =sBaseGeoEPSG;
	m_vBaseGeoCoord =vBaseGeoCoord;
	m_vBaseLocalXZ =vBaseLocalXZ;
	m_vLocalXGeoDiff =vLocalXGeoDiff;
	m_vLocalZGeoDiff =vLocalZGeoDiff;
	m_dLocalMeterScale =dLocalMeterScale;
	
	m_dDiffDelta =m_dEarthRadius/637100000.0;
	m_dLocalEarthRadius =m_dEarthRadius*m_dLocalMeterScale;
	m_vLocalEarthCenter(vBaseLocalXZ.x, -m_dLocalEarthRadius, vBaseLocalXZ.y);

	::CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	std::string strDataPath =cn::FileIO::GetEngineCWD(); strDataPath +="\\data";
	::CPLSetConfigOption("GDAL_DATA", strDataPath.c_str());
	::GDALAllRegister();
	::OGRRegisterAll();
	OGRSpatialReference *lpGeoSpace0 =(OGRSpatialReference *)(::OSRNewSpatialReference(NULL));
	if(lpGeoSpace0==NULL || lpGeoSpace0->importFromEPSG(m_sBaseGeoEPSG)!=OGRERR_NONE){throw 1;}
	OGRSpatialReference *lpGeoSpace1 =(OGRSpatialReference *)(::OSRNewSpatialReference(NULL));
	if(lpGeoSpace1==NULL || lpGeoSpace1->SetWellKnownGeogCS("WGS84")!=OGRERR_NONE){throw 1;}
	OGRCoordinateTransformation *lpGeoTrans0_1 =::OGRCreateCoordinateTransformation(lpGeoSpace0, lpGeoSpace1);
	if(lpGeoTrans0_1 == NULL){throw 1;}

	cn::Vec3D vBaseLocalPos(vBaseLocalXZ.x, 0.0, vBaseLocalXZ.y);
	cn::Vec3D vBaseGolPos; cn::QuaternionD qBaseGolRotate;
	{
		cn::Vec3D vLocalPos =vBaseLocalPos; cn::QuaternionD qLocalRotate =cn::QuaternionD::m_qIdentity;
		cn::Vec3D &vGolPos =vBaseGolPos; cn::QuaternionD &qGolRotate =qBaseGolRotate;

		cn::Vec3D vLocalDir_X(m_dDiffDelta*m_dLocalMeterScale, 0.0, 0.0); cn::Vec3D vLocalDir_Y(0.0, m_dDiffDelta*m_dLocalMeterScale, 0.0); cn::Vec3D vLocalDir_Z(0.0, 0.0, m_dDiffDelta*m_dLocalMeterScale);
		vLocalDir_X.TransformCoordInPlace(qLocalRotate); vLocalDir_Y.TransformCoordInPlace(qLocalRotate);
		cn::Math::Vec3DCross(vLocalDir_X, vLocalDir_Y, vLocalDir_Z); cn::Math::Vec3DCross(vLocalDir_Y, vLocalDir_Z, vLocalDir_X);
		cn::Vec3D vLocalPos_X =vLocalPos+vLocalDir_X; cn::Vec3D vLocalPos_Y =vLocalPos+vLocalDir_Y;
		cn::Vec2D vCoord_O =m_vBaseGeoCoord;
		cn::Vec2D vCoord_X =m_vBaseGeoCoord+vLocalDir_X.x*m_vLocalXGeoDiff+vLocalDir_X.z*m_vLocalZGeoDiff;
		cn::Vec2D vCoord_Y =m_vBaseGeoCoord+vLocalDir_Y.x*m_vLocalXGeoDiff+vLocalDir_Y.z*m_vLocalZGeoDiff;
		lpGeoTrans0_1->Transform(1, &(vCoord_O.x), &(vCoord_O.y));
		lpGeoTrans0_1->Transform(1, &(vCoord_X.x), &(vCoord_X.y));
		lpGeoTrans0_1->Transform(1, &(vCoord_Y.x), &(vCoord_Y.y));
		cn::Vec3D vGolPos_O =GetEarthPos(vCoord_O.x, vCoord_O.y, vLocalPos.y/m_dLocalMeterScale);
		cn::Vec3D vGolPos_X =GetEarthPos(vCoord_X.x, vCoord_X.y, vLocalPos_X.y/m_dLocalMeterScale);
		cn::Vec3D vGolPos_Y =GetEarthPos(vCoord_Y.x, vCoord_Y.y, vLocalPos_Y.y/m_dLocalMeterScale);
		cn::Vec3D vGolDir_X, vGolDir_Y, vGolDir_Z;
		(vGolPos_X-vGolPos_O).Normalize(vGolDir_X);
		(vGolPos_Y-vGolPos_O).Normalize(vGolDir_Y);
		cn::Math::Vec3DCross(vGolDir_X, vGolDir_Y, vGolDir_Z); cn::Math::Vec3DCross(vGolDir_Y, vGolDir_Z, vGolDir_X);
		
		vLocalDir_X.NormalizeInPlace(); vLocalDir_Y.NormalizeInPlace(); vLocalDir_Z.NormalizeInPlace();
		vGolDir_X.NormalizeInPlace(); vGolDir_Y.NormalizeInPlace(); vGolDir_Z.NormalizeInPlace();
		cn::Mat4x4D matTemp1, matTemp2;
		matTemp1._11 =vLocalDir_X.x; matTemp1._12 =vLocalDir_X.y; matTemp1._13 =vLocalDir_X.z; matTemp1._14 =0.0; 
		matTemp1._21 =vLocalDir_Y.x; matTemp1._22 =vLocalDir_Y.y; matTemp1._23 =vLocalDir_Y.z; matTemp1._24 =0.0; 
		matTemp1._31 =vLocalDir_Z.x; matTemp1._32 =vLocalDir_Z.y; matTemp1._33 =vLocalDir_Z.z; matTemp1._34 =0.0; 
		matTemp1._41 =0.0; matTemp1._42 =0.0; matTemp1._43 =0.0; matTemp1._44 =1.0;
		matTemp1.InverseInPlace();
		matTemp2._11 =vGolDir_X.x; matTemp2._12 =vGolDir_X.y; matTemp2._13 =vGolDir_X.z; matTemp2._14 =0.0; 
		matTemp2._21 =vGolDir_Y.x; matTemp2._22 =vGolDir_Y.y; matTemp2._23 =vGolDir_Y.z; matTemp2._24 =0.0; 
		matTemp2._31 =vGolDir_Z.x; matTemp2._32 =vGolDir_Z.y; matTemp2._33 =vGolDir_Z.z; matTemp2._34 =0.0; 
		matTemp2._41 =0.0; matTemp2._42 =0.0; matTemp2._43 =0.0; matTemp2._44 =1.0; 
		cn::Mat4x4D matTemp =matTemp1*matTemp2;
		cn::Vec3D vTemp1, vTemp2;
		matTemp.Decompose(vTemp1, qGolRotate, vTemp2);
		vGolPos =vGolPos_O;
	}
	m_vMidScale(1.0/m_dLocalMeterScale, 1.0/m_dLocalMeterScale, 1.0/m_dLocalMeterScale);
	m_qMidRotate =qBaseGolRotate; m_qMidRotate.Inverse(m_qMidRotateInv);
	m_vMidTrans =vBaseLocalPos*m_vMidScale; m_vMidTrans.TransformCoordInPlace(m_qMidRotate);
	m_vMidTrans =vBaseGolPos-m_vMidTrans;

	if(lpGeoTrans0_1){OGRCoordinateTransformation::DestroyCT(lpGeoTrans0_1); lpGeoTrans0_1 =NULL;}
	if(lpGeoSpace0){OGRSpatialReference::DestroySpatialReference(lpGeoSpace0); lpGeoSpace0 =NULL;}
	if(lpGeoSpace1){OGRSpatialReference::DestroySpatialReference(lpGeoSpace1); lpGeoSpace1 =NULL;}
}


//析构函数
WDYF2015Imp::~WDYF2015Imp()
{

}


//获取WDYF2015坐标系参数
void WDYF2015Imp::GetInfo(double &dEarthRadius, cn::s32 &sBaseGeoEPSG, cn::Vec2D &vBaseGeoCoord,
	cn::Vec2D &vBaseLocalXZ, cn::Vec2D &vLocalXGeoDiff, cn::Vec2D &vLocalZGeoDiff, double &dLocalMeterScale)
{
	dEarthRadius =m_dEarthRadius;
	sBaseGeoEPSG =m_sBaseGeoEPSG;
	vBaseGeoCoord =m_vBaseGeoCoord;
	vBaseLocalXZ =m_vBaseLocalXZ;
	vLocalXGeoDiff =m_vLocalXGeoDiff;
	vLocalZGeoDiff =m_vLocalZGeoDiff;
	dLocalMeterScale =m_dLocalMeterScale;
}


//WDYF2015局部空间坐标转换到全局唯一世界空间坐标
bool WDYF2015Imp::TransLocalToGol(cn::Vec3D vLocalPos, cn::Vec3D &vGolPos)
{
	cn::Vec3D vSeaPos(vLocalPos.x, 0.0, vLocalPos.z);
	vSeaPos *=m_vMidScale; vSeaPos.TransformCoordInPlace(m_qMidRotate); vSeaPos +=m_vMidTrans;

	double dLen =vSeaPos.Length();
	if(dLen < 0.000001){
		return false;
	}
	vGolPos =vSeaPos*((m_dEarthRadius+vLocalPos.y/m_dLocalMeterScale)/dLen);

	return true;
}


//WDYF2015局部空间方位转换到全局唯一世界空间方位
bool WDYF2015Imp::TransLocalToGol(cn::Vec3D vLocalPos, cn::QuaternionD vLocalRotate, cn::Vec3D &vGolPos, cn::QuaternionD &vGolRotate)
{
	return false;
}


//WDYF2015全局唯一世界空间坐标转换到局部空间坐标
bool WDYF2015Imp::TransGolToLocal(cn::Vec3D vGolPos, cn::Vec3D &vLocalPos)
{
	double dLen =vGolPos.Length();
	if(dLen < 0.000001){
		return false;
	}
	double dHeight =dLen-m_dEarthRadius;

	vLocalPos =vGolPos-m_vMidTrans; vLocalPos.TransformCoordInPlace(m_qMidRotateInv); vLocalPos /=m_vMidScale;
	if(vLocalPos.y <= m_vLocalEarthCenter.y){
		return false;
	}
	cn::Vec3D vDir =vLocalPos-m_vLocalEarthCenter;
	vLocalPos =m_vLocalEarthCenter+vDir*(m_dLocalEarthRadius/vDir.y);
	vLocalPos.y =dHeight*m_dLocalMeterScale;

	return true;
}


//WDYF2015全局唯一世界空间方位转换到局部空间方位
bool WDYF2015Imp::TransGolToLocal(cn::Vec3D vGolPos, cn::QuaternionD vGolRotate, cn::Vec3D &vLocalPos, cn::QuaternionD &vLocalRotate)
{
	return false;
}


//根据经纬度和海拔高度获取地球空间中的位置
//dLon,dLat：经纬度，以角度为单位(180度,...)
//dHeight：海拔高度
cn::Vec3D WDYF2015Imp::GetEarthPos(double dLon, double dLat, double dHeight)
{
	dLon =dLon/180.0*_PI; dLat =dLat/180.0*_PI; dHeight =m_dEarthRadius+dHeight;
	cn::Vec3D vPos(-dHeight*::cos(dLat)*::sin(dLon), dHeight*::sin(dLat), dHeight*::cos(dLat)*::cos(dLon));
	return vPos;
}







}