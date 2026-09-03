/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则主信息_查询
**************************************************/ 
//框架公用头文件，勿删
#include "stdafx.h"

//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN
//-此节代码请勿更改

/*<remark>=========================================================
/// <summary>
/// 规则主信息_查询 
/// </summary> 
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mm0097c1f2_inq)
//-EP_SYSTEM_HEAD_END
int f_mm0097c1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname="";														//定义函数英文名称
	CString FunctionCname="";														//定义函数中文名称

	FunctionEname = "mm0097c1f2_inq_";											    //赋值函数英文名称
	//EDLog(1, 1, " **************%s begin*****************",FunctionEname);

	//程序用变量
	int doFlag = 0;
	int fetchRowCount = 0;
	int v_total_count = 0;
	 
	CString sqlstr;
	
	try
	{
		//系统的分页类信息。
		CPageInfo pageInfo; 

		//SQL
		CDbCommand cmd_sql(conn);

		CString c_sql_condition  =  "   " ;
		CString c_sql_condition2 =	"   ";
		CString c_sql_orderBY    =  "   ";
		CString c_sql_where = "  ";
 
		//获得输入参数

		c_sql_condition = " SELECT  t.EVENT_ID,t.RULE_CODE,t.RULE_DESC "
			" ,t.MAT_KIND,t.MAT_LINE_TYPE,t.PRODUCT_FLAG "
			" ,t.RULE_TYPE_DESC,t.WHOLE_BACKLOG_CODE "
			" ,t.MAT_SHAPE_FLAG,t.FACTORY_DIV "//材料形态，厂别。
			" ,COUNT(1) AS RULE_NUM   "
			" FROM TMM009C t "
			" GROUP  BY t.EVENT_ID,t.RULE_CODE,t.RULE_DESC "
			"          ,t.MAT_KIND,t.MAT_LINE_TYPE,t.PRODUCT_FLAG "
			"          ,t.RULE_TYPE_DESC,t.WHOLE_BACKLOG_CODE "
			"          ,t.MAT_SHAPE_FLAG,t.FACTORY_DIV "//材料形态，厂别。
			" ORDER  BY t.MAT_KIND,t.MAT_LINE_TYPE,t.PRODUCT_FLAG "
			"          ,t.RULE_TYPE_DESC,t.WHOLE_BACKLOG_CODE "
			"          ,t.MAT_SHAPE_FLAG,t.FACTORY_DIV "//材料形态，厂别。
			;  
		Log::Trace("",__FUNCTION__,"IN:c_sql_condition = [{0}]  ",c_sql_condition );   

		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句 
		bcls_ret->Tables[0].set_TableName("TMM009C");
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_sql.Close();

		//返回的记录数。
		fetchRowCount = bcls_ret->Tables[0].Rows.get_Count();

		/*设置系统返回参数*/
		{
			CFormattable arguments[] = {fetchRowCount}; // 定义参数列表的数组
			CMessageFormat::Format(s.msg,  _RES("GCRSS0000004"),	arguments, 1); //格式化字符串 
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		 
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
	 
	/// <summary>
	/// 返回总记录数
	/// </summary>     
	bcls_ret->Tables.Add("PageInfo");
	bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
	bcls_ret->Tables["PageInfo"].Rows.Add();
	bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = v_total_count;
	 
	////EDLog(1, 1, " **************%s end*****************", (const char*)FunctionEname);

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
}
