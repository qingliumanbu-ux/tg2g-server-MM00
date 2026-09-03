/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪事件管理参数查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理参数查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

 

//外部函数声明

BM2F_ENTERACE(mm0097a1a1_inq)   

int f_mm0097a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmm0099("TMM0099");
	CModel tmm009a("TMM009A");
	CModel tmm009b("TMM009B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0099["EVENT_ID"]		= bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim();
		tmm0099["MAT_KIND"]		= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		tmm0099["EVENT_LINE_TYPE"] = bcls_rec->Tables[0].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();
	
		Log::Trace("",__FUNCTION__,"tmm0099.EVENT_ID		= [{0}]",(const char*)tmm0099["EVENT_ID"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0099.MAT_KIND		= [{0}]",(const char*)tmm0099["MAT_KIND"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0099.EVENT_LINE_TYPE	= [{0}]",(const char*)tmm0099["EVENT_LINE_TYPE"].ToString());
		
		/* 检查输入参数合法性 */
		if(tmm0099["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg,"事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  

		/* 查询抛帐参数信息 */
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= " SELECT * "
						  "   FROM TMM0099 "
						  "  WHERE EVENT_ID			= @tmm0099.EVENT_ID "
						  "	   AND MAT_KIND			= @tmm0099.MAT_KIND "
						  "	   AND EVENT_LINE_TYPE	= @tmm0099.EVENT_LINE_TYPE "
						  "	   AND ITEM_PARA		= 'Y' "	//抛帐参数Y-必须抛帐
						  "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0099.EVENT_ID",tmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0099.MAT_KIND",tmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0099.EVENT_LINE_TYPE",tmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],0,100);
		cmd_inq.Close();

		/* 查询参数信息 */
		bcls_ret->Tables.Add(); 
		bcls_ret->Tables[1].Columns.Add(DT_STRING,"ITEM_TYPE_0");
		bcls_ret->Tables[1].Columns.Add(DT_STRING,"ITEM_TYPE_1");
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= " SELECT * "
						  "   FROM TMM0099 "
						  "  WHERE EVENT_ID			= @tmm0099.EVENT_ID "
						  "	   AND MAT_KIND			= @tmm0099.MAT_KIND "
						  "	   AND EVENT_LINE_TYPE	= @tmm0099.EVENT_LINE_TYPE "
						  "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0099.EVENT_ID",tmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0099.MAT_KIND",tmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0099.EVENT_LINE_TYPE",tmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1],0,100);
		cmd_inq.Close();

		/* 查询事件电文配置表 */
		bcls_ret->Tables.Add(); 
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= " SELECT * "
						  "   FROM TMM009A "
						  "  WHERE EVENT_ID			= @tmm0099.EVENT_ID "
						  "	   AND MAT_KIND			= @tmm0099.MAT_KIND "
						  "	   AND EVENT_LINE_TYPE	= @tmm0099.EVENT_LINE_TYPE "
						  "  ORDER BY EVENT_ID ASC,TC_KEYVALUE ASC";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0099.EVENT_ID",tmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0099.MAT_KIND",tmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0099.EVENT_LINE_TYPE",tmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2],0,100);
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
						 "   FROM TMM009B "
						 "  WHERE EVENT_ID			= @tmm0099.EVENT_ID "
						 "	   AND MAT_KIND			= @tmm0099.MAT_KIND "
						 "	   AND EVENT_LINE_TYPE	= @tmm0099.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,THROW_KIND ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0099.EVENT_ID", tmm0099["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0099.MAT_KIND", tmm0099["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0099.EVENT_LINE_TYPE", tmm0099["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3], 0, 100);
		cmd_inq.Close();


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



