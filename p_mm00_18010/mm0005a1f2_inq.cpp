/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料路径跟踪查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料路径跟踪查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm0005a1f2_inq) 

int f_mm0005a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
	
	/* 实体类定义 */ 
	CModel tmm0005("TMM0005");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0005["MAT_NO"]			= bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();			//材料号
		record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	
		current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];	

		Log::Trace("",__FUNCTION__,"tmm0005.MAT_NO			= [{0}]",(const char*)tmm0005["MAT_NO"].ToString());
		Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);

		/* 检查输入参数合法性 */
		if(tmm0005["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	 		

		/* 根据材料号,得到材料跟踪号 MAT_TRACK_NO */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT MAT_TRACK_NO "
					     "  FROM TMM0005 "
					     " WHERE MAT_NO = @tmm0005.MAT_NO ";	
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0005.MAT_NO", tmm0005["MAT_NO"].ToString());
		Log::Trace("",__FUNCTION__,"MAT_TRACK_NO sqlstr	= [{0}]",(const char*)sqlstr);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())	
		{
			tmm0005["MAT_TRACK_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		Log::Trace("",__FUNCTION__,"tmm0005.MAT_TRACK_NO	= [{0}]",(const char*)tmm0005["MAT_TRACK_NO"].ToString());

		/* 查询物料路径跟踪表信息 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count = "SELECT COUNT(1) "
							   "  FROM TMM0005 "
							   " WHERE MAT_TRACK_NO = @tmm0005.MAT_TRACK_NO ";	
				sqlstr = "SELECT * "
						 "  FROM TMM0005 "
						 " WHERE MAT_TRACK_NO = @tmm0005.MAT_TRACK_NO "
						 " ORDER BY FAMILY_CODE ASC,PASS_BACKLOG_SEQ_NO ASC ";	
		}
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0005.MAT_TRACK_NO",tmm0005["MAT_TRACK_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if(start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
		cmd_inq.Close();

		bcls_ret->Tables.Add("PAGEINFO");
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");					
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
		
