


namespace cn
{




//������õ����ݶ����ϵ�ǿ���ü���
ULONG BaseObj::GetRefCount() const
{
	return m_lpAssocKernel ? m_lpAssocKernel->m_uRefCount : 0;
}


//������õ����ݶ����ϵ������ü���
ULONG BaseObj::GetWRefCount() const
{
	return m_lpAssocKernel ? m_lpAssocKernel->m_uWRefCount : 0;
}





































}