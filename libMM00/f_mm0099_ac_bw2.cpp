/*************************************************tmmsn01
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      wb
Version:     1.0
Date:        2022-08-03 10:20:19
Description: 物料跟踪抛成本函数（写中间表）
**************************************************/
#include "stdafx.h"
#include "CDynaTable.h"
#include "epex.h"
  
  
  
 



extern "C"  BM2_FUNCTION_EXPORT
int f_mm0099_ac_bw2(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = " ";//当前时间
	CString v_datetime = " ";//当前生产时间
	CString cost_account = "1818";//成本帐套
	CString factory_div = "";//厂别
	CString acct_period_no = " ";//会计期
	CString deal_flag = "0";//处理标记
	CString ac_keyword = " ";//关键字
	CString event_related_id = " ";//事件关联号
	CString event_id = " ";
	CString seqNo = "";
	CString transaction_code = " ";//交易代码,成本关键字串2第一位
	CString function_code_ac = " ";//成本功能代码,成本关键字串2第二位
	CString ac_flag = " ";//抛帐区分标记，1：产出，新，正；2：投入，旧，负
	CString confm_flag = " ";//确认标记
	CString prod_confm = " ";//计产标记   1 计 0 不计
	CString ac_way = "";		//事件抛成本类型 0：不抛 1：事件配置 2：程序判断 3：外部调用
	CString ac_kind = "";		//交易种类
	CString ac_func = "";		//功能代码
	CString ac_date = "";		//业务日期
	CString ac_time = "";		//业务时间

	CString unit_code_aa("");  //连轧成本中心机组
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_temp(conn);
	CDbCommand cmd_inq_rl_no(conn);
	CModel tmmsm01("TMMSM01");
	CModel tmmbw01("TMMBW01");
	CModel hmmsm01("HMMSM01");
	CModel hmmbw01("HMMBW01");
	CModel tom01("TOM01");
	CModel tqmbp01("TQMBP01");
	
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");//当前时间

		Log::Trace("", "", "f_mm0099_ac_bs -自动调用-----begin-------");
		CDbCommand getResumeSeqNo("SELECT TO_CHAR(SYSTIMESTAMP, 'YYYYMMDDHH24MISSFF6') FROM DUAL", conn); //获取抛帐履历序号

		//判断传入块是否存在
		if (bcls_rec->Tables.Contains("EVENT_DATA") == false)
		{
			strcpy(s.sysmsg, "函数名称：f_mm0099_ac_bw 传入数据块 EVENT_DATA 不存在。");
			Log::Trace("", "", "函数名称：f_mm0099_ac_bw 传入数据块 EVENT_DATA 不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables.Contains("OLDMM_TABLE") == false)
		{
			strcpy(s.sysmsg, "函数名称：f_mm0099_ac_bw 传入数据块 OLDMM_TABLE 不存在。");
			Log::Trace("", "", "函数名称：f_mm0099_ac_bw 传入数据块 OLDMM_TABLE 不存在。");

			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables.Contains("NEWMM_TABLE") == false)
		{
			strcpy(s.sysmsg, "函数名称：f_mm0099_ac_bw 传入数据块 NEWMM_TABLE 不存在。");
			Log::Trace("", "", "函数名称：f_mm0099_ac_bw 传入数据块 NEWMM_TABLE 不存在。");

			throw CApplicationException(-1, s.msg, log.Location);
		}
		CDataTable dtEventData = bcls_rec->Tables["EVENT_DATA"];
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];
		CString matKind = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["MAT_KIND"].ToString();
		//CString matKind = "BW";

		//设置传出块
		if (bcls_ret->Tables.Contains("MM00AC") == false)
		{
			bcls_ret->Tables.Add("MM00AC");
			bcls_ret->Tables["MM00AC"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_ret->Tables["MM00AC"].Columns.Add(DT_STRING, "AC_KEYWORD");
		}
		else
		{
			bcls_ret->Tables["MM00AC"].Rows.Clear();
		}

		event_id = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["EVENT_ID"].ToString();
		CDynaTable matAcjc("TMMBWAC", conn);
		Log::Trace("", __FUNCTION__, "1=【{0}】", dtOldMat.Rows.get_Count());
		Log::Trace("", __FUNCTION__, "2=【{0}】", dtNewMat.Rows.get_Count());

		ac_way = dtEventData.Rows[0]["EVENT_PROC_WAY_AC"].ToString().Trim();//抛成本标记
		//ac_kind = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString().Trim();//成本关键字串1
		//ac_func = dtEventData.Rows[0]["KEYVALUE_2_AC"].ToString().Trim();//成本关键字串2

		//写中间表改用成本关键字串2，调成本函数用成本关键字串1
		ac_kind = dtEventData.Rows[0]["KEYVALUE_2_AC"].ToString().Trim().SubstringNE(1, 1);//成本关键字串2
		ac_func = dtEventData.Rows[0]["KEYVALUE_2_AC"].ToString().Trim().SubstringNE(2, 1);//成本关键字串2


		PrintLog("成本抛帐配置-", "ac_way = [" + ac_way + "], ac_kind = [" + ac_kind + "], ac_func = [" + ac_func + "] ");
		if (ac_way != "0")
		{
			if (dtEventData.Rows[0]["EVENT_PROC_WAY_3"].ToString() == "1")//修改类
			{
				Log::Trace("", __FUNCTION__, "修改");

				for (int i = 0; i < dtOldMat.Rows.get_Count(); i++)
				{
					matAcjc.MergeFrom(dtOldMat.Rows[i]);
					//计算会计期
					v_datetime = dtOldMat.Rows[i]["PROD_TIME"].ToString();
					acct_period_no = v_datetime.SubstringNE(0, 6);

					//计算账务代码，功能
					if (ac_way == "1")
					{
						transaction_code = ac_kind;
						function_code_ac = ac_func;
					}
					ac_flag = "2";  //旧数据用2
					if (function_code_ac == "D")
					{
						ac_flag = "2";
					}
					//棒线关联是材料号
					event_related_id = dtOldMat.Rows[i]["RL_NO"].ToString();
					if (event_related_id == "" || event_related_id == " ")
					{
						event_related_id = dtOldMat.Rows[i]["HEAT_NO"].ToString();
					}

					//计算账套
					sqlstr = " select factory_div from tmmbw01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' "
						" union all "
						" select factory_div from hmmbw01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' "
						" union all "
						" select factory_div from tmmsm01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' "
						" union all "
						" select factory_div from hmmsm01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					Log::Trace("", "", "函数名称：f_mm0099_ac_bw sqlstr=" + sqlstr);
					if (cmd_inq.Read())
					{
						factory_div = cmd_inq.GetString(1);//交易类型
					}
					cmd_inq.Close();

					if (factory_div == "" || factory_div == " ")
					{
						Log::Trace("", "", "函数名称：f_mm0099_ac_bw factory_div为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					//修改前数据-ac_flag=‘2’------------------------------------------------------------------------------------------------------
					matAcjc.SetColVal("FACTORY_DIV", factory_div);//厂别
					matAcjc.SetColVal("REC_CREATOR", s.userid);//记录创建责任者
					matAcjc.SetColVal("REC_CREATE_TIME", datetime);//记录创建时刻
					matAcjc.SetColVal("COST_ACCOUNT", cost_account);//成本账套
					matAcjc.SetColVal("ACCT_YEAR", acct_period_no.SubstringNE(0, 4));//会计年度
					matAcjc.SetColVal("DEAL_FLAG", deal_flag);//处理标记
					matAcjc.SetColVal("AC_KEYWORD", "BW" + getResumeSeqNo.ExecuteScalar().ToString().Trim());//关键字
					getResumeSeqNo.Close();
					matAcjc.SetColVal("APP_THROW_AI_DATE", datetime.SubstringNE(0, 8));//抛帐日期
					matAcjc.SetColVal("APP_THROW_AI_TIME", datetime.SubstringNE(8, 6));//抛帐时间
					matAcjc.SetColVal("RESUME_SEQ_NO", matAcjc.GetColValString("AC_KEYWORD"));
					matAcjc.SetColVal("EVENT_RELATED_ID", event_related_id);//材料号
					matAcjc.CopyColVal("EVENT_ID", dtNewMat.Rows[i]);//事件号
					matAcjc.CopyColVal("EVENT_NAME", dtNewMat.Rows[i]); 
					matAcjc.SetColVal("TRANSACTION_CODE", transaction_code);//交易代码
					matAcjc.SetColVal("FUNCTION_CODE_AC", function_code_ac);//成本功能代码
					matAcjc.SetColVal("AC_FLAG", ac_flag);//抛帐区分标记
					matAcjc.SetColVal("UNIT_CODE_AA", matAcjc.GetColValString("UNIT_CODE"));
					matAcjc.SetColVal("UNIT_CODE_AC", matAcjc.GetColValString("UNIT_CODE"));
					matAcjc.SetColVal("ACCT_PERIOD_NO", acct_period_no.SubstringNE(0, 6));//会计期
					matAcjc.SetColVal("MAT_WT_ORIGIN", matAcjc.GetColValString("MAT_ACT_WT"));
					matAcjc.SetColVal("BASE_DIV", "A");         //基地区分
					matAcjc.SetColVal("LOG_BASE_DIV", "A");     //逻辑基地区分
					if (matAcjc.GetColValString("PROD_CODE").Trim() == "")
					{
						matAcjc.SetColVal("PROD_CODE", dtNewMat.Rows[i]["PROD_CODE"].ToString());   //品名
					}

					if (matAcjc.GetColValString("MAT_KIND") == "SM")
					{
						tmmsm01["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();
						if (tmmsm01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmsm01["FIX_FLAG"].ToString());     //定尺标记
							matAcjc.SetColVal("BACK_CODE_4", tmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
						}
						else
						{
							hmmsm01["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();
							if (hmmsm01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmsm01["FIX_FLAG"].ToString());     //定尺标记
								matAcjc.SetColVal("BACK_CODE_4", hmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
							}
						}

						if (tmmsm01["ORDER_NO"].ToString().Trim() != "" || hmmsm01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmsm01["ORDER_NO"].ToString().Trim();
							if (tmmsm01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmsm01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}
							}

						}
					}
					else
					{
						tmmbw01["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();
						if (tmmbw01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmbw01["SHORT_FLAG"].ToString());     //定尺标记
						}
						else
						{
							hmmbw01["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();
							if (hmmbw01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmbw01["SHORT_FLAG"].ToString());     //定尺标记
							}
						}
						if (tmmbw01["ORDER_NO"].ToString().Trim() != "" || hmmbw01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmbw01["ORDER_NO"].ToString().Trim();
							if (tmmbw01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmbw01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}
							}

						}
					}
					if (matAcjc.GetColValString("HOT_TREAT_METHOD_CODE").Trim() == "" || matAcjc.GetColValString("HOT_TREAT_METHOD_CODE") == " ")
					{
						matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", "0");     //热处理方式
					}
					if (event_id == "PM05" || event_id == "PM11")//准发
					{
						matAcjc.SetColVal("CONFM_FLAG", "1");
						matAcjc.SetColVal("AC_FLAG", "2");//抛帐区分标记
						matAcjc.SetColVal("PROD_CONFM", "1");//计产标记
						prod_confm = "1";                    //计产标记
					}
					if (event_id == "PM13")//准发红冲
					{
						matAcjc.SetColVal("CONFM_FLAG", "2");
						matAcjc.SetColVal("AC_FLAG", "2");//抛帐区分标记
						matAcjc.SetColVal("PROD_CONFM", "0");//计产标记
						prod_confm = "0";                    //计产标记
					}
					if (event_id == "PM05" || event_id == "PM13" || event_id == "PM11")//PM05准发确认，PM13准发红冲，PM11现货计划确认
					{
						tmmbw01["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();
						tmmbw01["PROD_CONFM"] = prod_confm;
						tmmbw01["PROD_CONFM_TIME"] = datetime;
						tmmbw01["PROD_CONFM_WT"] = dtOldMat.Rows[i]["MAT_ACT_WT"].ToDecimal();
						tmmbw01.Update("PROD_CONFM,"//记产标记
							"PROD_CONFM_TIME,"//记产时刻
							"PROD_CONFM_WT ", "MAT_NO");//记产重量
					}

					if (matAcjc.GetColValString("UNIT_CODE") == "A840")
					{
						
						matAcjc.SetColVal("UNIT_CODE_AA", "A830");   //连轧精整成本中心取消，合并到轧制
						matAcjc.SetColVal("UNIT_CODE_AC", "A830");   //连轧精整成本中心取消，合并到轧制

					}

					if (dtOldMat.Rows[i]["MAT_ORIGIN"].ToString() == "1" && (event_id == "PM02" || event_id == "MM03" || event_id == "MM04" || event_id == "MM27" || event_id == "QM66"))//外购材料
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");//交易代码
					}
					if (dtOldMat.Rows[i]["MAT_ORIGIN"].ToString() == "6")//内部互供料
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");//交易代码
					}
					matAcjc.MergeTo(bcls_ret->Tables["MM00AC"]);

					if (event_id != "QM05")//QM05判废事件只抛1条 ac_flag为 1
					{
						if (dtOldMat.Rows[i]["MAT_ORIGIN"].ToString() == "1"&&event_id == "WM01" )//材料不是外购,外购改配需要抛两条
						{
							//不抛成本
						}
						else
						{
							if (matAcjc.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}

					}

					//修改后数据-ac_flag=‘1’-----------------------------------------------------------------------------
					matAcjc.MergeFrom(dtNewMat.Rows[i]);

					ac_flag = "1";//新数据用1

					matAcjc.SetColVal("REC_CREATOR", s.userid);//记录创建责任者
					matAcjc.SetColVal("REC_CREATE_TIME", datetime);//记录创建时刻
					matAcjc.SetColVal("FACTORY_DIV", factory_div);//厂别
					matAcjc.SetColVal("AC_KEYWORD", "BW" + getResumeSeqNo.ExecuteScalar().ToString().Trim());//关键字（抛账履历序号）
					getResumeSeqNo.Close();
					matAcjc.SetColVal("RESUME_SEQ_NO", matAcjc.GetColValString("AC_KEYWORD"));
					matAcjc.SetColVal("EVENT_RELATED_ID", event_related_id);//材料号
					matAcjc.SetColVal("AC_FLAG", ac_flag);//抛帐区分标记
					matAcjc.SetColVal("MAT_WT_ORIGIN", matAcjc.GetColValString("MAT_ACT_WT"));
                 	matAcjc.SetColVal("UNIT_CODE_AA", matAcjc.GetColValString("UNIT_CODE"));
					matAcjc.SetColVal("UNIT_CODE_AC", matAcjc.GetColValString("UNIT_CODE"));
					
					if (matAcjc.GetColValString("MAT_KIND") == "SM")
					{
						tmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
						if (tmmsm01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmsm01["FIX_FLAG"].ToString());     //定尺标记
							matAcjc.SetColVal("BACK_CODE_4", tmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
						}
						else
						{
							hmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
							if (hmmsm01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmsm01["FIX_FLAG"].ToString());     //定尺标记
								matAcjc.SetColVal("BACK_CODE_4", hmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
							}
						}

						if (tmmsm01["ORDER_NO"].ToString().Trim() != "" || hmmsm01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmsm01["ORDER_NO"].ToString().Trim();
							if (tmmsm01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmsm01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}


							}

						}
					}
					else
					{
						tmmbw01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
						if (tmmbw01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmbw01["SHORT_FLAG"].ToString());     //定尺标记
						}
						else
						{
							hmmbw01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
							if (hmmbw01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmbw01["SHORT_FLAG"].ToString());     //定尺标记
							}
						}
						if (tmmbw01["ORDER_NO"].ToString().Trim() != "" || hmmbw01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmbw01["ORDER_NO"].ToString().Trim();
							if (tmmbw01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmbw01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}
							}

						}
					}


					if (matAcjc.GetColValString("HOT_TREAT_METHOD_CODE").Trim() == "" || matAcjc.GetColValString("HOT_TREAT_METHOD_CODE") == " ")
					{
						matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", "0");     //热处理方式
					}
					if (event_id == "WM01"&&dtNewMat.Rows[i]["MAT_ORIGIN"].ToString() == "2")//材料入口
					{
						matAcjc.SetColVal("PROD_CONFM", "1");//计产标记
						prod_confm = "1";                    //计产标记

						tmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
						if (tmmsm01.Query("MAT_NO") != false)
						{
							if (tmmsm01["PROD_CONFM"].ToString() == "0" || tmmsm01["PROD_CONFM"].ToString() == " ")
							{
								tmmsm01["PROD_CONFM"] = prod_confm;
								tmmsm01["PROD_CONFM_TIME"] = datetime;
								tmmsm01["PROD_CONFM_WT"] = dtOldMat.Rows[i]["MAT_ACT_WT"].ToDecimal();
								tmmsm01.Update("PROD_CONFM,"
									"PROD_CONFM_TIME,"
									"PROD_CONFM_WT ", "MAT_NO");
							}
							
						}

					}
					if (event_id == "PM05" || event_id == "PM11")
					{
						matAcjc.SetColVal("CONFM_FLAG", "2");
						matAcjc.SetColVal("PROD_CONFM", "1");//计产标记
						prod_confm = "1";                    //计产标记
					}
					if (event_id == "PM13")//准发红冲
					{
						matAcjc.SetColVal("CONFM_FLAG", "1");
						matAcjc.SetColVal("PROD_CONFM", "0");//计产标记
						prod_confm = "0";                    //计产标记
					}
	
					if (event_id == "QM05"&&matAcjc.GetColValString("MAT_KIND") != "SM") //非坯料判废
					{

						matAcjc.SetColVal("TRANSACTION_CODE", "T");
						matAcjc.SetColVal("FUNCTION_CODE_AC", "N");
						matAcjc.SetColVal("AC_FLAG", "1");//抛帐区分标记
						matAcjc.SetColVal("UNIT_CODE_AC", matAcjc.GetColValString("UNIT_CODE"));
					}
					if (event_id == "QM05"&&matAcjc.GetColValString("MAT_KIND") == "SM")  //坯料判废
					{

						matAcjc.SetColVal("TRANSACTION_CODE", "P");
						matAcjc.SetColVal("FUNCTION_CODE_AC", "N");
						matAcjc.SetColVal("AC_FLAG", "2");//抛帐区分标记
						matAcjc.SetColVal("EVENT_ID", "MM06");//事件号
						matAcjc.SetColVal("EVENT_NAME", "入口材料产出归档_坯料判废");
						matAcjc.SetColVal("UNIT_CODE_AC", matAcjc.GetColValString("UNIT_CODE"));
					}


					if (dtNewMat.Rows[i]["MAT_ORIGIN"].ToString() == "1" && (event_id == "PM02" || event_id == "MM03" || event_id == "MM04" || event_id == "MM27"||event_id=="QM66"))//外购材料
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");//交易代码
					}
					if (dtNewMat.Rows[i]["MAT_ORIGIN"].ToString() == "6" &&  event_id == "QM66")//6是互供料，成本说跟外购一样抛W
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");//交易代码
					}
					if (event_id == "WM01"&&dtNewMat.Rows[i]["MAT_ORIGIN"].ToString() == "1")//材料入口 外购的
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");
						matAcjc.SetColVal("FUNCTION_CODE_AC", "N");
						matAcjc.SetColVal("AC_FLAG", "1");//抛帐区分标记
						matAcjc.SetColVal("EVENT_NAME", "外购材料入库");
					}

					if (event_id == "MMPF")//本产线判废的原料算本产线投入
					{
						matAcjc.CopyColVal("EVENT_ID", "MM06");//事件号
					}

					

					matAcjc.MergeTo(bcls_ret->Tables["MM00AC"]);			
						if (matAcjc.Insert() < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
				
				}
			}
			else//非修改
			{
				Log::Trace("", __FUNCTION__, "非修改");

				for (int i = 0; i < dtNewMat.Rows.get_Count(); i++)
				{
					matAcjc.MergeFrom(dtNewMat.Rows[i]);
					//计算会计期
					v_datetime = dtNewMat.Rows[i]["PROD_TIME"].ToString();

					Log::Trace("", __FUNCTION__, "v_datetime=【{0}】", v_datetime);
					acct_period_no = v_datetime.SubstringNE(0, 6);

					//计算账务代码，功能
					if (ac_way == "1")
					{
						transaction_code = ac_kind;
						function_code_ac = ac_func;
					}

					Log::Trace("", __FUNCTION__, "transaction_code=【{0}】", transaction_code);
					Log::Trace("", __FUNCTION__, "function_code_ac=【{0}】", function_code_ac);

					
					//棒线关联是材料号
					event_related_id = dtNewMat.Rows[i]["RL_NO"].ToString();
					if (event_related_id == "" || event_related_id == " ")
					{
						event_related_id = dtNewMat.Rows[i]["HEAT_NO"].ToString();
					}
					ac_flag = "1";//新增类数据用1

					//计算账套
					sqlstr = " select factory_div from tmmbw01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' "
						" union all "
						" select factory_div from hmmbw01 "
						" WHERE Mat_no = '" + dtNewMat.Rows[i]["MAT_NO"].ToString() + "' ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						factory_div = cmd_inq.GetString(1);//交易类型
					}
					cmd_inq.Close();
					if (factory_div == "" || factory_div == " ")
					{
						Log::Trace("", "", "函数名称：f_mm0099_ac_bw factory_div为空。");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					matAcjc.SetColVal("REC_CREATOR", s.userid);//记录创建责任者
					matAcjc.SetColVal("REC_CREATE_TIME", datetime);//记录创建时刻
					matAcjc.SetColVal("COST_ACCOUNT", cost_account);//成本账套
					matAcjc.SetColVal("ACCT_YEAR", acct_period_no.SubstringNE(0, 4));//会计年度
					matAcjc.SetColVal("DEAL_FLAG", deal_flag);//处理标记
					matAcjc.SetColVal("AC_KEYWORD", "BW" + getResumeSeqNo.ExecuteScalar().ToString().Trim());//关键字
					getResumeSeqNo.Close();
					matAcjc.SetColVal("APP_THROW_AI_DATE", datetime.SubstringNE(0, 8));//抛帐日期
					matAcjc.SetColVal("APP_THROW_AI_TIME", datetime.SubstringNE(8, 6));//抛帐时间
					matAcjc.SetColVal("RESUME_SEQ_NO", matAcjc.GetColValString("AC_KEYWORD"));
					matAcjc.SetColVal("EVENT_RELATED_ID", event_related_id);//材料号
					matAcjc.CopyColVal("EVENT_ID", dtNewMat.Rows[i]);//事件号
					matAcjc.CopyColVal("EVENT_DESC", dtNewMat.Rows[i]);
					matAcjc.SetColVal("TRANSACTION_CODE", transaction_code);//交易代码
					matAcjc.SetColVal("FUNCTION_CODE_AC", function_code_ac);//成本功能代码
					matAcjc.SetColVal("AC_FLAG", ac_flag);//抛帐区分标记

					
					matAcjc.SetColVal("UNIT_CODE_AA", matAcjc.GetColValString("UNIT_CODE"));
					matAcjc.SetColVal("UNIT_CODE_AC", matAcjc.GetColValString("UNIT_CODE"));
				
					matAcjc.SetColVal("ACCT_PERIOD_NO", acct_period_no.SubstringNE(0, 6));//会计期
					matAcjc.SetColVal("MAT_WT_ORIGIN", matAcjc.GetColValString("MAT_ACT_WT"));
					matAcjc.SetColVal("BASE_DIV", "A");         //基地区分
					matAcjc.SetColVal("LOG_BASE_DIV", "A");     //逻辑基地区分

					if (matAcjc.GetColValString("MAT_KIND") == "SM")
					{
						tmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
						if (tmmsm01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmsm01["FIX_FLAG"].ToString());     //定尺标记
							matAcjc.SetColVal("BACK_CODE_4", tmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
						}
						else
						{
							hmmsm01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
							if (hmmsm01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmsm01["FIX_FLAG"].ToString());     //定尺标记
								matAcjc.SetColVal("BACK_CODE_4", hmmsm01["SPARE_ITEM_0"].ToString());     //清理方式
							}
						}

						if (tmmsm01["ORDER_NO"].ToString().Trim() != "" || hmmsm01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmsm01["ORDER_NO"].ToString().Trim();
							if (tmmsm01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmsm01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}


							}

						}
					}
					else
					{
						tmmbw01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
						if (tmmbw01.Query("MAT_NO") != false)
						{
							matAcjc.SetColVal("FIX_FLAG", tmmbw01["SHORT_FLAG"].ToString());     //定尺标记
						}
						else
						{
							hmmbw01["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();
							if (hmmbw01.Query("MAT_NO") != false)
							{
								matAcjc.SetColVal("FIX_FLAG", hmmbw01["SHORT_FLAG"].ToString());     //定尺标记
							}
						}

						if (tmmbw01["ORDER_NO"].ToString().Trim() != "" || hmmbw01["ORDER_NO"].ToString().Trim() != "")
						{
							tom01["ORDER_NO"] = tmmbw01["ORDER_NO"].ToString().Trim();
							if (tmmbw01["ORDER_NO"].ToString().Trim() == "")
							{
								tom01["ORDER_NO"] = hmmbw01["ORDER_NO"].ToString().Trim();
							}
							if (tom01.Query("ORDER_NO") != false)
							{
								tqmbp01["PSR"] = tom01["PSC"];
								if (tqmbp01.Query("PSR") != false)
								{
									matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", tqmbp01["DELIVY_STATUS_CODE"].ToString().Trim());     //热处理方式
								}


							}

						}
					}

					if (matAcjc.GetColValString("HOT_TREAT_METHOD_CODE").Trim() == "" || matAcjc.GetColValString("HOT_TREAT_METHOD_CODE") == " ")
					{
						matAcjc.SetColVal("HOT_TREAT_METHOD_CODE", "0");     //热处理方式
					}

					if (event_id == "PM05" || event_id == "PM11")
					{
						matAcjc.SetColVal("CONFM_FLAG", "2");
					
					}
					if (dtNewMat.Rows[i]["MAT_ORIGIN"].ToString() == "1"&&event_id == "PM02")//外购材料
					{
						matAcjc.SetColVal("TRANSACTION_CODE", "W");//交易代码
					}
					if (event_id == "MM09" || event_id == "MM06") //投入产出需写成本核算机组 
					{
						matAcjc.SetColVal("BACK_CODE_10", matAcjc.GetColValString("UNIT_CODE_AA"));
					}
					else	
					{
						matAcjc.SetColVal("BACK_CODE_10", matAcjc.GetColValString("UNIT_CODE"));
					}
					if (event_id == "MM00")
					{
						matAcjc.SetColVal("APP_THROW_AI_DATE", "20211101");   //期初固定写202111
					}
					matAcjc.Print();
					matAcjc.MergeTo(bcls_ret->Tables["MM00AC"]); 
					
						if (matAcjc.Insert() < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
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


