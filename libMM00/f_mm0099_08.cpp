/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-09-18 15:28:56
Description: 物料跟踪记录仓库履历
**************************************************/

#include "stdafx.h"
#if defined _SYS_MES || defined _SYS_PES     //MES或PES
#if defined(_WMS_DEPENDENT)   //非独立仓库
BM2_FUNCTION_IMPORT
int f_wm00_mat_info_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);	//仓库履历
#endif
#endif


BM2_FUNCTION_EXPORT
int f_mm0099_08(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	try
	{
#if defined _SYS_MES || defined _SYS_PES     //MES或PES
#if defined(_WMS_DEPENDENT)   //非独立仓库
		if (bcls_rec->Tables["EVENT_DATA"].Rows[0]["EVENT_PROC_WAY_3"].ToString() == "3" &&
			bcls_rec->Tables["OLDMM_TABLE"].Rows.get_Count() > 0 && bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count() > 0)
		{
			doFlag = f_wm00_mat_info_update(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#endif
#endif

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


