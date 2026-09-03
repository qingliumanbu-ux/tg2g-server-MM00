/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      K17017
Version:     1.0
Date:        2018-06-11 16:33:43
Description: 查询在库周期信息
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 查询在库周期信息
/// <para>
/// 查询在库周期信息
/// </para>
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表
///                 tmm0021 库区静态表         </para>
/// <para>主调用函数：前台MM0004A4画面F2(查询)调用。   </para>
/// </summary>
/// <param name="">    </param>
/// <returns>具体调用函数返回的材料在库周期统计信息</returns>
===========================================================</remark>*/

/******框架头******/
#include "stdafx.h"

/******service入口******/
BM2F_ENTERACE(mm00sud4f2_inq)


int f_mm00sud4f2_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

	/*定义程序用变量*/
	int doFlag = 0;

	/****** 业务处理开始 ******/
	try {
		/*实体类定义*/
		//CTMMSM01 tmmsmma01(conn);
		//CTMMHR01 tmmhrma01(conn);
		//CTMMCR01 tmmcrma01(conn);

		/*数据库操作类定义*/
		CDbCommand cmd(conn);

		int loop_mat_no = 0;
		int loop_cmd_query = 0;
		CString v_mat_no[9] = { "板坯", "板坯", "热轧", "热轧", "热轧", "冷轧", "冷轧", "冷轧", "冷轧" };//材料
		CString v_stock_no[9] = { "板坯库", "总计", "钢卷库", "成品库", "总计", "原料库", "中间库", "成品库", "总计" };//库名
		CString cmd_query[9] = { "", "", "", "", "", "", "", "", "" };//存放查询语句数组
		CDecimal v_mat_act_wt[9][12];//存放重量二维数组
		int loop_day = 0;


		/****** 获得输入参数 ******/
		CString v_order_div = bcls_rec->Tables[0].Rows[0]["ORDER_DIV"].ToString().Trim();/*有无合同区分*/


		//增加数据列
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_NO");//库名
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_1");//1天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_2");//2天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_3");//3天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_4");//4天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_5");//5天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_6");//6天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_7");//7天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_8");//8-14天
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_9");//15天-1月
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_10");//1月-3月
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_11");//3-6月
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DAY_12");//6月以上
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SUM");//合计

		//材料
		for (loop_mat_no = 0; loop_mat_no < 9; loop_mat_no++)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[loop_mat_no]["MAT_NO"] = v_mat_no[loop_mat_no];
			bcls_ret->Tables[0].Rows[loop_mat_no]["STOCK_NO"] = v_stock_no[loop_mat_no];
		}

		//材料状态27不计入统计量 2013-1-14 9:26:15
		/*定义查询SQL*/
		CString cmd_query_sm_slab = " SELECT SUM(NVL(MAT_ACT_WT,0)) as MAT_ACT_WT FROM TMMSM01 WHERE MAT_LINE_TYPE = 'SM'  AND MAT_STATUS <> '36' ";//板坯库
		CString cmd_query_sm_total = " SELECT SUM(NVL(MAT_ACT_WT,0)) as MAT_ACT_WT FROM TMMSM01 WHERE MAT_LINE_TYPE = 'SM'  AND MAT_STATUS <> '36' ";//板坯总计

		CString cmd_query_hr_coil = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE MAT_LINE_TYPE = 'HR' "
			" AND PRODUCT_FLAG = '0' ";//钢卷库

		CString cmd_query_hr_product = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE MAT_LINE_TYPE = 'HR' "
			" AND PRODUCT_FLAG = '1' "
			" AND MAT_STATUS <> '36' "
			;//成品库

		CString cmd_query_hr_total = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE MAT_LINE_TYPE = 'HR' "
			" AND (PRODUCT_FLAG = '0' OR (PRODUCT_FLAG = '1' AND MAT_STATUS <> '36')) "
			;//热卷总计													 

		CString cmd_query_cr_raw = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 "
			" WHERE MAT_LINE_TYPE = 'CR' ";//原料库 冷轧原料库取热卷表

		CString cmd_query_cr_mid = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 "
			" WHERE MAT_LINE_TYPE = 'CR' "
			" AND PRODUCT_FLAG = '0' ";//中间库

		CString cmd_query_cr_product = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 "
			" WHERE MAT_LINE_TYPE = 'CR' "
			" AND PRODUCT_FLAG = '1'  "
			" AND MAT_STATUS <> '36' "
			;//成品库

		CString cmd_query_cr_total = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 "
			" WHERE 1 = 1 "
			" AND (MAT_LINE_TYPE = 'CR' "
			" OR (MAT_LINE_TYPE = 'CR'  AND PRODUCT_FLAG = '0') "
			" OR (MAT_LINE_TYPE = 'CR'  AND PRODUCT_FLAG = '1' AND MAT_STATUS <> '36')) "
			;//冷卷总计


		if (v_order_div.Trim() == "1")//有合同
		{
			cmd_query_sm_slab += " AND ORDER_NO > ' ' ";
			cmd_query_sm_total += " AND ORDER_NO > ' ' ";

			cmd_query_hr_coil += " AND ORDER_NO > ' ' ";
			cmd_query_hr_product += " AND ORDER_NO > ' ' ";
			cmd_query_hr_total += " AND ORDER_NO > ' ' ";

			cmd_query_cr_raw += " AND ORDER_NO > ' ' ";
			cmd_query_cr_mid += " AND ORDER_NO > ' ' ";
			cmd_query_cr_product += " AND ORDER_NO > ' ' ";
			cmd_query_cr_total += " AND ORDER_NO > ' ' ";
		}

		if (v_order_div.Trim() == "0")//无合同
		{
			cmd_query_sm_slab += " AND ORDER_NO = ' ' ";
			cmd_query_sm_total += " AND ORDER_NO = ' ' ";

			cmd_query_hr_coil += " AND ORDER_NO = ' ' ";
			cmd_query_hr_product += " AND ORDER_NO = ' ' ";
			cmd_query_hr_total += " AND ORDER_NO = ' ' ";

			cmd_query_cr_raw += " AND ORDER_NO = ' ' ";
			cmd_query_cr_mid += " AND ORDER_NO = ' ' ";
			cmd_query_cr_product += " AND ORDER_NO = ' ' ";
			cmd_query_cr_total += " AND ORDER_NO = ' ' ";
		}

		CString date_0 = CDateTime::Now().ToString("yyyyMMddHHmmss");
		CString date_1 = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss");
		CString date_2 = CDateTime::Now().AddDays(-2).ToString("yyyyMMddHHmmss");
		CString date_3 = CDateTime::Now().AddDays(-3).ToString("yyyyMMddHHmmss");
		CString date_4 = CDateTime::Now().AddDays(-4).ToString("yyyyMMddHHmmss");
		CString date_5 = CDateTime::Now().AddDays(-5).ToString("yyyyMMddHHmmss");
		CString date_6 = CDateTime::Now().AddDays(-6).ToString("yyyyMMddHHmmss");
		CString date_7 = CDateTime::Now().AddDays(-7).ToString("yyyyMMddHHmmss");
		CString date_14 = CDateTime::Now().AddDays(-14).ToString("yyyyMMddHHmmss");
		CString date_30 = CDateTime::Now().AddDays(-30).ToString("yyyyMMddHHmmss");
		CString date_90 = CDateTime::Now().AddDays(-90).ToString("yyyyMMddHHmmss");
		CString date_180 = CDateTime::Now().AddDays(-180).ToString("yyyyMMddHHmmss");
		Log::Trace("", "mm00sud4f2_inq", "------date_180{0}--------", (const char*)date_180);
		//板坯 当前时间-板坯切断时刻 C14 SLAB_CUT_TIME
		//热卷、冷卷 当前时间-生产结束时间 C14 PROD_END_TIME
		//1.板坯
		cmd_query_sm_slab += " AND SLAB_CUT_TIME > @time1 AND SLAB_CUT_TIME <= @time2 ";
		cmd_query_sm_total += " AND SLAB_CUT_TIME > @time1 AND SLAB_CUT_TIME <= @time2 ";

		Log::Trace("", __FUNCTION__, "cmd_query_sm_total=[{0}]", cmd_query_sm_total);

		//2.热卷
		cmd_query_hr_coil += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
		cmd_query_hr_product += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
		cmd_query_hr_total += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";

		//3.冷卷
		cmd_query_cr_raw += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
		cmd_query_cr_mid += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
		cmd_query_cr_product += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
		cmd_query_cr_total += " AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";

		//将查询语句放入查询语句数组
		cmd_query[0] = cmd_query_sm_slab;
		cmd_query[1] = cmd_query_sm_total;
		cmd_query[2] = cmd_query_hr_coil;
		cmd_query[3] = cmd_query_hr_product;
		cmd_query[4] = cmd_query_hr_total;
		cmd_query[5] = cmd_query_cr_raw;
		cmd_query[6] = cmd_query_cr_mid;
		cmd_query[7] = cmd_query_cr_product;
		cmd_query[8] = cmd_query_cr_total;

		for (loop_cmd_query = 0; loop_cmd_query < 9; loop_cmd_query++)
		{
			cmd.SetCommandText(cmd_query[loop_cmd_query]);

			cmd.Parameters.Clear();
			for (loop_day = 0; loop_day < 13; loop_day++)
			{
				Log::Trace("", "mm00sud4f2_inq", "------case {0}--------", loop_day);
				switch (loop_day + 1)
				{
				case 1: cmd.Parameters.Set("time1", date_1);
					cmd.Parameters.Set("time2", date_0);
					break;//1天
				case 2: cmd.Parameters.Set("time1", date_2);
					cmd.Parameters.Set("time2", date_1);
					break;//2天
				case 3: cmd.Parameters.Set("time1", date_3);
					cmd.Parameters.Set("time2", date_2);
					break;//3天
				case 4: cmd.Parameters.Set("time1", date_4);
					cmd.Parameters.Set("time2", date_3);
					break;//4天
				case 5: cmd.Parameters.Set("time1", date_5);
					cmd.Parameters.Set("time2", date_4);
					break;//5天
				case 6: cmd.Parameters.Set("time1", date_6);
					cmd.Parameters.Set("time2", date_5);
					break;//6天
				case 7: cmd.Parameters.Set("time1", date_7);
					cmd.Parameters.Set("time2", date_6);
					break;//7天
				case 8: cmd.Parameters.Set("time1", date_14);
					cmd.Parameters.Set("time2", date_7);
					break;//8-14天
				case 9: cmd.Parameters.Set("time1", date_30);
					cmd.Parameters.Set("time2", date_14);
					break;//15-1月
				case 10: cmd.Parameters.Set("time1", date_90);
					cmd.Parameters.Set("time2", date_30);
					break;//1-3月
				case 11: cmd.Parameters.Set("time1", date_180);
					cmd.Parameters.Set("time2", date_90);
					break;//3-6月
				case 12: cmd.Parameters.Set("time1", "00000000000000");
					cmd.Parameters.Set("time2", date_180);
					break;//6月以上
				case 13: cmd.Parameters.Set("time1", "00000000000000");
					cmd.Parameters.Set("time2", date_0);
					break;//合计
				default: break;
				}

				cmd.ExecuteReader();
				if (cmd.Read())
				{
					Log::Trace("", "mm00sud4f2_inq", "------loop_cmd_query {0}--------", loop_cmd_query);
					Log::Trace("", "mm00sud4f2_inq", "------loop_day {0}--------", loop_day);
					Log::Trace("", "mm00sud4f2_inq", "------cmd.GetDecimal(1).Floor() {0}--------", cmd.GetDecimal(1).Floor());
					v_mat_act_wt[loop_cmd_query][loop_day + 2] = cmd.GetDecimal(1).Floor();//重量
					bcls_ret->Tables[0].Rows[loop_cmd_query][loop_day + 2] = v_mat_act_wt[loop_cmd_query][loop_day + 2];//各库区重量
				}
				cmd.Close();
			}//end for(loop_day=0;loop_day<9;loop_day++)

		}//end for(loop_cmd_query=0;loop_cmd_query<7;loop_cmd_query++)	

	}
	catch (const CApplicationException& ex)
	{
		strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	catch (const CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
		s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
		doFlag = -1;
	}

	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
