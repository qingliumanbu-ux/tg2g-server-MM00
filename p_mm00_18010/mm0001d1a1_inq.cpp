/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2016-08-03
Description: 材料详细信息查询(通用)
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料详细信息查询(通用)
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
//#include "tmmsm01.h"
//#if defined(_LINE_BW)
//#include "tmmbw01.h"
//#endif
//#if defined(_LINE_HP)
//#include "tmmhp01.h"
//#endif
//#if defined(_LINE_HR) || defined(_LINE_CR)
//#include "tmmhr01.h"
//#include "tmmcr01.h"  
//#endif
//#include "tep0002.h"
//#include "tqmtqq0.h"

//外部函数声明

BM2F_ENTERACE(mm0001d1a1_inq)   

int f_mm0001d1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	cs_mat_kind("");
	CString	cs_table_ename("");
	CString	cs_mat_no("");

	CString	cs_order_no("");
	CString	cs_pono("");
	/* 实体类定义 */ 
	//CTMMSM01 tmmsm01(conn);
	//#if defined(_LINE_BW)
	//CTMMBW01 tmmbw01(conn);
	//#endif
	//#if defined(_LINE_HP)
	//CTMMHP01 tmmhp01(conn);
	//#endif
	//#if defined(_LINE_HR) || defined(_LINE_CR)
	//CTMMHR01 tmmhr01(conn);
	//CTMMCR01 tmmcr01(conn);
	//#endif
	//CTEP0002 tep0002(conn);
	//CTQMTQQ0 tqmtqq0(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		//datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//
		///* 设置返回块列 */
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_WT");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_INNER_DIA");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_LEN");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_WIDTH");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ORDER_THICK");
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"PROD_CNAME");

		///* 获取输入参数 */
		//cs_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		//cs_table_ename = bcls_rec->Tables[0].Rows[0]["TABLE_ENAME"].ToString().Trim();
		//cs_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();

		//Log::Trace("", __FUNCTION__, "cs_mat_kind,cs_table_ename	= [{0}],[{1}]", (const char*)cs_mat_kind,(const char*)cs_table_ename);
		//Log::Trace("", __FUNCTION__, "cs_mat_no		= [{0}]", (const char*)cs_mat_no);
		//
		///* 检查输入参数合法性 */
		//if (cs_mat_no.Trim() == "")
		//{
		//	strcpy(s.msg,"材料号不能为空!");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}	  

		///* 查询材料当前档*/

		//switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:
		//		sqlstr	= "SELECT *  FROM " + cs_table_ename + " WHERE MAT_NO = @cs_mat_no ";
		//		break;
		//}     
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Clear();
		//cmd_inq.Parameters.Set("cs_mat_no",cs_mat_no); 
		//cmd_inq.ExecuteReader();
		//if(cmd_inq.Read())
		//{
		//	if (cs_mat_kind.Trim() == "SM")
		//	{
		//		cmd_inq.Fetch(tmmsm01);
		//		cs_order_no = tmmsm01.ORDER_NO;
		//		cs_pono = tmmsm01.PONO;

		//		/* 设置返回块的值 */
		//		tmmsm01.MergeTo(bcls_ret->Tables[0], false);
		//	}
		//	#if defined(_LINE_BW)
		//	if (cs_mat_kind.Trim() == "BW")
		//	{
		//		cmd_inq.Fetch(tmmbw01);
		//		cs_order_no = tmmbw01.ORDER_NO;
		//		cs_pono = tmmbw01.PONO;

		//		/* 设置返回块的值 */
		//		tmmbw01.MergeTo(bcls_ret->Tables[0], false);
		//	}
		//	#endif
		//	#if defined(_LINE_HP)
		//	if (cs_mat_kind.Trim() == "HP")
		//	{
		//		cmd_inq.Fetch(tmmhp01);
		//		cs_order_no = tmmhp01.ORDER_NO;
		//		cs_pono = tmmhp01.PONO;

		//		/* 设置返回块的值 */
		//		tmmhp01.MergeTo(bcls_ret->Tables[0], false);
		//	}
		//	#endif
		//	#if defined(_LINE_HR) || defined(_LINE_CR)
		//	if (cs_mat_kind.Trim() == "HR")
		//	{
		//		cmd_inq.Fetch(tmmhr01);
		//		cs_order_no = tmmhr01.ORDER_NO;
		//		cs_pono = tmmhr01.PONO;

		//		/* 设置返回块的值 */
		//		tmmhr01.MergeTo(bcls_ret->Tables[0], false);
		//	}
		//	#endif
		//	#if defined(_LINE_CR)
		//	if (cs_mat_kind.Trim() == "CR")
		//	{
		//		cmd_inq.Fetch(tmmcr01);
		//		cs_order_no = tmmcr01.ORDER_NO;
		//		cs_pono = tmmcr01.PONO;

		//		/* 设置返回块的值 */
		//		tmmcr01.MergeTo(bcls_ret->Tables[0], false);
		//	}
		//	#endif
		//}
		//else
		//{
		//	strcpy(s.msg,"材料号不存在!");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//cmd_inq.Close();

		//#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		///* 查询合同信息 */
	 //   switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:
		//		sqlstr	= "SELECT ORDER_WT, "
		//				  "		  ORDER_INNER_DIA," 
		//				  "		  ORDER_LEN," 
		//				  "		  ORDER_WIDTH," 
		//				  "		  ORDER_THICK," 
		//				  "		  PROD_CNAME " 
		//				  "  FROM TOM01 "
		//				  " WHERE ORDER_NO = @cs_order_no ";
		//		break;
		//}     
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Clear();
		//cmd_inq.Parameters.Set("cs_order_no",cs_order_no);
		//cmd_inq.ExecuteReader(); 
		//if(cmd_inq.Read())
		//{ 
		//	bcls_ret->Tables[0].Rows[0]["ORDER_WT"]			= cmd_inq.GetDecimal(1);
		//	bcls_ret->Tables[0].Rows[0]["ORDER_INNER_DIA"]	= cmd_inq.GetDecimal(2);
		//	bcls_ret->Tables[0].Rows[0]["ORDER_LEN"]		= cmd_inq.GetDecimal(3);
		//	bcls_ret->Tables[0].Rows[0]["ORDER_WIDTH"]		= cmd_inq.GetDecimal(4);
		//	bcls_ret->Tables[0].Rows[0]["ORDER_THICK"]		= cmd_inq.GetDecimal(5);
		//	bcls_ret->Tables[0].Rows[0]["PROD_CNAME"]		= cmd_inq.GetString(6);
		//}		
		//cmd_inq.Close();
		//#endif

		///* 根据代码配置表压入化学成分列名 */
	 //   switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:
		//		sqlstr	= "SELECT * "
		//				  "  FROM TEP0002 "
		//				  " WHERE CODE_CLASS = 'QMYS' "
		//				  "	  AND TRIM(CODE_DESC_2_CONTENT) IS NOT NULL "
		//				  " ORDER BY CODE_DESC_2_CONTENT ASC  ";
		//		break;
		//}     
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Clear();
		//cmd_inq.ExecuteReader(); 
		//while(cmd_inq.Read())
		//{ 
		//	cmd_inq.Fetch(tep0002);
		//	tep0002.CODE_DESC_1_CONTENT		= tep0002.CODE_DESC_1_CONTENT.ToLower();

		//	//增加返回块的列名(化学成分)。增加校验是为了避免列重名-化学成分代码定义重复(化学成分可定义大写，也可定义小写，而列名不区分大小写)
		//	if(bcls_ret->Tables[0].Columns.Contains(tep0002.CODE_DESC_1_CONTENT) == false)
		//	{
		//		bcls_ret->Tables[0].Columns.Add(DT_STRING,tep0002.CODE_DESC_1_CONTENT); 
		//	}	
		//}		
		//cmd_inq.Close();

		///* 根据PONO,取得各个化学元素的值 */
		//if (cs_pono.Trim() != "")
		//{
		//	switch (conn->DatabaseKind)
		//	{
		//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//		case DB_KIND_MSSQL:				// MS SQL Server数据库
		//		case DB_KIND_ORACLE:	        // Oracle 数据库
		//		default:
		//			sqlstr = "SELECT * "
		//				"  FROM TQMTQQ0 "
		//				" WHERE PONO	= @cs_pono "
		//				" ORDER BY	ELM_CODE ASC";
		//			break;
		//	}
		//	cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Clear();
		//	cmd_inq.Parameters.Set("cs_pono", cs_pono);
		//	cmd_inq.ExecuteReader();
		//	while (cmd_inq.Read())
		//	{
		//		cmd_inq.Fetch(tqmtqq0);
		//		tqmtqq0.TrimOrBlank();
		//		tqmtqq0.ELM_NAME = tqmtqq0.ELM_NAME.ToLower();
		//		Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_NAME	= [{0}]", (const char*)tqmtqq0.ELM_NAME);
		//		Log::Trace("", __FUNCTION__, "tqmtqq0.ELM_ACT		= [{0}]", tqmtqq0.ELM_ACT.ToDouble());
		//		//增加返回块的(化学成分)值
		//		if (bcls_ret->Tables[0].Columns.Contains(tqmtqq0.ELM_NAME) == true)
		//		{
		//			bcls_ret->Tables[0].Rows[0][tqmtqq0.ELM_NAME] = tqmtqq0.ELM_ACT;
		//		}
		//	}
		//	cmd_inq.Close();
		//}

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



