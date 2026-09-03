/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      KE2117
Version:     1.0
Date:        2023-12-08 15:54:42
Description: 物料跟踪事件字段查询
**************************************************/

//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件字段查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件




BM2F_ENTERACE(mm0097code_inq)


int f_mm0097code_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString vcolumn_name = "";//字段名
	CString vmat_kind = "";//物料种类

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		vcolumn_name = bcls_rec->Tables[0].Rows[0]["COLUMN_NAME"].ToString().Trim();
		vmat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "EVENT_ID			= [{0}]", vcolumn_name);
		Log::Trace("", __FUNCTION__, "MAT_KIND			= [{0}]", vmat_kind);

		/* 检查输入参数合法性 */
		if (vmat_kind.Trim() == "")
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
			sqlstr = " SELECT  A.COLUMN_NAME AS ITEM_ENAME, "
					"        B.COMMENTS AS ITEM_CNAME," 
				   "        CASE A.DATA_TYPE WHEN 'VARCHAR2' THEN 'C' ELSE 'N' END AS ITEM_TYPE," 
				   "        A.DATA_LENGTH AS ITEM_LEN" 
				   "  FROM USER_TAB_COLUMNS A, USER_COL_COMMENTS B" 
				   " WHERE A.TABLE_NAME = B.TABLE_NAME" 
				   "   AND A.COLUMN_NAME = B.COLUMN_NAME" 
				   "   AND A.COLUMN_NAME LIKE '%" + vcolumn_name.Trim() + "%'" 
				   "   AND A.TABLE_NAME = 'TMM" + vmat_kind + "96' ORDER BY A.COLUMN_ID";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
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


