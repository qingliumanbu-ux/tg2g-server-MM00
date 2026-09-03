/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-03-27 16:46:52
Description: 物料通用_机组工序对照基本信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 物料通用_机组工序对照基本信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm00si16f2_inq)


int f_mm00si16f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

		/* 获取输入参数 */
		tmm00si16.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmm00si16.TrimOrBlank();

		/* 打印传入参数 */
		Log::Trace("", __FUNCTION__, "tmm00si16.UNIT_CODE			= [{0}]", tmm00si16["UNIT_CODE"].ToString());

		/* 查询配置信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "	 FROM TMM00SI16 "
						 "	WHERE 1 = 1 ";
				if (tmm00si16["UNIT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND UNIT_CODE = @tmm00si16.UNIT_CODE ";
				}
				if (tmm00si16["MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_KIND = @tmm00si16.MAT_KIND ";
				}
				if (tmm00si16["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_LINE_TYPE = @tmm00si16.MAT_LINE_TYPE ";
				}
				if (tmm00si16["MAT_SHAPE_FLAG"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_SHAPE_FLAG = @tmm00si16.MAT_SHAPE_FLAG ";
				}
				if (tmm00si16["WHOLE_BACKLOG_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND WHOLE_BACKLOG_CODE = @tmm00si16.WHOLE_BACKLOG_CODE ";
				}
				if (tmm00si16["WHOLE_BACKLOG_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND WHOLE_BACKLOG_NAME = @tmm00si16.WHOLE_BACKLOG_NAME ";
				}
				if (tmm00si16["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmm00si16.FACTORY_DIV ";
				}
				if (tmm00si16["IN_MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND IN_MAT_KIND = @tmm00si16.IN_MAT_KIND ";
				}
				if (tmm00si16["PROD_TABLE_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PROD_TABLE_NAME = @tmm00si16.PROD_TABLE_NAME ";
				}
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_temp = [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (tmm00si16["UNIT_CODE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.UNIT_CODE", tmm00si16["UNIT_CODE"].ToString());
		}
		if (tmm00si16["MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.MAT_KIND", tmm00si16["MAT_KIND"].ToString());
		}
		if (tmm00si16["MAT_LINE_TYPE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.MAT_LINE_TYPE", tmm00si16["MAT_LINE_TYPE"].ToString());
		}
		if (tmm00si16["MAT_SHAPE_FLAG"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.MAT_SHAPE_FLAG", tmm00si16["MAT_SHAPE_FLAG"].ToString());
		}
		if (tmm00si16["WHOLE_BACKLOG_CODE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.WHOLE_BACKLOG_CODE", tmm00si16["WHOLE_BACKLOG_CODE"].ToString());
		}
		if (tmm00si16["WHOLE_BACKLOG_NAME"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.WHOLE_BACKLOG_NAME", tmm00si16["WHOLE_BACKLOG_NAME"].ToString());
		}
		if (tmm00si16["FACTORY_DIV"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.FACTORY_DIV", tmm00si16["FACTORY_DIV"].ToString());
		}
		if (tmm00si16["IN_MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.IN_MAT_KIND", tmm00si16["IN_MAT_KIND"].ToString());
		}
		if (tmm00si16["PROD_TABLE_NAME"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm00si16.PROD_TABLE_NAME", tmm00si16["PROD_TABLE_NAME"].ToString());
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


