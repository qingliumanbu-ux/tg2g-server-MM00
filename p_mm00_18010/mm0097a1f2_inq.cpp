/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪事件管理事件查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理事件查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(mm0097a1f2_inq) 

int f_mm0097a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString  cs_item_name("");      //字段名
	CDecimal cd_count	= 0;
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */

	/* 实体类定义 */ 
	CModel tmm0097("TMM0097");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		/* 获取输入参数 */
		tmm0097.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmm0097.TrimOrBlank();

		cs_item_name = bcls_rec->Tables[0].Rows[0]["ITEM_NAME"].ToString().Trim();          

		/* 打印输入参数 */
		Log::Trace("",__FUNCTION__,"tmm0097.MAT_KIND			= [{0}]",tmm0097["MAT_KIND"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0097.EVENT_ID			= [{0}]",tmm0097["EVENT_ID"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0097.EVENT_SUB_SYSTEM	= [{0}]",tmm0097["EVENT_SUB_SYSTEM"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0097.EVENT_USE_FLAG		= [{0}]",tmm0097["EVENT_USE_FLAG"].ToString());
		Log::Trace("",__FUNCTION__,"tmm0097.EVENT_NAME		    = [{0}]",tmm0097["EVENT_NAME"].ToString());
		Log::Trace("",__FUNCTION__,"cs_item_name		        = [{0}]",cs_item_name);

		/* 查询信息 */		
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMM0097 "
						 " WHERE 1 = 1 ";			
				if(tmm0097["EVENT_ID"].ToString().Trim() != "")
				{
					sqlstr	+= " AND EVENT_ID LIKE @tmm0097.EVENT_ID ||'%' ";
				}
				if(tmm0097["MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr	+= " AND MAT_KIND = @tmm0097.MAT_KIND "; 
				}
				if(tmm0097["EVENT_LINE_TYPE"].ToString().Trim() != "")
				{
					sqlstr	+= " AND EVENT_LINE_TYPE = @tmm0097.EVENT_LINE_TYPE "; 
				}
				if(tmm0097["EVENT_SUB_SYSTEM"].ToString().Trim() != "")
				{
					sqlstr	+= " AND EVENT_SUB_SYSTEM = @tmm0097.EVENT_SUB_SYSTEM "; 
				}
				if(tmm0097["EVENT_USE_FLAG"].ToString().Trim() != "")
				{
					sqlstr	+= " AND EVENT_USE_FLAG = @tmm0097.EVENT_USE_FLAG "; 
				}
				if (tmm0097["EVENT_NAME"].ToString().Trim() != "") 
				{
					sqlstr += " AND (EVENT_NAME LIKE '%'|| @tmm0097.EVENT_NAME ||'%'  "
							  "   OR EVENT_DESC LIKE '%'|| @tmm0097.EVENT_NAME ||'%')";
				}
				if (cs_item_name.Trim() != "")
				{
					sqlstr += " AND TMM0097.EVENT_ID IN (SELECT TMM0099.EVENT_ID "
				        	  "						       FROM TMM0099          "
							  "						      WHERE (TMM0099.ITEM_ENAME = @cs_item_name  OR TMM0099.ITEM_CNAME = @cs_item_name) "
							  "							    AND TMM0099.EVENT_ID = TMM0097.EVENT_ID "
							  "                             AND TMM0099.MAT_KIND = TMM0097.MAT_KIND "
						      "                             AND TMM0099.EVENT_LINE_TYPE = TMM0097.EVENT_LINE_TYPE)";
				}
				sqlstr	+= " ORDER BY EVENT_ID,MAT_KIND,EVENT_SUB_SYSTEM"; 
				break;
		}   
		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("",__FUNCTION__,"sqlstr	= [{0}]",(const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if(tmm0097["EVENT_ID"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.EVENT_ID",tmm0097["EVENT_ID"].ToString()); 
		}
		if(tmm0097["MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.MAT_KIND",tmm0097["MAT_KIND"].ToString()); 
		}
		if(tmm0097["EVENT_LINE_TYPE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.EVENT_LINE_TYPE",tmm0097["EVENT_LINE_TYPE"].ToString()); 
		}
		if(tmm0097["EVENT_SUB_SYSTEM"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.EVENT_SUB_SYSTEM",tmm0097["EVENT_SUB_SYSTEM"].ToString()); 
		}
		if(tmm0097["EVENT_USE_FLAG"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.EVENT_USE_FLAG",tmm0097["EVENT_USE_FLAG"].ToString()); 
		}
		if (tmm0097["EVENT_NAME"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0097.EVENT_NAME", tmm0097["EVENT_NAME"].ToString());
		}
		if (cs_item_name.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_item_name", cs_item_name);
		}
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],0,500);
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
