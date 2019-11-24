


namespace cn
{




//获得引用的数据对象上的强引用计数
ULONG BaseObj::GetRefCount() const
{
	return m_lpAssocKernel ? m_lpAssocKernel->m_uRefCount : 0;
}


//获得引用的数据对象上的弱引用计数
ULONG BaseObj::GetWRefCount() const
{
	return m_lpAssocKernel ? m_lpAssocKernel->m_uWRefCount : 0;
}





































}