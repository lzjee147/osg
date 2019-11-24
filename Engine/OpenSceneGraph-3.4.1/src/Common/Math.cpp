#include "Common/Math.h"

namespace cn
{


//����������Сֵ
#define _MIN_ERROR_THRE	0.00001f
#define _MIN_ERROR_THRE_D 0.00000001


//���ɵ�λ����任
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


//��ʾ��λ����任
Math::LOC_INFO Math::LOC_INFO::m_cIdentity =GenLocIdentity(); 


//��ʾ��λ����任
Math::LOC_INFO_D Math::LOC_INFO_D::m_cIdentity =GenLocIdentity_D(); 


//���ɷ�����̬�ֲ��������
float Math::GenNormalDistributedRandom()
{
	float fU1 =float(::rand())/float(RAND_MAX);
	float fU2 =float(::rand())/float(RAND_MAX);
	if (fU1 < 1.0e-6f){
		fU1 =1.0e-6f;
	}
	return ::sqrt(-2.0f*::log(fU1))*::cos(2.0f*_PI_F*fU2);
}


//����Halton�������
//uBase��Halton���еĻ���
//uSeqLoc����ʾҪ����Halton���еĵڼ���Ԫ��(��1��ʼ)
//����ֵ������Ԫ��ȡֵ(��ΧΪ[0,1])
float Math::GenHaltonSequence(cn::u32 uBase, cn::u32 uSeqLoc)
{
	uBase =max(1, uBase); uSeqLoc =max(1, uSeqLoc);
	float fF =1.0f; float fRet =0.0f; float fBase =float(uBase);
	while(uSeqLoc > 0){
		fF /=fBase; fRet +=fF*float(uSeqLoc%uBase); uSeqLoc =uSeqLoc/uBase;
	}
	return fRet;
}


//�ж�һ�������Ƿ�Ϊ2����
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


//2ά�������
float Math::Vec2Dot(const Vec2 &vSrc1, const Vec2 &vSrc2)
{
	return ::D3DXVec2Dot((const D3DXVECTOR2 *)(&vSrc1), (const D3DXVECTOR2 *)(&vSrc2));
}


//��2ά�����������Բ�ֵ
//fCoef==0��vRet==vSrc1; fCoef==1��vRet==vSrc2;
void Math::Vec2Lerp(const Vec2 &vSrc1, const Vec2 &vSrc2, float fCoef, Vec2 &vRet)
{
	::D3DXVec2Lerp((D3DXVECTOR2 *)(&vRet), (const D3DXVECTOR2 *)(&vSrc1), (const D3DXVECTOR2 *)(&vSrc2), fCoef);
}


//3ά�������
float Math::Vec3Dot(const Vec3 &vSrc1, const Vec3 &vSrc2)
{
	return ::D3DXVec3Dot((const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2));
}


//3ά�������
void Math::Vec3Cross(const Vec3 &vSrc1, const Vec3 &vSrc2, Vec3 &vRet)
{
	::D3DXVec3Cross((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2));
}


//��3ά�����������Բ�ֵ
//fCoef==0��vRet==vSrc1; fCoef==1��vRet==vSrc2;
void Math::Vec3Lerp(const Vec3 &vSrc1, const Vec3 &vSrc2, float fCoef, Vec3 &vRet)
{
	::D3DXVec3Lerp((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2), fCoef);
}


//��3ά�������������ֵ
//fCoef==0��vRet==vSrc1; fCoef==1��vRet==vSrc2;
void Math::Vec3Slerp(const Vec3 &vSrc1, const Vec3 &vSrc2, float fCoef, Vec3 &vRet)
{
	cn::Vec3 vSrc1Nor, vSrc2Nor;
	vSrc1.Normalize(vSrc1Nor); vSrc2.Normalize(vSrc2Nor);
	float fOmega =::acos(::D3DXVec3Dot((const D3DXVECTOR3 *)(&vSrc1Nor), (const D3DXVECTOR3 *)(&vSrc2Nor)));
	if(fOmega<0.0f || fOmega>_PI){
		vRet =cn::Vec3::m_vZero;
	}else if(fOmega < 0.001f){ //�������Բ�ֵ
		::D3DXVec3Lerp((D3DXVECTOR3 *)(&vRet), (const D3DXVECTOR3 *)(&vSrc1), (const D3DXVECTOR3 *)(&vSrc2), fCoef);
	}else{ //���������ֵ
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


//3ά�������(64λ������)
double Math::Vec3DDot(const Vec3D &vSrc1, const Vec3D &vSrc2)
{
	return vSrc1.x*vSrc2.x+vSrc1.y*vSrc2.y+vSrc1.z*vSrc2.z;
}


//3ά�������(64λ������)
void Math::Vec3DCross(const Vec3D &vSrc1, const Vec3D &vSrc2, Vec3D &vRet)
{
	vRet.x =vSrc1.y*vSrc2.z-vSrc1.z*vSrc2.y;
	vRet.y =vSrc1.z*vSrc2.x-vSrc1.x*vSrc2.z;
	vRet.z =vSrc1.x*vSrc2.y-vSrc1.y*vSrc2.x;
}


//��3ά�����������Բ�ֵ(64λ������)
//dCoef==0��vRet==vSrc1; dCoef==1��vRet==vSrc2;
void Math::Vec3DLerp(const Vec3D &vSrc1, const Vec3D &vSrc2, double dCoef, Vec3D &vRet)
{
	double dCoef2 =1.0-dCoef;
	vRet.x =vSrc1.x*dCoef2+vSrc2.x*dCoef;
	vRet.y =vSrc1.y*dCoef2+vSrc2.y*dCoef;
	vRet.z =vSrc1.z*dCoef2+vSrc2.z*dCoef;
}


//��3ά�������������ֵ(64λ������)
//dCoef==0��vRet==vSrc1; dCoef==1��vRet==vSrc2;
void Math::Vec3DSlerp(const Vec3D &vSrc1, const Vec3D &vSrc2, double dCoef, Vec3D &vRet)
{
	cn::Vec3D vSrc1Nor, vSrc2Nor;
	vSrc1.Normalize(vSrc1Nor); vSrc2.Normalize(vSrc2Nor);
	double dOmega =::acos(vSrc1Nor.x*vSrc2Nor.x+vSrc1Nor.y*vSrc2Nor.y+vSrc1Nor.z*vSrc2Nor.z);
	if(dOmega<0.0f || dOmega>_PI){
		vRet =cn::Vec3D::m_vZero;
	}else if(dOmega < 0.001f){ //�������Բ�ֵ
		Math::Vec3DLerp(vSrc1, vSrc2, dCoef, vRet);
	}else{ //���������ֵ
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


//��4ά�����������Բ�ֵ
//fCoef==0��qRet==qSrc1; fCoef==1��qRet==qSrc2;
void Math::Vec4Lerp(const Vec4 &qSrc1, const Vec4 &qSrc2, float fCoef, Vec4 &qRet)
{
	::D3DXVec4Lerp((D3DXVECTOR4 *)(&qRet), (const D3DXVECTOR4 *)(&qSrc1), (const D3DXVECTOR4 *)(&qSrc2), fCoef);
}


//5ά�������(64λ������)
double Math::Vec5DDot(const Vec5D &vSrc1, const Vec5D &vSrc2)
{
	return vSrc1.v1*vSrc2.v1+vSrc1.v2*vSrc2.v2+vSrc1.v3*vSrc2.v3+vSrc1.v4*vSrc2.v4+vSrc1.v5*vSrc2.v5;
}


//8ά�������(64λ������)
double Math::Vec8DDot(const Vec8D &vSrc1, const Vec8D &vSrc2)
{
	return vSrc1.v1*vSrc2.v1+vSrc1.v2*vSrc2.v2+vSrc1.v3*vSrc2.v3+vSrc1.v4*vSrc2.v4+vSrc1.v5*vSrc2.v5+vSrc1.v6*vSrc2.v6+vSrc1.v7*vSrc2.v7+vSrc1.v8*vSrc2.v8;
}


//����Ԫ�ؽ��������ֵ
//fCoef==0��qRet==qSrc1; fCoef==1��qRet==qSrc2;
void Math::QuaternionSlerp(const Quaternion &qSrc1, const Quaternion &qSrc2, float fCoef, Quaternion &qRet)
{
	::D3DXQuaternionSlerp((D3DXQUATERNION *)(&qRet), (const D3DXQUATERNION *)(&qSrc1), (const D3DXQUATERNION *)(&qSrc2), fCoef);
}


//�ϲ�����Rect����
void Math::MergeRect(const Rect &rc1,const Rect &rc2,Rect &rcRet)
{
	rcRet.m_fLeft = min(rc1.m_fLeft,rc2.m_fLeft);
	rcRet.m_fBottom=min(rc1.m_fBottom,rc2.m_fBottom);
	rcRet.m_fRight= max(rc1.m_fRight,rc2.m_fRight);
	rcRet.m_fTop  = max(rc1.m_fTop,rc2.m_fTop);
}


//�ϲ�����RectInt����
void Math::MergeRectInt(const RectInt &rc1,const RectInt &rc2,RectInt &rcRet)
{
	rcRet.m_sLeft = min(rc1.m_sLeft,rc2.m_sLeft);
	rcRet.m_sBottom=min(rc1.m_sBottom,rc2.m_sBottom);
	rcRet.m_sRight= max(rc1.m_sRight,rc2.m_sRight);
	rcRet.m_sTop  = max(rc1.m_sTop,rc2.m_sTop);
}


//�ϲ������Χ��
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


//�ϲ������Χ��
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


//��һ����һ��AABB��Χ������С����ƽ��
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


//��һ����һ���ռ���μ����С����ƽ��
//vSrcPot��һ�������
//vRectBase,vRectE0,vRectE1���Կռ������P�����ҽ����ھ�����ʱ�ɱ�ʾΪ P=vRectBase+vRectE0*i+vRectE1*j (i,jλ������[0,1])
//lpMinDistPot�����ؾ����Ͼ�������ĵ�����
//����ֵ��������̾���ƽ��
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


//��һ����һ���ռ���μ����С����ƽ��
//vSrcPot��һ�������
//vRectBase,vRectE0,vRectE1���Կռ������P�����ҽ����ھ�����ʱ�ɱ�ʾΪ P=vRectBase+vRectE0*i+vRectE1*j (i,jλ������[0,1])
//lpMinDistPot�����ؾ����Ͼ�������ĵ�����
//����ֵ��������̾���ƽ��
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


//��һ����һ��AABB��Χ����������ƽ��
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


//��һ����һ��AABB��Χ������С����������vDir�ϵ�ͶӰ����
//vDir����ʾҪͶӰ�ķ�������
//����ֵ��vDir.Length()*ͶӰ����
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


//��һ���������ε���̾���(64λ������)
//vSrcPot��һ�������
//vTriA,vTriB,vTriC����ʾ�����ε���������
//lpMinPosCoord��������̾������������ABC�ϵ���������
//����ֵ��������̾���
double Math::GetMinDist_D(const cn::Vec3D &vSrcPot, const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, cn::Vec3D *lpMinPosCoord)
{
	cn::Vec3D vTemp;
	double dCoef1, dCoef2, dVecLen;
	double dMinDist =(vSrcPot-vTriA).Length(); //��ʼ����С����
	if(lpMinPosCoord){(*lpMinPosCoord)(1.0, 0.0, 0.0);}

	cn::Vec3D vCross;
	cn::Math::Vec3DCross(vTriB-vTriA, vTriC-vTriA, vCross); //���AB�����AC
	double dCrossLen =vCross.Length(); //���������ABC�����*2

	if(dCrossLen > _MIN_ERROR_THRE_D){
		dCoef1 =cn::Math::Vec3DDot(vCross, vTriA-vSrcPot)/(dCrossLen*dCrossLen);
		cn::Vec3D vE =vSrcPot+vCross*dCoef1; //�����vPos����������ABC�ϵĴ���vE
		cn::Math::Vec3DCross(vE-vTriB, vE-vTriC, vTemp); double dA =vTemp.Length()/dCrossLen;
		cn::Math::Vec3DCross(vE-vTriA, vE-vTriC, vTemp); double dB =vTemp.Length()/dCrossLen;
		cn::Math::Vec3DCross(vE-vTriA, vE-vTriB, vTemp); double dC =vTemp.Length()/dCrossLen;
		if(dA+dB+dC < 1.0+_MIN_ERROR_THRE_D){ //������Eλ��������ABC�ڣ����vSrcPot��vE�ľ��뼴Ϊ��̾���
			if(lpMinPosCoord){(*lpMinPosCoord)(dA, dB, dC);}
			return ::abs(dCoef1*dCrossLen);
		}
	}

	//��ABC��1������Ѱ����̾����
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

	//��ABC��2������Ѱ����̾����
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

	//��ABC��3������Ѱ����̾����
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


//��һ����һ��AABB��Χ����������������vDir�ϵ�ͶӰ����
//vDir����ʾҪͶӰ�ķ�������
//����ֵ��vDir.Length()*ͶӰ����
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


//���һ����Χ������AABB��Χ���Ƿ��ཻ
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


//���һ����Χ������AABB��Χ���Ƿ��ཻ
bool Math::CheckIntersect(const cn::Vec3 &vSphereCenter, float fRadiusSq, const AABB &bbSrc2)
{
	return GetMinDistSq(vSphereCenter, bbSrc2) <= fRadiusSq;
}


//���AABB��Χ������׵���Ƿ��ཻ
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

		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMax)) < 0.0f){ //����Χ����ȫ����׵��ĳ������Ļ�
			return false;
		}
	}

	return true;
}


//���AABB��Χ���������Ƿ��ཻ
//fMinDist,fMaxDist������������ֱ�����Χ���ཻ�򷵻���������Ĳ���λ��fMinDist��fMaxDist�������ཻ��fMinDist��fMaxDist�޶���
bool Math::CheckIntersect(const Ray &raySrc1, const AABB &bbSrc2, float &fMinDist, float &fMaxDist)
{
	//���������뵱ǰ�����İ�Χ�е�������֮��Ľ��������������������
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

	//�ж������������İ�Χ���Ƿ��ཻ
	if(fXMinDist > fXMaxDist){fTemp =fXMinDist; fXMinDist =fXMaxDist; fXMaxDist =fTemp;}
	if(fYMinDist > fYMaxDist){fTemp =fYMinDist; fYMinDist =fYMaxDist; fYMaxDist =fTemp;}
	if(fZMinDist > fZMaxDist){fTemp =fZMinDist; fZMinDist =fZMaxDist; fZMaxDist =fTemp;}

	fMinDist =max(fXMinDist, fYMinDist); fMinDist =max(fZMinDist, fMinDist);
	fMaxDist =min(fXMaxDist, fYMaxDist); fMaxDist =min(fZMaxDist, fMaxDist);

	if(fMinDist>fMaxDist || (fMinDist<=0.0f&&fMaxDist<=0.0f)){ //������߲����Χ���ཻ�Ļ�
		return false;
	}
	return true;
}


//���ƽ���������Ƿ��ཻ
//fDist�����������������������ֱ����ƽ���ཻ�򷵻ؽ����ڵ�ǰ�����ϵĲ���λ�ã������ཻ��fDist==-FLT_MAX
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


//����������������Ƿ��ཻ
//fDist�����������������������ֱ�����������ཻ�򷵻ؽ����ڵ�ǰ�����ϵĲ���λ�ã������ཻ��fDist==-FLT_MAX
bool Math::CheckIntersect(const cn::Vec3 &vTriA, const cn::Vec3 &vTriB, const cn::Vec3 &vTriC, const Ray &raySrc1, float &fDist)
{
	cn::Mat4x4 matTemp;

	matTemp._11 =1.0f; matTemp._12 =0.0f; matTemp._13 =0.0f; matTemp._14 =0.0f;
	matTemp._21 =0.0f; matTemp._22 =vTriC.x-vTriA.x; matTemp._23 =vTriC.x-vTriB.x; matTemp._24 =raySrc1.m_vDir.x;
	matTemp._31 =0.0f; matTemp._32 =vTriC.y-vTriA.y; matTemp._33 =vTriC.y-vTriB.y; matTemp._34 =raySrc1.m_vDir.y;
	matTemp._41 =0.0f; matTemp._42 =vTriC.z-vTriA.z; matTemp._43 =vTriC.z-vTriB.z; matTemp._44 =raySrc1.m_vDir.z;
	float fDivide =matTemp.Determinant();
	if(::abs(fDivide)<0.00001f){ //��������еķ�ĸ�ӽ�0���������false
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


//����������������Ƿ��ཻ(64λ������)
//dDist�����������������������ֱ�����������ཻ�򷵻ؽ����ڵ�ǰ�����ϵĲ���λ�ã������ཻ��fDist==-DBL_MAX
bool Math::CheckIntersect_D(const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, const RayD &raySrc1, double &dDist)
{
	cn::Mat3x3D matTemp;

	matTemp._11 =vTriC.x-vTriA.x; matTemp._12 =vTriC.x-vTriB.x; matTemp._13 =raySrc1.m_vDir.x;
	matTemp._21 =vTriC.y-vTriA.y; matTemp._22 =vTriC.y-vTriB.y; matTemp._23 =raySrc1.m_vDir.y;
	matTemp._31 =vTriC.z-vTriA.z; matTemp._32 =vTriC.z-vTriB.z; matTemp._33 =raySrc1.m_vDir.z;
	double dDivide =matTemp.Determinant();
	if(::abs(dDivide)<_MIN_ERROR_THRE_D){ //��������еķ�ĸ�ӽ�0���������false
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


//���һ����������AABB��Χ���Ƿ��ཻ(64λ������)
bool Math::CheckIntersect_D(const cn::Vec3D &vTriA, const cn::Vec3D &vTriB, const cn::Vec3D &vTriC, const AABB_D &bbSrc2)
{
	cn::u32 i;

	if((vTriA.x>=bbSrc2.m_vMin.x && vTriA.x<=bbSrc2.m_vMax.x && vTriA.y>=bbSrc2.m_vMin.y && vTriA.y<=bbSrc2.m_vMax.y && vTriA.z>=bbSrc2.m_vMin.z && vTriA.z<=bbSrc2.m_vMax.z) ||
		(vTriB.x>=bbSrc2.m_vMin.x && vTriB.x<=bbSrc2.m_vMax.x && vTriB.y>=bbSrc2.m_vMin.y && vTriB.y<=bbSrc2.m_vMax.y && vTriB.z>=bbSrc2.m_vMin.z && vTriB.z<=bbSrc2.m_vMax.z) ||
		(vTriC.x>=bbSrc2.m_vMin.x && vTriC.x<=bbSrc2.m_vMax.x && vTriC.y>=bbSrc2.m_vMin.y && vTriC.y<=bbSrc2.m_vMax.y && vTriC.z>=bbSrc2.m_vMin.z && vTriC.z<=bbSrc2.m_vMax.z))
	{ //��������ĳ��������ȫ�ڰ�Χ���ڲ�����ֱ�ӷ���
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

	for(i =0; i<6; i++){ //������Χ���6����
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

		//�ж�һ�������Ƿ��н���
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
		if(uLineIndex>=2){ //˵����ǰ�����������н���
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


//�������<vRayPos,vRayDir>������<vSphCenter,dSphRadius>�Ƿ��ཻ(64λ������)
//dDist�����ཻ�򷵻ؽ����ڵ�ǰ�����ϵĲ���λ�ã������ཻ��fDist==-DBL_MAX
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


//���ֱ��<vLinePos,vLineDir>������<vSphCenter,dSphRadius>�Ƿ��ཻ(64λ������)
//dDist1,dDist2�����ཻ�򷵻����������ڵ�ǰֱ���ϵĲ���λ�ã������ཻ��fDist1==-DBL_MAX,fDist2==-DBL_MAX
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


//���AABB��Χ���������׵������λ��
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

		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMax)) < 0.0f){ //����Χ����ȫ����׵��ĳ������Ļ�
			return Math::INTERSECT_TYPE_OUTSIDE;
		}
		if(CurPlane.DotCoord((const Vec3 &)(*pRelPlaneMin)) < 0.0f){ //����Χ������׵��ĳ�����ཻ�Ļ�
			eInterType =Math::INTERSECT_TYPE_INTER;
		}
	}

	return eInterType;
}


//��������<lpTriVers[0],lpTriVers[1],lpTriVers[2]>�õ�λ��ƽ��qSplit�����и�
//�ָ��ָ�ƽ�淴�����������ΪuSplitBackNum���ָ�ƽ���������������ΪuSplitFrontNum
//�ָ��������ΰ��ȷ���������˳����������Ϊ<lpTriVers[0],lpTriVers[1],lpTriVers[2]>��<lpTriVers[3],lpTriVers[4],lpTriVers[5]>��<lpTriVers[6],lpTriVers[7],lpTriVers[8]>
//�ָ������������εĻ��Ʒ�ʽ��ԭ�����εĻ��Ʒ�ʽ��ͬ
void Math::SplitTriangle(const cn::PlaneD &qSplit, cn::u32 &uSplitBackNum, cn::u32 &uSplitFrontNum, cn::Vec3D *lpTriVers)
{
	cn::u32 i, j, k;
	double dDist1, dDist2, dDist3;
	cn::Vec3D vPos[5]; //�洢�����еĶ���λ��
	cn::u32 pSide[5]; //�洢�������ڵķ�λ(=1��ʾ���࣬=2��ʾ����, =3��ʾ�ڷָ�����)
	cn::u32 uPosNum =0; //��ʾ������Ķ�����Ŀ

	//�����һ������
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
	//����ڶ�������
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
	//�������������
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

	//���÷���������
	uSplitBackNum =0;
	for(i =0; i<uPosNum; i++){
		if(pSide[i]==1){
			//���õ�һ��������
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
			//���õڶ���������
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
	//��������������
	uSplitFrontNum =0;
	for(i =0; i<uPosNum; i++){
		if(pSide[i]==2){
			//���õ�һ��������
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
			//���õڶ���������
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


//��UV����Ϊ<sIDU,sIDV>�����ؽ��б任
//sUNum,sVNum����ʾ����ĳߴ�
//uTrans���任��ʽ(cn::Math::TEXEL_TRANS��λ��)
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


//����������rcSrc���б任(�ɵ��򶥣���������ֵ����)
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


//����������rcSrc���б任(�ɵ��򶥣���������ֵ�ݼ�)
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


//��ʾWDYF2015(WangDongYingFan2015)����ϵ��ʵ��
class WDYF2015Imp : public WDYF2015
{
public:

	//���캯��
	WDYF2015Imp(double dEarthRadius, cn::s32 sBaseGeoEPSG, const cn::Vec2D &vBaseGeoCoord,
		const cn::Vec2D &vBaseLocalXZ, const cn::Vec2D &vLocalXGeoDiff, const cn::Vec2D &vLocalZGeoDiff, double dLocalMeterScale);
	//��������
	virtual ~WDYF2015Imp();

	//��ȡWDYF2015����ϵ����
	virtual void GetInfo(double &dEarthRadius, cn::s32 &sBaseGeoEPSG, cn::Vec2D &vBaseGeoCoord,
		cn::Vec2D &vBaseLocalXZ, cn::Vec2D &vLocalXGeoDiff, cn::Vec2D &vLocalZGeoDiff, double &dLocalMeterScale);

	//WDYF2015�ֲ��ռ�����ת����ȫ��Ψһ����ռ�����
	virtual bool TransLocalToGol(cn::Vec3D vLocalPos, cn::Vec3D &vGolPos);
	//WDYF2015�ֲ��ռ䷽λת����ȫ��Ψһ����ռ䷽λ
	virtual bool TransLocalToGol(cn::Vec3D vLocalPos, cn::QuaternionD vLocalRotate, cn::Vec3D &vGolPos, cn::QuaternionD &vGolRotate);
	//WDYF2015ȫ��Ψһ����ռ�����ת�����ֲ��ռ�����
	virtual bool TransGolToLocal(cn::Vec3D vGolPos, cn::Vec3D &vLocalPos);
	//WDYF2015ȫ��Ψһ����ռ䷽λת�����ֲ��ռ䷽λ
	virtual bool TransGolToLocal(cn::Vec3D vGolPos, cn::QuaternionD vGolRotate, cn::Vec3D &vLocalPos, cn::QuaternionD &vLocalRotate);

	//���ݾ�γ�Ⱥͺ��θ߶Ȼ�ȡ����ռ��е�λ��
	//dLon,dLat����γ�ȣ��ԽǶ�Ϊ��λ(180��,...)
	//dHeight�����θ߶�
	virtual cn::Vec3D GetEarthPos(double dLon, double dLat, double dHeight);

protected:

	double m_dEarthRadius; //��ʾȫ��Ψһ����ռ��е���İ뾶(����Ϊ��λ)
	cn::s32 m_sBaseGeoEPSG; //��ʾ��׼��ĵ�������EPSG����
	cn::Vec2D m_vBaseGeoCoord; //��ʾ��׼��ĵ�������
	cn::Vec2D m_vBaseLocalXZ; //��ʾ��׼����WDYF2015�ֲ��ռ��е�XZ����
	cn::Vec2D m_vLocalXGeoDiff, m_vLocalZGeoDiff; //��ʾWDYF2015�ֲ��ռ��е�λX(Z)�����ϵ�������ı仯����������������X(Z)���ϵ�ƫ����
	double m_dLocalMeterScale; //��ʾWDYF2015�ֲ��ռ��У��߼�1������Ӧ�ĳ���

	double m_dDiffDelta; //��ʾ����΢�ּ���ʱ�Ĳ���
	double m_dLocalEarthRadius; //��ʾWDYF2015�ֲ��ռ��еĵ���뾶
	cn::Vec3D m_vLocalEarthCenter; //��ʾWDYF2015�ֲ��ռ��еĵ�������λ��
	cn::Vec3D m_vMidScale; cn::QuaternionD m_qMidRotate; cn::QuaternionD m_qMidRotateInv; cn::Vec3D m_vMidTrans; //��ʾ�м�ռ�任��Ϣ
};


//����һ��WDYF2015����ϵ����
//dEarthRadius����ʾȫ��Ψһ����ռ��е���İ뾶(����Ϊ��λ)
//sBaseGeoEPSG����ʾ��׼��ĵ�������EPSG����
//vBaseGeoCoord����ʾ��׼��ĵ�������
//vBaseLocalXZ����ʾ��׼����WDYF2015�ֲ��ռ��е�XZ����
//vLocalXGeoDiff,vLocalZGeoDiff����ʾWDYF2015�ֲ��ռ��е�λX(Z)�����ϵ�������ı仯����������������X(Z)���ϵ�ƫ����
//dLocalMeterScale����ʾWDYF2015�ֲ��ռ��У��߼�1������Ӧ�ĳ���
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


//��������
WDYF2015::~WDYF2015()
{

}


//���캯��
WDYF2015::WDYF2015()
{

}


//���캯��
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


//��������
WDYF2015Imp::~WDYF2015Imp()
{

}


//��ȡWDYF2015����ϵ����
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


//WDYF2015�ֲ��ռ�����ת����ȫ��Ψһ����ռ�����
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


//WDYF2015�ֲ��ռ䷽λת����ȫ��Ψһ����ռ䷽λ
bool WDYF2015Imp::TransLocalToGol(cn::Vec3D vLocalPos, cn::QuaternionD vLocalRotate, cn::Vec3D &vGolPos, cn::QuaternionD &vGolRotate)
{
	return false;
}


//WDYF2015ȫ��Ψһ����ռ�����ת�����ֲ��ռ�����
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


//WDYF2015ȫ��Ψһ����ռ䷽λת�����ֲ��ռ䷽λ
bool WDYF2015Imp::TransGolToLocal(cn::Vec3D vGolPos, cn::QuaternionD vGolRotate, cn::Vec3D &vLocalPos, cn::QuaternionD &vLocalRotate)
{
	return false;
}


//���ݾ�γ�Ⱥͺ��θ߶Ȼ�ȡ����ռ��е�λ��
//dLon,dLat����γ�ȣ��ԽǶ�Ϊ��λ(180��,...)
//dHeight�����θ߶�
cn::Vec3D WDYF2015Imp::GetEarthPos(double dLon, double dLat, double dHeight)
{
	dLon =dLon/180.0*_PI; dLat =dLat/180.0*_PI; dHeight =m_dEarthRadius+dHeight;
	cn::Vec3D vPos(-dHeight*::cos(dLat)*::sin(dLon), dHeight*::sin(dLat), dHeight*::cos(dLat)*::cos(dLon));
	return vPos;
}







}