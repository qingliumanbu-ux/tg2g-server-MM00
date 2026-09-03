/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-07-24
Description: 获取数据库流水号
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 获取数据库流水号
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2_FUNCTION_EXPORT

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn)
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

		Log::Trace("",__FUNCTION__,"传入参数 SeqName	= [{0}]",SeqName);
		Log::Trace("",__FUNCTION__,"传入参数 SeqLen		= [{0}]",SeqLen);
		
		/* 检查输入参数合法性 */
		if (SeqName.Trim() == "")
		{
			strcpy(s.msg, "传入的SEQUENCE名不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (SeqLen <= 0)
		{
			strcpy(s.msg, "获取流水号长度必须大于0!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获取履历流水号 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				sprintf(sql, "SELECT LPAD(TO_CHAR(%s.NEXTVAL), %d, '0') FROM  sysibm.sysdummy1", (const char*)SeqName.Trim(), SeqLen.ToInt32());
				break;

			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sprintf(sql, "SELECT LPAD(TO_CHAR(%s.NEXTVAL), %d, '0') FROM DUAL", (const char*)SeqName.Trim(), SeqLen.ToInt32());
				break;
		}
		Log::Trace("",__FUNCTION__,"获取履历流水号 sql		= [{0}]",sql);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();		
		if(cmd_inq.Read())
		{
			SeqNo = cmd_inq.GetString(1).Trim();
		}
		else
		{
			sprintf(s.msg, "获取流水号【%s】失败，请检查数据库配置!", (const char*)SeqName.Trim());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "SeqNo = [{0}]", SeqNo);
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

