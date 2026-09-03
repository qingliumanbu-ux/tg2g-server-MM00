/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    李婧昊
Version:    1.0
Date:       2018-07-12
Description: 外购料进料计划管理_材料查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
///  外购料进料计划管理_材料查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明
BM2F_ENTERACE(mm0010b1a1_inq)

int f_mm0010b1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	  //系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmm0010("TMM0010");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0010["DEMAND_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["DEMAND_PLAN_NO"].ToString().Trim();  //进料需求计划号

		Log::Trace("", __FUNCTION__, "传入参数 tmm0010.DEMAND_PLAN_NO		= [{0}]", tmm0010["DEMAND_PLAN_NO"].ToString());

		/* 检查输入参数合法性 */
		if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "进料计划号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询材料信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
					     "   FROM TMM0010 "
					     "  WHERE DEMAND_PLAN_NO = @tmm0010.DEMAND_PLAN_NO "
						 "  ORDER BY MAT_NO ASC ";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr			= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0010.DEMAND_PLAN_NO", tmm0010["DEMAND_PLAN_NO"].ToString());    
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}



