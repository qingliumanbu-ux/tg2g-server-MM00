/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2014-11-01
Description:物料跟踪规则配置校验函数
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 棒线物料跟踪
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
//#include "tmm0097.h"
//#include "tmm0099.h"
//#include "tmm009c.h"
//#include "tpmoa10_char.h"
#include <math.h>

BM2_FUNCTION_EXPORT
 int f_mm009c(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义
	 
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString v_ingot_code = "";//锭坯型。	
	CString temp_string = ""; //500
	CString catString = "";

	CString v_event_proc_flag = "1";

	CString	ret_code = ""; //对比结果。
	CString	ret_error_string = "";	//返回的错误信息。
	const float EPSINON = 0.000001;

	//返回块的行数信息的个数。
	int bcls_ret_1_temp = 0; //某一个材料+某组规则下的，规则匹配失败明细信息。
	int bcls_ret_1 = 0;      //返回块1#blk中,规则明细匹配失败信息。
	int bcls_ret_2 = 0;      //返回块2#blk中,规则匹配失败的材料号+合同号+允许强制标记.
	int bcls_ret_3 = 0;      //返回块3#blk中,规则匹配成功的材料号信息

	/* 在SQL语句中使用的变量 */
	// EXEC SQL BEGIN DECLARE SECTION; 
	CString  v_rule_code = ""; //规则代码

	CString	left_expr = "";
	CString	right_expr = "";

	CString	left_item_type = "";
	CString	right_item_type = "";

	CString	v_left_value = "";
	CString	v_right_value = "";

	//数字类型的属性内容的
	CDecimal v_left_value_n = 0;
	CDecimal v_right_value_n = 0;

	CString  v_allow_force_flag = "1";    //允许强制挂合同操纵:0/1=不允许/允许

	CString  match_error_desc = "";//规则匹配失败明细信息。
	CString  match_error_msg = "";//规则匹配失败,返回前台的总信息。
	CString  v_error_remark = "";//规则匹配明细信息，记录到履历表。

	CString  v_rule_grade = "";    /* 匹配优先级别 */
	int   v_dj_check_num = 0;      //当前的等级
	int   v_dj_check_num_max = 9;  //设定最多支持9个规则等级
	int   v_dj_check_flag = 0;     //指定[优先级]规则的比对成功标志0/1=成功/失败  
	int   v_j = 0;

	CDecimal   intBoolean = 0; //单记录比对成功标志  0/1=成功/失败 
	int        v_mat_error = 0; //某个材料在某个规则下比对失败的明细个数 
	CString    v_fun_msg = "";//校验函数返回信息。
	
	/* 实体类定义 */
	/*CTMM0097 tmm0097(conn);  
	CTMM0099 tmm0099(conn);  */
	//CTMM009C tmm009c(conn);
	CModel tmm009c("TMM009C");
	CModel tpmoa10_char("TPMOA10_CHAR");
	CModel tpmoa10_char_mx("TPMOA10_CHAR");
	/*CTPMOA10_CHAR tpmoa10_char(conn);
	CTPMOA10_CHAR tpmoa10_char_mx(conn);*/  //存放校验失败的明细信息。

	/* 数据库SQL操作字符串 */ 
	CString sqlstr;
	CString sqlstr_upd;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//传入参数检核
		if (bcls_rec->Tables["EVENT_TABLE"].Rows.get_Count() == 0 ||
			bcls_rec->Tables["RIGHT_TABLE"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, _RES("YM00S0000710")/*传入记录数不能为0！*/);
			sprintf(s.sysmsg, "调用函数传入记录数不能为0!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* 获取输入参数 */		
		tmm009c["EVENT_ID"]		= bcls_rec->Tables["EVENT_TABLE"].Rows[0]["EVENT_ID"].ToString().Trim();
		tmm009c["MAT_LINE_TYPE"]   = bcls_rec->Tables["EVENT_TABLE"].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();
		tmm009c["MAT_KIND"]        = bcls_rec->Tables["EVENT_TABLE"].Rows[0]["MAT_KIND"].ToString().Trim();
		tmm009c["RULE_TYPE_DESC"]  = bcls_rec->Tables["EVENT_TABLE"].Rows[0]["RULE_TYPE_DESC"].ToString().Trim();
		tmm009c["PRODUCT_FLAG"]    = bcls_rec->Tables["LEFT_TABLE"].Rows[0]["PRODUCT_FLAG"].ToString().Trim();
		tmm009c["MAT_SHAPE_FLAG"]  = bcls_rec->Tables["LEFT_TABLE"].Rows[0]["MAT_SHAPE_FLAG"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "传入参数,tmm009c.EVENT_ID(事件标识)			= [{0}]", tmm009c["EVENT_ID"].ToString());
		//Log::Trace("", __FUNCTION__, "传入参数,tmm009c.MAT_LINE_TYPE 		= [{0}]", tmm009c.MAT_LINE_TYPE);
		//Log::Trace("", __FUNCTION__, "传入参数,tmm009c.MAT_KIND 		= [{0}]", tmm009c.MAT_KIND);
		//Log::Trace("", __FUNCTION__, "传入参数,tmm009c.RULE_TYPE_DESC 		= [{0}]", tmm009c.RULE_TYPE_DESC);
		//Log::Trace("", __FUNCTION__, "传入参数,tmm009c.PRODUCT_FLAG 		= [{0}]", tmm009c.PRODUCT_FLAG);
		//Log::Trace("", __FUNCTION__, "传入参数,tmm009c.MAT_SHAPE_FLAG 		= [{0}]", tmm009c.MAT_SHAPE_FLAG);

		/* 检查输入参数合法性 */
		if (tmm009c["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg,"事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		} 		
		if (tmm009c["MAT_LINE_TYPE"].ToString().Trim() == "")
		{
			strcpy(s.msg,"事件产线类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		} 	

		bcls_ret->Tables[0].set_TableName("MM009C");
		bcls_ret->Tables["MM009C"].Columns.Add(DT_STRING, "EVENT_PROC_FLAG");
		bcls_ret->Tables["MM009C"].Columns.Add(DT_STRING, "EVENT_PROC_REMARK");
		bcls_ret->Tables["MM009C"].Rows.Add();

		/* 校验该事件是否存在 */
		if (tmm009c.QueryCount("EVENT_ID,MAT_LINE_TYPE,MAT_KIND,RULE_TYPE_DESC,PRODUCT_FLAG,MAT_SHAPE_FLAG") <= 0)
		{
			bcls_ret->Tables["MM009C"].Rows[0]["EVENT_PROC_FLAG"] = v_event_proc_flag;
			sprintf(s.msg, "事件号" + tmm009c["EVENT_ID"].ToString() + "物料" + tmm009c["MAT_KIND"].ToString() + "产线类型" + tmm009c["MAT_LINE_TYPE"].ToString() + "在MM0097C1画面中无相关规则配置,请确定接口!");
			Log::Trace("", __FUNCTION__, s.msg);
			return 0;
		}

		//初始化=
		doFlag = 0;
		ret_code = "";
		ret_error_string = "";

		/* 查询抛帐规则配置 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT t.* "
				"   FROM TMM009C t "
				"  WHERE EVENT_ID = @tmm009c.EVENT_ID   "
				"    AND RULE_TYPE_DESC = @tmm009c.RULE_TYPE_DESC "
				"    AND MAT_LINE_TYPE = @tmm009c.MAT_LINE_TYPE "
				"    AND MAT_KIND = @tmm009c.MAT_KIND "
				"    AND PRODUCT_FLAG = @tmm009c.PRODUCT_FLAG "
				"    AND MAT_SHAPE_FLAG = @tmm009c.MAT_SHAPE_FLAG "
				"  ORDER BY  t.RULE_CODE,t.RULE_GRADE,t.RULE_SEQ_NO ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm009c.EVENT_ID", tmm009c["EVENT_ID"]);
		cmd_inq.Parameters.Set("tmm009c.RULE_TYPE_DESC", tmm009c["RULE_TYPE_DESC"]);
		cmd_inq.Parameters.Set("tmm009c.MAT_LINE_TYPE", tmm009c["MAT_LINE_TYPE"]);
		cmd_inq.Parameters.Set("tmm009c.MAT_KIND", tmm009c["MAT_KIND"]);
		cmd_inq.Parameters.Set("tmm009c.PRODUCT_FLAG", tmm009c["PRODUCT_FLAG"]);
		cmd_inq.Parameters.Set("tmm009c.MAT_SHAPE_FLAG", tmm009c["MAT_SHAPE_FLAG"]);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm009c);
			tmm009c.TrimOrBlank();

			//转换成大写，获取对应的块中的字段信息。 
			//=======================
			left_expr = tmm009c["MAT_CHAR"].ToString().ToUpper();
			right_expr = tmm009c["ORDER_CHAR"].ToString().ToUpper();

			//获取左边的属性信息(材料属性)
			v_left_value = bcls_rec->Tables["LEFT_TABLE"].Rows[0][left_expr].ToString().Trim();

			//获取右边属性的值信息(合同属性)
			{
				if (tmm009c["RULE_TYPE"].ToString().Trim() == "2")
				{/*若规则类型=2，说明是2个变量的比较，则从EIClass中获取数据 ==rule_type*/
					//Log::Trace("",__FUNCTION__,"规则类型=2，说明是2个变量的比较,获取第2个变量" );
					v_right_value = bcls_rec->Tables["RIGHT_TABLE"].Rows[0][right_expr].ToString().Trim();

				}
				else if (tmm009c["RULE_TYPE"].ToString().Trim() == "1" || tmm009c["RULE_TYPE"].ToString().Trim() == "3")
				{//1变量与1个固定值比较 or 1变量与一个字符串定值的[属于]，[不属于]的关系比较 。 
					//Log::Trace("",__FUNCTION__,"规则类型=1，说明是1变量与固定值比较。" );
					v_right_value = tmm009c["PM_CONSTANT"].ToString().Trim(); //令右属性的值= 定制信息。
				}
				else
				{//错误的规则类型。
					sprintf(s.msg, "规则类型[%s]错误，当前业务不支持"
						, (const char*)tmm009c["RULE_TYPE"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//因为规则表中字段的信息来自ED54的功能号 ，可能存在 ‘代码’，‘代码描述’，‘按钮’等字段类型。
			//非[数字]类型的都视为[字符]类型。
			//========
			if (tmm009c["ITEM_TYPE"].ToString().Trim() != "N")
			{//非数字类型的，都视为C。
				tmm009c["ITEM_TYPE"] = "C";
			}

			//字段类型。
			left_item_type = tmm009c["ITEM_TYPE"];  // ITEM_TYPE,变量应该视为什么类型进行处理(字符串类型还是整型)
			right_item_type = tmm009c["ITEM_TYPE"];
			Log::Trace("", __FUNCTION__, "左字段名[{0}] 内容[{1}]类型[{2}]", left_expr, v_left_value, left_item_type);
			Log::Trace("", __FUNCTION__, "右字段名[{0}] 内容[{1}]类型[{2}]", right_expr, v_right_value, right_item_type);

			if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
			{//数字类型的变量，需要校验，内容不允许为空。
				if (v_left_value.Trim() == "")
				{
					sprintf(s.msg, "左属性[%s]的内容[%s]为空，当前操作失败。"
						, (const char*)left_expr, (const char*)v_left_value);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (v_right_value.Trim() == "")
				{
					sprintf(s.msg, "右属性[%s]的内容[%s]为空，当前操作失败。"
						, (const char*)right_expr, (const char*)v_right_value);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (tmm009c["ITEM_TYPE"].ToString().Trim() != "C" &&  tmm009c["ITEM_TYPE"].ToString().Trim() != "N")
			{//错误的字段类型。 
				sprintf(s.msg, "规则数据[%s]的类型[%s]不属于当前业务支持范围。"
					, (const char*)tmm009c["MAT_CHAR"], (const char*)tmm009c["ITEM_TYPE"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//若是[数字类型]的，那么，需要对规则值进行转换。
			//=========
			v_left_value_n = 0;
			v_right_value_n = 0;
			if (tmm009c["ITEM_TYPE"].ToString() .Trim() == "N")
			{
				v_left_value_n = CDecimal::Parse(v_left_value);
				v_right_value_n = CDecimal::Parse(v_right_value);
			}

			/* ******************************************************** */
			/* 操作代码说明：						                                */
			/*   GT：大于		GE：大于等于		EQ：等于		NE：不等于      */
			/*   LT：小于		LE：小于等于		IN: 属于    NOT_IN:不属于  HIGHER:优于， LOWER:低于		*/
			/* ******************************************************** */
			//    ret_string = "开始比较";
			Log::Trace("", __FUNCTION__, "v_left_value =[{0}] v_right_value =[{1}] v_left_value_n=[{2}]v_right_value_n[{3}] tmm009c.MATCH_RELATION[{4}]"
				, v_left_value, v_right_value, v_left_value_n, v_right_value_n, tmm009c["MATCH_RELATION"].ToString());

			intBoolean = 0; //初始化比对标志信息。//单记录比对成功标志  0/1=成功/失败
			{//===规格比较====begin
				if (tmm009c["MATCH_RELATION"].ToString().Trim() == "GT")
				{//大于
					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n > v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型
						if (!(v_left_value > v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Trim() == "GE")
				{//大于等于

					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n >= v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型

						if (!(v_left_value >= v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("EQ") == 0)
				{//等于
					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n == v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型
						if (!(v_left_value == v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("NE") == 0)
				{//不等于
					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n != v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型
						if (!(v_left_value != v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("LT") == 0)
				{//小于 
					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n < v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型
						if (!(v_left_value < v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("LE") == 0)
				{//小于等于 
					if (tmm009c["ITEM_TYPE"].ToString().Trim() == "N")
					{//数字型
						if (!(v_left_value_n <= v_right_value_n))
						{
							intBoolean = 1;
						}
					}
					else
					{//非数字型
						if (!(v_left_value <= v_right_value))
						{
							intBoolean = 1;
						}
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("IN") == 0)
				{//属于，再进行属于的比较（在tmm009c.PM_CONSTANT字段中） 
					//Log::Trace("",__FUNCTION__,"xxxxxxxxxxxxIN关系左值 = [{0}]",v_left_value);     
					//Log::Trace("",__FUNCTION__,"xxxxxxxxxxxxIN关系右值 = [{0}]",v_right_value);   
					//Log::Trace("",__FUNCTION__,"xxxxxxxxxxxxIN关系结果 = [{0}]",v_right_value.Find(v_left_value));    
					if ((v_left_value.Trim() != "" || v_right_value.Trim() != "") && v_right_value.Find(v_left_value) < 0)
					{//若没有找到，说明关系比对失败 
						intBoolean = 1;
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Compare("NOT_IN") == 0)
				{//不属于，先进行等于比较，若不行，再进行不属于的比较（在tmm009c.PM_CONSTANT字段中） 
					if ((v_left_value.Trim() != "" || v_right_value.Trim() != "")
						&& v_right_value.Find(v_left_value) >= 0)
					{//若找到，说明关系比对失败 
						intBoolean = 1;
					}
				}
				else if (tmm009c["MATCH_RELATION"].ToString().Trim() == "FUN")
				{//若是有函数校验的规则比对关系。
					Log::Trace("", __FUNCTION__, "tmm009c.MATCH_RELATION[{0}] tmm009c.PM_CONSTANT[{1}] "
						, tmm009c["MATCH_RELATION"].ToString(), tmm009c["PM_CONSTANT"].ToString());

					//新增函数调用的规则维护,
					//固定值列中，存放的是调用函数的名称。

					Log::Trace("", __FUNCTION__, "最终===v_fun_msg[{0}]", v_fun_msg);
				}
				else
				{//错误的比对关系。

					sprintf(s.msg, "对比关系[%s]不在当前业务支持范围内。"
						, (const char*)tmm009c["MATCH_RELATION"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/*	返回代码组合，如果单个比较成功，拼0，否则拼1	*/
				//组织报错信息,到结构 TPMOA10_CHAR，便于返回前台。
				//============ 
				//指定结构体相同列名的值复制
				
				tpmoa10_char.CopyFrom(tmm009c);
				tpmoa10_char.TrimOrBlank();

				//获取：材料号,合同号，规则代码。
				//Log::Trace("",__FUNCTION__," 材料[{0}]合同[{1}]规则代码[{2}]=比对失败。",tpmoa10_char.MAT_NO,tpmoa10_char.ORDER_NO,tpmoa10_char.RULE_CODE);

				//获取：'材料属性值','合同属性值'			
				tpmoa10_char["MAT_CHAR_VALUE"] = v_left_value;
				tpmoa10_char["ORDER_CHAR_VALUE"] = v_right_value;

				//若是函数校验模式， 则返回对应的函数返回信息。
				if (tmm009c["MATCH_RELATION"].ToString().Trim() == "FUN")
				{
					tpmoa10_char["PM_CONSTANT"] = v_fun_msg;
				}

				ret_code = "1";//默认失败。

				if (intBoolean == 1)
				{
					//若某一个规则明细，比对的结果是失败的，则记录失败明细。 
					//比对失败了,并且是[一般操作]模式,拼一个‘1’
					ret_code = "1";
					v_mat_error++; //某个规则下的比对失败的个数。
				}
				else
				{//若某一个规则明细，比对的结果是成功的。

					//比对成功了，拼个0
					ret_code = "0";
				}
				if (intBoolean == 1)
				{
					//拼接规则比对明细信息。
					//0 [牌号/Q345B] = [牌号/Q345B] \n 
					//1 [标准/1234] = [表准/12345] \n
					//0 [产品规范/1] = [产品规范/2]\n
					temp_string = "[" + tpmoa10_char["MAT_CHAR_DESC"].ToString() + "/" + tpmoa10_char["MAT_CHAR_VALUE"].ToString() + "]不符合"
						+ tmm009c["MATCH_RELATION_DESC"].ToString() + "[" + tmm009c["ORDER_CHAR_DESC"].ToString() + "/" + tpmoa10_char["ORDER_CHAR_VALUE"].ToString() + "]规则" + "\n";

					//Log::Trace("",__FUNCTION__," 某规则-temp_string[{0}] intBoolean[{1}]ret_code[{2}]",temp_string,intBoolean,ret_code); 

					//新增规则比对明细履历，履历[tpmoa00]表加字段 “ERROR_REMARK”
					//数据库里存放的字段内容如下：利用\n【换行符】作为分隔符。
					//0[sg_sign/牌号/Q345B] = [sg_sign/牌号/Q345B] \n  1[sg_std/标准/1234] = [sg_std/表准/12345] \n 0[psc/产品规范/1] = [psc/产品规范/2]\n 
					v_error_remark = v_error_remark + temp_string;
				}

				//拼接结果字符串。
				//=========== 
				catString = catString + ret_code;

			}//===一组规则明细比较====end 
		}
		cmd_inq.Close();

		Log::Trace("", __FUNCTION__, "材料[{0}]合同[{1}]比对结果字符串==catString[{2}]   ", tpmoa10_char["MAT_NO"].ToString(), tpmoa10_char["ORDER_NO"].ToString(), catString);
		Log::Trace("", __FUNCTION__, "比对结果描述v_error_remark[{0}]   ", v_error_remark);

		if (catString.Find("1") < 0)
		{
			v_event_proc_flag = "0";
			bcls_ret->Tables["MM009C"].Rows[0]["EVENT_PROC_FLAG"] = v_event_proc_flag;
		}
		else
		{
			v_event_proc_flag = "1";
			bcls_ret->Tables["MM009C"].Rows[0]["EVENT_PROC_FLAG"] = v_event_proc_flag;
			bcls_ret->Tables["MM009C"].Rows[0]["EVENT_PROC_REMARK"] = v_error_remark;
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
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
