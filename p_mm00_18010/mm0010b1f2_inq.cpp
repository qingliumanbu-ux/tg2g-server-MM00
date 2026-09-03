/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    李婧昊
Version:    1.0
Date:       2018-07-12
Description: 外购料进料计划管理_计划查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料进料计划管理_计划查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明
BM2F_ENTERACE(mm0010b1f2_inq)

int f_mm0010b1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0;      /* 需查询的页号,从0开始计数 */
	int	start_row = 0;           /* 将要压入outBlock的起始行 */

	/* 实体类定义 */
	CModel tmm0011("TMM0011");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0011["RAW_ORIGIN"]		= bcls_rec->Tables[0].Rows[0]["RAW_ORIGIN"].ToString().Trim();		//原料来源
		tmm0011["DEMAND_PLAN_NO"]	= bcls_rec->Tables[0].Rows[0]["DEMAND_PLAN_NO"].ToString().Trim();	//进料需求计划号
		tmm0011["PLAN_MAKER"]		= bcls_rec->Tables[0].Rows[0]["PLAN_MAKER"].ToString().Trim();	    //计划责任者 计划员
		tmm0011["PLAN_STATUS"]		= bcls_rec->Tables[0].Rows[0]["PLAN_STATUS"].ToString().Trim();		//计划状态
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		Log::Trace("", __FUNCTION__, "传入参数 tmm0011.RAW_ORIGIN		= [{0}]", tmm0011["RAW_ORIGIN"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数 tmm0011.DEMAND_PLAN_NO	= [{0}]", tmm0011["DEMAND_PLAN_NO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数 tmm0011.PLAN_MAKER		= [{0}]", tmm0011["PLAN_MAKER"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数 tmm0011.PLAN_STATUS		= [{0}]", tmm0011["PLAN_STATUS"].ToString());		
		Log::Trace("", __FUNCTION__, "传入参数 record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "传入参数 current_page_no			= [{0}]", current_page_no);

		/* 查询计划信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:	
				sqlstr_count = " SELECT COUNT(1) "
							   "   FROM TMM0011 "
							   "  WHERE 1 = 1 ";
				sqlstr = " SELECT * "
						 "   FROM TMM0011 "
						 "  WHERE 1 = 1 ";									
				if (tmm0011["RAW_ORIGIN"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND RAW_ORIGIN = @tmm0011.RAW_ORIGIN ";
				}
				if (tmm0011["DEMAND_PLAN_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND DEMAND_PLAN_NO LIKE @tmm0011.DEMAND_PLAN_NO ||'%' ";
				}
				if (tmm0011["PLAN_MAKER"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PLAN_MAKER LIKE @tmm0011.PLAN_MAKER ||'%' ";
				}
				if (tmm0011["PLAN_STATUS"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PLAN_STATUS = @tmm0011.PLAN_STATUS ";
				}			
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr = sqlstr + sqlstr_temp + " ORDER BY DEMAND_PLAN_NO ASC";
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (tmm0011["RAW_ORIGIN"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0011.RAW_ORIGIN", tmm0011["RAW_ORIGIN"].ToString());
		}
		if (tmm0011["DEMAND_PLAN_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0011.DEMAND_PLAN_NO", tmm0011["DEMAND_PLAN_NO"].ToString());
		}
		if (tmm0011["PLAN_MAKER"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0011.PLAN_MAKER", tmm0011["PLAN_MAKER"].ToString());
		}
		if (tmm0011["PLAN_STATUS"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0011.PLAN_STATUS", tmm0011["PLAN_STATUS"].ToString());
		}		
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "cd_count			= [{0}]", cd_count);
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "current_page_no		= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "record_count_per_page= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "start_row			= [{0}]", start_row);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
