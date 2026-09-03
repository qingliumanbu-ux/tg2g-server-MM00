/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-04-28 15:48:54
Description: 物料跟踪事件历史信息查询
**************************************************/

//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件历史信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm0097d1f2_inq)


int f_mm0097d1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString  cs_item_name("");      //字段名

	/* 实体类定义 */
	CModel hmm0097("HMM0097");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		hmm0097.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		hmm0097.TrimOrBlank();

		cs_item_name = bcls_rec->Tables[0].Rows[0]["ITEM_NAME"].ToString().Trim();

		/* 打印输入参数 */
		Log::Trace("", __FUNCTION__, "hmm0097.EVENT_ID			= [{0}]", hmm0097["EVENT_ID"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0097.MAT_KIND			= [{0}]", hmm0097["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0097.EVENT_LINE_TYPE	= [{0}]", hmm0097["EVENT_LINE_TYPE"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0097.EVENT_SUB_SYSTEM	= [{0}]", hmm0097["EVENT_SUB_SYSTEM"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0097.EVENT_NAME		= [{0}]", hmm0097["EVENT_NAME"].ToString());
		Log::Trace("", __FUNCTION__, "cs_item_name		        = [{0}]", cs_item_name);

		/* 查询信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM HMM0097 "
					" WHERE 1 = 1 ";
				if (hmm0097["EVENT_ID"].ToString().Trim() != "")
				{
					sqlstr += " AND EVENT_ID LIKE @hmm0097.EVENT_ID ||'%' ";
				}
				if (hmm0097["MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr += " AND MAT_KIND = @hmm0097.MAT_KIND ";
				}
				if (hmm0097["EVENT_LINE_TYPE"].ToString().Trim() != "")
				{
					sqlstr += " AND EVENT_LINE_TYPE = @hmm0097.EVENT_LINE_TYPE ";
				}
				if (hmm0097["EVENT_SUB_SYSTEM"].ToString().Trim() != "")
				{
					sqlstr += " AND EVENT_SUB_SYSTEM = @hmm0097.EVENT_SUB_SYSTEM ";
				}
				if (hmm0097["EVENT_NAME"].ToString().Trim() != "")
				{
					sqlstr += " AND (EVENT_NAME LIKE '%'|| @hmm0097.EVENT_NAME ||'%'  "
							  "  OR EVENT_DESC LIKE '%'|| @hmm0097.EVENT_NAME ||'%') ";
				}
				if (cs_item_name.Trim() != "")
				{
					sqlstr += " AND HMM0097.EVENT_ID IN (SELECT HMM0099.EVENT_ID "
							  "						       FROM HMM0099 "
							  "						      WHERE (HMM0099.ITEM_ENAME = @cs_item_name  OR HMM0099.ITEM_CNAME = @cs_item_name) "
							  "							    AND HMM0099.EVENT_ID = HMM0097.EVENT_ID "
							  "                             AND HMM0099.MAT_KIND = HMM0097.MAT_KIND "
							  "                             AND HMM0099.EVENT_LINE_TYPE = HMM0097.EVENT_LINE_TYPE) ";
				}
				sqlstr += " ORDER BY EVENT_ID,MAT_KIND,EVENT_SUB_SYSTEM";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (hmm0097["EVENT_ID"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("hmm0097.EVENT_ID", hmm0097["EVENT_ID"].ToString());
		}
		if (hmm0097["MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("hmm0097.MAT_KIND", hmm0097["MAT_KIND"].ToString());
		}
		if (hmm0097["EVENT_LINE_TYPE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("hmm0097.EVENT_LINE_TYPE", hmm0097["EVENT_LINE_TYPE"].ToString());
		}
		if (hmm0097["EVENT_SUB_SYSTEM"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("hmm0097.EVENT_SUB_SYSTEM", hmm0097["EVENT_SUB_SYSTEM"].ToString());
		}
		if (hmm0097["EVENT_NAME"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("hmm0097.EVENT_NAME", hmm0097["EVENT_NAME"].ToString());
		}
		if (cs_item_name.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_item_name", cs_item_name);
		}
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, 500);
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


