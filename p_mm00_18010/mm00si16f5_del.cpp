/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-03-27 16:49:31
Description: 物料通用_机组工序对照基本信息删除
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 物料通用_机组工序对照基本信息删除
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm00si16f5_del)


int f_mm00si16f5_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmm00si16("TMM00SI16");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 获取输入参数 */
			tmm00si16.Reset();
			tmm00si16.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm00si16.TrimOrBlank();

			/* 打印输入参数 */
			Log::Trace("", __FUNCTION__, "删除tmm00si16.UNIT_CODE		= [{0}]", tmm00si16["UNIT_CODE"].ToString());
	
			/* 检查输入参数合法性 */
			if (tmm00si16["UNIT_CODE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "机组代码不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 检查机组是否存在 */
			if (tmm00si16.Query("UNIT_CODE") == false)
			{
				strcpy(s.msg, "机组信息不存在，请重新查询！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmm00si16.TrimOrBlank();
			if (tmm00si16["MAT_LINE_TYPE"].ToString().Trim() == "CR")
			{
				strcpy(s.msg, "冷轧产线的机组，请在MMCRSI16画面维护！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 删除信息 */
			tmm00si16.Delete();
		}
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


