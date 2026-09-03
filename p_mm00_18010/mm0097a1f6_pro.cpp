/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪事件管理事件维护
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理事件维护
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "tmm0097.h"  
#include "tmm0099.h"  
#include "tmm009a.h"  
#include "tep0002.h"  

//外部函数声明

BM2F_ENTERACE(mm0097a1f6_pro)

int f_mm0097a1f6_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal cd_count = 0;

	/* 实体类定义 */
	CTMM0097 tmm0097(conn);
	CTMM0099 tmm0099(conn);
	CTMM009A tmm009a(conn);
	CTEP0002 tep0002(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 校验操作者权限 */
		tep0002.CODE_CLASS = "M09M";
		tep0002.CODE = s.userid;
		if (tep0002.QueryCount("CODE_CLASS,CODE") <= 0)
		{
			sprintf(s.msg, "事件操作者[%s]无维护权限,请联系MM管理员,在EPEP01画面配置代码为[M09M]的工号!", (const char*)tep0002.CODE);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获得传入参数 */
		/* 维护事件表 */
		// 新增事件
		if (bcls_rec->Tables.IndexOf("MM0097A1_97_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_97_INS"].Rows.get_Count(); i++)
			{
				tmm0097.Reset();
				tmm0099.Reset();
				tmm0097.MergeFrom(bcls_rec->Tables["MM0097A1_97_INS"].Rows[i]);
				tmm0097.TrimOrBlank();

				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_ID		= [{0}]", tmm0097.EVENT_ID);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.MAT_KIND		= [{0}]", tmm0097.MAT_KIND);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_LINE_TYPE	= [{0}]", tmm0097.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_0= [{0}]", tmm0097.EVENT_PROC_WAY_0);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_1 = [{0}]", tmm0097.EVENT_PROC_WAY_1);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_2 = [{0}]", tmm0097.EVENT_PROC_WAY_2);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_3 = [{0}]", tmm0097.EVENT_PROC_WAY_3);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_4 = [{0}]", tmm0097.EVENT_PROC_WAY_4);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_AC= [{0}]", tmm0097.EVENT_PROC_WAY_AC);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_KC= [{0}]", tmm0097.EVENT_PROC_WAY_KC);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_5 = [{0}]", tmm0097.EVENT_PROC_WAY_5);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_6 = [{0}]", tmm0097.EVENT_PROC_WAY_6);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_7 = [{0}]", tmm0097.EVENT_PROC_WAY_7);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_8 = [{0}]", tmm0097.EVENT_PROC_WAY_8);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_9 = [{0}]", tmm0097.EVENT_PROC_WAY_9);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_9 = [{0}]", tmm0097.EVENT_PROC_WAY_10);
				Log::Info("", __FUNCTION__, "新增事件 tmm0097.EVENT_PROC_WAY_WM = [{0}]", tmm0097.EVENT_PROC_WAY_WM);

				/* 检查输入参数合法性 */
				if (tmm0097.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.TC_SEND_FLAG.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文发送标记不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_TYPE.Trim() == "")
				{
					strcpy(s.msg, "事件性质不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_LOG_SWITCH.Trim() == "")
				{
					strcpy(s.msg, "打印TRACE开关不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmm0097.EVENT_PROC_WAY_1.Trim() == "1"	//从历史档返回
					&& tmm0097.EVENT_PROC_WAY_6.Trim() == "1")	//归历史档
				{
					strcpy(s.msg, "从历史档返回和归历史档,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_PROC_WAY_1.Trim() == "1"	//从历史档返回
					&& tmm0097.EVENT_PROC_WAY_6.Trim() == "1")	//材料删除
				{
					strcpy(s.msg, "从历史档返回和材料删除,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_PROC_WAY_6.Trim() == "1"	//归历史档
					&& tmm0097.EVENT_PROC_WAY_7.Trim() == "1")	//材料删除
				{
					strcpy(s.msg, "归历史档和材料删除,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if ((tmm0097.EVENT_CALL_TYPE_CODE.Trim() == "11"
					|| tmm0097.EVENT_CALL_TYPE_CODE.Trim() == "2")	//事件调用类型有单独函数 11-配置&单独函数；2-单独函数
					&& tmm0097.EVENT_SPEC_PRO.Trim() == "")
				{
					sprintf(s.msg, "事件号[%s]物料[%s]产线类型[%s]调用类型是[配置&单独函数],事件特殊处理标记需配置!",
						(const char*)tmm0097.EVENT_ID, (const char*)tmm0097.MAT_KIND, (const char*)tmm0097.EVENT_LINE_TYPE);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* 校验主键不能重复 */
				if (tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE") > 0)
				{
					strcpy(s.msg, "主键 事件号[" + tmm0097.EVENT_ID + "]种类[" + tmm0097.MAT_KIND + "]产线[" + tmm0097.EVENT_LINE_TYPE + "]重复!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 设置事件处理规则 */
				tmm0097.EVENT_PROC_WAY_0 = "0";
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY_0;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_1;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_2;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_3;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_4;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_AC;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_KC;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_5;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_6;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_7;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_8;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_9;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_10;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_WM;

				Log::Info("", __FUNCTION__, "设置事件处理规则 tmm0097.EVENT_PROC_WAY	= [{0}]", tmm0097.EVENT_PROC_WAY);

				/* 非全产线事件,校验事件处理规则需与全产线相同 */
				if (tmm0097.EVENT_LINE_TYPE.Trim() != "00"
					&& tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_PROC_WAY") < 0)  //新增的产线的业务规则与全产线不同
				{
					strcpy(s.msg, "事件号[" + tmm0097.EVENT_ID + "]种类[" + tmm0097.MAT_KIND + "]产线[" + tmm0097.EVENT_LINE_TYPE + "]处理规则[" + tmm0097.EVENT_PROC_WAY + "]必须于全产线相同!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 新增事件信息 */
				tmm0097.REC_CREATOR = s.userid;   //记录创建责任者
				tmm0097.REC_CREATE_TIME = datetime;   //记录创建时刻
				tmm0097.TrimOrBlank();
				tmm0097.Insert();

				/* 设置事件接口表流水号 EVENT_ITEM_SEQ_NO */
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT MAX(EVENT_ITEM_SEQ_NO) "
						"  FROM TMM0099 ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0099.EVENT_ITEM_SEQ_NO = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				/* 自动新增事件接口信息 */
				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;
				tmm0099.EVENT_ID = tmm0097.EVENT_ID;
				tmm0099.MAT_KIND = tmm0097.MAT_KIND;
				tmm0099.EVENT_LINE_TYPE = tmm0097.EVENT_LINE_TYPE;
				tmm0099.SEQ_NO = 1;
				tmm0099.ITEM_ENAME = "EVENT_ID";
				tmm0099.ITEM_CNAME = "事件标识";
				tmm0099.ITEM_PARA = "Y";
				tmm0099.ITEM_PARA_ALLOW_NULL = "N";
				tmm0099.ITEM_UPD_TYPE = "1";
				tmm0099.ITEM_UPD_MODE = "0";
				tmm0099.ITEM_PROC_VALUE = "";
				tmm0099.ITEM_DESC = "";
				tmm0099.ITEM_KIND = "S";
				tmm0099.ITEM_TYPE = "C";
				tmm0099.ITEM_LEN = "4";
				tmm0099.REC_CREATOR = s.userid;   //记录创建责任者
				tmm0099.REC_CREATE_TIME = datetime;   //记录创建时刻
				tmm0099.TrimOrBlank();
				tmm0099.Insert();

				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;
				tmm0099.SEQ_NO = 2;
				tmm0099.ITEM_ENAME = "EVENT_LINE_TYPE";
				tmm0099.ITEM_CNAME = "事件产线类型";
				tmm0099.ITEM_PARA = "Y";
				tmm0099.ITEM_PARA_ALLOW_NULL = "N";
				tmm0099.ITEM_UPD_TYPE = "1";
				tmm0099.ITEM_UPD_MODE = "0";
				tmm0099.ITEM_PROC_VALUE = "";
				if (tmm0097.EVENT_LINE_TYPE.Trim() == "00")
				{
					tmm0099.ITEM_DESC = "传入[00]全产线";
				}
				else if (tmm0097.EVENT_LINE_TYPE.Trim() == "SM")
				{
					tmm0099.ITEM_DESC = "传入[SM]炼钢侧";
				}
				else if (tmm0097.EVENT_LINE_TYPE.Trim() == "HR")
				{
					tmm0099.ITEM_DESC = "传入[HR]热轧侧";
				}
				else if (tmm0097.EVENT_LINE_TYPE.Trim() == "CR")
				{
					tmm0099.ITEM_DESC = "传入[CR]冷轧侧";
				}
				else if (tmm0097.EVENT_LINE_TYPE.Trim() == "BW")
				{
					tmm0099.ITEM_DESC = "传入[BW]棒线侧";
				}
				else if (tmm0097.EVENT_LINE_TYPE.Trim() == "HP")
				{
					tmm0099.ITEM_DESC = "传入[HP]厚板侧";
				}
				else
				{
				}
				tmm0099.ITEM_KIND = "S";
				tmm0099.ITEM_TYPE = "C";
				tmm0099.ITEM_LEN = "2";
				tmm0099.TrimOrBlank();
				tmm0099.Insert();

				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;
				tmm0099.SEQ_NO = 3;
				tmm0099.ITEM_ENAME = "SYSTEM_ID";
				tmm0099.ITEM_CNAME = "系统标识";
				tmm0099.ITEM_PARA = "Y";
				tmm0099.ITEM_PARA_ALLOW_NULL = "N";
				tmm0099.ITEM_UPD_TYPE = "1";
				tmm0099.ITEM_UPD_MODE = "0";
				tmm0099.ITEM_PROC_VALUE = "";
				tmm0099.ITEM_DESC = "二级模块名";
				tmm0099.ITEM_KIND = "S";
				tmm0099.ITEM_TYPE = "C";
				tmm0099.ITEM_LEN = "4";
				tmm0099.TrimOrBlank();
				tmm0099.Insert();

				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;
				tmm0099.SEQ_NO = 4;
				tmm0099.ITEM_ENAME = "FUNC_ID";
				tmm0099.ITEM_CNAME = "功能标识";
				tmm0099.ITEM_PARA = "Y";
				tmm0099.ITEM_PARA_ALLOW_NULL = "N";
				tmm0099.ITEM_UPD_TYPE = "1";
				tmm0099.ITEM_UPD_MODE = "0";
				tmm0099.ITEM_PROC_VALUE = "";
				tmm0099.ITEM_DESC = "调用程序名";
				tmm0099.ITEM_KIND = "S";
				tmm0099.ITEM_TYPE = "C";
				tmm0099.ITEM_LEN = "30";
				tmm0099.TrimOrBlank();
				tmm0099.Insert();

				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;
				tmm0099.SEQ_NO = 5;
				tmm0099.ITEM_ENAME = "MAT_NO";
				tmm0099.ITEM_CNAME = "材料号";
				tmm0099.ITEM_PARA = "Y";
				tmm0099.ITEM_PARA_ALLOW_NULL = "N";
				tmm0099.ITEM_UPD_TYPE = "1";
				tmm0099.ITEM_UPD_MODE = "0";
				tmm0099.ITEM_PROC_VALUE = "";
				tmm0099.ITEM_DESC = "";
				tmm0099.ITEM_KIND = "S";
				tmm0099.ITEM_TYPE = "C";
				tmm0099.ITEM_LEN = "20";
				tmm0099.TrimOrBlank();
				tmm0099.Insert();
			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MM0097A1_97_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_97_UPD"].Rows.get_Count(); i++)
			{
				tmm0097.Reset();
				tmm0099.Reset();
				tmm0097.MergeFrom(bcls_rec->Tables["MM0097A1_97_UPD"].Rows[i]);
				tmm0097.TrimOrBlank();

				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_ID		= [{0}]", tmm0097.EVENT_ID);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.MAT_KIND		= [{0}]", tmm0097.MAT_KIND);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_LINE_TYPE	= [{0}]", tmm0097.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_0= [{0}]", tmm0097.EVENT_PROC_WAY_0);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_1 = [{0}]", tmm0097.EVENT_PROC_WAY_1);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_2 = [{0}]", tmm0097.EVENT_PROC_WAY_2);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_3 = [{0}]", tmm0097.EVENT_PROC_WAY_3);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_4 = [{0}]", tmm0097.EVENT_PROC_WAY_4);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_AC= [{0}]", tmm0097.EVENT_PROC_WAY_AC);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_KC= [{0}]", tmm0097.EVENT_PROC_WAY_KC);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_5 = [{0}]", tmm0097.EVENT_PROC_WAY_5);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_6 = [{0}]", tmm0097.EVENT_PROC_WAY_6);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_7 = [{0}]", tmm0097.EVENT_PROC_WAY_7);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_8 = [{0}]", tmm0097.EVENT_PROC_WAY_8);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_9 = [{0}]", tmm0097.EVENT_PROC_WAY_9);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_10 = [{0}]", tmm0097.EVENT_PROC_WAY_10);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_11 = [{0}]", tmm0097.EVENT_PROC_WAY_11);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_12 = [{0}]", tmm0097.EVENT_PROC_WAY_12);
				Log::Info("", __FUNCTION__, "修改事件 tmm0097.EVENT_PROC_WAY_WM = [{0}]", tmm0097.EVENT_PROC_WAY_WM);

				/* 检查输入参数合法性 */
				if (tmm0097.EVENT_ID.Trim() == "")
				{
					//sprintf(s.msg, _RES("GCRSS0000035")/*事件号不能为空*/);
					strcpy(s.msg, "事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.TC_SEND_FLAG.Trim() == "")
				{
					strcpy(s.msg, "电文发送标记不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_TYPE.Trim() == "")
				{
					strcpy(s.msg, "事件性质不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_LOG_SWITCH.Trim() == "")
				{
					strcpy(s.msg, "打印TRACE开关不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmm0097.EVENT_PROC_WAY_1.Trim() == "1"	//从历史档返回
					&& tmm0097.EVENT_PROC_WAY_6.Trim() == "1")	//归历史档
				{
					strcpy(s.msg, "从历史档返回和归历史档,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_PROC_WAY_1.Trim() == "1"	//从历史档返回
					&& tmm0097.EVENT_PROC_WAY_6.Trim() == "1")	//材料删除
				{
					strcpy(s.msg, "从历史档返回和材料删除,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_PROC_WAY_6.Trim() == "1"	//归历史档
					&& tmm0097.EVENT_PROC_WAY_7.Trim() == "1")	//材料删除
				{
					strcpy(s.msg, "归历史档和材料删除,从业务逻辑上考虑,不能并行!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if ((tmm0097.EVENT_CALL_TYPE_CODE.Trim() == "11"
					|| tmm0097.EVENT_CALL_TYPE_CODE.Trim() == "2")	//事件调用类型有单独函数 11-配置&单独函数；2-单独函数
					&& tmm0097.EVENT_SPEC_PRO.Trim() == "")
				{
					sprintf(s.msg, "事件号[%s]物料[%s]产线类型[%s]调用类型是[配置&单独函数],事件特殊处理标记需配置!",
						(const char*)tmm0097.EVENT_ID, (const char*)tmm0097.MAT_KIND, (const char*)tmm0097.EVENT_LINE_TYPE);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* 设置事件处理规则 */
				tmm0097.EVENT_PROC_WAY_0 = "0";
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY_0;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_1;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_2;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_3;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_4;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_AC;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_KC;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_5;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_6;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_7;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_8;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_9;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_10;
				tmm0097.EVENT_PROC_WAY = tmm0097.EVENT_PROC_WAY + tmm0097.EVENT_PROC_WAY_WM;

				Log::Info("", __FUNCTION__, "设置事件处理规则 tmm0097.EVENT_PROC_WAY	= [{0}]", tmm0097.EVENT_PROC_WAY);

				/* 非全产线事件,校验事件处理规则需与全产线相同 */
				if (tmm0097.EVENT_LINE_TYPE.Trim() != "00"
					&& tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_PROC_WAY") < 0)  //新增的产线的业务规则与全产线不同
				{
					strcpy(s.msg, "事件号[" + tmm0097.EVENT_ID + "]种类[" + tmm0097.MAT_KIND + "]产线[" + tmm0097.EVENT_LINE_TYPE + "]处理规则[" + tmm0097.EVENT_PROC_WAY + "]必须于全产线相同!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 修改事件信息 */
				tmm0097.REC_REVISOR = s.userid;
				tmm0097.REC_REVISE_TIME = datetime;
				tmm0097.TrimOrBlank();
				tmm0097.Update("EVENT_NAME,"
					"EVENT_SUB_SYSTEM,"
					"EVENT_USE_FLAG,"
					"EVENT_CALL_TYPE_CODE,"
					"EVENT_CALL_FUNC,"
					"EVENT_PROC_WAY_0,"
					"EVENT_PROC_WAY_1,"
					"EVENT_PROC_WAY_2,"
					"EVENT_PROC_WAY_3,"
					"EVENT_PROC_WAY_4,"
					"EVENT_PROC_WAY_AC,"
					"EVENT_PROC_WAY_KC,"
					"EVENT_PROC_WAY_5,"
					"EVENT_PROC_WAY_6,"
					"EVENT_PROC_WAY_7,"
					"EVENT_PROC_WAY_8,"
					"EVENT_PROC_WAY_9,"
					"EVENT_PROC_WAY_10,"
					"EVENT_PROC_WAY_WM,"
					"EVENT_DESC,"
					"KEYVALUE_1,"
					"KEYVALUE_1_DESC,"
					"EVENT_PROC_WAY,"
					"TC_SEND_FLAG,"
					"TC_NO,"
					"EVENT_TYPE,"
					"EVENT_SYS,"
					"EVENT_LOG_SWITCH,"
					"EVENT_SPEC_PRO,"
					"KEYVALUE_2,"
					"KEYVALUE_2_DESC,"
					"KEYVALUE_3,"
					"KEYVALUE_3_DESC,"
					"KEYVALUE_4,"
					"KEYVALUE_4_DESC,"
					"KEYVALUE_5,"
					"KEYVALUE_5_DESC,"
					"KEYVALUE_6,"
					"KEYVALUE_6_DESC,"
					"KEYVALUE_1_AC,"
					"KEYVALUE_2_AC,"
					"KEYVALUE_1_WM,"
					"REC_REVISOR,"
					"REC_REVISE_TIME",
					"EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");
			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MM0097A1_97_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_97_DEL"].Rows.get_Count(); i++)
			{
				tmm0097.Reset();
				tmm0099.Reset();
				tmm009a.Reset();
				tmm0097.MergeFrom(bcls_rec->Tables["MM0097A1_97_DEL"].Rows[i]);
				tmm0097.TrimOrBlank();

				Log::Info("", __FUNCTION__, "删除事件 tmm0097.EVENT_ID		= [{0}]", tmm0097.EVENT_ID);
				Log::Info("", __FUNCTION__, "删除事件 tmm0097.MAT_KIND		= [{0}]", tmm0097.MAT_KIND);
				Log::Info("", __FUNCTION__, "删除事件 tmm0097.EVENT_LINE_TYPE	= [{0}]", tmm0097.EVENT_LINE_TYPE);

				/* 检查输入参数合法性 */
				if (tmm0097.EVENT_ID.Trim() == "")
				{
					//sprintf(s.msg, _RES("GCRSS0000035")/*事件号不能为空*/);
					strcpy(s.msg, "事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0097.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 删除事件信息 */
				tmm0097.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");

				/* 删除事件参数信息 */
				tmm0099.EVENT_ID = tmm0097.EVENT_ID;
				tmm0099.MAT_KIND = tmm0097.MAT_KIND;
				tmm0099.EVENT_LINE_TYPE = tmm0097.EVENT_LINE_TYPE;
				tmm0099.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");

				/* 删除电文配置表 */
				tmm009a.EVENT_ID = tmm0097.EVENT_ID;
				tmm009a.MAT_KIND = tmm0097.MAT_KIND;
				tmm009a.EVENT_LINE_TYPE = tmm0097.EVENT_LINE_TYPE;
				tmm009a.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");

			}
		}

		/* 维护接口参数表 */
		// 新增事件接口参数表
		if (bcls_rec->Tables.IndexOf("MM0097A1_99_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_99_INS"].Rows.get_Count(); i++)
			{
				tmm0099.Reset();
				tmm0099.MergeFrom(bcls_rec->Tables["MM0097A1_99_INS"].Rows[i]);
				tmm0099.TrimOrBlank();

				Log::Info("", __FUNCTION__, "新增事件接口参数表 tmm0099.EVENT_ID		= [{0}]", tmm0099.EVENT_ID);
				Log::Info("", __FUNCTION__, "新增事件接口参数表 tmm0099.MAT_KIND		= [{0}]", tmm0099.MAT_KIND);
				Log::Info("", __FUNCTION__, "新增事件接口参数表 tmm0099.EVENT_LINE_TYPE	= [{0}]", tmm0099.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "新增事件接口参数表 tmm0099.SEQ_NO		= [{0}]", tmm0099.SEQ_NO);

				/* 检查输入参数合法性 */
				if (tmm0099.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_PARA.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段接口参数不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_TYPE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段修改类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段修改方式不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "1"
					&& tmm0099.ITEM_PARA_ALLOW_NULL.Trim() == "")	//1-接口值
				{
					strcpy(s.msg, "字段" + tmm0099.ITEM_ENAME + "修改方式是接口值,需设置参数是否为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if ((tmm0099.ITEM_UPD_MODE.Trim() == "2"	//2-固定值
					|| tmm0099.ITEM_UPD_MODE.Trim() == "3"	//3-表中字段
					|| tmm0099.ITEM_UPD_MODE.Trim() == "5")//5-按字段设置值中字段的接口值修改
					&& tmm0099.ITEM_PROC_VALUE.Trim() == "")//字段设置值
				{
					strcpy(s.msg, "字段" + tmm0099.ITEM_ENAME + "修改方式是[" + tmm0099.ITEM_UPD_MODE + "],字段设置值[" + tmm0099.ITEM_PROC_VALUE + "]不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//校验相同事件的字段英文名不能重复
				if (tmm0099.QueryCount("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE,ITEM_ENAME") > 0)	//字段修改类型是3-修改类
				{
					strcpy(s.msg, "事件号[" + tmm0099.EVENT_ID + "]新增的字段[" + tmm0099.ITEM_ENAME + "]在该事件中已配置,不能重复新增!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//设置修改主档类的字段，事件的事件处理规则需勾选 修改类
				if (tmm0099.ITEM_UPD_TYPE.Trim() == "3")	//字段修改类型是3-修改类
				{
					tmm0097.EVENT_ID = tmm0099.EVENT_ID;
					tmm0097.MAT_KIND = tmm0099.MAT_KIND;
					tmm0097.EVENT_LINE_TYPE = tmm0099.EVENT_LINE_TYPE;
					tmm0097.EVENT_PROC_WAY_3 = "1";						//事件处理规则_修改类是否勾选
					if (tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE,EVENT_PROC_WAY_3") <= 0)
					{
						strcpy(s.msg, "事件号[" + tmm0099.EVENT_ID + "]新增的字段[" + tmm0099.ITEM_ENAME + "]修改类型为修改类,需将该事件的事件信息栏的[修改类]勾选!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				/* 设置默认值 */
				if (tmm0099.ITEM_UPD_MODE.Trim() == "1")	//1-接口值 
				{
					tmm0099.ITEM_PARA = "Y";	//Y-抛帐参数
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "2"	//2-固定值
					|| tmm0099.ITEM_UPD_MODE.Trim() == "3"	//3-表中字段
					|| tmm0099.ITEM_UPD_MODE.Trim() == "5")	//5-按字段设置值中字段的接口值修改
				{
					tmm0099.ITEM_PARA = "N";	//N-抛帐参数
					tmm0099.ITEM_PARA_ALLOW_NULL = "";
				}

				/* 设置事件接口表流水号 EVENT_ITEM_SEQ_NO */
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT MAX(EVENT_ITEM_SEQ_NO) "
						"  FROM TMM0099 ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0099.EVENT_ITEM_SEQ_NO = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				tmm0099.EVENT_ITEM_SEQ_NO = tmm0099.EVENT_ITEM_SEQ_NO + 1;


				/* 新增事件接口参数表信息 */
				tmm0099.REC_CREATOR = s.userid;   //记录创建责任者
				tmm0099.REC_CREATE_TIME = datetime;   //记录创建时刻
				tmm0099.TrimOrBlank();
				tmm0099.Insert();
			}
		}

		// 修改事件接口参数表
		if (bcls_rec->Tables.IndexOf("MM0097A1_99_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_99_UPD"].Rows.get_Count(); i++)
			{
				tmm0099.Reset();
				tmm0099.MergeFrom(bcls_rec->Tables["MM0097A1_99_UPD"].Rows[i]);
				tmm0099.TrimOrBlank();

				Log::Info("", __FUNCTION__, "修改事件接口参数表 tmm0099.EVENT_ID		= [{0}]", tmm0099.EVENT_ID);
				Log::Info("", __FUNCTION__, "修改事件接口参数表 tmm0099.MAT_KIND		= [{0}]", tmm0099.MAT_KIND);
				Log::Info("", __FUNCTION__, "修改事件接口参数表 tmm0099.EVENT_LINE_TYPE	= [{0}]", tmm0099.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "修改事件接口参数表 tmm0099.EVENT_ITEM_SEQ_NO	= [{0}]", tmm0099.EVENT_ITEM_SEQ_NO);

				/* 检查输入参数合法性 */
				if (tmm0099.EVENT_ITEM_SEQ_NO <= 0)
				{
					strcpy(s.msg, "接口参数表的事件接口表流水号不能小于0。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_PARA.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段接口参数不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_TYPE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段修改类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的字段修改方式不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "1"
					&& tmm0099.ITEM_PARA_ALLOW_NULL.Trim() == "")	//1-接口值
				{
					strcpy(s.msg, "字段" + tmm0099.ITEM_ENAME + "修改方式是接口值,需设置参数是否为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if ((tmm0099.ITEM_UPD_MODE.Trim() == "2"	//2-固定值
					|| tmm0099.ITEM_UPD_MODE.Trim() == "3"	//3-表中字段
					|| tmm0099.ITEM_UPD_MODE.Trim() == "5")//5-按字段设置值中字段的接口值修改
					&& tmm0099.ITEM_PROC_VALUE.Trim() == "")//字段设置值
				{
					strcpy(s.msg, "字段" + tmm0099.ITEM_ENAME + "修改方式是[" + tmm0099.ITEM_UPD_MODE + "],字段设置值[" + tmm0099.ITEM_PROC_VALUE + "]不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 设置默认值 */
				if (tmm0099.ITEM_UPD_MODE.Trim() == "1")	//1-接口值 
				{
					tmm0099.ITEM_PARA = "Y";	//Y-抛帐参数
				}
				if (tmm0099.ITEM_UPD_MODE.Trim() == "2"	//2-固定值
					|| tmm0099.ITEM_UPD_MODE.Trim() == "3"	//3-表中字段
					|| tmm0099.ITEM_UPD_MODE.Trim() == "5")	//5-按字段设置值中字段的接口值修改
				{
					tmm0099.ITEM_PARA = "N";	//N-抛帐参数
					tmm0099.ITEM_PARA_ALLOW_NULL = "";
				}

				/* 修改事件接口参数表信息 */
				tmm0099.REC_REVISOR = s.userid;
				tmm0099.REC_REVISE_TIME = datetime;

				tmm0099.TrimOrBlank();
				tmm0099.Update("EVENT_ID,"
					"MAT_KIND,"
					"EVENT_LINE_TYPE,"
					"SEQ_NO,"
					"ITEM_ENAME,"
					"ITEM_CNAME,"
					"ITEM_PARA,"
					"ITEM_PARA_ALLOW_NULL,"
					"ITEM_UPD_TYPE,"
					"ITEM_UPD_MODE,"
					"ITEM_PROC_VALUE,"
					"ITEM_DESC,"
					"ITEM_KIND,"
					"ITEM_TYPE,"
					"ITEM_LEN,"
					"ITEM_CHECK_VALUE,"
					"DEFAULT_VALUE,"
					"REC_REVISOR,"
					"REC_REVISE_TIME",
					"EVENT_ID,EVENT_ITEM_SEQ_NO");
			}
		}

		// 删除事件接口参数表
		if (bcls_rec->Tables.IndexOf("MM0097A1_99_DEL") >= 0)
		{
			// 删除事件接口参数表
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_99_DEL"].Rows.get_Count(); i++)
			{
				tmm0099.Reset();
				tmm0099.MergeFrom(bcls_rec->Tables["MM0097A1_99_DEL"].Rows[i]);
				tmm0099.TrimOrBlank();

				Log::Info("", __FUNCTION__, "删除事件接口参数表 tmm0099.EVENT_ID		= [{0}]", tmm0099.EVENT_ID);
				Log::Info("", __FUNCTION__, "删除事件接口参数表 tmm0099.MAT_KIND		= [{0}]", tmm0099.MAT_KIND);
				Log::Info("", __FUNCTION__, "删除事件接口参数表 tmm0099.EVENT_LINE_TYPE	= [{0}]", tmm0099.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "删除事件接口参数表 tmm0099.EVENT_ITEM_SEQ_NO	= [{0}]", tmm0099.EVENT_ITEM_SEQ_NO);

				/* 检查输入参数合法性 */
				if (tmm0099.EVENT_ITEM_SEQ_NO <= 0)
				{
					strcpy(s.msg, "接口参数表的事件接口表流水号不能小于0。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm0099.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "接口参数表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 删除事件接口参数表信息 */
				tmm0099.Delete("EVENT_ITEM_SEQ_NO");
			}
		}

		/* 维护事件电文配置表 */
		// 新增事件电文配置表
		if (bcls_rec->Tables.IndexOf("MM0097A1_9A_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_9A_INS"].Rows.get_Count(); i++)
			{
				tmm009a.Reset();
				tmm009a.MergeFrom(bcls_rec->Tables["MM0097A1_9A_INS"].Rows[i]);
				tmm009a.TrimOrBlank();

				Log::Info("", __FUNCTION__, "新增事件电文配置表 tmm009a.EVENT_ID		= [{0}]", tmm009a.EVENT_ID);
				Log::Info("", __FUNCTION__, "新增事件电文配置表 tmm009a.MAT_KIND		= [{0}]", tmm009a.MAT_KIND);
				Log::Info("", __FUNCTION__, "新增事件电文配置表 tmm009a.EVENT_LINE_TYPE	= [{0}]", tmm009a.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "新增事件电文配置表 tmm009a.TC_KEYVALUE		= [{0}]", tmm009a.TC_KEYVALUE);

				/* 检查输入参数合法性 */
				if (tmm009a.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_KEYVALUE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文关键值不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文发送标记不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "Y"	//发送电文
					&& tmm009a.TC_NO.Trim() == "")	//电文号
				{
					strcpy(s.msg, "设置发送电文时,电文号不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "Y"	//发送电文
					&& tmm009a.TC_DATA_TRANS_MODE.Trim() == "")	//电文数据传输方式
				{
					strcpy(s.msg, "设置发送电文时,电文数据传输方式不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 设置默认值 暂时 在2019年后删除这3个列 */
				tmm009a.TC_BACKLOG = tmm009a.TC_KEYVALUE;			//电文工序值
				tmm009a.MM_TABLE_ENAME = tmm009a.TC_KEYVALUE_CNAME;	//实绩表英文名
				tmm009a.MM_TABLE_CNAME = tmm009a.TC_KEYVALUE_ENAME;	//实绩表中文名

				/* 新增事件电文配置表信息 */
				tmm009a.REC_CREATOR = s.userid;   //记录创建责任者
				tmm009a.REC_CREATE_TIME = datetime;   //记录创建时刻
				tmm009a.TrimOrBlank();
				tmm009a.Insert();
			}
		}

		// 修改事件电文配置表
		if (bcls_rec->Tables.IndexOf("MM0097A1_9A_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_9A_UPD"].Rows.get_Count(); i++)
			{
				tmm009a.Reset();
				tmm009a.MergeFrom(bcls_rec->Tables["MM0097A1_9A_UPD"].Rows[i]);
				tmm009a.TrimOrBlank();

				Log::Info("", __FUNCTION__, "修改事件电文配置表 tmm009a.EVENT_ID		= [{0}]", tmm009a.EVENT_ID);
				Log::Info("", __FUNCTION__, "修改事件电文配置表 tmm009a.MAT_KIND		= [{0}]", tmm009a.MAT_KIND);
				Log::Info("", __FUNCTION__, "修改事件电文配置表 tmm009a.EVENT_LINE_TYPE	= [{0}]", tmm009a.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "修改事件电文配置表 tmm009a.TC_KEYVALUE	= [{0}]", tmm009a.TC_KEYVALUE);

				/* 检查输入参数合法性 */
				if (tmm009a.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_KEYVALUE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文关键值不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文发送标记不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "Y"	//发送电文
					&& tmm009a.TC_NO.Trim() == "")	//电文号
				{
					strcpy(s.msg, "设置发送电文时,电文号不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_SEND_FLAG.Trim() == "Y"	//发送电文
					&& tmm009a.TC_DATA_TRANS_MODE.Trim() == "")	//电文数据传输方式
				{
					strcpy(s.msg, "设置发送电文时,电文数据传输方式不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 设置默认值 暂时 在2019年后删除 */
				tmm009a.MM_TABLE_ENAME = tmm009a.TC_KEYVALUE_CNAME;	//实绩表英文名
				tmm009a.MM_TABLE_CNAME = tmm009a.TC_KEYVALUE_ENAME;	//实绩表中文名

				/* 修改事件电文配置表信息 */
				tmm009a.REC_REVISOR = s.userid;
				tmm009a.REC_REVISE_TIME = datetime;

				tmm009a.TrimOrBlank();
				tmm009a.Update("TC_NO,"
					"TC_SEND_FLAG,"
					"TC_DATA_TRANS_MODE,"
					"TC_KEYVALUE_CNAME,"
					"TC_KEYVALUE_ENAME,"
					"TC_KEYVALUE_TYPE,"
					"EVENT_DESC,"
					"MM_TABLE_ENAME,"
					"MM_TABLE_CNAME,"
					"REC_REVISOR,"
					"REC_REVISE_TIME",
					"EVENT_ID,MAT_KIND,EVENT_LINE_TYPE,TC_KEYVALUE");
			}
		}

		// 删除事件电文配置表
		if (bcls_rec->Tables.IndexOf("MM0097A1_9A_DEL") >= 0)
		{
			// 删除事件事件电文配置表
			for (int i = 0; i < bcls_rec->Tables["MM0097A1_9A_DEL"].Rows.get_Count(); i++)
			{
				tmm009a.Reset();
				tmm009a.MergeFrom(bcls_rec->Tables["MM0097A1_9A_DEL"].Rows[i]);
				tmm009a.TrimOrBlank();

				Log::Info("", __FUNCTION__, "删除事件电文配置表 tmm009a.EVENT_ID		= [{0}]", tmm009a.EVENT_ID);
				Log::Info("", __FUNCTION__, "删除事件电文配置表 tmm009a.MAT_KIND		= [{0}]", tmm009a.MAT_KIND);
				Log::Info("", __FUNCTION__, "删除事件电文配置表 tmm009a.EVENT_LINE_TYPE	= [{0}]", tmm009a.EVENT_LINE_TYPE);
				Log::Info("", __FUNCTION__, "删除事件电文配置表 tmm009a.TC_KEYVALUE	= [{0}]", tmm009a.TC_KEYVALUE);

				/* 检查输入参数合法性 */
				if (tmm009a.EVENT_ID.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的事件号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.MAT_KIND.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的物料种类不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.EVENT_LINE_TYPE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的产线类型不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmm009a.TC_KEYVALUE.Trim() == "")
				{
					strcpy(s.msg, "事件电文配置表的电文关键值不能为空。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 删除事件事件电文配置表信息 */
				tmm009a.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE,TC_KEYVALUE");
			}
		}

		Log::Info("", __FUNCTION__, "事件维护时,校验参数配置表中修改类字段 tmm0097.EVENT_ID	= [{0}]", tmm0097.EVENT_ID);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

