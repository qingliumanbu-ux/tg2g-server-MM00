/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-04-24
Description: 获取班次班组（登陆画面选择）
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 获取班次班组（登陆画面选择）
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2_FUNCTION_EXPORT

int f_mm0017(CString strUserId, CString& strShiftNo, CString& strShiftGroup, CString& strShiftDay, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = "";
	CString v_userid = "";

	//CTSI0030 tsi0030(conn);

	try
	{
		Log::Trace("", __FUNCTION__, "传入参数：USER_ID = [{0}]", strUserId);

		if (strUserId.Trim() == "")
		{
			strcpy(s.msg, "传入的USER_ID不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//tsi0030.USER_ID = strUserId.Trim();
		//if (!tsi0030.Query("USER_ID"))
		//{
		//	sprintf(s.msg, "用户[%s]对应的班次班组不存在", (const char*)strUserId);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//tsi0030.TrimOrBlank();

		//strShiftNo = tsi0030.SHIFT_NO;
		//strShiftGroup = tsi0030.SHIFT_GROUP;
		//strShiftDay = tsi0030.OPERATE_TIME;

		Log::Trace("", __FUNCTION__, "返回值：strShiftNo = [{0}]", strShiftNo);
		Log::Trace("", __FUNCTION__, "返回值：strShiftGroup = [{0}]", strShiftGroup);
		Log::Trace("", __FUNCTION__, "返回值：strShiftDay = [{0}]", strShiftDay);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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

