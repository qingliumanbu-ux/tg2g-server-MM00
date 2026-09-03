/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-07-24
Description: 按库区或厂别获取系统别(SYS_CODE)
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 按库区或厂别获取系统别(SYS_CODE)
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2_FUNCTION_EXPORT

int f_mm0016(CString StockNo, CString FactoryDiv, CString &SysCode, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义
	 
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	char sql[2001] = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		Log::Trace("",__FUNCTION__,"传入参数 StockNo	= [{0}]",StockNo);
		Log::Trace("",__FUNCTION__,"传入参数 FactoryDiv	= [{0}]",FactoryDiv);

		/* 根据库区获取系统别 */
		if (StockNo.Trim() != "")
		{
			//根据库区获取系统别
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
				sqlstr = " SELECT CODE_DESC_3_CONTENT "
						 "   FROM TEP0002 "
						 "  WHERE CODE_CLASS = 'M0BS' "
						 "    AND CODE = @StockNo ";
					break;
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("StockNo", StockNo);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				SysCode = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		/* 根据厂别获取系统别 */
		else if (FactoryDiv.Trim() != "")
		{
			//根据厂别获取系统别
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
				sqlstr = " SELECT CODE_DESC_3_CONTENT "
						 "   FROM TEP0002 "
						 "  WHERE CODE_CLASS = 'M0BS' "
						 "    AND CODE_DESC_2_CONTENT = @FactoryDiv ";
					break;
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("FactoryDiv", FactoryDiv);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				SysCode = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		else
		{
			strcpy(s.msg, "传入的库区号或厂别不能同时为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		Log::Trace("", __FUNCTION__, "SysCode = [{0}]", SysCode);
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

