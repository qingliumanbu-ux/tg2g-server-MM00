/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-29 11:04:25
Description: 物料通用跨系统对账配置信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 物料通用跨系统对账配置信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm00si02f2_inq)


int f_mm00si02f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmm00si02("TMM00SI02");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm00si02.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmm00si02.TrimOrBlank();

		/* 打印传入参数 */
		Log::Trace("", __FUNCTION__, "tmm00si02.SYS_CODE			= [{0}]", tmm00si02["SYS_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "tmm00si02.MAT_KIND			= [{0}]", tmm00si02["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "tmm00si02.FACTORY_DIV			= [{0}]", tmm00si02["FACTORY_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "tmm00si02.ITEM_ENAME			= [{0}]", tmm00si02["ITEM_ENAME"].ToString());

		/* 查询配置信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
					"	 FROM TMM00SI02 "
					"	WHERE 1=1 ";
				if (tmm00si02["SYS_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND SYS_CODE = @tmm00si02.SYS_CODE ";
				}
				if (tmm00si02["MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_KIND = @tmm00si02.MAT_KIND ";
				}
				if (tmm00si02["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmm00si02.FACTORY_DIV ";
				}
				if (tmm00si02["ITEM_ENAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND ITEM_ENAME = @tmm00si02.ITEM_ENAME ";
				}
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_temp = [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (tmm00si02["SYS_CODE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si02.SYS_CODE", tmm00si02["SYS_CODE"].ToString());
		}
		if (tmm00si02["MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si02.MAT_KIND", tmm00si02["MAT_KIND"].ToString());
		}
		if (tmm00si02["FACTORY_DIV"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si02.FACTORY_DIV", tmm00si02["FACTORY_DIV"].ToString());
		}
		if (tmm00si02["ITEM_ENAME"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si02.ITEM_ENAME", tmm00si02["ITEM_ENAME"].ToString());
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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


