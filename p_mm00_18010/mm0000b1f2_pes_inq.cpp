/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-22 10:19:48
Description: L3L4材料对账信息查询
**************************************************/

//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// L3L4材料对账信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
BM2_FUNCTION_IMPORT
void f_epex_call_cgi_svc(CDbConnection * conn, const CString& system_code, const CString& svc_name, EIClass * blks_in, EIClass * blks_out, int timeout);

BM2F_ENTERACE(mm0000b1f2_pes_inq)


int f_mm0000b1f2_pes_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_factory_div("");
	CString cs_mat_kind("");

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_factory_div		= bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		cs_mat_kind			= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();

		/* 打印传入参数 */
		Log::Trace("", __FUNCTION__, "传入参数FACTORY_DIV		= [{0}]", cs_factory_div);
		Log::Trace("", __FUNCTION__, "传入参数MAT_KIND			= [{0}]", cs_mat_kind);

		/* 检查输入参数合法性 */
		if (cs_mat_kind.Trim() == "")
		{
			strcpy(s.msg, "物料种类不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 设置返回块PES的数据为PES材料信息 */
		bcls_ret->Tables.SetTableName(0, "PES");

		/* 获取PES的物料信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM" + cs_mat_kind + "01 "
						 "  WHERE MAT_KIND = @cs_mat_kind ";
				if (cs_factory_div.Trim() != "")
				{
					sqlstr_temp = " AND FACTORY_DIV = @cs_factory_div";
				}
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("cs_mat_kind", cs_mat_kind);
		if (cs_factory_div.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_factory_div", cs_factory_div);
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["PES"]);
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


