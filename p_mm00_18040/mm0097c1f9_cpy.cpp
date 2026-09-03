/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则明细信息_复制
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
 
 

// 获得流水号-ZY版，序号类型(0/1/2/3=到最大值后归零，按年归零，按月归零，按日归零)
CString f_GetNextSeq_pm(CString v_seq_name, CDecimal v_seq_length, CString v_seq_type, CDbConnection* conn); //序号名称，序号长度，序号类型，返回的序号
int f_mm0097c1f9_cpy(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改 
/*<remark>=========================================================
/// <summary>
/// 规则集复制
/// <para>

/// </summary>
/// <param name="RULE_CODE">规范代码  </param>
/// <param name="RULE_SEQ_NO">规则序号  </param>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mm0097c1f9_cpy)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f9_cpy(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1f9_cpy";  //赋值函数英文名称

	//程序用变量
	int doFlag = 0;
	int i;

	CString match_relation = "";

	CString v_seq = "";
	CString v_seq_name = "";
	int  v_cnt = 0;

	CString systime = CDateTime::Now().ToString("yyyyMMddHHmmss");//systime  
	CString userid  = s.userid;//获得userid

	CModel tmm009c("TMM009C");
	CModel tpmoa10_item("TPMOA10_ITEM");
	 
	CString sqlstr;

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition = " ";

		/* 数据库操作类定义2 */
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CString  c_sql_where2 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition2 = " ";

		//获取3号块的以下信息。，并进行校验。
		
		CString v_rule_desc = "";
		CString v_product_flag = "";
		CString v_pmoa_flag = ""; //业务操作类型。
		CString v_rule_code = ""; //规则代码。

		CString v_talbe_name = "MM0097C1_QT";
		if (bcls_rec->Tables[v_talbe_name].Columns.Contains("RULE_CODE"))
			v_rule_code = bcls_rec->Tables[v_talbe_name].Rows[0]["RULE_CODE"];

		if (bcls_rec->Tables[v_talbe_name].Columns.Contains("RULE_DESC"))
			v_rule_desc = bcls_rec->Tables[v_talbe_name].Rows[0]["RULE_DESC"]; 
		
		if (bcls_rec->Tables[v_talbe_name].Columns.Contains("PMOA_FLAG"))
		v_pmoa_flag = bcls_rec->Tables[v_talbe_name].Rows[0]["PMOA_FLAG"]; //INS/APP/UPD/DEL= 创建/添加/修改/删除
 
		Log::Trace("", __FUNCTION__, " v_rule_desc = [{0}]", v_rule_desc);
		Log::Trace("", __FUNCTION__, " v_rule_code = [{0}]", v_rule_code);
		Log::Trace("", __FUNCTION__, " v_pmoa_flag = [{0}]", v_pmoa_flag);
		
		if (v_rule_code.Trim() == "")
		{
			sprintf(s.msg, "源[规则代码]不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_rule_desc.Trim() == "")
		{
			sprintf(s.msg, "目标[规则描述]不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}  
		Log::Trace("", __FUNCTION__, "最终获取的规则代码 = [{0}]", tmm009c["RULE_CODE"].ToString());

		//获取目标[规则代码]
		//v_seq_name = CString::Format("PMOA_%s", (const char*)tmm009c["MAT_KIND"].ToString()); //序号名称=PMOA_SP 
		//序号类型(0/1/2/3=到最大值后归零(0)，按年归零(1)，按月归零(2)，按日归零(3))
		v_seq_name = "MM0097C_RULE_CODE";
		v_seq = f_GetNextSeq_pm(v_seq_name, 10, "0", conn); //序号名称，序号长度，序号类型，返回的序号
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CString   v_rule_code_new = v_seq;        /* [新]规则代码 */   
		CString   v_rule_desc_new = v_rule_desc;  //[新]规则描述。

		Log::Trace("", __FUNCTION__, "v_rule_code_new = [{0}]", v_rule_code_new);
		Log::Trace("", __FUNCTION__, "v_rule_desc_new = [{0}]", v_rule_desc_new);

		//获取源规则信息，进行规则复制。
		c_sql_condition = " SELECT t.*  "
			" FROM   TMM009C  t         "
			" WHERE  t.rule_code = @rule_code  "
			;
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);
		cmd_sql.Parameters.Set("rule_code", v_rule_code);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			cmd_sql.Fetch(tmm009c);
			tmm009c["RULE_CODE"] = v_rule_code_new;
			tmm009c["RULE_DESC"] = v_rule_desc_new;

			tmm009c.TrimOrBlank();
			sqlstr = "insert into tmm009c,规则代码[" + tmm009c["RULE_CODE"].ToString() + "]";
			tmm009c.Insert();
		}
		cmd_sql.Close(); 

		//返回规则代码。
		//========== 
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "RULE_CODE");
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["RULE_CODE"] = tmm009c["RULE_CODE"];

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
