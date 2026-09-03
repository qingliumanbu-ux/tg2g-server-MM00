/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: 配置属性信息_查询
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
 
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改   

/*<remark>=========================================================
/// <summary>
/// 材料/合同属性查询
/// </summary>
/// <param name="item_ename">项目英文名  </param>
===========================================================</remark>*/                                                
// service入口

BM2F_ENTERACE(mm0097c1a2_inq)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1a2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1a2_inq";  //赋值函数英文名称
	
	//程序用变量
	int doFlag = 0;
	int fetchRowCount  = 0;
	CString  function_id = " " ;
	
	CString c_mat_char = "";
	CString c_mat_comments = "";
	CString c_mat_kind = "";
	CString c_col_name = "";

	CString v_func_id = ""; 
	CString v_item_ename = "";
	
	CString sqlstr;

	CDbCommand cmd_sql(conn);
	CString  c_sql_condition = " "; 
	CString  c_sql_orderBY = " ";
	CString  c_sql_where = "  WHERE  1= 1 ";
		
	try
	{ 
		//获得输入参数   
		if (bcls_rec->Tables[0].Columns.Contains("ITEM_ENAME"))
			v_item_ename = bcls_rec->Tables[0].Rows[0]["ITEM_ENAME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
			v_func_id = bcls_rec->Tables[0].Rows[0]["FUNC_ID"].ToString().Trim(); 

		c_sql_condition = "SELECT t.*  "
			              "  FROM TED54  t "
				          " WHERE t.func_id = @func_id "
			              "   AND t.item_ename LIKE @item_ename || '%' "
				          " ORDER BY t.SEQ_NO ASC";

		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);
		cmd_sql.Parameters.Set("func_id", v_func_id);
		cmd_sql.Parameters.Set("item_ename", v_item_ename);
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);//信息返回到第一个BLK中。 
		cmd_sql.Close(); 

		fetchRowCount = bcls_ret->Tables[0].Rows.get_Count();

		/*设置系统返回参数*/
		{
			CFormattable arguments[] = { fetchRowCount }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("GCRSS0000004"), arguments, 1); //格式化字符串 
		}
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
	
	return doFlag;

}
