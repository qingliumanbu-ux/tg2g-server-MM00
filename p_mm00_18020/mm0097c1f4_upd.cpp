/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则主信息_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
 
 

int f_mm0097c1f4_upd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改 
/*<remark>=========================================================
/// <summary>
/// 规则主信息修改
/// <para>

/// </summary>
/// <param name="RULE_CODE">规范代码  </param>
/// <param name="RULE_SEQ_NO">规则序号  </param>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mm0097c1f4_upd)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f4_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1f4_upd";  //赋值函数英文名称

	//程序用变量
	int doFlag = 0;
	int i;

	CString match_relation = "";

	CString v_seq = "";
	CString v_seq_name = "";
	int  v_cnt = 0;

	CString systime = CDateTime::Now().ToString("yyyyMMddHHmmss");//systime  
	CString userid = s.userid;//获得userid

	CModel tmm009c("TMM009C");
	CModel tpmoa10_item("TPMOA10_ITEM");

	CString sqlstr;

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition = "  ";

		/* 数据库操作类定义2 */
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CString  c_sql_where2 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition2 = "  ";

		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//获取前台输入数据
			//整表信息的获取模式
			tmm009c.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm009c.TrimOrBlank();

			Log::Trace("", __FUNCTION__, " RULE_CODE = [{0}]", tmm009c["RULE_CODE"].ToString());
			Log::Trace("", __FUNCTION__, " RULE_DESC = [{0}]", tmm009c["RULE_DESC"].ToString());
			Log::Trace("", __FUNCTION__, " MAT_KIND = [{0}]", tmm009c["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, " EVENT_ID = [{0}]", tmm009c["EVENT_ID"].ToString());
			Log::Trace("", __FUNCTION__, " MAT_LINE_TYPE = [{0}]", tmm009c["MAT_LINE_TYPE"].ToString());
			Log::Trace("", __FUNCTION__, " RULE_TYPE_DESC = [{0}]", tmm009c["RULE_TYPE_DESC"].ToString());
			Log::Trace("", __FUNCTION__, " MAT_SHAPE_FLAG = [{0}]", tmm009c["MAT_SHAPE_FLAG"].ToString());

			/*
			CString RULE_DESC;   //规则描述
			CString MAT_KIND;   //物料种类
			CString EVENT_ID;   //事件号
			CString RULE_TYPE_DESC;   //规则类型
			CString WHOLE_BACKLOG_CODE;   //全程工序代码
			//新增以下信息。
			CString MAT_LINE_TYPE;   //产线类型
			CString FACTORY_DIV;   //厂别区分
			*/

			if (tmm009c["RULE_CODE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "[规则代码]不允许为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//修改规则描述等信息。
			c_sql_condition = " update tmm009c t "
				" set    t.rule_desc          = @rule_desc "
				"       ,t.mat_kind           = @mat_kind "
				"       ,t.mat_line_type      = @mat_line_type "
				"       ,t.product_flag       = @product_flag "
				"       ,t.rule_type_desc     = @rule_type_desc "
				"       ,t.mat_shape_flag     = @mat_shape_flag "
				"       ,t.whole_backlog_code = @whole_backlog_code "
				"       ,t.event_id           = @event_id " //事件号
				"       ,t.factory_div        = @factory_div " //厂别区分
				" WHERE  t.rule_code = @rule_code  "
				;
			sqlstr = c_sql_condition;
			cmd_sql.SetCommandText(c_sql_condition);
			cmd_sql.Parameters.Set("rule_desc", tmm009c["RULE_DESC"].ToString());
			cmd_sql.Parameters.Set("mat_kind", tmm009c["MAT_KIND"].ToString());
			cmd_sql.Parameters.Set("mat_line_type", tmm009c["MAT_LINE_TYPE"].ToString());
			cmd_sql.Parameters.Set("product_flag", tmm009c["PRODUCT_FLAG"].ToString());
			cmd_sql.Parameters.Set("mat_shape_flag", tmm009c["MAT_SHAPE_FLAG"].ToString());
			cmd_sql.Parameters.Set("rule_type_desc", tmm009c["RULE_TYPE_DESC"].ToString());
			cmd_sql.Parameters.Set("whole_backlog_code", tmm009c["WHOLE_BACKLOG_CODE"].ToString());
			cmd_sql.Parameters.Set("event_id", tmm009c["EVENT_ID"].ToString());
			cmd_sql.Parameters.Set("factory_div", tmm009c["FACTORY_DIV"].ToString());
			cmd_sql.Parameters.Set("rule_code", tmm009c["RULE_CODE"].ToString());
			cmd_sql.ExecuteNonQuery();
			cmd_sql.Close();

		}

		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  
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

	return doFlag;

}
