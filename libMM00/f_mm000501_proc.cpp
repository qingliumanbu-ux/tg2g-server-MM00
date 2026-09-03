/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪路径新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪路径新增
/// <para>
/// 1.根据传入参数生产TMM0005表
/// 2.按材料号查询当前档和历史档,录入材料信息
/// PASS_BACKLOG_SEQ_NO:第1个母材料号从1开始
/// IN_MAT_ID:第1个母材料无入口材料号,其他均必须有入口材料号
/// MAT_ARCHIVE_FLAG://00-当前档；10-历史档；01-红冲回当前档(目前不记录该标记)
/// 亲属码的用法
根据 MAT_PROD_FLAG 材料产出标记 判断//10-原料录入;20-机组产出;21-并卷;22-分卷;;31-半卷回退;50-清盘库
一、材料产出标记是 10-原料录入;50-清盘库;20-机组产出且入口材料为空,通常是炼钢板坯产出 时，
亲属码默认是01
二、材料产出标记是20-机组产出且入口材料不为空;21-并卷&&主材料标记是1-主材料；22-分卷;;31-半卷回退;
亲属码是IN_MAT_ID到TMM0005表读 记录数+1，然后前拼0
三、材料产出标记是 21-并卷&&主材料标记是0
亲属码是主材料那条记录的亲属码。
注：并卷情况，TMM0005表有2条记录的，出口材料号相同，入口材料号不同，材料号1有主材料标记。主材料的那条记录，先写入TMM0005表。并卷情况在实绩表之类的都是一条。
/// <para>
/// </summary>
/// <param name="MAT_NO">材料号  必输</param>
/// <param name="MAT_KIND">材料类型  必输</param>//SM、CR、HR、HP
/// <param name="IN_MAT_ID">入口材料标识号  必输</param>
/// <param name="IN_MAT_KIND">入口物料种类 必输</param>//SM、CR、HR、HP
/// <param name="MAT_PROD_FLAG">材料产出标记  必输</param>//10-原料录入;20-机组产出;21-并卷;22-分卷;(30-整卷回退-暂不用);31-半卷回退;50-清盘库
/// <param name="MAIN_MAT_FLAG">主材料标记  必输</param>//0-非主材料,1-主材料,针对并卷
/// <param name="SPECAIL_FLAG">特殊标记 非必输</param>//0-默认值,1-并卷同时分卷
/// <returns>新增路径表</returns>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 


#endif
#if defined(_LINE_HR) || defined(_LINE_CR)
//#include "tmmhr01.h"
//#include "hmmhr01.h"
#endif
#if defined(_LINE_CR)
//#include "tmmcr01.h"
//#include "hmmcr01.h"
#endif
#if defined(_LINE_HP)
//#include "tmmhp01.h"
//#include "hmmhp01.h"
#endif
BM2_FUNCTION_IMPORT
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

BM2_FUNCTION_EXPORT
int f_mm000501_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_mm_resume_seq_no("");
	CString	cs_h_mat_no("");
	CDecimal cd_count = 0;

	/* 实体类定义 */
	CModel tmm0005("TMM0005");
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
#endif
#if defined(_LINE_HR) || defined(_LINE_CR)
	CModel tmmhr01("TMMHR01");
	CModel hmmhr01("HMMHR01");
/*	CTMMHR01 tmmhr01(conn);
	CHMMHR01 hmmhr01(conn)*/;
#endif
#if defined(_LINE_CR)
	CModel tmmcr01("TMMCR01");
	CModel hmmcr01("HMMCR01");
	/*CTMMCR01 tmmcr01(conn);
	CHMMCR01 hmmcr01(conn);*/
#endif
#if defined(_LINE_HP)
	CModel tmmhp01("TMMHP01");
	CModel hmmhp01("HMMHP01");
	/*CTMMHP01 tmmhp01(conn);
	CHMMHP01 hmmhp01(conn);*/
#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MM000501");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MM000501 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables["MM000501"].Rows.get_Count(); i++)
		{
			/* 取得单行传入信息 */
			tmm0005.Reset();
			tmm0005.MergeFrom(bcls_rec->Tables["MM000501"].Rows[i]);
			tmm0005.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.MAT_NO			= [{0}]", (const char*)tmm0005["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.MAT_KIND		    = [{0}]", (const char*)tmm0005["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.MAIN_MAT_FLAG	= [{0}]", (const char*)tmm0005["MAIN_MAT_FLAG"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.MAT_PROD_FLAG	= [{0}]", (const char*)tmm0005["MAT_PROD_FLAG"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.IN_MAT_NO		= [{0}]", (const char*)tmm0005["IN_MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.IN_MAT_ID		= [{0}]", (const char*)tmm0005["IN_MAT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.IN_MAT_KIND		= [{0}]", (const char*)tmm0005["IN_MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0005.SPECAIL_FLAG	    = [{0}]", (const char*)tmm0005["SPECAIL_FLAG"].ToString());

			/* 检查输入参数合法性 */
			if (tmm0005["MAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料号[%s]不存在", (const char*)tmm0005["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0005["MAT_KIND"].ToString().Trim() == "")
			{
				sprintf(s.msg, "物料种类[%s]不存在", (const char*)tmm0005["MAT_KIND"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0005["MAIN_MAT_FLAG"].ToString().Trim() == "")
			{
				sprintf(s.msg, "主材料标记[%s]不存在", (const char*)tmm0005["MAIN_MAT_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "")
			{
				sprintf(s.msg, "材料产出标记[%s]不存在", (const char*)tmm0005["MAT_PROD_FLAG"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/*if(tmm0005["IN_MAT_ID"].ToString().Trim() == "")
			{
			sprintf(s.msg,"入口材料标识号[%s]不存在",(const char*)tmm0005["IN_MAT_ID"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(tmm0005["IN_MAT_KIND"].ToString().Trim() == "")
			{
			sprintf(s.msg,"入口物料种类[%s]不存在",(const char*)tmm0005["IN_MAT_KIND"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
			}	*/

			Log::Trace("", __FUNCTION__, "查询材料主档 tmm0005.MAT_KIND	= [{0}]", (const char*)tmm0005["MAT_KIND"].ToString());

			/* 查询材料主档 */
			if (tmm0005["MAT_KIND"].ToString().Trim() == "SM")
			{
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
				tmmsm01.Reset();
				Log::Trace("", __FUNCTION__, "查询材料主档-----1-------- tmmsm01.MAT_ID	= [{0}]", (const char*)tmmsm01["MAT_ID"].ToString());
				tmmsm01["MAT_NO"] = tmm0005["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				tmmsm01.TrimOrBlank();
				
				tmm0005["MAT_TRACK_NO"] = tmmsm01["MAT_TRACK_NO"];
				/*if (tmmsm01["MAT_ID"].ToString().Trim() != "")
				{*/
					tmm0005["MAT_ID"] = tmmsm01["MAT_ID"];
				/*}
				else if (tmm0005["MAT_ID"].ToString().Trim() != ""&&tmmsm01["MAT_ID"].ToString().Trim() == "")
				{
					tmmsm01["MAT_ID"] = tmm0005["MAT_ID"];
				}*/
				Log::Trace("", __FUNCTION__, "查询材料主档-----2-------- tmmsm01.MAT_ID	= [{0}]", (const char*)tmmsm01["MAT_ID"].ToString());
				Log::Trace("", __FUNCTION__, "查询材料主档-----3-------- tmm0005.MAT_ID	= [{0}]", (const char*)tmm0005["MAT_ID"].ToString());
				tmm0005["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
				tmm0005["PASS_BACKLOG_SEQ_NO"] = tmmsm01["PASS_BACKLOG_SEQ_NO"];
				tmm0005["REPAIR_FLAG"] = tmmsm01["REPAIR_FLAG"];
				tmm0005["WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG_CODE"];
				tmm0005["UNIT_CODE"] = tmmsm01["UNIT_CODE"];
				tmm0005["MAT_THICK"] = tmmsm01["MAT_THICK"];
				tmm0005["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
				tmm0005["MAT_LEN"] = tmmsm01["MAT_LEN"];
				tmm0005["MAT_WT"] = tmmsm01["MAT_WT"];
				tmm0005["MEASURE_WT_FLAG"] = tmmsm01["MEASURE_WT_FLAG"];
				tmm0005["PONO"] = tmmsm01["PONO"];
				tmm0005["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];
				tmm0005["ORDER_NO"] = tmmsm01["ORDER_NO"];
				tmm0005["WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"];
				tmm0005["WHOLE_BACKLOG_ACT"] = tmmsm01["WHOLE_BACKLOG_ACT"];
				tmm0005["MAT_SHAPE_FLAG"] = tmmsm01["MAT_SHAPE_FLAG"];
				tmm0005["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
				tmm0005["MAT_NUM"] = tmmsm01["MAT_NUM"];
				tmm0005["PROD_TIME"] = tmmsm01["PROD_TIME"];
#endif

			}
			if (tmm0005["MAT_KIND"].ToString().Trim() == "HR")
			{
#if defined(_LINE_HR) || defined(_LINE_CR)
				tmmhr01["MAT_NO"] = tmm0005["MAT_NO"];
				tmmhr01.Query("MAT_NO");
				tmmhr01.TrimOrBlank();
				tmm0005["MAT_TRACK_NO"] = tmmhr01["MAT_TRACK_NO"];
				tmm0005["MAT_ID"] = tmmhr01["MAT_ID"];
				tmm0005["MAT_LINE_TYPE"] = tmmhr01["MAT_LINE_TYPE"];
				tmm0005["PASS_BACKLOG_SEQ_NO"] = tmmhr01["PASS_BACKLOG_SEQ_NO"];
				tmm0005["DUMMY_COIL_FLAG"] = tmmhr01["DUMMY_COIL_FLAG"];
				tmm0005["REPAIR_FLAG"] = tmmhr01["REPAIR_FLAG"];
				tmm0005["WHOLE_BACKLOG_CODE"] = tmmhr01["WHOLE_BACKLOG_CODE"];
				tmm0005["UNIT_CODE"] = tmmhr01["UNIT_CODE"];
				tmm0005["MAT_THICK"] = tmmhr01["MAT_THICK"];
				tmm0005["MAT_WIDTH"] = tmmhr01["MAT_WIDTH"];
				tmm0005["MAT_LEN"] = tmmhr01["MAT_LEN"];
				tmm0005["MAT_ACT_INNER_DIA"] = tmmhr01["MAT_ACT_INNER_DIA"];
				tmm0005["MAT_ACT_OUTER_DIA"] = tmmhr01["MAT_ACT_OUTER_DIA"];
				tmm0005["MAT_WT"] = tmmhr01["MAT_WT"];
				tmm0005["MEASURE_WT_FLAG"] = tmmhr01["MEASURE_WT_FLAG"];
				tmm0005["PONO"] = tmmhr01["PONO"];
				tmm0005["PRODUCT_FLAG"] = tmmhr01["PRODUCT_FLAG"];
				tmm0005["ORDER_NO"] = tmmhr01["ORDER_NO"];
				tmm0005["WHOLE_BACKLOG_SEQ"] = tmmhr01["WHOLE_BACKLOG_SEQ"];
				tmm0005["WHOLE_BACKLOG_ACT"] = tmmhr01["WHOLE_BACKLOG_ACT"];
				tmm0005["MAT_SHAPE_FLAG"] = tmmhr01["MAT_SHAPE_FLAG"];
				tmm0005["MAT_STATUS"] = tmmhr01["MAT_STATUS"];
				tmm0005["MAT_NUM"] = tmmhr01["MAT_NUM"];
				tmm0005["PROD_TIME"] = tmmhr01["PROD_TIME"];
#endif

			}
			if (tmm0005["MAT_KIND"].ToString().Trim() == "CR")
			{
#if defined(_LINE_CR)
				tmmcr01["MAT_NO"] = tmm0005["MAT_NO"];
				tmmcr01.Query("MAT_NO");
				tmmcr01.TrimOrBlank();
				tmm0005["MAT_TRACK_NO"] = tmmcr01["MAT_TRACK_NO"];
				tmm0005["MAT_ID"] = tmmcr01["MAT_ID"];
				tmm0005["MAT_LINE_TYPE"] = tmmcr01["MAT_LINE_TYPE"];
				tmm0005["PASS_BACKLOG_SEQ_NO"] = tmmcr01["PASS_BACKLOG_SEQ_NO"];
				tmm0005["DUMMY_COIL_FLAG"] = tmmcr01["DUMMY_COIL_FLAG"];
				tmm0005["REPAIR_FLAG"] = tmmcr01["REPAIR_FLAG"];
				tmm0005["WHOLE_BACKLOG_CODE"] = tmmcr01["WHOLE_BACKLOG_CODE"];
				tmm0005["UNIT_CODE"] = tmmcr01["UNIT_CODE"];
				tmm0005["MAT_THICK"] = tmmcr01["MAT_THICK"];
				tmm0005["MAT_WIDTH"] = tmmcr01["MAT_WIDTH"];
				tmm0005["MAT_LEN"] = tmmcr01["MAT_LEN"];
				tmm0005["MAT_ACT_INNER_DIA"] = tmmcr01["MAT_ACT_INNER_DIA"];
				tmm0005["MAT_ACT_OUTER_DIA"] = tmmcr01["MAT_ACT_OUTER_DIA"];
				tmm0005["MAT_WT"] = tmmcr01["MAT_WT"];
				tmm0005["MEASURE_WT_FLAG"] = tmmcr01["MEASURE_WT_FLAG"];
				tmm0005["PONO"] = tmmcr01["PONO"];
				tmm0005["PRODUCT_FLAG"] = tmmcr01["PRODUCT_FLAG"];
				tmm0005["ORDER_NO"] = tmmcr01["ORDER_NO"];
				tmm0005["WHOLE_BACKLOG_SEQ"] = tmmcr01["WHOLE_BACKLOG_SEQ"];
				tmm0005["WHOLE_BACKLOG_ACT"] = tmmcr01["WHOLE_BACKLOG_ACT"];
				tmm0005["MAT_SHAPE_FLAG"] = tmmcr01["MAT_SHAPE_FLAG"];
				tmm0005["MAT_STATUS"] = tmmcr01["MAT_STATUS"];
				//tmm0005["MAT_NUM"]					= tmmcr01.MAT_NUM;
				tmm0005["PROD_TIME"] = tmmcr01["PROD_TIME"];
#endif

			}
			if (tmm0005["MAT_KIND"].ToString().Trim() == "HP")
			{
#if defined(_LINE_HP)
				tmmhp01["MAT_NO"] = tmm0005["MAT_NO"];
				tmmhp01.Query("MAT_NO");
				tmmhp01.TrimOrBlank();
				tmm0005["MAT_TRACK_NO"] = tmmhp01["MAT_TRACK_NO"];
				tmm0005["MAT_ID"] = tmmhp01["MAT_ID"];
				tmm0005["MAT_LINE_TYPE"] = tmmhp01["MAT_LINE_TYPE"];
				tmm0005["PASS_BACKLOG_SEQ_NO"] = tmmhp01["PASS_BACKLOG_SEQ_NO"];
				tmm0005["REPAIR_FLAG"] = tmmhp01["REPAIR_FLAG"];
				tmm0005["WHOLE_BACKLOG_CODE"] = tmmhp01["WHOLE_BACKLOG_CODE"];
				tmm0005["UNIT_CODE"] = tmmhp01["UNIT_CODE"];
				tmm0005["MAT_THICK"] = tmmhp01["MAT_THICK"];
				tmm0005["MAT_WIDTH"] = tmmhp01["MAT_WIDTH"];
				tmm0005["MAT_LEN"] = tmmhp01["MAT_LEN"];
				tmm0005["MAT_WT"] = tmmhp01["MAT_WT"];
				tmm0005["MEASURE_WT_FLAG"] = tmmhp01["MEASURE_WT_FLAG"];
				tmm0005["PONO"] = tmmhp01["PONO"];
				tmm0005["PRODUCT_FLAG"] = tmmhp01["PRODUCT_FLAG"];
				tmm0005["ORDER_NO"] = tmmhp01["ORDER_NO"];
				tmm0005["WHOLE_BACKLOG_SEQ"] = tmmhp01["WHOLE_BACKLOG_SEQ"];
				tmm0005["WHOLE_BACKLOG_ACT"] = tmmhp01["WHOLE_BACKLOG_ACT"];
				tmm0005["MAT_SHAPE_FLAG"] = tmmhp01["MAT_SHAPE_FLAG"];
				tmm0005["MAT_STATUS"] = tmmhp01["MAT_STATUS"];
				tmm0005["MAT_NUM"] = tmmhp01["MAT_NUM"];
				tmm0005["PROD_TIME"] = tmmhp01["PROD_TIME"];
#endif
			}

			Log::Trace("", __FUNCTION__, "校验入口材料信息 tmm0005.IN_MAT_ID	= [{0}]", (const char*)tmm0005["IN_MAT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "校验入口材料信息 tmm0005.IN_MAT_KIND	= [{0}]", (const char*)tmm0005["IN_MAT_KIND"].ToString());

			/* 校验入口材料信息 第1个母材料和半卷回退的材料,不校验入口材料号,前提是入口材料已归入历史档 */
			if (tmm0005["IN_MAT_KIND"].ToString().Trim() != "")
			{
				Log::Trace("", __FUNCTION__, "1111 校验入口材料信息 tmm0005.IN_MAT_KIND	= [{0}]", (const char*)tmm0005["IN_MAT_KIND"].ToString());
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS SQL Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库
					default:
						sqlstr = "SELECT MAT_NO,  "
							"		  MAT_THICK,"
							"		  MAT_WIDTH,"
							"		  MAT_LEN,";
						if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "SM")
						{
							sqlstr += "	  MAT_WT "
								"  FROM HMMSM01 ";
						}
						if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HR")
						{
							sqlstr += "   MAT_ACT_OUTER_DIA,"
								"   MAT_ACT_INNER_DIA,"
								"	  MAT_WT "
								"  FROM HMMHR01 ";
						}
						if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "CR")
						{
							sqlstr += "   MAT_ACT_OUTER_DIA,"
								"   MAT_ACT_INNER_DIA,"
								"	  MAT_WT "
								"  FROM HMMCR01 ";
						}
						if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HP")
						{
							sqlstr += "	  MAT_WT "
								"  FROM HMMHP01 ";
						}
						sqlstr = sqlstr + " WHERE MAT_ID = @tmm0005.IN_MAT_ID AND MAT_KIND = @tmm0005.IN_MAT_KIND";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				Log::Trace("", __FUNCTION__, "校验入口材料信息 sqlstr	= [{0}]", sqlstr);
				Log::Trace("", __FUNCTION__, "校验入口材料信息 tmm0005.IN_MAT_KIND	= [{0}]", tmm0005["IN_MAT_KIND"].ToString());
				Log::Trace("", __FUNCTION__, "校验入口材料信息 tmm0005.IN_MAT_ID	= [{0}]", tmm0005["IN_MAT_ID"].ToString());
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_ID", tmm0005["IN_MAT_ID"].ToString());
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_KIND", tmm0005["IN_MAT_KIND"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cs_h_mat_no = cmd_inq.GetString(1).Trim();
					Log::Trace("", __FUNCTION__, "校验入口材料信息 cs_h_mat_no	= [{0}]", cs_h_mat_no);
					tmm0005["IN_MAT_THICK"] = cmd_inq.GetDecimal(2);
					tmm0005["IN_MAT_WIDTH"] = cmd_inq.GetDecimal(3);
					tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(4);

					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "SM")
					{
						tmm0005["IN_MAT_WT"] = cmd_inq.GetDecimal(5);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HR")
					{
						tmm0005["IN_MAT_OUTER_DIA"] = cmd_inq.GetDecimal(5);
						tmm0005["IN_MAT_INNER_DIA"] = cmd_inq.GetDecimal(6);
						tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(7);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "CR")
					{
						tmm0005["IN_MAT_OUTER_DIA"] = cmd_inq.GetDecimal(5);
						tmm0005["IN_MAT_INNER_DIA"] = cmd_inq.GetDecimal(6);
						tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(7);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HP")
					{
						tmm0005["IN_MAT_WT"] = cmd_inq.GetDecimal(5);
					}
					tmm0005["IN_MAT_NO"] = cs_h_mat_no;
				}
				cmd_inq.Close();
			}

			Log::Trace("", __FUNCTION__, "校验材料是否已在路径跟踪表 tmm0005.IN_MAT_ID	= [{0}]", (const char*)tmm0005["IN_MAT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "校验材料是否已在路径跟踪表 tmm0005.MAT_ID	= [{0}]", (const char*)tmm0005["MAT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "校验材料是否已在路径跟踪表 tmm0005.MAT_KIND	= [{0}]", (const char*)tmm0005["MAT_KIND"].ToString());

			/* 校验材料是否已在路径跟踪表 不同的物料主档MAT_ID可能相同,因此按MAT_ID和MAT_KIND 来获取路径信息*/
			if (tmm0005["MAIN_MAT_FLAG"].ToString().Trim() == "1")	//主材料标记  0-非主材料,1-主材料
			{
				if (tmm0005.QueryCount("MAT_ID,MAT_KIND") > 0)
				{
					sprintf(s.msg, "材料号[%s]在路径跟踪表中已存在!", (const char*)tmm0005["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			Log::Trace("", __FUNCTION__, "入口材料不在历史档 在当前档查询入口材料信息 tmm0005.IN_MAT_NO	= [{0}]", tmm0005["IN_MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "入口材料不在历史档 在当前档查询入口材料信息 tmm0005.PASS_BACKLOG_SEQ_NO	= [{0}]", tmm0005["PASS_BACKLOG_SEQ_NO"].ToDecimal());

			/* 入口材料不在历史档 在当前档查询入口材料信息  */
			if (cs_h_mat_no.Trim() == "" && tmm0005["PASS_BACKLOG_SEQ_NO"].ToDecimal() > 1) //入口材料为空且材料非母材料
			{
				//31-半卷回退,入口材料不在历史档,查询当前档材料信息
				// 或调用本函数之前，入口材料还未归入历史档

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr = "SELECT MAT_NO,  "
						"		  MAT_THICK,"
						"		  MAT_WIDTH,"
						"		  MAT_LEN,";
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "SM")
					{
						sqlstr += "	  MAT_WT "
							"  FROM TMMSM01 ";
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HR")
					{
						sqlstr += "   MAT_ACT_OUTER_DIA,"
							"   MAT_ACT_INNER_DIA,"
							"	  MAT_WT "
							"  FROM TMMHR01 ";
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "CR")
					{
						sqlstr += "   MAT_ACT_OUTER_DIA,"
							"   MAT_ACT_INNER_DIA,"
							"	  MAT_WT "
							"  FROM TMMCR01 ";
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HP")
					{
						sqlstr += "	  MAT_WT "
							"  FROM TMMHP01 ";
					}
					sqlstr = sqlstr + " WHERE MAT_ID = @tmm0005.IN_MAT_ID AND MAT_KIND = @tmm0005.IN_MAT_KIND";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_ID", tmm0005["IN_MAT_ID"].ToString());
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_KIND", tmm0005["IN_MAT_KIND"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0005["IN_MAT_NO"] = cmd_inq.GetString(1).Trim();
					tmm0005["IN_MAT_THICK"] = cmd_inq.GetDecimal(2);
					tmm0005["IN_MAT_WIDTH"] = cmd_inq.GetDecimal(3);
					tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(4);
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "SM")
					{
						tmm0005["IN_MAT_WT"] = cmd_inq.GetDecimal(5);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HR")
					{
						tmm0005["IN_MAT_OUTER_DIA"] = cmd_inq.GetDecimal(5);
						tmm0005["IN_MAT_INNER_DIA"] = cmd_inq.GetDecimal(6);
						tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(7);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "CR")
					{
						tmm0005["IN_MAT_OUTER_DIA"] = cmd_inq.GetDecimal(5);
						tmm0005["IN_MAT_INNER_DIA"] = cmd_inq.GetDecimal(6);
						tmm0005["IN_MAT_LEN"] = cmd_inq.GetDecimal(7);
					}
					if (tmm0005["IN_MAT_KIND"].ToString().Trim() == "HP")
					{
						tmm0005["IN_MAT_WT"] = cmd_inq.GetDecimal(5);
					}
					Log::Trace("", __FUNCTION__, "入口材料在当前档查询入口材料信息 tmm0005.IN_MAT_THICK	= [{0}]", tmm0005["IN_MAT_THICK"].ToDecimal());
					Log::Trace("", __FUNCTION__, "入口材料在当前档查询入口材料信息 tmm0005.IN_MAT_WIDTH	= [{0}]", tmm0005["IN_MAT_WIDTH"].ToDecimal());
					Log::Trace("", __FUNCTION__, "入口材料在当前档查询入口材料信息 tmm0005.IN_MAT_LEN	= [{0}]", tmm0005["IN_MAT_LEN"].ToDecimal());
					Log::Trace("", __FUNCTION__, "入口材料在当前档查询入口材料信息 tmm0005.IN_MAT_WT	= [{0}]", tmm0005["IN_MAT_WT"].ToDecimal());
					Log::Trace("", __FUNCTION__, "入口材料在当前档查询入口材料信息 tmm0005.IN_MAT_THICK	= [{0}]", tmm0005["IN_MAT_THICK"].ToDecimal());
				}
				cmd_inq.Close();
			}

			/* 获取亲属码 根据 材料产出标记 判断  */
			/* MAT_PROD_FLAG 10-原料录入;20-机组产出;21-并卷;22-分卷;31-半卷回退;50-清盘库 */
			if (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "10"	//原料录入
				|| tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "50"    //清盘库
				|| (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "20" && tmm0005["IN_MAT_NO"].ToString().Trim() == ""))	//机组产出且入口材料为空,通常是炼钢板坯产出
			{
				tmm0005["FAMILY_CODE"] = "01";
			}
			else if (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "20"	//机组产出 除炼钢板坯产出之外的机组产出，入口材料号不为空的情况
				|| (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "21" && tmm0005["MAIN_MAT_FLAG"].ToString().Trim() == "1") //并卷&&主材料标记是1-主材料
				|| tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "22"   //分卷
				|| tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "31")  //半卷回退
			{
				/* 亲属码用IN_MAT_ID到TMM0005表读 记录数+1，然后前拼0 */
				// 获取IN_MAT_ID在TMM0005表的记录数+1
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr_count = "SELECT COUNT(1)  "
						"  FROM TMM0005 "
						" WHERE IN_MAT_ID = @tmm0005.IN_MAT_ID";
					break;
				}
				Log::Trace("", __FUNCTION__, "获取IN_MAT_ID在TMM0005表的记录数 sqlstr_count	= [{0}]", sqlstr_count);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_ID", tmm0005["IN_MAT_ID"].ToString());
				cmd_inq.SetCommandText(sqlstr_count);
				cd_count = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				Log::Trace("", __FUNCTION__, "获取IN_MAT_ID在TMM0005表的记录数 cd_count	= [{0}]", cd_count);

				// 获取IN_MAT_ID在TMM0005表的记录数+1
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr = "SELECT FAMILY_CODE "
						"  FROM TMM0005 "
						" WHERE MAT_ID = @tmm0005.IN_MAT_ID AND MAT_KIND = @tmm0005.IN_MAT_KIND";
					break;
				}
				Log::Trace("", __FUNCTION__, "查询入口材料的亲属码 sqlstr	= [{0}]", sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_ID", tmm0005["IN_MAT_ID"].ToString());
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_KIND", tmm0005["IN_MAT_KIND"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0005["FAMILY_CODE"] = cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();
				Log::Trace("", __FUNCTION__, "查询入口材料的亲属码 tmm0005.FAMILY_CODE	= [{0}]", tmm0005["FAMILY_CODE"].ToString());
				// 亲属码 = 原亲属码 + (记录数 + 1后前拼0)
				cd_count = cd_count + 1;
				tmm0005["FAMILY_CODE"] = tmm0005["FAMILY_CODE"].ToString() + CString::Format("%02d", cd_count.ToInt32());
				Log::Trace("", __FUNCTION__, "材料的亲属码 tmm0005.FAMILY_CODE	= [{0}]", tmm0005["FAMILY_CODE"].ToString());
			}
			else if (tmm0005["MAT_PROD_FLAG"].ToString().Trim() == "21" && tmm0005["MAIN_MAT_FLAG"].ToString().Trim() == "0") //并卷&&主材料标记是0-非主材料
			{
				// 前提 分卷的主材料先新增到TMM0005，非主材料后新增到TMM0005表
				// 获取TMM0005表的出口材料号相同,主材料标记是1的亲属码
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:
					sqlstr = "SELECT FAMILY_CODE "
						"  FROM TMM0005 "
						" WHERE MAT_ID = @tmm0005.MAT_ID"
						"   AND MAT_KIND = @tmm0005.MAT_KIND "
						"   AND MAIN_MAT_FLAG = '1' ";
					break;
				}
				Log::Trace("", __FUNCTION__, "获取TMM0005表的出口材料号相同,主材料标记是1的亲属码 sqlstr	= [{0}]", sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.MAT_ID", tmm0005["MAT_ID"].ToString());
				cmd_inq.Parameters.Set("tmm0005.MAT_KIND", tmm0005["MAT_KIND"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0005["FAMILY_CODE"] = cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();
			}

			/* 设置默认值 */
			if (tmm0005["DUMMY_COIL_FLAG"].ToString().Trim() == "")
			{
				tmm0005["DUMMY_COIL_FLAG"] = "0";//过渡卷标记 0-正常卷,1-过渡卷
			}
			if (tmm0005["REPAIR_FLAG"].ToString().Trim() == "")
			{
				tmm0005["REPAIR_FLAG"] = "0";//返修标记 0-不返修,1-返修
			}
			if (tmm0005["SPECAIL_FLAG"].ToString().Trim() == "")
			{
				tmm0005["SPECAIL_FLAG"] = "0";//特殊标记 0-默认值,1-并卷同时分卷
			}
			if (tmm0005["MAT_ARCHIVE_FLAG"].ToString().Trim() == "")
			{
				tmm0005["MAT_ARCHIVE_FLAG"] = "00";//材料归历史档标记 00-当前档；10-历史档；01-红冲回当前档
			}

			/* 查询工序名称 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT WHOLE_BACKLOG_NAME "
					"  FROM TSI0001 "
					" WHERE WHOLE_BACKLOG_CODE = @tmm0005.WHOLE_BACKLOG_CODE";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmm0005.WHOLE_BACKLOG_CODE", tmm0005["WHOLE_BACKLOG_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmm0005["WHOLE_BACKLOG_NAME"] = cmd_inq.GetString(1).Trim();
			}
			cmd_inq.Close();

			/* 查询机组名称 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT UNIT_CNAME "
					"  FROM TSI0015 "
					" WHERE UNIT_CODE = @tmm0005.UNIT_CODE";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmm0005.UNIT_CODE", tmm0005["UNIT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmm0005["UNIT_CNAME"] = cmd_inq.GetString(1).Trim();
			}
			cmd_inq.Close();

			/* 新增物料跟踪路径表 */
			//获取生产流水号
			doFlag = f_mm0011("MM00_RESUME_SEQ_NO", 6, cs_mm_resume_seq_no, conn);
			if (doFlag < 0 || cs_mm_resume_seq_no.Trim() == "")
			{
				sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_RESUME_SEQ_NO】是否正常!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", __FUNCTION__, "cs_mm_resume_seq_no	= [{0}]", cs_mm_resume_seq_no);
			tmm0005["RESUME_SEQ_NO"] = datetime + cs_mm_resume_seq_no;
			Log::Trace("", __FUNCTION__, "tmm0005.RESUME_SEQ_NO	= [{0}]", tmm0005["RESUME_SEQ_NO"].ToString());

			tmm0005["FUNC_ID"] = s.svc_name;
			tmm0005["REC_CREATOR"] = s.userid;		// 记录新增责任者  
			tmm0005["REC_CREATE_TIME"] = datetime;		// 记录新增时刻
			tmm0005.TrimOrBlank();
			//tmm0005.Print();
			tmm0005.Insert();

			/* 修改相同入口材料号的材料产出标记为22 */
			if (tmm0005["SPECAIL_FLAG"].ToString().Trim() == "1")//特殊标记 1-并卷同时分卷
			{
				/* 根据入口材料号,得到履历序号 RESUME_SEQ_NO */
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT RESUME_SEQ_NO "
						"  FROM TMM0005 "
						" WHERE IN_MAT_NO		= @tmm0005.IN_MAT_NO "
						"   AND MAT_PROD_FLAG	= '20' ";
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0005.IN_MAT_NO", tmm0005["IN_MAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmm0005["RESUME_SEQ_NO"] = cmd_inq.GetString(1).Trim();
				}

				/* 设置修改值 */
				tmm0005["MAT_PROD_FLAG"] = "22";//材料产出标记 10-原料录入;20-机组产出;21-并卷;22-分卷;30-整卷回退;31-半卷回退;50-清盘库
				tmm0005["REC_REVISOR"] = s.userid;/* 记录修改责任者 */
				tmm0005["REC_REVISE_TIME"] = datetime;/* 记录修改时刻 */

				/* 修改产出标记 */
				tmm0005.Update("MAT_PROD_FLAG,"
					"REC_REVISOR,"
					"REC_REVISE_TIME",
					"RESUME_SEQ_NO");
			}

			/*目前不记录材料归历史档或从历史档回冲回当前档*/
			//else  // if(tmm0005["MAT_ARCHIVE_FLAG"].ToString().Trim()	==	"10" || tmm0005["MAT_ARCHIVE_FLAG"].ToString().Trim()	==	"01")//00-当前档；10-历史档；01-红冲回当前档
			//{
			//	/* 检查输入参数合法性 */		
			//	if(tmm0005["MAT_ID"].ToString().Trim() == "")
			//	{
			//		sprintf(s.msg,_RES("MM00S0000171")/*材料标识号不能为空*/);
			//		sprintf(s.sysmsg,_RES("MM00S0000171")/*材料标识号不能为空。*/);
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	/* 设置修改值 */
			//	tmm0005.REC_REVISOR	    = s.userid;/* 记录修改责任者 */ 
			//	tmm0005.REC_REVISE_TIME	= datetime;/* 记录修改时刻 */

			//	/* 修改产出标记 */
			//	tmm0005.Update("MAT_ARCHIVE_FLAG,"
			//				   "REC_REVISOR,"
			//				   "REC_REVISE_TIME",
			//				   "MAT_ID");			
			//}
		}
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



