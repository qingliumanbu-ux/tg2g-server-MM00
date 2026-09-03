/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则明细信息_修改
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件


int f_mm0097c1f6_upd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改  
/*<remark>=========================================================
/// <summary>
/// 修改转用充当规则
/// <para>

/// </summary>
/// <param name="RULE_CODE">规范代码  </param>
/// <param name="RULE_SEQ_NO">规则序号  </param>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mm0097c1f6_upd)

//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f6_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1f6_upd";  //赋值函数英文名称

	//程序用变量
	int doFlag = 0;
	int fetchRowCount = 0;
	int i;

	int  v_seq_now = 0;
	int  v_cnt = 0;
	CString sqlstr = "";

	CString systime = "";
	CString userid = "";
	CString v_update = "";  //修改的字段信息
	CString v_condi = "";  //过滤的字段信息。

	CString v_oper_flag = "0";  //'oper_flag'= 0/1=一般用户操作/MES操作

	try
	{
	CModel tmm009c("TMM009C");

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CString  c_sql_where = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition = "  ";

		//获得当前时间
		systime = CDateTime::Now().ToString("yyyyMMddHHmmss");//systime  
		//获得userid 
		userid = s.userid;

		//对输入信息循环处理 
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tmm009c.MergeFrom(bcls_rec->Tables[0].Rows[i - 1]);
			tmm009c["REC_REVISE_TIME"] = systime;
			tmm009c["REC_REVISOR"] = s.userid;
			tmm009c.TrimOrBlank();

			//规则等级,0/1=无效等级/有效等级
			//若前台没有信息，则默认给1.
			if (tmm009c["RULE_GRADE"].ToString().Trim() == "")
			{
				tmm009c["RULE_GRADE"] = "1";
			}

			//根据 [匹配关系代码]获取对应的[匹配关系描述], 代码[PMA5]
			//=============
			tmm009c["MATCH_RELATION_DESC"] = "";
			c_sql_condition = "select t.CODE_DESC_1_CONTENT from tep0002 t where t.code_class = 'PMA5' and t.code = @code ";
			sqlstr = c_sql_condition;
			cmd_sql.SetCommandText(c_sql_condition);
			cmd_sql.Parameters.Set("code", tmm009c["MATCH_RELATION"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				tmm009c["MATCH_RELATION_DESC"] = cmd_sql.GetString(1);
			}
			cmd_sql.Close();


			v_update = "REC_REVISE_TIME,REC_REVISOR,MATCH_RELATION,RULE_GRADE,FORCE_MATCH_FLAG,MATCH_FLAG,REMARK_DESC,MAT_CHAR_DESC,ORDER_CHAR_DESC,MATCH_RELATION_DESC,RULE_TYPE,PM_CONSTANT"
				",KEYVALUE_1,KEYVALUE_2,KEYVALUE_3,KEYVALUE_4,KEYVALUE_5,KEYVALUE_6";
			v_condi = "RULE_CODE,RULE_SEQ_NO";
			sqlstr = "update tmm009c ";
			tmm009c.Update(v_update, v_condi);
		}

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
