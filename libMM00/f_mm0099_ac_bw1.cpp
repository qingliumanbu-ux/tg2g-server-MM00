/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      wb
Version:     1.0
Date:        2022年8月3日10点19分
Description: 物料抛成本函数(调成本函数)
**************************************************/

#include "CDynaTable.h"
#include "tacaich.h"





 


BM2_FUNCTION_IMPORT
int f_mm009b(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2_FUNCTION_IMPORT
int f_acaich_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);  //成本抛账处理函数
CDecimal f_weight_mmbw(CString v_mat_no /*v_seq_name*/, CDbConnection* conn);

BM2_FUNCTION_EXPORT
int f_mm0099_ac_bw1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString order_no = " ";
	CString order_no_2 = " ";
	int MAT_THICK_CODE = 0;
	int MAT_WIDTH_CODE = 0;

	CString	datetime("");

	EIClass bcls_rec_ac;
	EIClass bcls_tmmbwac;

	CDbCommand cmd_upd(conn);
	CDbCommand cmd_inq(conn);

	CTACAICH tacaich(conn);
	CModel tmmbwac("TMMBWAC");
	CModel tom01("TOM01");
	CModel tmmsm01("TMMSM01");
	CModel tmmbw01("TMMBW01"); 
	CModel tep0002("TEP0002");


	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		CDataTable dtEventData = bcls_rec->Tables["EVENT_DATA"];
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];

		CString matKind = " ";
		CString matShapeFlag = "";//材料形态 1:板坯  2:钢板 6:型材 7:线材 8:棒材 9:中宽带 A:方坯 C:矩形坯 
		CString eventId = "";
		CString event_name = "";
		CString factoryDiv = "";//厂别区分
		CString cs_rec_creator = "";

		CString table_ac = "tacaich";
		CString table_mm = "";
		int blkNum = 0;
		CDecimal d_in_mat_wt = 0;
		CString new_wt_method_code = "";
		CString old_wt_method_code = "";
		CString old_order_no = "";
		CString is_flag = "1";//挂合同是否抛成本标记 1：抛成本,0：不抛成本

		//成本抛账用字段
		CString TRANSACTION_CODE = "";//获取 关键字串1_成本的第1位值-- 交易种类 C:原料消耗 F:准发 K:准发红冲 N:国内销售 P:生产实绩 T:质量判定 X:修正
		CString FUNCTION_CODE_AC = "";//获取 关键字串1_成本的第2位值-- 功能代码 D:删除 N:新增 R:修改
		CString AC_TYPE = " "; // 抛账次数 ' '为默认两笔记录 取自tmm0097.EVENT_PROC_WAY_AC.Substring(2,1);
		CString PRODUCT_CODE_OUTPUT = "";//获取成本返回产副品代码 更新物料主档
		CString PRODUCT_CODE_IN = "";//投料产副品代码  入口材料产副品代码

		CString PRODUCT_CODE_IN_SPEC = "";  //逆流程时，原产副品代码【目前针对板坯切断删除】

		CString cs_quality_test_code = ""; //出口材料质量码

		CString BLOCKNAME = "TACAICH_REC";

		blkNum = bcls_rec_ac.Tables.IndexOf(BLOCKNAME);
		if (blkNum < 0)
		{
			bcls_rec_ac.Tables.Add(BLOCKNAME);
		}
		blkNum = bcls_tmmbwac.Tables.IndexOf("TMMBWAC");
		if (blkNum < 0)
		{
			bcls_tmmbwac.Tables.Add("TMMBWAC");
		}
		//特殊情况用变量
		CString ORDER_TYPE_CODE = " ";
		CString cs_unit_code_spec = " ";//产出机组模式
		CString EVENT_PROC_WAY_AC = " ";//EVENT_PROC_WAY_AC  关键字串1_成本

		EVENT_PROC_WAY_AC = dtEventData.Rows[0]["EVENT_PROC_WAY_AC"].ToString();
		eventId = dtEventData.Rows[0]["EVENT_ID"].ToString();
		event_name = dtEventData.Rows[0]["EVENT_NAME"].ToString();

		if (dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().GetLength() == 3)
		{
			TRANSACTION_CODE = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Substring(0, 1);
			FUNCTION_CODE_AC = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Substring(1, 1);
			AC_TYPE = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Substring(2, 1);
		}
		else if (dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().GetLength() == 2)
		{
			TRANSACTION_CODE = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Substring(0, 1);
			FUNCTION_CODE_AC = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Substring(1, 1);
			AC_TYPE = " ";
		}
		else
		{
			sprintf(s.msg, "抛成本事件EVENT_PROC_WAY_AC字段未维护，请维护抛账类型...");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (dtEventData.Rows[0]["MAT_KIND"].ToString() == "SM")
		{
			table_mm = "TMMSM01";
			tmmsm01["MAT_NO"] = dtNewMat.Rows[0]["MAT_NO"].ToString();
			tmmsm01.Query("MAT_NO");
			cs_rec_creator = tmmsm01["REC_CREATOR"];
		}
		else if (dtEventData.Rows[0]["MAT_KIND"].ToString() == "BW")
		{
			table_mm = "TMMBW01";
			tmmbw01["MAT_NO"] = dtNewMat.Rows[0]["MAT_NO"].ToString();
			tmmbw01.Query("MAT_NO");
			cs_rec_creator = tmmbw01["REC_CREATOR"];
		}

		//原料库处置机组需做定制
		CString cs_mat_line_type = dtNewMat.Rows[0]["MAT_LINE_TYPE"].ToString();
		CString cs_stock_no = dtNewMat.Rows[0]["STOCK_NO"].ToString();
		if (eventId == "WM14")//线材领用
		{
			cs_unit_code_spec = dtNewMat.Rows[0]["STOCK_NO"].ToString();
			blkNum = bcls_rec->Tables.IndexOf("MM0099");
			if (blkNum > 0)
			{
				cs_unit_code_spec = bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"].ToString();
			}
		}

		CString in_product_code = " ";
		if (dtOldMat.Rows.get_Count() > 0)
		{
			factoryDiv = dtOldMat.Rows[0]["FACTORY_DIV"].ToString();
			in_product_code = dtOldMat.Rows[0]["PRODUCT_CODE"].ToString();
		}
		else if (dtNewMat.Rows.get_Count() > 0)
		{
			factoryDiv = dtNewMat.Rows[0]["FACTORY_DIV"].ToString();
			in_product_code = dtNewMat.Rows[0]["PRODUCT_CODE"].ToString();
		}
		/*Log::Trace("", __FUNCTION__, " newfactorydiv = {0}", dtNewMat.Rows[0]["FACTORY_DIV"].ToString());
		Log::Trace("", __FUNCTION__, " 账务代码为=[{0}],抛账类型为=[{1}]", TRANSACTION_CODE + FUNCTION_CODE_AC, AC_TYPE);
		Log::Trace("", __FUNCTION__, " oldRows = {0}", dtOldMat.Rows.get_Count());
		Log::Trace("", __FUNCTION__, " newRows = {0}", dtNewMat.Rows.get_Count());
		Log::Trace("", __FUNCTION__, " factoryDiv = {0}", factoryDiv);
		Log::Trace("", __FUNCTION__, " in_product_code = {0}", in_product_code);
		Log::Trace("", __FUNCTION__, " FACTORY_STORE={0}", dtNewMat.Rows[0]["FACTORY_STORE"].ToString());
		Log::Trace("", __FUNCTION__, " cs_rec_creator={0}", cs_rec_creator);*/
		//TACAICH
		CString cs_shift_name;

		/*tacaich.ACCOUNT = "7208";*/
		if (factoryDiv == "A1" || factoryDiv == "A2" || factoryDiv == "A3" || factoryDiv == "A6")
		{
			tacaich.APP_CODE = "IS";  //根据厂别对应 IS  IH  IP  IW
			cs_shift_name = "SM";
			if (factoryDiv == "A1" || factoryDiv == "A2")//草铺
			{
				tacaich.ACCOUNT = "2801";
			}
			else if (factoryDiv == "A3")//红河
			{
				tacaich.ACCOUNT = "2812";
			}
			else if (factoryDiv == "A6")//玉溪
			{
				tacaich.ACCOUNT = "2811";
			}
		}
		else if (factoryDiv == "L1" || factoryDiv == "L2" || factoryDiv == "L5" || factoryDiv == "L4" || factoryDiv == "L7" ||
			factoryDiv == "W1" || factoryDiv == "W2" || factoryDiv == "W4" || 
			factoryDiv == "X1" ||
			factoryDiv == "L3" || factoryDiv == "W3" || 
			factoryDiv == "L6" )
		{
			tacaich.APP_CODE = "IW";
			cs_shift_name = "BW";
			if (factoryDiv == "L1" || factoryDiv == "L2" || factoryDiv == "L5" || factoryDiv == "L4" || factoryDiv == "L7" ||
				factoryDiv == "W1" || factoryDiv == "W2" || factoryDiv == "W4" || factoryDiv == "X1")//草铺
			{
				tacaich.ACCOUNT = "2801";
			}
			else if (factoryDiv == "L3" || factoryDiv == "W3")//红河
			{
				tacaich.ACCOUNT = "2812";
			}
			else if (factoryDiv == "L6")//玉溪
			{
				tacaich.ACCOUNT = "2811";
			}
		}

		//20210928 成本规定外购钢坯同意抛IS,MAT_ORIGIN材料来源 1-外购
		if (dtNewMat.Rows[0]["MAT_ORIGIN"].ToString().Trim() == "1" && cs_shift_name == "SM")
		{
			tacaich.APP_CODE = "IS";
		}

		/*Log::Trace("", __FUNCTION__, " MAT_ORIGIN = {0}", dtNewMat.Rows[0]["MAT_ORIGIN"].ToString());
		Log::Trace("", __FUNCTION__, " APP_CODE = {0}", tacaich.APP_CODE);
		Log::Trace("", __FUNCTION__, " eventId = {0}", eventId);*/

		//定制情况规避抛账(棒线未完成抛账时不抛)
		if (tacaich.APP_CODE == "IW" && eventId == "MM03" && in_product_code == " ")
		{
			Log::Trace("", __FUNCTION__, " 特殊处理：棒线称重实绩未抛成本时不抛修正");
			return doFlag;
		}

		//根据app_code和事件发生时间获取 班次班组并记录back_code_1 back_code_2
		CString cs_shift_no;
		CString cs_shift_group;
		CString cs_date;

		//建议按事务发生时间调用，暂时先按接收datetime来处理
		//f_epep_get_shift_group(cs_shift_name, datetime, cs_shift_no,cs_shift_group);
		f_epep_get_shift_group_day(cs_shift_name, datetime, cs_shift_no, cs_shift_group, cs_date, conn);

		/*Log::Trace("", __FUNCTION__, " cs_shift_name = {0}", cs_shift_name);
		Log::Trace("", __FUNCTION__, " cs_shift_no = {0}", cs_shift_no);
		Log::Trace("", __FUNCTION__, " cs_shift_group = {0}", cs_shift_group);*/


		if (TRANSACTION_CODE == "F")
		{
			tacaich.APP_CODE = tacaich.APP_CODE + "OL";
		}


		/*Log::Trace("", __FUNCTION__, " APP_CODE = {0}", tacaich.APP_CODE);*/

		tacaich.TRANSACTION_CODE = TRANSACTION_CODE;
		tacaich.FUNCTION_CODE_AC = FUNCTION_CODE_AC;

		tacaich.QUALITY_CODE = " "; //不用
		tacaich.AI_SEQ_NUM = "1"; //目前都给1
		tacaich.APP_THROW_AI_OK = " ";//XX??

		CString cs_throw_ai_date = "";

		cs_throw_ai_date = datetime.Substring(0, 8);
		tacaich.APP_TRNC_DATE = cs_throw_ai_date;
		tacaich.APP_TRANSACTION_T = datetime.Substring(8, 6);
		tacaich.APP_THROW_AI_DATE = tacaich.APP_TRNC_DATE;
		tacaich.APP_THROW_AI_T = tacaich.APP_TRANSACTION_T;
		tacaich.APP_THROW_AI_MODE = " "; //此字段由成本生成
		tacaich.PRODUCT_CODE = " "; //此字段由成本计算后生成，MM返写主档
		tacaich.ACCOUNT_TITLE_ITEM = " "; //账务代码由成本生成
		tacaich.COST_CENTER = " "; //成本中心由成本根据机组代码生成 

		//linggu 2021年6月17日 还是需要根据产出带出投入，因此在此处增加入口信息的自动获取
		CString upd_mat_no = "";
		CDecimal mat_act_wt_in = 0; //记录入口材料重量总和

		CString cs_in_mat_no_bw = "";//棒材的入口材料号 就是轧制计划号

		/*Log::Trace("", __FUNCTION__, " = [{0}]", dtEventData.Rows[0]["EVENT_ID"].ToString());*/

		for (int i = 0; i < dtNewMat.Rows.get_Count(); i++)
		{
			if (dtEventData.Rows[0]["MAT_KIND"].ToString() == "SM")
			{
				tmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
				tmmsm01.Query("MAT_NO");
				cs_rec_creator = tmmsm01["REC_CREATOR"];
			}
			else if (dtEventData.Rows[0]["MAT_KIND"].ToString() == "BW")
			{
				tmmbw01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
				tmmbw01.Query("MAT_NO");
				cs_rec_creator = tmmbw01["REC_CREATOR"];
			}
		    
			mat_act_wt_in = 0;

			bcls_rec_ac.Tables[BLOCKNAME].Rows.Clear();

			if (TRANSACTION_CODE == "P" && AC_TYPE == "1" && (dtEventData.Rows[0]["EVENT_ID"].ToString() == "MM18" || dtEventData.Rows[0]["EVENT_ID"].ToString() == "MM29" || dtEventData.Rows[0]["EVENT_ID"].ToString() == "MMF9" || dtEventData.Rows[0]["EVENT_ID"].ToString() == "MM09" || dtEventData.Rows[0]["EVENT_ID"].ToString() == "MMZ9" || dtEventData.Rows[0]["EVENT_ID"].ToString() == "MMP9"))
			{
				//当时机组产出事件，且为单独产出逻辑时，需补充入口投入信息
				/*Log::Trace("", __FUNCTION__, " PN1 模式分布判断");*/

				CString cs_unit_code = "";//产出材料机组
				CString cs_slab_no = ""; //I101 使用

				CString cs_mat_kind = "";

				CString table_name_in = "";  //原料表名
				

				cs_unit_code = dtNewMat.Rows[i]["UNIT_CODE"].ToString();
				cs_mat_kind = dtNewMat.Rows[i]["MAT_KIND"].ToString();

				/*Log::Trace("", __FUNCTION__, " PN1 unit_code = {0}", cs_unit_code);
				Log::Trace("", __FUNCTION__, " PN1 unit_code = {0}", cs_mat_kind);*/

				//linggu add 分布式的产出逻辑
				//1. 先根据机组和产出类型，找到对应的投入信息
				if (cs_unit_code == "I601")  //厚板、热轧均可按此逻辑获取
				{
					/*Log::Trace("", __FUNCTION__, " " + cs_unit_code + "分布抛账模式...");*/

					//投入信息根据板坯号获取
					cs_slab_no = dtNewMat.Rows[i]["SLAB_NO"].ToString();

					/*Log::Trace("", __FUNCTION__, " slab_no...{0}", cs_slab_no);*/

					//准发时，根据合同性质
					sqlstr = "select 'TMMSM01',t.* from tmmsm01 t where mat_no = @cs_slab_no union all select 'HMMSM01',b.* from hmmsm01 b where mat_no = @cs_slab_no ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("cs_slab_no", cs_slab_no);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						table_name_in = cmd_inq.GetString(1);
						cmd_inq.Fetch(tmmsm01, 2);

						PRODUCT_CODE_IN = tmmsm01["PRODUCT_CODE"];

						mat_act_wt_in = tmmsm01["MAT_WT"];

						d_in_mat_wt = dtNewMat.Rows[i]["IN_MAT_WT"].ToDecimal();
						if (d_in_mat_wt < mat_act_wt_in)
						{
							mat_act_wt_in = d_in_mat_wt;
							tmmsm01["MAT_ACT_WT"] = mat_act_wt_in;
							/*Log::Trace("", __FUNCTION__, " 中间坯一卷一坯原料重量为mat_act_wt_in...{0}", mat_act_wt_in);*/
						}
					}
					cmd_inq.Close();

					if (table_name_in > " ")
					{
						tmmsm01.MergeTo(dtOldMat, false);

						//更新计产投料标记
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:
							sqlstr = " UPDATE " + table_name_in + " "
								" SET PROD_CONFM = '2' "
								" WHERE MAT_NO = @cs_slab_no ";
							break;
						}
						cmd_upd.SetCommandText(sqlstr);
						cmd_upd.Parameters.Set("cs_slab_no", cs_slab_no);
						cmd_upd.ExecuteNonQuery();
					}
				}
				else if (eventId == "MM18")  //厚板、热轧均可按此逻辑获取
				{
					//Log::Trace("", __FUNCTION__, " 二切分布抛账模式...");

					//投入信息根据板坯号获取
					cs_slab_no = dtNewMat.Rows[i]["IN_MAT_NO"].ToString();

					//Log::Trace("", __FUNCTION__, " in_mat_no...{0}", cs_slab_no);

					//准发时，根据合同性质
					sqlstr = "select 'TMMSM01',t.* from tmmsm01 t where mat_no = @cs_slab_no union all select 'HMMSM01',b.* from hmmsm01 b where mat_no = @cs_slab_no";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("cs_slab_no", cs_slab_no);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						table_name_in = cmd_inq.GetString(1);
						cmd_inq.Fetch(tmmsm01, 2);

						PRODUCT_CODE_IN = tmmsm01["PRODUCT_CODE"];

						if (tmmsm01["PROD_CONFM"].ToString() == " ")
						{
							//抛投料一笔抛掉
							mat_act_wt_in = tmmsm01["MAT_ACT_WT"];
							d_in_mat_wt = mat_act_wt_in; // 二切时第一笔就把入口都抛掉
						}

					}
					cmd_inq.Close();

					//Log::Trace("", __FUNCTION__, " table_name_in = [{0}]..", table_name_in);

					if (table_name_in > " ")  //如果是第一笔的时候，则更新产出标记，但不插入历史表
					{
						if (tmmsm01["PROD_CONFM"].ToString() == "2")
						{
							tmmsm01["MAT_ACT_WT"] = 0; //当已经投料时，发一条重量空的空投入
						}

						tmmsm01.MergeTo(dtOldMat, false);

						//更新计产投料标记
						switch (conn->DatabaseKind)
						{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:
							sqlstr = " UPDATE " + table_name_in + " "
								" SET PROD_CONFM = '2' "
								" WHERE MAT_NO = @cs_slab_no ";
							break;
						}
						cmd_upd.SetCommandText(sqlstr);
						cmd_upd.Parameters.Set("cs_slab_no", cs_slab_no);
						cmd_upd.ExecuteNonQuery();
					}

					//Log::Trace("", __FUNCTION__, " 111");

					if (factoryDiv == "J1")
					{
						//4100
						//Log::Trace("", __FUNCTION__, " 222");
						if (dtOldMat.Rows.get_Count() > 0)
						{
							dtOldMat.Rows[i]["UNIT_CODE"] = "J107";
						}
						dtNewMat.Rows[i]["UNIT_CODE"] = "J107";
					}
					else if (factoryDiv == "J2")
					{
						//2700
						if (dtOldMat.Rows.get_Count() > 0)
						{
							dtOldMat.Rows[i]["UNIT_CODE"] = "J206";
						}
						dtNewMat.Rows[i]["UNIT_CODE"] = "J206";
					}
				}
				else if (eventId == "MMP9")  //棒线侧产出抛账
				{
					//Log::Trace("", __FUNCTION__, " " + cs_unit_code + "长材分布抛账模式...");

					matShapeFlag = dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString();

					//棒材 根据-轧制计划号-获取投料的-产副品代码
					if (matShapeFlag == "8" || matShapeFlag == "6" || matShapeFlag == "7")//材料形态 8：棒材
					{

						cs_slab_no = dtNewMat.Rows[i]["ROLL_PLAN_NO"].ToString();

						//Log::Trace("", __FUNCTION__, " slab_no...{0}", cs_slab_no);
						dtOldMat.Rows.Clear();
						//准发时，根据合同性质
						sqlstr = "select 'TMMSM01',t.* from tmmsm01 t where plan_no = @cs_slab_no  union all select 'HMMSM01',b.* from hmmsm01 b where plan_no = @cs_slab_no ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("cs_slab_no", cs_slab_no);
						cmd_inq.ExecuteReader();
						while (cmd_inq.Read())
						{
							table_name_in = cmd_inq.GetString(1);
							cmd_inq.Fetch(tmmsm01, 2);

							PRODUCT_CODE_IN = tmmsm01["PRODUCT_CODE"];

							//Log::Trace("", __FUNCTION__, " 获取投料的方坯号 tmmsm01["MAT_NO"].ToString()...{0}", tmmsm01["MAT_NO"].ToString());

							if (tmmsm01["PROD_CONFM"].ToString() != "2")
							{
								/*d_in_mat_wt = tmmsm01["MAT_ACT_WT"];*/
								//20210928 钢坯已理论重量抛账
								d_in_mat_wt = tmmsm01["MAT_WT"];
								//20220412 钢坯重量用入炉称重重量
								if (tmmsm01["SPARE_ITEM_N1"].ToDecimal() > 0)
								{
									d_in_mat_wt = tmmsm01["SPARE_ITEM_N1"];
								}

								mat_act_wt_in = mat_act_wt_in + d_in_mat_wt;
								tmmsm01.MergeTo(dtOldMat, false);
								//更新计产投料标记
								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
								default:
									sqlstr = " UPDATE " + table_name_in + " "
										" SET PROD_CONFM = '2' "
										" WHERE MAT_NO = @tmmsm01.MAT_NO ";
									break;
								}
								cmd_upd.SetCommandText(sqlstr);
								cmd_upd.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"].ToString());
								cmd_upd.ExecuteNonQuery();
							}
						}
						cmd_inq.Close();
					}
					else //除棒材外   根据-入口材料号-获取投料的-产副品代码
					{
						cs_slab_no = dtNewMat.Rows[i]["IN_MAT_NO"].ToString();

						//Log::Trace("", __FUNCTION__, " slab_no...{0}", cs_slab_no);
						dtOldMat.Rows.Clear();
						//准发时，根据合同性质
						sqlstr = "select 'TMMSM01',t.* from tmmsm01 t where mat_no = @cs_slab_no  union all select 'HMMSM01',b.* from hmmsm01 b where mat_no = @cs_slab_no ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("cs_slab_no", cs_slab_no);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							table_name_in = cmd_inq.GetString(1);
							cmd_inq.Fetch(tmmsm01, 2);

							PRODUCT_CODE_IN = tmmsm01["PRODUCT_CODE"];

							//Log::Trace("", __FUNCTION__, " 获取投料的方坯号 tmmsm01["MAT_NO"].ToString()...{0}", tmmsm01["MAT_NO"].ToString());

							if (tmmsm01["PROD_CONFM"].ToString() != "2")
							{
								/*d_in_mat_wt = tmmsm01["MAT_ACT_WT"];*/
								//20210928 钢坯已理论重量抛账
								d_in_mat_wt = tmmsm01["MAT_THEORY_WT"];
								mat_act_wt_in = mat_act_wt_in + d_in_mat_wt;
								tmmsm01.MergeTo(dtOldMat, false);
								//更新计产投料标记
								switch (conn->DatabaseKind)
								{
								case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
								case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
								case DB_KIND_MSSQL:				// MS SQL Server数据库
								case DB_KIND_ORACLE:	        // Oracle 数据库
								default:
									sqlstr = " UPDATE " + table_name_in + " "
										" SET PROD_CONFM = '2' "
										" WHERE MAT_NO = @tmmsm01.MAT_NO ";
									break;
								}
								cmd_upd.SetCommandText(sqlstr);
								cmd_upd.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"].ToString());
								cmd_upd.ExecuteNonQuery();
							}
						}
						cmd_inq.Close();
					}
				}
				AC_TYPE = " ";  // 分段模式取值后变为双记录模式
			}

			//热处理按JA00 JD00机组抛一笔记录TODO: 

			//当炼钢侧


			if (AC_TYPE == " " || AC_TYPE == "1") //以NEW数据抛
			{
				//Log::Trace("", __FUNCTION__, " ready");

				if (dtNewMat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "抛成本事件出口信息缺失，请查看事件配置抛账类型...");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tom01.Reset();
				//获取可能有的合同信息
				if (dtNewMat.Rows[i]["ORDER_NO"].ToString() > " ")
				{
					tom01["ORDER_NO"] = dtNewMat.Rows[i]["ORDER_NO"].ToString();
					tom01.Query("ORDER_NO");
				}

				matKind = dtNewMat.Rows[i]["MAT_KIND"].ToString();
				matShapeFlag = dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString();
				upd_mat_no = dtNewMat.Rows[i]["MAT_NO"].ToString();
				//TODO: 获取入口产副品代码 需要根据每个机组的情况去考虑，投入产副品需带在主档上product_code_2字段上
				//如果是炼钢侧的数据，投入产副品需定制
				if (dtOldMat.Rows.get_Count() > 0)
				{
					tacaich.DEVO_PRODUCT_CODE = dtOldMat.Rows[i]["PRODUCT_CODE"].ToString();
				}
				//20211022 盘盈新增增加投入产副品代码
				if (TRANSACTION_CODE == "X" && FUNCTION_CODE_AC == "N" && matKind == "SM" && dtNewMat.Rows[i]["HEAT_NO"].ToString() != "")
				{
					if (dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "E" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "F")//草铺一期转炉
					{
						tacaich.DEVO_PRODUCT_CODE = "80210";
					}
					else if (dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "G" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "H")//草铺二期转炉
					{
						tacaich.DEVO_PRODUCT_CODE = "80220";
					}
					else if (dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "A" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "B" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "9")//红河
					{
						tacaich.DEVO_PRODUCT_CODE = "80230";
					}
					else if (dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "7" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "8" || dtNewMat.Rows[i]["HEAT_NO"].ToString().Substring(2, 1) == "C")//玉溪
					{
						tacaich.DEVO_PRODUCT_CODE = "80240";
					}
				}
				//20211022 盘盈新增增加投入产副品代码
				if (TRANSACTION_CODE == "X" && FUNCTION_CODE_AC == "N" && matKind != "SM")
				{
					if (dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "I601")
					{
						tacaich.DEVO_PRODUCT_CODE = "A1240";
					}
					else if (dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "L601")
					{
						tacaich.DEVO_PRODUCT_CODE = "B1240";
					}
					else if (dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "W301" || dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "L301")
					{
						tacaich.DEVO_PRODUCT_CODE = "B1230";
					}
					else if (dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "L101" || dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "L201" || dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "L501" || dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "W101" || dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "W201")
					{
						tacaich.DEVO_PRODUCT_CODE = "B1210";
					}
					else if (dtNewMat.Rows[i]["UNIT_CODE"].ToString() == "X101")
					{
						tacaich.DEVO_PRODUCT_CODE = "B2210";
					}
				}

				/*Log::Trace("", __FUNCTION__, " oldMat.rowcount= {0}", dtOldMat.Rows.get_Count());
				Log::Trace("", __FUNCTION__, " i= {0}", i);

				Log::Trace("", __FUNCTION__, " OUT-SIDE DEVO_PRODUCT_CODE = {0}", tacaich.DEVO_PRODUCT_CODE);*/

				if (TRANSACTION_CODE != "F")
				{
					if (cs_unit_code_spec > " ")
					{
						tacaich.TOWARD_MCHN_MODE = cs_unit_code_spec;
					}
					else
					{
						tacaich.TOWARD_MCHN_MODE = dtNewMat.Rows[i]["UNIT_CODE"].ToString();  //TODO: 暂时按出口侧机组代码抛，需考虑原料机组的情况

						//炼钢产出机组是两位，通过转换小代码转换成4位抛成本
						sqlstr = "SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MM01' AND CODE_DESC_2_CONTENT=@UNIT_CODE ";

						//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Clear();
						cmd_inq.Parameters.Set("UNIT_CODE", tacaich.TOWARD_MCHN_MODE);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							tacaich.TOWARD_MCHN_MODE = cmd_inq.GetString(1);
						}
						cmd_inq.Close();

					}
				}
				else tacaich.TOWARD_MCHN_MODE = " ";

				//20211006 成本要求不加去向机组
				tacaich.TOWARD_MACHINE = " ";
				//Log::Trace("", __FUNCTION__, " COMPLEX_DECIDE_CODE1= {0}", dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString());
				if (dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString() == "4")
				{
					tacaich.QUALITY_TEST_CODE = "4"; //质量码
				}
				else if (dtNewMat.Columns.Contains("PROD_CLASS") && dtNewMat.Rows[i]["PROD_CLASS"].ToString() == "3")
				{
					tacaich.QUALITY_TEST_CODE = "3"; //可利用材
				}
				else
				{
					tacaich.QUALITY_TEST_CODE = "1"; //合格品
				}

				//Log::Trace("", __FUNCTION__, " tacaich.QUALITY_TEST_CODE3{0}", tacaich.QUALITY_TEST_CODE);
				//	CAST_NO
				if (matKind == "SM")
				{
					tacaich.CAST_NO = dtNewMat.Rows[i]["CAST_NO"].ToString();
				}
				else tacaich.CAST_NO = " ";
				//	EVENT_ID
				tacaich.EVENT_ID = dtEventData.Rows[0]["EVENT_ID"].ToString();
				//	FACTORY_DIV
				tacaich.FACTORY_DIV = dtNewMat.Rows[i]["FACTORY_DIV"].ToString();
				//Log::Trace("", __FUNCTION__, " FACTORY-div_new = {0}", tacaich.FACTORY_DIV);
				//ST_NO
				tacaich.ST_NO = dtNewMat.Rows[i]["ST_NO"].ToString();
				//PSR
				tacaich.PSR = dtNewMat.Rows[i]["PSC"].ToString();
				//APN
				tacaich.APN = dtNewMat.Rows[i]["APN"].ToString();
				//MSC
				tacaich.MSC = dtNewMat.Rows[i]["MSC"].ToString();
				//SG_SIGN
				tacaich.SG_SIGN = dtNewMat.Rows[i]["SG_SIGN"].ToString();
				//MAT_NO
				tacaich.MAT_NO = dtNewMat.Rows[i]["MAT_NO"].ToString();
				//MAT_ACT_WIDTH
				tacaich.MAT_ACT_WIDTH = dtNewMat.Rows[i]["MAT_ACT_WIDTH"];
				//MAT_ACT_THICK
				tacaich.MAT_ACT_THICK = dtNewMat.Rows[i]["MAT_ACT_THICK"];
				//MAT_ACT_LEN
				tacaich.MAT_ACT_LEN = dtNewMat.Rows[i]["MAT_ACT_LEN"];
				//MAT_WIDTH_CODE 宽度代码 //TODO等定义下来后
				//MAT_THICK_CODE 厚度代码 //TODO等定义下来后
				if (matKind != "SM")
				{
					if (dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8" || dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "7")
					{
						sqlstr = "select OUT_MAT_THICK from tmm0023 where OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and MAT_SHAPE_FLAG=@MAT_SHAPE_FLAG";

						//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Clear();
						//cmd_inq.Parameters.Set("st_no", dtNewMat.Rows[i]["ST_NO"].ToString());
						//cmd_inq.Parameters.Set("SG_SIGN", dtNewMat.Rows[i]["SG_SIGN"].ToString());
						cmd_inq.Parameters.Set("MAT_THICK", dtNewMat.Rows[i]["MAT_ACT_THICK"].ToString());
						if (dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8")
						{
							cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "8");
						}
						else
						{
							cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "7");
						}
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
						}
						cmd_inq.Close();
					}
					else if (dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "6")
					{
						sqlstr = "select OUT_MAT_THICK,OUT_MAT_WIDTH from tmm0023 where OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAg='6'";

						//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Clear();
						//cmd_inq.Parameters.Set("st_no", dtNewMat.Rows[i]["ST_NO"].ToString());
						//cmd_inq.Parameters.Set("SG_SIGN", dtNewMat.Rows[i]["SG_SIGN"].ToString());
						cmd_inq.Parameters.Set("MAT_THICK", dtNewMat.Rows[i]["MAT_ACT_THICK"].ToString());
						cmd_inq.Parameters.Set("MAT_WIDTH", dtNewMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
							tacaich.MAT_WIDTH_CODE = to_string(cmd_inq.GetInt32(2));
						}
						cmd_inq.Close();
					}
					else if (dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "9")
					{
						sqlstr = "select OUT_MAT_MIN_THICK,OUT_MAT_MAX_THICK,OUT_MAT_MIN_WIDTH,OUT_MAT_MAX_WIDTH from tmm0023 where  OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAG='9'";

						//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Clear();
						//cmd_inq.Parameters.Set("st_no", dtNewMat.Rows[i]["ST_NO"].ToString());
						//cmd_inq.Parameters.Set("SG_SIGN", dtNewMat.Rows[i]["SG_SIGN"].ToString());
						cmd_inq.Parameters.Set("MAT_THICK", dtNewMat.Rows[i]["MAT_ACT_THICK"].ToString());
						cmd_inq.Parameters.Set("MAT_WIDTH", dtNewMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							tacaich.MAT_THICK_CODE = "[" + to_string(cmd_inq.GetInt32(1)) + "," + to_string(cmd_inq.GetInt32(2)) + "]";
							tacaich.MAT_WIDTH_CODE = "[" + to_string(cmd_inq.GetInt32(3)) + "," + to_string(cmd_inq.GetInt32(4)) + "]";
						}
						cmd_inq.Close();
					}

				}



				//SURPLUS_FLAG 余材代码
				tacaich.SURPLUS_FLAG = dtNewMat.Rows[i]["ORDER_NO"].ToString() == " " ? "1" : "0";
				//MAT_ACT_WT
				//20210928 钢坯产出时已理论重量抛账
				if (tacaich.APP_CODE == "IS")
				{
					/*tacaich.MAT_ACT_WT = dtNewMat.Rows[i]["MAT_THEORY_WT"];*/
					//20211124 统一改成以三级抛上来的mat_wt抛账
					tacaich.MAT_ACT_WT = dtNewMat.Rows[i]["MAT_WT"];
				}
				else
				{
					//20211124 统一改成以三级抛上来的mat_wt抛账
					tacaich.MAT_ACT_WT = dtNewMat.Rows[i]["MAT_WT"];
				}
				//WEIGHT_UNITM
				tacaich.WEIGHT_UNITM = "T";
				//NUM
				tacaich.NUM = 1;//暂时默认为1
				//QTY_UNIT
				tacaich.QTY_UNIT = " "; //暂时不给
				//IN_MAT_NO
				//Log::Trace("", __FUNCTION__, " matShapeFlag = {0}", matShapeFlag);
				if (matShapeFlag == "8" || matShapeFlag == "6")
				{
					tacaich.IN_MAT_NO = dtNewMat.Rows[i]["ROLL_PLAN_NO"].ToString(); //棒型按轧制计划号传入

					cs_in_mat_no_bw = tacaich.IN_MAT_NO;
				}
				else if (matKind == "SM")
				{
					//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
					if (dtNewMat.Rows[i]["SPARE_ITEM_1"].ToString().Trim() != "")
					{
						tacaich.IN_MAT_NO = dtNewMat.Rows[i]["SPARE_ITEM_1"].ToString(); // 板坯按炉号传
					}
					else
					{
						tacaich.IN_MAT_NO = dtNewMat.Rows[i]["HEAT_NO"].ToString(); // 板坯按炉号传
					}
				}
				else if (matShapeFlag == "7")
				{
					tacaich.IN_MAT_NO = dtNewMat.Rows[i]["IN_MAT_NO"].ToString();  //线材按传入板坯号
				}
				else
				{
					tacaich.IN_MAT_NO = dtNewMat.Rows[i]["SLAB_NO"].ToString();  //其他按传入板坯号
				}
				//OLD_PSR
				tacaich.OLD_PSR = " ";    //入口信息不给
				//DEVO_PRODUCT_CODE

				/*Log::Trace("", __FUNCTION__, " product_code_2 = {0}", dtNewMat.Rows[i]["PRODUCT_CODE_2"].ToString());
				Log::Trace("", __FUNCTION__, " unit_code = {0}", dtNewMat.Rows[i]["UNIT_CODE"].ToString());
				Log::Trace("", __FUNCTION__, " PRODUCT_CODE_IN = {0}", PRODUCT_CODE_IN);*/

				if (PRODUCT_CODE_IN > " ")
				{
					tacaich.PRDT_CODE_BFR_TRNC = PRODUCT_CODE_IN;
				}
				else
				{
					/*if (TRANSACTION_CODE == "P" && (FUNCTION_CODE_AC == "N" || FUNCTION_CODE_AC == "D"))*/
					if ((TRANSACTION_CODE == "P" || TRANSACTION_CODE == "X") && FUNCTION_CODE_AC == "N")//20211018 材料删除时前产副品代码在PRODUCT_CODE上，PRODUCT_CODE_2存的是入口的产副品代码
					{
						tacaich.PRDT_CODE_BFR_TRNC = dtNewMat.Rows[i]["PRODUCT_CODE_2"].ToString();
						if (eventId == "MMA7" || eventId == "WMA1")// && (tacaich.APP_CODE == "IW" || tacaich.APP_CODE == "IH"))//WMA1互供钢坯入库确认，互供前产副品代码取其他基地准发后产副品代码
						{
							tacaich.PRDT_CODE_BFR_TRNC = dtNewMat.Rows[i]["PRODUCT_CODE"].ToString();//棒线关联交易，已外购事件号抛账，产副品代码存在PRODUCT_CODE上
							if (eventId == "MMA7")//MMA7关联交易
							{
								tacaich.ACCOUNT = "2801";//目前只有其他基地往草铺卖，账套固定为2801
							}
						}
					}
					else
					{
						if (dtOldMat.Rows.get_Count() > 0)
						{
							//非投入产出类型的，需要取历史数据
							tacaich.PRDT_CODE_BFR_TRNC = dtOldMat.Rows[i]["PRODUCT_CODE"].ToString();
						}
						else
						{
							if (eventId == "MM27")
							{
								//如果是外购，则直接用材料
								tacaich.PRDT_CODE_BFR_TRNC = dtNewMat.Rows[i]["PRODUCT_CODE"].ToString();
							}
							else
							{
								//Log::Trace("", __FUNCTION__, " 非投入产出类型的数据没有入口信息！！");
								sprintf(s.msg, "非投入产出类型的数据没有入口信息！！...");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

						}
					}
				}
				//Log::Trace("", __FUNCTION__, " tacaich.PRDT_CODE_BFR_TRNC1 = {0}", tacaich.PRDT_CODE_BFR_TRNC);
				if (tacaich.PRDT_CODE_BFR_TRNC.Trim() == "")
				{
					sprintf(s.msg, "前产副品代码不允许为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				//IN_MAT_WIDTH
				tacaich.IN_MAT_WIDTH = 0; //入口信息不给
				//IN_MAT_THICK
				tacaich.IN_MAT_THICK = 0; //入口信息不给
				//IN_MAT_LEN
				tacaich.IN_MAT_LEN = 0; //入口信息不给
				//PASS_DSS_OK
				tacaich.PASS_DSS_OK = " ";  //暂不提供
				//CTRL_ROLL_THICK_1
				//tacaich.CTRL_ROLL_THICK_1 = //TODO: 第一阶段控轧点厚度暂时不给
				//IN_MAT_WT_AI
				//DEVO_PRODUCT_CODE
				if (mat_act_wt_in > 0)
				{
					//如果入口重量之前有赋值，则直接用此字段
					tacaich.IN_MAT_WT_AI = mat_act_wt_in;
				}
				else
				{
					//如果之前没赋值，则不是投入产出分布情况，则可以直接用old记录，或不给（AC并不需要非投入产出类型的入口重量ｍａｙｂｅ）
					if (eventId == "MM09" && tacaich.APP_CODE == "IS")
					{
						//如果是炼钢侧切断产出，则入口材料重量用出口材料重量代替
						/*tacaich.IN_MAT_WT_AI = tacaich.MAT_ACT_WT;*/
						//20210928 钢坯已理论重量抛账
						tacaich.IN_MAT_WT_AI = d_in_mat_wt;
					}
					else tacaich.IN_MAT_WT_AI = 0;
				}
				if (eventId == "MMF6")
				{
					//线材轧废的时候，new记录
					tacaich.IN_MAT_WT_AI = tacaich.MAT_ACT_WT;
					tacaich.MAT_ACT_WT = 0;
					tacaich.PRDT_CODE_BFR_TRNC = dtNewMat.Rows[i]["PRODUCT_CODE"].ToString();
					tacaich.IN_MAT_NO = dtNewMat.Rows[i]["MAT_NO"].ToString(); //线材轧废时 入口材料保留为方坯号
				}

				//NEW_PROD_AGREE_NO
				tacaich.NEW_PROD_AGREE_NO = dtNewMat.Rows[i]["NEW_TEST_NO"].ToString();
				//PERSIST_PROD_TIME 持续生产时间
				//tacaich.PERSIST_PROD_TIME //暂时不提供
				//BACKLOG
				tacaich.BACKLOG = dtNewMat.Rows[i]["WHOLE_BACKLOG"].ToString();
				//INT_BACKLOG
				tacaich.INT_BACKLOG = dtNewMat.Rows[i]["WHOLE_BACKLOG"].ToString();  //先随便糊弄一个
				//AC_ROUTE
				tacaich.AC_ROUTE = "00"; //由成本自己生成
				//PRE_UNIT_CODE
				tacaich.PRE_UNIT_CODE = " "; //暂时不给
				//PLAN_NO
				tacaich.PLAN_NO = dtNewMat.Rows[i]["PLAN_NO"].ToString();
				//STOCK_NO
				tacaich.STOCK_NO = dtNewMat.Rows[i]["STOCK_NO"].ToString();
				//CUST_ORDER_NO
				tacaich.CUST_ORDER_NO = dtNewMat.Rows[i]["ORDER_NO"].ToString();
				//ROLL_ABN_CODE
				tacaich.ROLL_ABN_CODE = " ";  //暂时不给
				//FIN_SURF_CODE
				tacaich.FIN_SURF_CODE = " ";  //暂时用不上
				//PACK_TYPE_CODE
				tacaich.PACK_TYPE_CODE = dtNewMat.Rows[i]["PACK_TYPE_CODE"].ToString();
				//SURF_STRUC_CODE  表面结构码
				tacaich.SURF_STRUC_CODE = " ";  //暂时用不上
				//PROD_END_TIME
				tacaich.PROD_END_TIME = " "; // dtNewMat.Rows[i]["PROD_END_TIME"].ToString();
				//PROD_START_TIME
				tacaich.PROD_START_TIME = " ";  //暂时用不上 如需要需分机组定制
				//CTRL_ROLL_CODE_1
				//CTRL_ROLL_CODE
				if (matKind == "HP")
				{
					//厚板产线才有控轧代码
					tacaich.CTRL_ROLL_CODE_1 = dtNewMat.Rows[i]["CTRL_ROLL_CODE"].ToString().Substring(0, 1);
					tacaich.CTRL_ROLL_CODE = dtNewMat.Rows[i]["CTRL_ROLL_CODE"].ToString();
				}
				else
				{
					tacaich.CTRL_ROLL_CODE_1 = " ";
					tacaich.CTRL_ROLL_CODE = " ";
				}
				//HEAT_MODE
				tacaich.HEAT_MODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
				//PAINT_CODE
				tacaich.PAINT_CODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
				//HOT_SEND_DIV
				//HOT_SEND_FLAG_ACT
				if (matKind == "SM")
				{
					tacaich.HOT_SEND_DIV = dtNewMat.Rows[i]["HOT_SEND_FLAG"].ToString();
					tacaich.HOT_SEND_FLAG_ACT = tacaich.HOT_SEND_DIV;
				}
				else
				{
					tacaich.HOT_SEND_DIV = " ";
					tacaich.HOT_SEND_FLAG_ACT = " ";
				}
				//SHOT_PAINT_AI
				tacaich.SHOT_PAINT_AI = " "; // 涂料涂漆代码，暂不需要
				//PROD_CODE_HP
				//tacaich.PROD_CODE_HP = dtNewMat.Rows[i]["PROD_CODE_HP"].ToString(); // TODO:目前hr bw没此字段，需后处理
				//SHP_CODE
				tacaich.SHP_CODE = dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString();
				//PLATE_ID
				tacaich.PLATE_ID = 0; //目前已不使用此字段，暂默认0
				//HEAT_PROC_LOG
				tacaich.HEAT_PROC_LOG = " "; //HP暂无，需后处理			
				//IN_PLATE_ID
				tacaich.IN_PLATE_ID = 0; //目前已不使用此字段，暂默认0
				//SPECIAL_FLAG_TYPE
				tacaich.SPECIAL_FLAG_TYPE = " ";  //暂不使用此字段
				//PRST_BK_PT
				tacaich.PRST_BK_PT = dtNewMat.Rows[i]["WHOLE_BACKLOG_CODE"].ToString();
				//LN_SLAB_APPT_WT
				tacaich.LN_SLAB_APPT_WT = 0; //暂不使用
				//SH_SLAB_APPT_WT
				tacaich.SH_SLAB_APPT_WT = 0; //暂不使用
				//AD_MODE_CODE
				if (matKind == "HP")
				{
					tacaich.AD_MODE_CODE = dtNewMat.Rows[i]["AD_MODE_CODE"].ToString();
				}
				else tacaich.AD_MODE_CODE = " ";
				//FUR_NO
				tacaich.FUR_NO = " ";  //暂不传
				//PICK_PLATE_MARK
				if (matKind == "HP")
				{
					tacaich.PICK_PLATE_MARK = dtNewMat.Rows[i]["PICK_PLATE_MARK"].ToString();
				}
				else tacaich.PICK_PLATE_MARK = " ";
				//COMMAND_YIELD
				tacaich.COMMAND_YIELD = 0;  //暂不传，需要则从四大命令表取组板成材率
				//OA_PLATE_FLAG
				tacaich.OA_PLATE_FLAG = " ";  //暂不传，感觉只有准发点才有用
				//HEAD_SAMPLE_SUM_LEN,
				tacaich.HEAD_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
				//HEAD_SAMPLE_SUM_WIDTH,
				tacaich.HEAD_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
				//BOT_SAMPLE_SUM_WIDTH,
				tacaich.BOT_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
				//BOT_SAMPLE_SUM_LEN,
				tacaich.BOT_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
				//HEAT_TREAT_KEEP,
				tacaich.HEAT_TREAT_KEEP = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//HEAT_TREAT_STAY_TIME,
				tacaich.HEAT_TREAT_STAY_TIME = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_SLAB_THICK,
				tacaich.ROLL_SLAB_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_SLAB_WIDTH,
				tacaich.ROLL_SLAB_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_SLAB_LEN,
				tacaich.ROLL_SLAB_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_PLT_THICK,
				tacaich.ROLL_PLT_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_PLT_WIDTH,
				tacaich.ROLL_PLT_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//ROLL_PLT_LEN,
				tacaich.ROLL_PLT_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
				//MAT_TRACK_NO,
				tacaich.MAT_TRACK_NO = dtNewMat.Rows[i]["MAT_TRACK_NO"].ToString();
				//DEST_ID,
				//tacaich.DEST_ID = " "; //暂不使用
				//RAW_ORIGIN,
				tacaich.RAW_ORIGIN = dtNewMat.Rows[i]["RAW_ORIGIN"].ToString();
				//HEAT_TREAT_PLANT,
				tacaich.HEAT_TREAT_PLANT = " "; //暂不使用，需要则根据热处理机组记录即可
				//IC_CC_FLAG,
				tacaich.IC_CC_FLAG = " "; //暂不使用，重钢都是连铸：模连铸标记（此处不做赋值先）
				//ORDER_TYPE_CODE,
				tacaich.ORDER_TYPE_CODE = tom01["ORDER_TYPE_CODE"];
				//ACCP_AUTH_CODE,
				tacaich.ACCP_AUTH_CODE = " "; //暂不使用，验收机关需要也只有tom01hp表中有
				//ROLL_DIRECT_CODE,
				tacaich.ROLL_DIRECT_CODE = " "; //暂不使用，需要则从RS代码中截取
				//SIN_TWO_FUR_DIV,
				tacaich.SIN_TWO_FUR_DIV = " "; //暂不使用，单双炉区分
				//IN_FUR_PRED,
				if (matKind != "SM")
				{
					tacaich.IN_FUR_PRED = 0; //暂不使用，需要时取轧制实绩表
				}
				else tacaich.IN_FUR_PRED = 0; //暂不使用，单双炉区分
				//ROLL_CYC_NUM,
				if (matKind != "SM")
				{
					tacaich.ROLL_CYC_NUM = 0; //暂不使用，需要时取轧制实绩表
				}
				else tacaich.ROLL_CYC_NUM = 0; //暂不使用，单双炉区分
				//TAP_SLAB_TEMP_AVE,
				if (matKind != "SM")
				{
					tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，需要时取轧制实绩表
				}
				else tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，单双炉区分
				//WT_METHOD_CODE,
				tacaich.WT_METHOD_CODE = tom01["WT_METHOD_CODE"];
				//WITH_SIDE_FLAG,
				if (matKind == "HP")
				{
					tacaich.WITH_SIDE_FLAG = dtNewMat.Rows[i]["WITH_SIDE_FLAG"].ToString();
				}
				else if (matKind == "HR")
				{
					tacaich.WITH_SIDE_FLAG = dtNewMat.Rows[i]["TRIM_FLAG"].ToString();
				}
				else tacaich.WITH_SIDE_FLAG = " ";
				//FIX_FLAG,
				tacaich.FIX_FLAG = tom01["FIX_FLAG"];
				//ACT_BACKLOG,
				tacaich.ACT_BACKLOG = dtNewMat.Rows[i]["WHOLE_BACKLOG"].ToString();
				//NOW_BACKLOG,
				tacaich.NOW_BACKLOG = dtNewMat.Rows[i]["WHOLE_BACKLOG_ACT"].ToString();
				//SUB_BACKLOG_CODE,
				if (matKind == "HP")
				{
					tacaich.SUB_BACKLOG_CODE = dtNewMat.Rows[i]["SUB_BACKLOG_CODE"].ToString();
				}
				else tacaich.SUB_BACKLOG_CODE = " ";
				//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
				if (matKind == "SM" && dtNewMat.Rows[i]["SPARE_ITEM_1"].ToString().Trim() != "")
				{
					tacaich.HEAT_NO = dtNewMat.Rows[i]["SPARE_ITEM_1"].ToString();
					tacaich.PONO = dtNewMat.Rows[i]["SPARE_ITEM_2"].ToString();
				}
				else
				{
					//HEAT_NO,
					tacaich.HEAT_NO = dtNewMat.Rows[i]["HEAT_NO"].ToString();
					//PONO,
					tacaich.PONO = dtNewMat.Rows[i]["PONO"].ToString();
				}
				//RL_NO,
				if (matKind != "SM")
				{
					tacaich.RL_NO = dtNewMat.Rows[i]["SAMPLE_LOT_NO"].ToString();
				}
				else tacaich.RL_NO = " ";
				//PRODUCT_TOL,
				tacaich.PRODUCT_TOL = 0;
				//BAF_NO,
				tacaich.BAF_NO = " ";
				//ROLL_MODE,
				tacaich.ROLL_MODE = " "; //暂不提供，需要时从计划或命令表中获取
				//HSF_PLAN_NO,
				tacaich.HSF_PLAN_NO = " "; //精整计划号，暂不提供，需要的话，根据机组取plan_no即可
				//REFINE_ROUTE_CODE,
				if (matKind == "SM")
				{
					tacaich.REFINE_ROUTE_CODE = dtNewMat.Rows[i]["REFINE_ROUTE_CODE"].ToString();
				}
				else tacaich.REFINE_ROUTE_CODE = " ";
				//HOT_TREAT_METHOD,
				tacaich.HOT_TREAT_METHOD = " "; //暂不提供，如需要，根据热处理小工序截取
				//APP_THROW_AI_HEAD,
				tacaich.APP_THROW_AI_HEAD = " "; //由成分生成
				//APP_THROW_AI_ID,
				tacaich.APP_THROW_AI_ID = " "; //由成分生成
				//APP_THROW_AI_NUM,
				tacaich.APP_THROW_AI_NUM = 0; //由成分生成
				//COILED_TIME, 
				tacaich.COILED_TIME = " ";  //暂不提供，钢卷卷曲时间，如需获取取HR生产时刻或实绩时间
				//COLD_HOT_FLAG1,
				tacaich.COLD_HOT_FLAG1 = 0;  //重钢无
				//DEST_FIN,
				tacaich.DEST_FIN = " ";  //最终去向，暂不提供
				//DISCH_TIME,
				tacaich.DISCH_TIME = " ";  //出炉时间，暂不提供，需要则要从轧制实绩取
				//GRADE_AI,
				tacaich.GRADE_AI = " ";  //暂不提供
				//HARDNESS_GROUP_CODE,
				tacaich.HARDNESS_GROUP_CODE = " ";  //暂不提供
				//IN_INGOT_TYPE,
				tacaich.IN_INGOT_TYPE = " ";  //暂不提供
				//IN_MELT_TIME,
				tacaich.IN_MELT_TIME = " ";  //暂不提供
				//INGOT_TYPE,
				tacaich.INGOT_TYPE = " ";  //暂不提供
				//NEW_TEST_NO,
				tacaich.NEW_TEST_NO = dtNewMat.Rows[i]["NEW_TEST_NO"].ToString();
				//PICKL_TRIM_FLAG,
				tacaich.PICKL_TRIM_FLAG = " ";  //暂不提供 酸洗切边标记（重钢无）
				//PRIM_UNIT,
				tacaich.PRIM_UNIT = " ";  //暂不提供 原机组代码，暂无法提供
				//PROD_TIME,
				tacaich.PROD_TIME = dtNewMat.Rows[i]["PROD_TIME"].ToString();
				//REPAIR_FLAG_AI,
				tacaich.REPAIR_FLAG_AI = dtNewMat.Rows[i]["REPAIR_FLAG"].ToString();
				//TEMPER,
				tacaich.TEMPER = " ";
				//WEIGHT_REAL_STEEL,
				tacaich.WEIGHT_REAL_STEEL = 0; //暂不提供，需要则需从装炉实际中获取
				//WT_METHOD,
				tacaich.WT_METHOD = dtNewMat.Rows[i]["MEASURE_WT_FLAG"].ToString();
				//TRIM_FLAG,
				tacaich.TRIM_FLAG = tom01["TRIM_FLAG"];
				//RS_CODE,
				tacaich.RS_CODE = " "; //暂不提供，只有厚板轧制才有，从命令表中获取 
				tacaich.BACK_CODE_1 = cs_shift_no;//记录班次
				//tacaich.BACK_CODE_2 = cs_shift_group; //记录班组
				tacaich.BACK_CODE_2 = " "; //记录班组
				tacaich.BACK_CODE_6 = cs_date; //生产日
				tacaich.BACK_CODE_7 = eventId;//20211008 添加事件号
				//BACK_CODE_4,
				//BACK_CODE_5,
				//BACK_CODE_6,
				//BACK_CODE_7,
				//tacaich.BACK_CODE_8,//宽度码厚度码合并字段
				if ((dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8" || dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "7") && tacaich.MAT_THICK_CODE.Trim() != "")
				{

					tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE;

				}
				else if ((dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "6" || dtNewMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "9") && tacaich.MAT_THICK_CODE.Trim() != "" && tacaich.MAT_WIDTH_CODE.Trim() != "")
				{
					tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE + ";" + tacaich.MAT_WIDTH_CODE;
				}

				tacaich.MergeTo(bcls_rec_ac.Tables[BLOCKNAME], false);
				tmmbwac.Reset();
				tmmbwac.CopyFrom(tacaich);
				tmmbwac["EVENT_DATETIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmbwac["EVENT_ID"] = eventId;
				tmmbwac["EVENT_DESC"] = event_name + "-产出信息";
				tmmbwac["MAT_ACT_WT"] = tacaich.MAT_ACT_WT;
				tmmbwac["MAT_NUM"] = tacaich.NUM;
				tmmbwac["ORDER_NO"] = tacaich.CUST_ORDER_NO;
				tmmbwac["APP_CODE"]= tacaich.APP_CODE;
				tmmbwac["COMPANY_CODE"] = tacaich.ACCOUNT;
				tmmbwac["TRANSACTION_CODE"] = tacaich.TRANSACTION_CODE;
				tmmbwac["FUNCTION_CODE_AC"] = tacaich.FUNCTION_CODE_AC;
				tmmbwac["UNIT_CODE"] = tacaich.TOWARD_MCHN_MODE;
				tmmbwac["QUALITY_TEST_CODE"] = tacaich.QUALITY_TEST_CODE;
				bcls_tmmbwac.Tables["TMMBWAC"].Clear();
				tmmbwac.MergeTo(bcls_tmmbwac.Tables["TMMBWAC"], false);
			}
			// 需单独取的字段
			if ((AC_TYPE == " " || AC_TYPE == "2") && dtOldMat.Rows.get_Count() > 0 ) //以OLD数据抛
			{
				//Log::Trace("", __FUNCTION__, " ready to insert in-side info");

				if (dtOldMat.Rows.get_Count() == 0)
				{
					sprintf(s.msg, "抛成本事件入口信息缺失，请查看事件配置抛账类型...");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (eventId == "MMP9")
				{
					//如果是MMP9，长材按计划抛投入产出时，old记录里有记录则为第一次抛投料，一次抛完
					for (int j = 0; j < dtOldMat.Rows.get_Count(); j++)
					{
						//获取可能有的合同信息
						if (dtOldMat.Rows[j]["ORDER_NO"].ToString() > " ")
						{
							tom01["ORDER_NO"] = dtOldMat.Rows[j]["ORDER_NO"].ToString();
							tom01.Query("ORDER_NO");
						}

						matKind = dtOldMat.Rows[j]["MAT_KIND"].ToString();
						//TODO: 获取入口产副品代码 需要根据每个机组的情况去考虑，投入产副品需带在主档上product_code_2字段上
						//如果是炼钢侧的数据，投入产副品需定制
						tacaich.PRDT_CODE_BFR_TRNC = dtOldMat.Rows[j]["PRODUCT_CODE"].ToString();
						//Log::Trace("", __FUNCTION__, " tacaich.PRDT_CODE_BFR_TRNC2 = {0}", tacaich.PRDT_CODE_BFR_TRNC);
						if (tacaich.PRDT_CODE_BFR_TRNC.Trim() == "")
						{
							sprintf(s.msg, "前产副品代码不允许为空");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
						if (TRANSACTION_CODE != "F")
						{
							if (cs_unit_code_spec > " ")
							{
								tacaich.TOWARD_MCHN_MODE = cs_unit_code_spec;
							}
							else
							{
								tacaich.TOWARD_MCHN_MODE = dtNewMat.Rows[i]["UNIT_CODE"].ToString();  //TODO: 暂时按出口侧机组代码抛，需考虑原料机组的情况
								//炼钢产出机组是两位，通过转换小代码转换成4位抛成本
								sqlstr = "SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MM01' AND CODE_DESC_2_CONTENT=@UNIT_CODE ";

								//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Clear();
								cmd_inq.Parameters.Set("UNIT_CODE", tacaich.TOWARD_MCHN_MODE);
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									tacaich.TOWARD_MCHN_MODE = cmd_inq.GetString(1);
								}
								cmd_inq.Close();
							}
						}
						else tacaich.TOWARD_MCHN_MODE = " ";

						//20211006 成本要求不加去向机组
						tacaich.TOWARD_MACHINE = " ";
						//Log::Trace("", __FUNCTION__, " COMPLEX_DECIDE_CODE2= {0}", dtOldMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString());
						if (dtOldMat.Rows[j]["COMPLEX_DECIDE_CODE"].ToString() == "4")
						{
							tacaich.QUALITY_TEST_CODE = "4"; //质量码
						}
						else if (dtNewMat.Columns.Contains("PROD_CLASS") && dtOldMat.Rows[j]["PROD_CLASS"].ToString() == "3")
						{
							tacaich.QUALITY_TEST_CODE = "3"; //可利用材
						}
						else
						{
							tacaich.QUALITY_TEST_CODE = "1"; //合格品
						}

						//Log::Trace("", __FUNCTION__, " tacaich.QUALITY_TEST_CODE2{0}", tacaich.QUALITY_TEST_CODE);
						//	CAST_NO
						if (matKind == "SM")
						{
							tacaich.CAST_NO = dtOldMat.Rows[j]["CAST_NO"].ToString();
						}
						else tacaich.CAST_NO = " ";
						//	EVENT_ID
						tacaich.EVENT_ID = dtEventData.Rows[0]["EVENT_ID"].ToString();
						//	FACTORY_DIV
						tacaich.FACTORY_DIV = dtOldMat.Rows[j]["FACTORY_DIV"].ToString();

						//ST_NO
						tacaich.ST_NO = dtOldMat.Rows[j]["ST_NO"].ToString();
						//PSR
						tacaich.PSR = dtOldMat.Rows[j]["PSC"].ToString();
						//APN
						tacaich.APN = dtOldMat.Rows[j]["APN"].ToString();
						//MSC
						tacaich.MSC = dtOldMat.Rows[j]["MSC"].ToString();
						//SG_SIGN
						tacaich.SG_SIGN = dtOldMat.Rows[j]["SG_SIGN"].ToString();
						//MAT_NO
						tacaich.MAT_NO = dtOldMat.Rows[j]["MAT_NO"].ToString();
						//MAT_ACT_WIDTH
						tacaich.MAT_ACT_WIDTH = dtOldMat.Rows[j]["MAT_ACT_WIDTH"];
						//MAT_ACT_THICK
						tacaich.MAT_ACT_THICK = dtOldMat.Rows[j]["MAT_ACT_THICK"];
						//MAT_ACT_LEN
						tacaich.MAT_ACT_LEN = dtOldMat.Rows[j]["MAT_ACT_LEN"];
						//MAT_WIDTH_CODE 宽度代码 //TODO等定义下来后
						//MAT_THICK_CODE 厚度代码 //TODO等定义下来后
						if (matKind != "SM")
						{
							if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8" || dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "7")
							{
								sqlstr = "select OUT_MAT_THICK from tmm0023 where OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and MAT_SHAPE_FLAG=@MAT_SHAPE_FLAG";

								//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Clear();
								//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
								//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
								cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
								if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8")
								{
									cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "8");
								}
								else
								{
									cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "7");
								}
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
								}
								cmd_inq.Close();
							}
							else if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "6")
							{
								sqlstr = "select OUT_MAT_THICK,OUT_MAT_WIDTH from tmm0023 where OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAg='6'";

								//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Clear();
								//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
								//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
								cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
								cmd_inq.Parameters.Set("MAT_WIDTH", dtOldMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
									tacaich.MAT_WIDTH_CODE = to_string(cmd_inq.GetInt32(2));
								}
								cmd_inq.Close();
							}
							else if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "9")
							{
								sqlstr = "select OUT_MAT_MIN_THICK,OUT_MAT_MAX_THICK,OUT_MAT_MIN_WIDTH,OUT_MAT_MAX_WIDTH from tmm0023 where OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAG='9'";

								//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Clear();
								//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
								//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
								cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
								cmd_inq.Parameters.Set("MAT_WIDTH", dtOldMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									tacaich.MAT_THICK_CODE = "[" + to_string(cmd_inq.GetInt32(1)) + "," + to_string(cmd_inq.GetInt32(2)) + "]";
									tacaich.MAT_WIDTH_CODE = "[" + to_string(cmd_inq.GetInt32(3)) + "," + to_string(cmd_inq.GetInt32(4)) + "]";
								}
								cmd_inq.Close();
							}

						}
						//SURPLUS_FLAG 余材代码
						tacaich.SURPLUS_FLAG = dtOldMat.Rows[j]["ORDER_NO"].ToString() == " " ? "1" : "0";
						//MAT_ACT_WT
						//20211114材也按理重抛账
						/*tacaich.MAT_ACT_WT = dtOldMat.Rows[j]["MAT_THEORY_WT"];*/
						//20211124 统一改成以三级抛上来的mat_wt抛账
						tacaich.MAT_ACT_WT = dtOldMat.Rows[j]["MAT_WT"];

						//20220412 add 安宁基地钢坯按入炉称重重量抛轧钢消耗量
						if (tacaich.ACCOUNT == "2801")
						{
							if (dtOldMat.Rows[j]["SPARE_ITEM_N1"].ToDecimal() > 0)
							{
								tacaich.MAT_ACT_WT = dtOldMat.Rows[j]["SPARE_ITEM_N1"];
							}
						}
		
						//WEIGHT_UNITM
						tacaich.WEIGHT_UNITM = "T";
						//NUM
						tacaich.NUM = 1;//暂时默认为1
						//QTY_UNIT
						tacaich.QTY_UNIT = " "; //暂时不给
						//IN_MAT_NO
						//tacaich.IN_MAT_NO = " ";  //入口信息可以不给
						if (matShapeFlag == "8" || matShapeFlag == "6")
						{
							if (matKind == "SM")
							{
								//如果是棒材原料，则传plan_no
								tacaich.IN_MAT_NO = dtOldMat.Rows[j]["PLAN_NO"].ToString();  //其他按传入板坯号
							}
							else tacaich.IN_MAT_NO = cs_in_mat_no_bw;
						}
						else if (matShapeFlag == "7")
						{
							if (matKind == "SM")
							{
								tacaich.IN_MAT_NO = dtOldMat.Rows[j]["MAT_NO"].ToString();  //线材原料按入口材料1对1 传
							}
							else tacaich.IN_MAT_NO = dtOldMat.Rows[j]["IN_MAT_NO"].ToString();
						}
						else if (matShapeFlag == "2" || matShapeFlag == "3" || matShapeFlag == "6")
						{
							if (matKind == "SM")
							{
								tacaich.IN_MAT_NO = dtOldMat.Rows[j]["MAT_NO"].ToString();  //厚板 热轧
							}
							else tacaich.IN_MAT_NO = dtOldMat.Rows[j]["SLAB_NO"].ToString();
						}
						else
						{
							//板坯的处理
							//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
							if (dtOldMat.Rows[j]["SPARE_ITEM_1"].ToString().Trim() != "")
							{
								tacaich.IN_MAT_NO = dtOldMat.Rows[j]["SPARE_ITEM_1"].ToString(); // 板坯按炉号传
							}
							else
							{
								tacaich.IN_MAT_NO = dtOldMat.Rows[j]["HEAT_NO"].ToString();  //炼钢侧用HEAT_NO
							}
						}

						//OLD_PSR
						tacaich.OLD_PSR = " ";    //入口信息不给
						//DEVO_PRODUCT_CODE
						tacaich.DEVO_PRODUCT_CODE = dtOldMat.Rows[j]["PRODUCT_CODE"];  //入口信息不给投入产副品
						//IN_MAT_WIDTH
						tacaich.IN_MAT_WIDTH = 0; //入口信息不给
						//IN_MAT_THICK
						tacaich.IN_MAT_THICK = 0; //入口信息不给
						//IN_MAT_LEN
						tacaich.IN_MAT_LEN = 0; //入口信息不给
						//PASS_DSS_OK
						tacaich.PASS_DSS_OK = " ";  //暂不提供
						//CTRL_ROLL_THICK_1
						//tacaich.CTRL_ROLL_THICK_1 = //TODO: 第一阶段控轧点厚度暂时不给
						//IN_MAT_WT_AI
						tacaich.IN_MAT_WT_AI = 0; //入口信息不给
						//NEW_PROD_AGREE_NO
						tacaich.NEW_PROD_AGREE_NO = dtOldMat.Rows[j]["NEW_TEST_NO"].ToString();
						//PERSIST_PROD_TIME 持续生产时间
						//tacaich.PERSIST_PROD_TIME //暂时不提供
						//BACKLOG
						tacaich.BACKLOG = dtOldMat.Rows[j]["WHOLE_BACKLOG"].ToString();
						//INT_BACKLOG
						tacaich.INT_BACKLOG = dtOldMat.Rows[j]["WHOLE_BACKLOG"].ToString();  //先随便糊弄一个
						//AC_ROUTE
						tacaich.AC_ROUTE = "00"; //由成本自己生成
						//PRE_UNIT_CODE
						tacaich.PRE_UNIT_CODE = " "; //暂时不给
						//PLAN_NO
						tacaich.PLAN_NO = dtOldMat.Rows[j]["PLAN_NO"].ToString();
						//STOCK_NO
						tacaich.STOCK_NO = dtOldMat.Rows[j]["STOCK_NO"].ToString();
						//CUST_ORDER_NO
						tacaich.CUST_ORDER_NO = dtOldMat.Rows[j]["ORDER_NO"].ToString();
						//ROLL_ABN_CODE
						tacaich.ROLL_ABN_CODE = " ";  //暂时不给
						//FIN_SURF_CODE
						tacaich.FIN_SURF_CODE = " ";  //暂时用不上
						//PACK_TYPE_CODE
						tacaich.PACK_TYPE_CODE = dtOldMat.Rows[j]["PACK_TYPE_CODE"].ToString();
						//SURF_STRUC_CODE  表面结构码
						tacaich.SURF_STRUC_CODE = " ";  //暂时用不上
						//PROD_END_TIME
						tacaich.PROD_END_TIME = " "; // dtOldMat.Rows[i]["PROD_END_TIME"].ToString();
						//PROD_START_TIME
						tacaich.PROD_START_TIME = " ";  //暂时用不上 如需要需分机组定制
						//CTRL_ROLL_CODE_1
						//CTRL_ROLL_CODE
						if (matKind == "HP")
						{
							//厚板产线才有控轧代码
							tacaich.CTRL_ROLL_CODE_1 = dtOldMat.Rows[j]["CTRL_ROLL_CODE"].ToString().Substring(0, 1);
							tacaich.CTRL_ROLL_CODE = dtOldMat.Rows[j]["CTRL_ROLL_CODE"].ToString();
						}
						else
						{
							tacaich.CTRL_ROLL_CODE_1 = " ";
							tacaich.CTRL_ROLL_CODE = " ";
						}
						//HEAT_MODE
						tacaich.HEAT_MODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
						//PAINT_CODE
						tacaich.PAINT_CODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
						//HOT_SEND_DIV
						//HOT_SEND_FLAG_ACT
						if (matKind == "SM")
						{
							tacaich.HOT_SEND_DIV = dtOldMat.Rows[j]["HOT_SEND_FLAG"].ToString();
							tacaich.HOT_SEND_FLAG_ACT = tacaich.HOT_SEND_DIV;
						}
						else
						{
							tacaich.HOT_SEND_DIV = " ";
							tacaich.HOT_SEND_FLAG_ACT = " ";
						}
						//SHOT_PAINT_AI
						tacaich.SHOT_PAINT_AI = " "; // 涂料涂漆代码，暂不需要
						//PROD_CODE_HP
						//tacaich.PROD_CODE_HP = dtOldMat.Rows[i]["PROD_CODE_HP"].ToString(); // TODO:目前hr bw没此字段，需后处理
						//SHP_CODE
						tacaich.SHP_CODE = dtOldMat.Rows[j]["MAT_SHAPE_FLAG"].ToString();
						//PLATE_ID
						tacaich.PLATE_ID = 0; //目前已不使用此字段，暂默认0
						//HEAT_PROC_LOG
						tacaich.HEAT_PROC_LOG = " "; //HP暂无，需后处理			
						//IN_PLATE_ID
						tacaich.IN_PLATE_ID = 0; //目前已不使用此字段，暂默认0
						//SPECIAL_FLAG_TYPE
						tacaich.SPECIAL_FLAG_TYPE = " ";  //暂不使用此字段
						//PRST_BK_PT
						tacaich.PRST_BK_PT = dtOldMat.Rows[j]["WHOLE_BACKLOG_CODE"].ToString();
						//LN_SLAB_APPT_WT
						tacaich.LN_SLAB_APPT_WT = 0; //暂不使用
						//SH_SLAB_APPT_WT
						tacaich.SH_SLAB_APPT_WT = 0; //暂不使用
						//AD_MODE_CODE
						if (matKind == "HP")
						{
							tacaich.AD_MODE_CODE = dtOldMat.Rows[j]["AD_MODE_CODE"].ToString();
						}
						else tacaich.AD_MODE_CODE = " ";
						//FUR_NO
						tacaich.FUR_NO = " ";  //暂不传
						//PICK_PLATE_MARK
						if (matKind == "HP")
						{
							tacaich.PICK_PLATE_MARK = dtOldMat.Rows[j]["PICK_PLATE_MARK"].ToString();
						}
						else tacaich.PICK_PLATE_MARK = " ";
						//COMMAND_YIELD
						tacaich.COMMAND_YIELD = 0;  //暂不传，需要则从四大命令表取组板成材率
						//OA_PLATE_FLAG
						tacaich.OA_PLATE_FLAG = " ";  //暂不传，感觉只有准发点才有用
						//HEAD_SAMPLE_SUM_LEN,
						tacaich.HEAD_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
						//HEAD_SAMPLE_SUM_WIDTH,
						tacaich.HEAD_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
						//BOT_SAMPLE_SUM_WIDTH,
						tacaich.BOT_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
						//BOT_SAMPLE_SUM_LEN,
						tacaich.BOT_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
						//HEAT_TREAT_KEEP,
						tacaich.HEAT_TREAT_KEEP = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//HEAT_TREAT_STAY_TIME,
						tacaich.HEAT_TREAT_STAY_TIME = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_SLAB_THICK,
						tacaich.ROLL_SLAB_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_SLAB_WIDTH,
						tacaich.ROLL_SLAB_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_SLAB_LEN,
						tacaich.ROLL_SLAB_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_PLT_THICK,
						tacaich.ROLL_PLT_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_PLT_WIDTH,
						tacaich.ROLL_PLT_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//ROLL_PLT_LEN,
						tacaich.ROLL_PLT_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
						//MAT_TRACK_NO,
						tacaich.MAT_TRACK_NO = dtOldMat.Rows[j]["MAT_TRACK_NO"].ToString();
						//DEST_ID,
						//tacaich.DEST_ID = " "; //暂不使用
						//RAW_ORIGIN,
						tacaich.RAW_ORIGIN = dtOldMat.Rows[j]["RAW_ORIGIN"].ToString();
						//HEAT_TREAT_PLANT,
						tacaich.HEAT_TREAT_PLANT = " "; //暂不使用，需要则根据热处理机组记录即可
						//IC_CC_FLAG,
						tacaich.IC_CC_FLAG = " "; //暂不使用，重钢都是连铸：模连铸标记（此处不做赋值先）
						//ORDER_TYPE_CODE,
						tacaich.ORDER_TYPE_CODE = tom01["ORDER_TYPE_CODE"];
						//ACCP_AUTH_CODE,
						tacaich.ACCP_AUTH_CODE = " "; //暂不使用，验收机关需要也只有tom01hp表中有
						//ROLL_DIRECT_CODE,
						tacaich.ROLL_DIRECT_CODE = " "; //暂不使用，需要则从RS代码中截取
						//SIN_TWO_FUR_DIV,
						tacaich.SIN_TWO_FUR_DIV = " "; //暂不使用，单双炉区分
						//IN_FUR_PRED,
						if (matKind != "SM")
						{
							tacaich.IN_FUR_PRED = 0; //暂不使用，需要时取轧制实绩表
						}
						else tacaich.IN_FUR_PRED = 0; //暂不使用，单双炉区分
						//ROLL_CYC_NUM,
						if (matKind != "SM")
						{
							tacaich.ROLL_CYC_NUM = 0; //暂不使用，需要时取轧制实绩表
						}
						else tacaich.ROLL_CYC_NUM = 0; //暂不使用，单双炉区分
						//TAP_SLAB_TEMP_AVE,
						if (matKind != "SM")
						{
							tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，需要时取轧制实绩表
						}
						else tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，单双炉区分
						//WT_METHOD_CODE,
						tacaich.WT_METHOD_CODE = tom01["WT_METHOD_CODE"];
						//WITH_SIDE_FLAG,
						if (matKind == "HP")
						{
							tacaich.WITH_SIDE_FLAG = dtOldMat.Rows[j]["WITH_SIDE_FLAG"].ToString();
						}
						else if (matKind == "HR")
						{
							tacaich.WITH_SIDE_FLAG = dtOldMat.Rows[j]["TRIM_FLAG"].ToString();
						}
						else tacaich.WITH_SIDE_FLAG = " ";
						//FIX_FLAG,
						tacaich.FIX_FLAG = tom01["FIX_FLAG"];
						//ST_NO_INPUT,
						tacaich.ST_NO_INPUT = " ";  //入口信息不用
						//SG_SIGN_INPUT,
						tacaich.SG_SIGN_INPUT = " ";  //入口信息不用
						//ACT_BACKLOG,
						tacaich.ACT_BACKLOG = dtOldMat.Rows[j]["WHOLE_BACKLOG"].ToString();
						//NOW_BACKLOG,
						tacaich.NOW_BACKLOG = dtOldMat.Rows[j]["WHOLE_BACKLOG_ACT"].ToString();
						//SUB_BACKLOG_CODE,
						if (matKind == "HP")
						{
							tacaich.SUB_BACKLOG_CODE = dtOldMat.Rows[j]["SUB_BACKLOG_CODE"].ToString();
						}
						else tacaich.SUB_BACKLOG_CODE = " ";
						//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
						if (matKind == "SM" && dtOldMat.Rows[j]["SPARE_ITEM_1"].ToString().Trim() != "")
						{
							tacaich.HEAT_NO = dtOldMat.Rows[j]["SPARE_ITEM_1"].ToString();
							tacaich.PONO = dtOldMat.Rows[j]["SPARE_ITEM_2"].ToString();
						}
						else
						{
							//HEAT_NO,
							tacaich.HEAT_NO = dtOldMat.Rows[j]["HEAT_NO"].ToString();
							//PONO,
							tacaich.PONO = dtOldMat.Rows[j]["PONO"].ToString();
						}
						//RL_NO,
						if (matKind != "SM")
						{
							tacaich.RL_NO = dtOldMat.Rows[j]["SAMPLE_LOT_NO"].ToString();
						}
						else tacaich.RL_NO = " ";
						//PRODUCT_TOL,
						tacaich.PRODUCT_TOL = 0;
						//BAF_NO,
						tacaich.BAF_NO = " ";
						//ROLL_MODE,
						tacaich.ROLL_MODE = " "; //暂不提供，需要时从计划或命令表中获取
						//HSF_PLAN_NO,
						tacaich.HSF_PLAN_NO = " "; //精整计划号，暂不提供，需要的话，根据机组取plan_no即可
						//REFINE_ROUTE_CODE,
						if (matKind == "SM")
						{
							tacaich.REFINE_ROUTE_CODE = dtOldMat.Rows[j]["REFINE_ROUTE_CODE"].ToString();
						}
						else tacaich.REFINE_ROUTE_CODE = " ";
						//HOT_TREAT_METHOD,
						tacaich.HOT_TREAT_METHOD = " "; //暂不提供，如需要，根据热处理小工序截取
						//APP_THROW_AI_HEAD,
						tacaich.APP_THROW_AI_HEAD = " "; //由成分生成
						//APP_THROW_AI_ID,
						tacaich.APP_THROW_AI_ID = " "; //由成分生成
						//APP_THROW_AI_NUM,
						tacaich.APP_THROW_AI_NUM = 0; //由成分生成
						//COILED_TIME, 
						tacaich.COILED_TIME = " ";  //暂不提供，钢卷卷曲时间，如需获取取HR生产时刻或实绩时间
						//COLD_HOT_FLAG1,
						tacaich.COLD_HOT_FLAG1 = 0;  //重钢无
						//DEST_FIN,
						tacaich.DEST_FIN = " ";  //最终去向，暂不提供
						//DISCH_TIME,
						tacaich.DISCH_TIME = " ";  //出炉时间，暂不提供，需要则要从轧制实绩取
						//GRADE_AI,
						tacaich.GRADE_AI = " ";  //暂不提供
						//HARDNESS_GROUP_CODE,
						tacaich.HARDNESS_GROUP_CODE = " ";  //暂不提供
						//IN_INGOT_TYPE,
						tacaich.IN_INGOT_TYPE = " ";  //暂不提供
						//IN_MELT_TIME,
						tacaich.IN_MELT_TIME = " ";  //暂不提供
						//INGOT_TYPE,
						tacaich.INGOT_TYPE = " ";  //暂不提供
						//NEW_TEST_NO,
						tacaich.NEW_TEST_NO = dtOldMat.Rows[j]["NEW_TEST_NO"].ToString();
						//PICKL_TRIM_FLAG,
						tacaich.PICKL_TRIM_FLAG = " ";  //暂不提供 酸洗切边标记（重钢无）
						//PRIM_UNIT,
						tacaich.PRIM_UNIT = " ";  //暂不提供 原机组代码，暂无法提供
						//PROD_TIME,
						tacaich.PROD_TIME = dtOldMat.Rows[j]["PROD_TIME"].ToString();
						//REPAIR_FLAG_AI,
						tacaich.REPAIR_FLAG_AI = dtOldMat.Rows[j]["REPAIR_FLAG"].ToString();
						//TEMPER,
						tacaich.TEMPER = " ";
						//WEIGHT_REAL_STEEL,
						tacaich.WEIGHT_REAL_STEEL = 0; //暂不提供，需要则需从装炉实际中获取
						//WT_METHOD,
						tacaich.WT_METHOD = dtOldMat.Rows[j]["MEASURE_WT_FLAG"].ToString();
						//TRIM_FLAG,
						tacaich.TRIM_FLAG = tom01["TRIM_FLAG"];
						//RS_CODE,
						tacaich.RS_CODE = " "; //暂不提供，只有厚板轧制才有，从命令表中获取 
						tacaich.BACK_CODE_1 = cs_shift_no;//记录班次
						//tacaich.BACK_CODE_2 = cs_shift_group; //记录班组
						tacaich.BACK_CODE_2 = " "; //记录班组
						tacaich.BACK_CODE_3 = cs_date;
						tacaich.BACK_CODE_7 = eventId;//20211008 添加事件号
						//BACK_CODE_4,
						//BACK_CODE_5,
						//BACK_CODE_6,
						//BACK_CODE_7,
						//BACK_CODE_8,
						if ((dtOldMat.Rows[j]["MAT_SHAPE_FLAG"].ToString() == "8" || dtOldMat.Rows[j]["MAT_SHAPE_FLAG"].ToString() == "7") && tacaich.MAT_THICK_CODE.Trim() != "")
						{
							tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE;
						}
						else if ((dtOldMat.Rows[j]["MAT_SHAPE_FLAG"].ToString() == "6" || dtOldMat.Rows[j]["MAT_SHAPE_FLAG"].ToString() == "9") && tacaich.MAT_THICK_CODE.Trim() != "" && tacaich.MAT_WIDTH_CODE.Trim() != "")
						{
							tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE + ";" + tacaich.MAT_WIDTH_CODE;
						}
						//BACK_CODE_9,
						//BACK_CODE_10,
						//BACK_CODE_11,
						//BACK_CODE_12,
						//BACK_CODE_13,
						//BACK_CODE_14,
						//BACK_CODE_15,

						if (eventId == "MMF6")
						{
							//线材轧废的时候，new记录
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["MAT_NO"].ToString(); //线材轧废时 入口材料保留为方坯号
						}

						tacaich.MergeTo(bcls_rec_ac.Tables[BLOCKNAME], false);
						tmmbwac.Reset();
						tmmbwac.CopyFrom(tacaich);
						tmmbwac["EVENT_DATETIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						tmmbwac["EVENT_ID"] = eventId;
						tmmbwac["EVENT_DESC"] = event_name + "-投入信息";
						tmmbwac["MAT_ACT_WT"] = tacaich.MAT_ACT_WT;
						tmmbwac["MAT_NUM"] = tacaich.NUM;
						tmmbwac["ORDER_NO"] = tacaich.CUST_ORDER_NO;
						tmmbwac["APP_CODE"] = tacaich.APP_CODE;
						tmmbwac["COMPANY_CODE"] = tacaich.ACCOUNT;
						tmmbwac["TRANSACTION_CODE"] = tacaich.TRANSACTION_CODE;
						tmmbwac["FUNCTION_CODE_AC"] = tacaich.FUNCTION_CODE_AC;
						tmmbwac["UNIT_CODE"] = tacaich.TOWARD_MCHN_MODE;
						tmmbwac["QUALITY_TEST_CODE"] = tacaich.QUALITY_TEST_CODE;
						tmmbwac.MergeTo(bcls_tmmbwac.Tables["TMMBWAC"], false);
						/*tmmbwac.Insert();*/
					}
				}
				else
				{
					//获取可能有的合同信息
					if (dtOldMat.Rows[i]["ORDER_NO"].ToString() > " ")
					{
						tom01["ORDER_NO"] = dtOldMat.Rows[i]["ORDER_NO"].ToString();
						tom01.Query("ORDER_NO");
					}

					matKind = dtOldMat.Rows[i]["MAT_KIND"].ToString();
					//TODO: 获取入口产副品代码 需要根据每个机组的情况去考虑，投入产副品需带在主档上product_code_2字段上
					//如果是炼钢侧的数据，投入产副品需定制
					tacaich.PRDT_CODE_BFR_TRNC = dtOldMat.Rows[i]["PRODUCT_CODE"].ToString();
					if (tacaich.PRDT_CODE_BFR_TRNC.Trim() == "")
					{
						sprintf(s.msg, "前产副品代码不允许为空");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					if (TRANSACTION_CODE != "F")
					{
						if (cs_unit_code_spec > " ")
						{
							tacaich.TOWARD_MCHN_MODE = cs_unit_code_spec;
						}
						else
						{
							tacaich.TOWARD_MCHN_MODE = dtNewMat.Rows[i]["UNIT_CODE"].ToString();  //TODO: 暂时按出口侧机组代码抛，需考虑原料机组的情况
							//炼钢产出机组是两位，通过转换小代码转换成4位抛成本
							sqlstr = "SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MM01' AND CODE_DESC_2_CONTENT=@UNIT_CODE ";

							//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Clear();
							cmd_inq.Parameters.Set("UNIT_CODE", tacaich.TOWARD_MCHN_MODE);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								tacaich.TOWARD_MCHN_MODE = cmd_inq.GetString(1);
							}
							cmd_inq.Close();
						}
					}
					else tacaich.TOWARD_MCHN_MODE = " ";

					//20211006 成本要求不加去向机组
					tacaich.TOWARD_MACHINE = " ";

					//Log::Trace("", __FUNCTION__, " COMPLEX_DECIDE_CODE3= {0}", dtOldMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString());
					if (dtOldMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString() == "4")
					{
						tacaich.QUALITY_TEST_CODE = "4"; //质量码
					}
					else if (dtNewMat.Columns.Contains("PROD_CLASS") && dtOldMat.Rows[i]["PROD_CLASS"].ToString() == "3")
					{
						tacaich.QUALITY_TEST_CODE = "3"; //可利用材
					}
					else
					{
						tacaich.QUALITY_TEST_CODE = "1"; //合格品
					}
					if (eventId == "QM05" || eventId == "MM05")//20211020 成本规定判废抛两条数据，第二条数据的质量码也为4
					{
						tacaich.QUALITY_TEST_CODE = "4"; //质量码
					}
					//Log::Trace("", __FUNCTION__, " tacaich.QUALITY_TEST_CODE{0}", tacaich.QUALITY_TEST_CODE);
					//	CAST_NO
					if (matKind == "SM")
					{
						tacaich.CAST_NO = dtOldMat.Rows[i]["CAST_NO"].ToString();
					}
					else tacaich.CAST_NO = " ";
					//	EVENT_ID
					tacaich.EVENT_ID = dtEventData.Rows[0]["EVENT_ID"].ToString();
					//	FACTORY_DIV
					tacaich.FACTORY_DIV = dtOldMat.Rows[i]["FACTORY_DIV"].ToString();

					//ST_NO
					tacaich.ST_NO = dtOldMat.Rows[i]["ST_NO"].ToString();
					//PSR
					tacaich.PSR = dtOldMat.Rows[i]["PSC"].ToString();
					//APN
					tacaich.APN = dtOldMat.Rows[i]["APN"].ToString();
					//MSC
					tacaich.MSC = dtOldMat.Rows[i]["MSC"].ToString();
					//SG_SIGN
					tacaich.SG_SIGN = dtOldMat.Rows[i]["SG_SIGN"].ToString();
					//MAT_NO
					tacaich.MAT_NO = dtOldMat.Rows[i]["MAT_NO"].ToString();
					//MAT_ACT_WIDTH
					tacaich.MAT_ACT_WIDTH = dtOldMat.Rows[i]["MAT_ACT_WIDTH"];
					//MAT_ACT_THICK
					tacaich.MAT_ACT_THICK = dtOldMat.Rows[i]["MAT_ACT_THICK"];
					//MAT_ACT_LEN
					tacaich.MAT_ACT_LEN = dtOldMat.Rows[i]["MAT_ACT_LEN"];
					//MAT_WIDTH_CODE 宽度代码 //TODO等定义下来后
					//MAT_THICK_CODE 厚度代码 //TODO等定义下来后
					if (matKind != "SM")
					{
						if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8" || dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "7")
						{
							sqlstr = "select OUT_MAT_THICK from tmm0023 where  OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and MAT_SHAPE_FLAG=@MAT_SHAPE_FLAG";

							//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Clear();
							//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
							//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
							cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
							if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8")
							{
								cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "8");
							}
							else
							{
								cmd_inq.Parameters.Set("MAT_SHAPE_FLAG", "7");
							}
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
							}
							cmd_inq.Close();
						}
						else if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "6")
						{
							sqlstr = "select OUT_MAT_THICK,OUT_MAT_WIDTH from tmm0023 where  OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAg='6'";

							//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Clear();
							//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
							//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
							cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
							cmd_inq.Parameters.Set("MAT_WIDTH", dtOldMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								tacaich.MAT_THICK_CODE = to_string(cmd_inq.GetInt32(1));
								tacaich.MAT_WIDTH_CODE = to_string(cmd_inq.GetInt32(2));
							}
							cmd_inq.Close();
						}
						else if (dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "9")
						{
							sqlstr = "select OUT_MAT_MIN_THICK,OUT_MAT_MAX_THICK,OUT_MAT_MIN_WIDTH,OUT_MAT_MAX_WIDTH from tmm0023 where  OUT_MAT_MIN_THICK<=@MAT_THICK and OUT_MAT_MAX_THICK>=@MAT_THICK and OUT_MAT_MIN_WIDTH<=@MAT_WIDTH and OUT_MAT_MAX_WIDTH >=@MAT_WIDTH and MAT_SHAPE_FLAG='9'";

							//Log::Trace("", __FUNCTION__, "  TOWARD_MCHN_MODE = {0}", tacaich.TOWARD_MCHN_MODE);

							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Clear();
							//cmd_inq.Parameters.Set("st_no", dtOldMat.Rows[i]["ST_NO"].ToString());
							//cmd_inq.Parameters.Set("SG_SIGN", dtOldMat.Rows[i]["SG_SIGN"].ToString());
							cmd_inq.Parameters.Set("MAT_THICK", dtOldMat.Rows[i]["MAT_ACT_THICK"].ToString());
							cmd_inq.Parameters.Set("MAT_WIDTH", dtOldMat.Rows[i]["MAT_ACT_WIDTH"].ToString());
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								tacaich.MAT_THICK_CODE = "[" + to_string(cmd_inq.GetInt32(1)) + "," + to_string(cmd_inq.GetInt32(2)) + "]";
								tacaich.MAT_WIDTH_CODE = "[" + to_string(cmd_inq.GetInt32(3)) + "," + to_string(cmd_inq.GetInt32(4)) + "]";
							}
							cmd_inq.Close();
						}

					}

					//SURPLUS_FLAG 余材代码
					tacaich.SURPLUS_FLAG = dtOldMat.Rows[i]["ORDER_NO"].ToString() == " " ? "1" : "0";
					//MAT_ACT_WT
					//20211114材也按理重抛账
					/*tacaich.MAT_ACT_WT = dtOldMat.Rows[i]["MAT_THEORY_WT"];*/
					//20211124 统一改成以三级抛上来的mat_wt抛账
					tacaich.MAT_ACT_WT = dtOldMat.Rows[i]["MAT_WT"];

					tacaich.WEIGHT_UNITM = "T";
					//NUM
					tacaich.NUM = 1;//暂时默认为1
					//QTY_UNIT
					tacaich.QTY_UNIT = " "; //暂时不给
					//IN_MAT_NO
					if (matShapeFlag == "8" || matShapeFlag == "6")
					{
						if (matKind == "SM")
						{
							//如果是棒材原料，则传plan_no
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["PLAN_NO"].ToString();  //其他按传入板坯号
						}
						else tacaich.IN_MAT_NO = cs_in_mat_no_bw;
					}
					else if (matShapeFlag == "7")
					{
						if (matKind == "SM")
						{
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["MAT_NO"].ToString();  //线材原料按入口材料1对1 传
						}
						else tacaich.IN_MAT_NO = dtOldMat.Rows[i]["IN_MAT_NO"].ToString();
					}
					else if (matShapeFlag == "2" || matShapeFlag == "3" || matShapeFlag == "6")
					{
						if (matKind == "SM")
						{
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["MAT_NO"].ToString();  //厚板 热轧
						}
						else tacaich.IN_MAT_NO = dtOldMat.Rows[i]["SLAB_NO"].ToString();
					}
					else
					{
						//板坯的处理
						//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
						if (dtOldMat.Rows[i]["SPARE_ITEM_1"].ToString().Trim() != "")
						{
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["SPARE_ITEM_1"].ToString(); // 板坯按炉号传
						}
						else
						{
							tacaich.IN_MAT_NO = dtOldMat.Rows[i]["HEAT_NO"].ToString();  //炼钢侧用HEAT_NO
						}
					}
					//OLD_PSR
					tacaich.OLD_PSR = " ";    //入口信息不给
					//DEVO_PRODUCT_CODE
					tacaich.DEVO_PRODUCT_CODE = dtOldMat.Rows[i]["PRODUCT_CODE"];  //入口信息不给投入产副品
					//IN_MAT_WIDTH
					tacaich.IN_MAT_WIDTH = 0; //入口信息不给
					//IN_MAT_THICK
					tacaich.IN_MAT_THICK = 0; //入口信息不给
					//IN_MAT_LEN
					tacaich.IN_MAT_LEN = 0; //入口信息不给
					//PASS_DSS_OK
					tacaich.PASS_DSS_OK = " ";  //暂不提供
					//CTRL_ROLL_THICK_1
					//tacaich.CTRL_ROLL_THICK_1 = //TODO: 第一阶段控轧点厚度暂时不给
					//IN_MAT_WT_AI
					tacaich.IN_MAT_WT_AI = 0; //入口信息不给
					//NEW_PROD_AGREE_NO
					tacaich.NEW_PROD_AGREE_NO = dtOldMat.Rows[i]["NEW_TEST_NO"].ToString();
					//PERSIST_PROD_TIME 持续生产时间
					//tacaich.PERSIST_PROD_TIME //暂时不提供
					//BACKLOG
					tacaich.BACKLOG = dtOldMat.Rows[i]["WHOLE_BACKLOG"].ToString();
					//INT_BACKLOG
					tacaich.INT_BACKLOG = dtOldMat.Rows[i]["WHOLE_BACKLOG"].ToString();  //先随便糊弄一个
					//AC_ROUTE
					tacaich.AC_ROUTE = "00"; //由成本自己生成
					//PRE_UNIT_CODE
					tacaich.PRE_UNIT_CODE = " "; //暂时不给
					//PLAN_NO
					tacaich.PLAN_NO = dtOldMat.Rows[i]["PLAN_NO"].ToString();
					//STOCK_NO
					tacaich.STOCK_NO = dtOldMat.Rows[i]["STOCK_NO"].ToString();
					//CUST_ORDER_NO
					tacaich.CUST_ORDER_NO = dtOldMat.Rows[i]["ORDER_NO"].ToString();
					//ROLL_ABN_CODE
					tacaich.ROLL_ABN_CODE = " ";  //暂时不给
					//FIN_SURF_CODE
					tacaich.FIN_SURF_CODE = " ";  //暂时用不上
					//PACK_TYPE_CODE
					tacaich.PACK_TYPE_CODE = dtOldMat.Rows[i]["PACK_TYPE_CODE"].ToString();
					//SURF_STRUC_CODE  表面结构码
					tacaich.SURF_STRUC_CODE = " ";  //暂时用不上
					//PROD_END_TIME
					tacaich.PROD_END_TIME = " "; // dtOldMat.Rows[i]["PROD_END_TIME"].ToString();
					//PROD_START_TIME
					tacaich.PROD_START_TIME = " ";  //暂时用不上 如需要需分机组定制
					//CTRL_ROLL_CODE_1
					//CTRL_ROLL_CODE
					if (matKind == "HP")
					{
						//厚板产线才有控轧代码
						tacaich.CTRL_ROLL_CODE_1 = dtOldMat.Rows[i]["CTRL_ROLL_CODE"].ToString().Substring(0, 1);
						tacaich.CTRL_ROLL_CODE = dtOldMat.Rows[i]["CTRL_ROLL_CODE"].ToString();
					}
					else
					{
						tacaich.CTRL_ROLL_CODE_1 = " ";
						tacaich.CTRL_ROLL_CODE = " ";
					}
					//HEAT_MODE
					tacaich.HEAT_MODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
					//PAINT_CODE
					tacaich.PAINT_CODE = " ";  //如需要，需根据冶金规范或合同号获取,暂时不提供
					//HOT_SEND_DIV
					//HOT_SEND_FLAG_ACT
					if (matKind == "SM")
					{
						tacaich.HOT_SEND_DIV = dtOldMat.Rows[i]["HOT_SEND_FLAG"].ToString();
						tacaich.HOT_SEND_FLAG_ACT = tacaich.HOT_SEND_DIV;
					}
					else
					{
						tacaich.HOT_SEND_DIV = " ";
						tacaich.HOT_SEND_FLAG_ACT = " ";
					}
					//SHOT_PAINT_AI
					tacaich.SHOT_PAINT_AI = " "; // 涂料涂漆代码，暂不需要
					//PROD_CODE_HP
					//tacaich.PROD_CODE_HP = dtOldMat.Rows[i]["PROD_CODE_HP"].ToString(); // TODO:目前hr bw没此字段，需后处理
					//SHP_CODE
					tacaich.SHP_CODE = dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString();
					//PLATE_ID
					tacaich.PLATE_ID = 0; //目前已不使用此字段，暂默认0
					//HEAT_PROC_LOG
					tacaich.HEAT_PROC_LOG = " "; //HP暂无，需后处理			
					//IN_PLATE_ID
					tacaich.IN_PLATE_ID = 0; //目前已不使用此字段，暂默认0
					//SPECIAL_FLAG_TYPE
					tacaich.SPECIAL_FLAG_TYPE = " ";  //暂不使用此字段
					//PRST_BK_PT
					tacaich.PRST_BK_PT = dtOldMat.Rows[i]["WHOLE_BACKLOG_CODE"].ToString();
					//LN_SLAB_APPT_WT
					tacaich.LN_SLAB_APPT_WT = 0; //暂不使用
					//SH_SLAB_APPT_WT
					tacaich.SH_SLAB_APPT_WT = 0; //暂不使用
					//AD_MODE_CODE
					if (matKind == "HP")
					{
						tacaich.AD_MODE_CODE = dtOldMat.Rows[i]["AD_MODE_CODE"].ToString();
					}
					else tacaich.AD_MODE_CODE = " ";
					//FUR_NO
					tacaich.FUR_NO = " ";  //暂不传
					//PICK_PLATE_MARK
					if (matKind == "HP")
					{
						tacaich.PICK_PLATE_MARK = dtOldMat.Rows[i]["PICK_PLATE_MARK"].ToString();
					}
					else tacaich.PICK_PLATE_MARK = " ";
					//COMMAND_YIELD
					tacaich.COMMAND_YIELD = 0;  //暂不传，需要则从四大命令表取组板成材率
					//OA_PLATE_FLAG
					tacaich.OA_PLATE_FLAG = " ";  //暂不传，感觉只有准发点才有用
					//HEAD_SAMPLE_SUM_LEN,
					tacaich.HEAD_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
					//HEAD_SAMPLE_SUM_WIDTH,
					tacaich.HEAD_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
					//BOT_SAMPLE_SUM_WIDTH,
					tacaich.BOT_SAMPLE_SUM_WIDTH = 0;  //暂不传，需要则从四大命令表取组板成材率
					//BOT_SAMPLE_SUM_LEN,
					tacaich.BOT_SAMPLE_SUM_LEN = 0;  //暂不传，需要则从四大命令表取组板成材率
					//HEAT_TREAT_KEEP,
					tacaich.HEAT_TREAT_KEEP = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//HEAT_TREAT_STAY_TIME,
					tacaich.HEAT_TREAT_STAY_TIME = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_SLAB_THICK,
					tacaich.ROLL_SLAB_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_SLAB_WIDTH,
					tacaich.ROLL_SLAB_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_SLAB_LEN,
					tacaich.ROLL_SLAB_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_PLT_THICK,
					tacaich.ROLL_PLT_THICK = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_PLT_WIDTH,
					tacaich.ROLL_PLT_WIDTH = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//ROLL_PLT_LEN,
					tacaich.ROLL_PLT_LEN = 0;  //暂不传，需要则从轧制或加热炉实绩表获取 
					//MAT_TRACK_NO,
					tacaich.MAT_TRACK_NO = dtOldMat.Rows[i]["MAT_TRACK_NO"].ToString();
					//DEST_ID,
					//tacaich.DEST_ID = " "; //暂不使用
					//RAW_ORIGIN,
					tacaich.RAW_ORIGIN = dtOldMat.Rows[i]["RAW_ORIGIN"].ToString();
					//HEAT_TREAT_PLANT,
					tacaich.HEAT_TREAT_PLANT = " "; //暂不使用，需要则根据热处理机组记录即可
					//IC_CC_FLAG,
					tacaich.IC_CC_FLAG = " "; //暂不使用，重钢都是连铸：模连铸标记（此处不做赋值先）
					//ORDER_TYPE_CODE,
					tacaich.ORDER_TYPE_CODE = tom01["ORDER_TYPE_CODE"];
					//ACCP_AUTH_CODE,
					tacaich.ACCP_AUTH_CODE = " "; //暂不使用，验收机关需要也只有tom01hp表中有
					//ROLL_DIRECT_CODE,
					tacaich.ROLL_DIRECT_CODE = " "; //暂不使用，需要则从RS代码中截取
					//SIN_TWO_FUR_DIV,
					tacaich.SIN_TWO_FUR_DIV = " "; //暂不使用，单双炉区分
					//IN_FUR_PRED,
					if (matKind != "SM")
					{
						tacaich.IN_FUR_PRED = 0; //暂不使用，需要时取轧制实绩表
					}
					else tacaich.IN_FUR_PRED = 0; //暂不使用，单双炉区分
					//ROLL_CYC_NUM,
					if (matKind != "SM")
					{
						tacaich.ROLL_CYC_NUM = 0; //暂不使用，需要时取轧制实绩表
					}
					else tacaich.ROLL_CYC_NUM = 0; //暂不使用，单双炉区分
					//TAP_SLAB_TEMP_AVE,
					if (matKind != "SM")
					{
						tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，需要时取轧制实绩表
					}
					else tacaich.TAP_SLAB_TEMP_AVE = 0; //暂不使用，单双炉区分
					//WT_METHOD_CODE,
					tacaich.WT_METHOD_CODE = tom01["WT_METHOD_CODE"];
					//WITH_SIDE_FLAG,
					if (matKind == "HP")
					{
						tacaich.WITH_SIDE_FLAG = dtOldMat.Rows[i]["WITH_SIDE_FLAG"].ToString();
					}
					else if (matKind == "HR")
					{
						tacaich.WITH_SIDE_FLAG = dtOldMat.Rows[i]["TRIM_FLAG"].ToString();
					}
					else tacaich.WITH_SIDE_FLAG = " ";
					//FIX_FLAG,
					tacaich.FIX_FLAG = tom01["FIX_FLAG"];
					//ST_NO_INPUT,
					tacaich.ST_NO_INPUT = " ";  //入口信息不用
					//SG_SIGN_INPUT,
					tacaich.SG_SIGN_INPUT = " ";  //入口信息不用
					//ACT_BACKLOG,
					tacaich.ACT_BACKLOG = dtOldMat.Rows[i]["WHOLE_BACKLOG"].ToString();
					//NOW_BACKLOG,
					tacaich.NOW_BACKLOG = dtOldMat.Rows[i]["WHOLE_BACKLOG_ACT"].ToString();
					//SUB_BACKLOG_CODE,
					if (matKind == "HP")
					{
						tacaich.SUB_BACKLOG_CODE = dtOldMat.Rows[i]["SUB_BACKLOG_CODE"].ToString();
					}
					else tacaich.SUB_BACKLOG_CODE = " ";
					//20220330轧钢并炉不修改HEAT_NO借用字段SPARE_ITEM_1
					if (matKind == "SM" && dtOldMat.Rows[i]["SPARE_ITEM_1"].ToString().Trim() != "")
					{
						tacaich.HEAT_NO = dtOldMat.Rows[i]["SPARE_ITEM_1"].ToString();
						tacaich.PONO = dtOldMat.Rows[i]["SPARE_ITEM_2"].ToString();
					}
					else
					{
						//HEAT_NO,
						tacaich.HEAT_NO = dtOldMat.Rows[i]["HEAT_NO"].ToString();
						//PONO,
						tacaich.PONO = dtOldMat.Rows[i]["PONO"].ToString();
					}
					//RL_NO,
					if (matKind != "SM")
					{
						tacaich.RL_NO = dtOldMat.Rows[i]["SAMPLE_LOT_NO"].ToString();
					}
					else tacaich.RL_NO = " ";
					//PRODUCT_TOL,
					tacaich.PRODUCT_TOL = 0;
					//BAF_NO,
					tacaich.BAF_NO = " ";
					//ROLL_MODE,
					tacaich.ROLL_MODE = " "; //暂不提供，需要时从计划或命令表中获取
					//HSF_PLAN_NO,
					tacaich.HSF_PLAN_NO = " "; //精整计划号，暂不提供，需要的话，根据机组取plan_no即可
					//REFINE_ROUTE_CODE,
					if (matKind == "SM")
					{
						tacaich.REFINE_ROUTE_CODE = dtOldMat.Rows[i]["REFINE_ROUTE_CODE"].ToString();
					}
					else tacaich.REFINE_ROUTE_CODE = " ";
					//HOT_TREAT_METHOD,
					tacaich.HOT_TREAT_METHOD = " "; //暂不提供，如需要，根据热处理小工序截取
					//APP_THROW_AI_HEAD,
					tacaich.APP_THROW_AI_HEAD = " "; //由成分生成
					//APP_THROW_AI_ID,
					tacaich.APP_THROW_AI_ID = " "; //由成分生成
					//APP_THROW_AI_NUM,
					tacaich.APP_THROW_AI_NUM = 0; //由成分生成
					//COILED_TIME, 
					tacaich.COILED_TIME = " ";  //暂不提供，钢卷卷曲时间，如需获取取HR生产时刻或实绩时间
					//COLD_HOT_FLAG1,
					tacaich.COLD_HOT_FLAG1 = 0;  //重钢无
					//DEST_FIN,
					tacaich.DEST_FIN = " ";  //最终去向，暂不提供
					//DISCH_TIME,
					tacaich.DISCH_TIME = " ";  //出炉时间，暂不提供，需要则要从轧制实绩取
					//GRADE_AI,
					tacaich.GRADE_AI = " ";  //暂不提供
					//HARDNESS_GROUP_CODE,
					tacaich.HARDNESS_GROUP_CODE = " ";  //暂不提供
					//IN_INGOT_TYPE,
					tacaich.IN_INGOT_TYPE = " ";  //暂不提供
					//IN_MELT_TIME,
					tacaich.IN_MELT_TIME = " ";  //暂不提供
					//INGOT_TYPE,
					tacaich.INGOT_TYPE = " ";  //暂不提供
					//NEW_TEST_NO,
					tacaich.NEW_TEST_NO = dtOldMat.Rows[i]["NEW_TEST_NO"].ToString();
					//PICKL_TRIM_FLAG,
					tacaich.PICKL_TRIM_FLAG = " ";  //暂不提供 酸洗切边标记（重钢无）
					//PRIM_UNIT,
					tacaich.PRIM_UNIT = " ";  //暂不提供 原机组代码，暂无法提供
					//PROD_TIME,
					tacaich.PROD_TIME = dtOldMat.Rows[i]["PROD_TIME"].ToString();
					//REPAIR_FLAG_AI,
					tacaich.REPAIR_FLAG_AI = dtOldMat.Rows[i]["REPAIR_FLAG"].ToString();
					//TEMPER,
					tacaich.TEMPER = " ";
					//WEIGHT_REAL_STEEL,
					tacaich.WEIGHT_REAL_STEEL = 0; //暂不提供，需要则需从装炉实际中获取
					//WT_METHOD,
					tacaich.WT_METHOD = dtOldMat.Rows[i]["MEASURE_WT_FLAG"].ToString();
					//TRIM_FLAG,
					tacaich.TRIM_FLAG = tom01["TRIM_FLAG"];
					//RS_CODE,
					tacaich.RS_CODE = " "; //暂不提供，只有厚板轧制才有，从命令表中获取 
					tacaich.BACK_CODE_1 = cs_shift_no;//记录班次
					//tacaich.BACK_CODE_2 = cs_shift_group; //记录班组
					tacaich.BACK_CODE_2 = " "; //记录班组
					tacaich.BACK_CODE_6 = cs_date;
					tacaich.BACK_CODE_7 = eventId;//20211008 添加事件号
					//BACK_CODE_4,
					//BACK_CODE_5,
					//BACK_CODE_6,
					//BACK_CODE_7,
					//BACK_CODE_8,
					if ((dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "8" || dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "7") && tacaich.MAT_THICK_CODE.Trim() != "")
					{
						tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE;
					}
					else if ((dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "6" || dtOldMat.Rows[i]["MAT_SHAPE_FLAG"].ToString() == "9") && tacaich.MAT_THICK_CODE.Trim() != "" && tacaich.MAT_WIDTH_CODE.Trim() != "")
					{
						tacaich.BACK_CODE_8 = tacaich.MAT_THICK_CODE + ";" + tacaich.MAT_WIDTH_CODE;
					}
					//BACK_CODE_9,
					//BACK_CODE_10,
					//BACK_CODE_11,
					//BACK_CODE_12,
					//BACK_CODE_13,
					//BACK_CODE_14,
					//BACK_CODE_15,

					tacaich.MergeTo(bcls_rec_ac.Tables[BLOCKNAME], false);
					tmmbwac.Reset();
					tmmbwac.CopyFrom(tacaich);
					tmmbwac["EVENT_DATETIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmbwac["EVENT_ID"] = eventId;
					tmmbwac["EVENT_DESC"] = event_name + "-投入信息";
					tmmbwac["MAT_ACT_WT"] = tacaich.MAT_ACT_WT;
					tmmbwac["MAT_NUM"] = tacaich.NUM;
					tmmbwac["ORDER_NO"] = tacaich.CUST_ORDER_NO;
					tmmbwac["APP_CODE"] = tacaich.APP_CODE;
					tmmbwac["COMPANY_CODE"] = tacaich.ACCOUNT;
					tmmbwac["TRANSACTION_CODE"] = tacaich.TRANSACTION_CODE;
					tmmbwac["FUNCTION_CODE_AC"] = tacaich.FUNCTION_CODE_AC;
					tmmbwac["UNIT_CODE"] = tacaich.TOWARD_MCHN_MODE;
					tmmbwac["QUALITY_TEST_CODE"] = tacaich.QUALITY_TEST_CODE;
					tmmbwac.MergeTo(bcls_tmmbwac.Tables["TMMBWAC"], false);
					/*tmmbwac.Insert();*/
				}
			}

			/*Log::Trace("", __FUNCTION__, " 开始调用函数...{0}", table_ac);


			Log::Trace("", __FUNCTION__, " r0 devo_product_code...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[0]["DEVO_PRODUCT_CODE"].ToString());

			Log::Trace("", __FUNCTION__, " r0 PRDT_CODE_BFR_TRNC...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[0]["PRDT_CODE_BFR_TRNC"].ToString());
			Log::Trace("", __FUNCTION__, " r0 TOWARD_MCHN_MODE...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[0]["TOWARD_MCHN_MODE"].ToString());
			Log::Trace("", __FUNCTION__, " r0 TOWARD_MACHINE...[{0}]", bcls_rec_ac.Tables[BLOCKNAME].Rows[0]["TOWARD_MACHINE"].ToString());*/

			if (bcls_rec_ac.Tables[BLOCKNAME].Rows.get_Count() > 1)
			{
				/*Log::Trace("", __FUNCTION__, " r1 devo_product_code...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[1]["DEVO_PRODUCT_CODE"].ToString());
				Log::Trace("", __FUNCTION__, " r1 PRDT_CODE_BFR_TRNC...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[1]["PRDT_CODE_BFR_TRNC"].ToString());
				Log::Trace("", __FUNCTION__, " r1 TOWARD_MCHN_MODE...{0}", bcls_rec_ac.Tables[BLOCKNAME].Rows[1]["TOWARD_MCHN_MODE"].ToString());
				Log::Trace("", __FUNCTION__, " r1 TOWARD_MACHINE...[{0}]", bcls_rec_ac.Tables[BLOCKNAME].Rows[1]["TOWARD_MACHINE"].ToString());*/

			}
		/*	Log::Trace("", __FUNCTION__, "调用成本函数1");
			Log::Trace("", __FUNCTION__, " is_flag[{0}]", is_flag);*/
			if (is_flag != "0")//只有挂合同计重方式一致时才不抛合同成本
			{
				doFlag = f_acaich_rcv(&bcls_rec_ac, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			//Log::Trace("", __FUNCTION__, "调用成本函数2");
			////tmmhp01.PRODUCT_CODE 存放修改字段
			//Log::Trace("", __FUNCTION__, "调用成本函数...");
			PRODUCT_CODE_OUTPUT = bcls_rec_ac.Tables[BLOCKNAME].Rows[0]["PRODUCT_CODE"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "调用成本函数 END,PRODUCT_CODE	= [{0}];", PRODUCT_CODE_OUTPUT);

			/* 成本三版数据生成 */
			//投入产出只有一版数据
			//只有当是带有产出性质的事件时才更新主档
			if (AC_TYPE != "2")
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " UPDATE " + table_mm + " "
						" SET PRODUCT_CODE = @PRODUCT_CODE_OUTPUT, "
						" PRODUCT_CODE_1 = @PRODUCT_CODE_OUTPUT, "
						" AI_TIME_1 = @datetime "
						" WHERE MAT_NO = @mat_no_ac ";
					break;
				}
				cmd_upd.SetCommandText(sqlstr);

				/*Log::Trace("", __FUNCTION__, "更新材料档产副品代码[{0}];", sqlstr);
				Log::Trace("", __FUNCTION__, "更新材料档 mat_no = [{0}];", upd_mat_no);*/

				cmd_upd.Parameters.Set("PRODUCT_CODE_OUTPUT", PRODUCT_CODE_OUTPUT);
				cmd_upd.Parameters.Set("datetime", CDateTime::Now().ToString("yyyyMMddHHmmss"));
				cmd_upd.Parameters.Set("mat_no_ac", upd_mat_no);
				int affectRow = cmd_upd.ExecuteNonQuery();

				/*Log::Trace("", __FUNCTION__, "更新材料档记录返回 = [{0}];", affectRow);*/

				if (affectRow != 1 && affectRow != 0)
				{
					strcpy(s.msg, "产出材料成本数据更新失败!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				cmd_upd.Close();
			}

			//记录抛账履历
			if (is_flag == "1")//只有挂合同计重方式一致时才不抛合同成本
			{
				for (int k = 0; k < bcls_tmmbwac.Tables["TMMBWAC"].Rows.get_Count(); k++)
				{
					tmmbwac.MergeFrom(bcls_tmmbwac.Tables["TMMBWAC"].Rows[k]);
					tmmbwac.TrimOrBlank();
					tmmbwac.Insert();
				}
			}

		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


