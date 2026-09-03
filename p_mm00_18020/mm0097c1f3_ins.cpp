/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则主信息_新增
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
 
 

// 获得流水号-ZY版，序号类型(0/1/2/3=到最大值后归零，按年归零，按月归零，按日归零)
CString f_GetNextSeq_pm(CString v_seq_name, CDecimal v_seq_length, CString v_seq_type, CDbConnection* conn); //序号名称，序号长度，序号类型，返回的序号
int f_mm0097c1f3_ins(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改 
/*<remark>=========================================================
/// <summary>
/// 插入双边转用充当规则
/// <para>

/// </summary>
/// <param name="RULE_CODE">规范代码  </param>
/// <param name="RULE_SEQ_NO">规则序号  </param>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mm0097c1f3_ins)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1f3_ins";  //赋值函数英文名称
	
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
		CString  c_sql_condition = "  ";

		/* 数据库操作类定义2 */
		CDbCommand cmd_sql2(conn); //与DB 建立连接。
		CString  c_sql_where2 = "  WHERE   1 = 1 "; //查询条件。
		CString  c_sql_condition2 = "  ";


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
		
		if (v_pmoa_flag.Trim() == "")
		{
			sprintf(s.msg, "[业务类型]不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
	
		//==================
		if (v_pmoa_flag.Trim() == "INS")
		{
			//规则创建，则创建新的规则代码。
			//序号类型(0/1/2/3=到最大值后归零(0)，按年归零(1)，按月归零(2)，按日归零(3))
			v_seq_name = "MM0097C_RULE_CODE";
			v_seq = f_GetNextSeq_pm(v_seq_name, 10, "0", conn); //序号名称，序号长度，序号类型，返回的序号
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//CString   rule_code;                    /* 规则代码 */
			//物料类型(2)+成品标志(1)+流水号(4)= SP1_0001(炼钢在制品1#规则) 
			//tmm009c["RULE_CODE"] = tmm009c["MAT_KIND"].ToString() + tmm009c["PRODUCT_FLAG"].ToString() + "_" + v_seq;
			tmm009c["RULE_CODE"] = v_seq;
			tmm009c["RULE_DESC"] = v_rule_desc; //规则描述。

			Log::Trace("", __FUNCTION__, "v_seq_name = [{0}]", v_seq_name);
			Log::Trace("", __FUNCTION__, "最终获取的规则代码 = [{0}]", tmm009c["RULE_CODE"].ToString());
		}
		else if (v_pmoa_flag.Trim() == "APP") 
		{//若是规则添加，则规则代码不允许为空。

			if (v_rule_code.Trim() == "")
			{
				sprintf(s.msg, "[规则代码]不允许为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			 
			//根据规则代码，随机获取一条规则主信息。
			//==============
			//CDecimal RULE_SEQ_NO;   //规则序号
			c_sql_condition = " SELECT t.* "
				" FROM   TMM009C   t         "
				" WHERE  t.rule_code = @rule_code  "
				" ORDER  BY t.RULE_SEQ_NO "
				;
			sqlstr = c_sql_condition;
			cmd_sql.SetCommandText(c_sql_condition);
			cmd_sql.Parameters.Set("rule_code", v_rule_code);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				cmd_sql.Fetch(tmm009c);
			}
			cmd_sql.Close();
			if (tmm009c["RULE_CODE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "根据规则代码[%s],没有获取对应的规则主信息。",(const char*)v_rule_code);
				throw CApplicationException(-1, s.msg, log.Location); 
			} 

			Log::Trace("", __FUNCTION__, "APP业务-rule_code = [{0}]PRODUCT_FLAG[{1}]"
				, v_rule_code,tmm009c["PRODUCT_FLAG"].ToString());

		}
		else
		{
			//其他业务。
			sprintf(s.msg, "抱歉，业务类型[%s]不在本程序中实施。",(const char*)v_pmoa_flag);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		 
		Log::Trace("", __FUNCTION__, "最终获取的规则代码 = [{0}]", tmm009c["RULE_CODE"].ToString());

		//获取1#BLK的【材料属性】信息，取得单行传入信息
		//      CString   item_ename;                  /* 字段英文名 */
		//      CString   item_display_cname;          /* 中文名 */
		//      CString   item_type;                   /* 字段类型 */
		//      CString   item_len;                    /* 字段长度 */
		tpmoa10_item.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tpmoa10_item.TrimOrBlank();
	 
		//材料属性
		tmm009c["MAT_CHAR"] = tpmoa10_item["ITEM_ENAME"];
		tmm009c["MAT_CHAR_DESC"] = tpmoa10_item["ITEM_CNAME"];
		tmm009c["KEYVALUE_2"] = tpmoa10_item["ITEM_TYPE"];//获取，材料属性的字段类型

		//获取2#BLK的【合同属性】信息，取得单行传入信息
		//      CString   item_ename;                  /* 字段英文名 */
		//      CString   item_display_cname;          /* 中文名 */
		//      CString   item_type;                   /* 字段类型 */
		//      CString   item_len;                    /* 字段长度 */
		int v_ord_char_num = bcls_rec->Tables[1].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "v_ord_char_num = [{0}]", v_ord_char_num);

		tmm009c["ORDER_CHAR"] = "";
		tmm009c["ORDER_CHAR_DESC"] = "";
		tmm009c["KEYVALUE_3"] = "";
		if (v_ord_char_num >= 1)
		{
			tpmoa10_item.MergeFrom(bcls_rec->Tables[1].Rows[0]);
			tpmoa10_item.TrimOrBlank();

			//合同属性
			tmm009c["ORDER_CHAR"] = tpmoa10_item["ITEM_ENAME"];
			tmm009c["ORDER_CHAR_DESC"] = tpmoa10_item["ITEM_CNAME"];
			tmm009c["KEYVALUE_3"] = tpmoa10_item["ITEM_TYPE"];//获取，合同属性的字段类型
		}
		 
		//若没有传入‘合同属性’信息，则默认为单边规则。
		//tmm009c["RULE_TYPE"] = "2";                    /* 规则类型=1/2/3=单变量比较/2个变量比较/单变量与代码比较 */
		//=================================
		if (tmm009c["ORDER_CHAR"].ToString().Trim() == "")
		{
			tmm009c["RULE_TYPE"] = "1";                    /* 规则类型=1/2/3=单变量比较/2个变量比较/单变量与代码比较 */
		}
		else
		{
			//若有材料属性+合同属性，则说明是双边规则。
			tmm009c["RULE_TYPE"] = "2";                    /* 规则类型=1/2/3=单变量比较/2个变量比较/单变量与代码比较 */

			/*	如果数据项类型不匹配，则不能比较 */
			if (tmm009c["KEYVALUE_2"].ToString().Trim() != tmm009c["KEYVALUE_3"].ToString().Trim())
			{
				sprintf(s.msg, "[左/右]==>[%s]/[%s]两边的数据项【类型】不一致，则不能比较。"
					,(const char*)tmm009c["KEYVALUE_2"].ToString(),(const char*)tmm009c["KEYVALUE_3"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//记录下属性类型。
			//ITEM_TYPE
			tmm009c["ITEM_TYPE"] = tmm009c["KEYVALUE_2"];
		}

		//获取规则号内的序号。
		c_sql_condition = " SELECT NVL(MAX(t.RULE_SEQ_NO),0) + 1 "
			" FROM   TMM009C   t         "
			" WHERE  t.rule_code = @rule_code  "
			;
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);
		cmd_sql.Parameters.Set("rule_code", tmm009c["RULE_CODE"].ToString());
		tmm009c["RULE_SEQ_NO"] = cmd_sql.ExecuteScalar();
		cmd_sql.Close();

		//初始化以下字段； 
		tmm009c["MATCH_RELATION"] = "EQ";              /* 匹配关系 A*/
		tmm009c["MATCH_RELATION_DESC"] = "等于";       /* 匹配关系描述 A*/
		tmm009c["RULE_GRADE"] = "1";                   // 规则等级,0/1=无效等级/有效等级
		tmm009c["FORCE_MATCH_FLAG"] = "1";             /* 可强制匹配 ; 0/1=不可强制匹配/可强制匹配*/
		tmm009c["PM_CONSTANT"] = "";                   /* 定值信息=初始化 */
	
		tmm009c["KEYVALUE_1"] = "";
		tmm009c["KEYVALUE_3"] = "";
		tmm009c["REC_CREATE_TIME"] = systime;
		tmm009c["REC_CREATOR"]     = userid;
		tmm009c.TrimOrBlank();

		sqlstr = "insert into tmm009c ";
		tmm009c.TrimOrBlank();
		tmm009c.Insert();

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
