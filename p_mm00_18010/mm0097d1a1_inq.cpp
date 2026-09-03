/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-04-28 15:54:42
Description: 物料跟踪事件历史参数查询
**************************************************/

//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件历史参数查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  



BM2F_ENTERACE(mm0097d1a1_inq)


int f_mm0097d1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel hmm0099("HMM0099");
	CModel hmm009a("HMM009A");
	CModel hmm009b("HMM009B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		hmm0099["EVENT_ID"]		= bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim();
		hmm0099["MAT_KIND"]		= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		hmm0099["EVENT_LINE_TYPE"] = bcls_rec->Tables[0].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "hmm0099.EVENT_ID			= [{0}]", (const char*)hmm0099["EVENT_ID"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0099.MAT_KIND			= [{0}]", (const char*)hmm0099["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "hmm0099.EVENT_LINE_TYPE	= [{0}]", (const char*)hmm0099["EVENT_LINE_TYPE"].ToString());

		/* 检查输入参数合法性 */
		if (hmm0099["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg, "事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询抛帐参数信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM HMM0099 "
						 "  WHERE EVENT_ID = @hmm0099.EVENT_ID "
						 "	  AND MAT_KIND = @hmm0099.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @hmm0099.EVENT_LINE_TYPE "
						 "	  AND ITEM_PARA = 'Y' "	//抛帐参数Y-必须抛帐
						 "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("hmm0099.EVENT_ID", hmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("hmm0099.MAT_KIND", hmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("hmm0099.EVENT_LINE_TYPE", hmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, 100);
		cmd_inq.Close();

		/* 查询参数信息 */
		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "ITEM_TYPE_0");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "ITEM_TYPE_1");
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM HMM0099 "
						 "  WHERE EVENT_ID = @hmm0099.EVENT_ID "
						 "	  AND MAT_KIND = @hmm0099.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @hmm0099.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("hmm0099.EVENT_ID", hmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("hmm0099.MAT_KIND", hmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("hmm0099.EVENT_LINE_TYPE", hmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1], 0, 100);
		cmd_inq.Close();

		/* 查询事件电文配置表 */
		bcls_ret->Tables.Add();
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM HMM009A "
						 "  WHERE EVENT_ID = @hmm0099.EVENT_ID "
						 "	  AND MAT_KIND = @hmm0099.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @hmm0099.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,TC_KEYVALUE ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("hmm0099.EVENT_ID", hmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("hmm0099.MAT_KIND", hmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("hmm0099.EVENT_LINE_TYPE", hmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2], 0, 100);
		cmd_inq.Close();

		/* 查询事件抛帐字段配置表 */
		bcls_ret->Tables.Add();
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM HMM009B "
						 "  WHERE EVENT_ID = @hmm0099.EVENT_ID "
						 "	  AND MAT_KIND = @hmm0099.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @hmm0099.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,THROW_KIND ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("hmm0099.EVENT_ID", hmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("hmm0099.MAT_KIND", hmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("hmm0099.EVENT_LINE_TYPE", hmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3], 0, 100);
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


