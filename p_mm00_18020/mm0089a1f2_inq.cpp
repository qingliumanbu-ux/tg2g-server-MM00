/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-12-12
Description: 材料标签信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料标签信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(mm0089a1f2_inq) 

int f_mm0089a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	cs_prod_time_from("");	
	CString	cs_prod_time_to("");	
	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    
	/* 实体类定义 */ 

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
		cs_prod_time_from		= bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();	//起始时间
		cs_prod_time_to			= bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();	//终止时间
		record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];	
		
		Log::Trace("",__FUNCTION__,"cs_prod_time_from		= [{0}]",(const char*)cs_prod_time_from);
		Log::Trace("",__FUNCTION__,"cs_prod_time_to			= [{0}]",(const char*)cs_prod_time_to);
		Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);
 
			
		/* 设置开始时刻和结束时刻 */
		if(cs_prod_time_from.Trim() != "")
		{
			cs_prod_time_from += "000000";
		}
		if(cs_prod_time_to.Trim() != "")
		{
			cs_prod_time_to += "235959";
		}

		/* 查询材料信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count = " SELECT COUNT(1) "
							   "   FROM TMM0089 "
					           "  WHERE 1 = 1 ";
				sqlstr = " SELECT * "
					    "   FROM TMM0089 "
					    "  WHERE 1 = 1 ";
				if (cs_prod_time_from.Trim() != "")
				{
					sqlstr_temp += " AND PROD_TIME >=	@cs_prod_time_from ";
				}
				if (cs_prod_time_to.Trim() != "")
				{
					sqlstr_temp += " AND PROD_TIME <=	@cs_prod_time_to ";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr = sqlstr + sqlstr_temp + " ORDER BY IN_MAT_NO ASC";
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (cs_prod_time_from.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_from", cs_prod_time_from);
		}
		if (cs_prod_time_to.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_to", cs_prod_time_to);
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
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0]	= cd_count.ToInt32();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
		