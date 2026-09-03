/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      KE1809
Version:     1.0
Date:        2022-07-05 16:33:43
Description: 查询各机组封锁量信息
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 查询各机组封锁量信息
/// <para>
/// 查询各机组封锁量信息
/// </para>
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表         </para>
/// <para>主调用函数：前台MM00SUD1画面F2(查询)调用。   </para>
/// </summary>
/// <param name="">    </param>
/// <returns>具体调用函数返回的各机组封锁量信息</returns>
===========================================================</remark>*/

/******框架头******/
#include "stdafx.h"

/******service入口******/
BM2F_ENTERACE(mm00sud1f2_inq)

int f_mm00sud1f2_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

	/*定义程序用变量*/
	int doFlag = 0;

	/****** 业务处理开始 ******/
	try {
		/*实体类定义*/


		/*数据库操作类定义*/
		CDbCommand cmd(conn);
		CDbCommand cmd_count(conn);

		CString cmd_query = "";

		CString v_dest = "";//下游/返修机组
		int loop_factory_div = 0;
		CString v_factory_div[10] = { "A10", "A20", "H10", "HA0", "E10", "H10", "HA0", "E10", "C10", "C10" };//板坯为库区，冷热卷为存储厂别
		CString v_factory_div_desc[20] = {
			"一炼钢", "板坯", "二炼钢", "板坯", "一热轧", "板坯", "二热轧", "板坯", "连铸连轧", "板坯", "一热轧", "热卷", "二热轧", "热卷", "连铸连轧", "热卷", "冷轧厂", "热卷", "冷轧", "冷卷"
		};
		int loop_mat_status = 0;
		CString v_mat_status[7] = { "21", "22", "03", "13", " ", "31", "32" };//材料状态 v_mat_act_wt[i][loop_mat_status+5]
		CDecimal v_mat_act_wt[1000][12];


#pragma region		/****** 获得输入参数 ******/
		CString v_order_div = bcls_rec->Tables[0].Rows[0]["ORDER_DIV"].ToString().Trim();//有无合同区分
		CDecimal v_hold_time_from = bcls_rec->Tables[0].Rows[0]["HOLD_TIME_FROM"];//封锁时间起
		CDecimal v_hold_time_to = bcls_rec->Tables[0].Rows[0]["HOLD_TIME_TO"];//封锁时间止


		//增加数据列
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");//存储厂别
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_SHAPE");//物料形态
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");//下游/返修机组
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "TOTAL_SUM");//合计
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SUM_1");//在制品_小计
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_01");//在制品_01
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_02");//在制品_02
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_03");//在制品_03
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_13");//在制品_13
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SUM_2");//成品_小计
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_21");//成品_21
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAT_STATUS_22");//成品_22
#pragma endregion
#pragma region		/*定义查询SQL*/
		//板坯
		//查板坯所有去向
		CString cmd_query_sm_count = " SELECT DISTINCT DEST_FIN FROM TMMSM01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS IN('31','32','03','13','21','22') "
			;

		CString cmd_query_sm = " SELECT SUM(NVL(MAT_ACT_WT,0)) FROM TMMSM01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND DEST_FIN = @dest "//下游/返修机组，板坯根据板坯去向
			" AND MAT_STATUS = @mat_status "
			;

		CString cmd_query_sm_xj = " SELECT SUM(NVL(MAT_ACT_WT,0)) FROM TMMSM01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS = @mat_status "
			;

		//热卷
		CString cmd_query_hr_count = " SELECT DISTINCT DEST FROM ( "
			" SELECT DISTINCT NEXT_UNIT_CODE DEST,ORDER_NO,HOLD_TIME FROM TMMHR01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS IN('31','32','21','22') "
			" UNION "
			" SELECT DISTINCT REPAIR_BACKLOG_CODE DEST,ORDER_NO,HOLD_TIME FROM TMMHR01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS IN('03','13')) "
			" WHERE 1=1 "
			;
		CString cmd_query_hr = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS = @mat_status "
			;

		CString cmd_query_hr_xj = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE FACTORY_STORE LIKE @factory_div||'%' "
			" AND MAT_STATUS = @mat_status ";
		;//行小计

		//冷卷
		CString cmd_query_cr_count = " SELECT DISTINCT DEST FROM ( "
			" SELECT DISTINCT NEXT_UNIT_CODE DEST,ORDER_NO,HOLD_TIME,FACTORY_STORE FROM TMMCR01 WHERE "
			" MAT_STATUS IN('31','32','21','22') "
			" UNION "
			" SELECT DISTINCT REPAIR_BACKLOG_CODE DEST,ORDER_NO,HOLD_TIME,FACTORY_STORE FROM TMMCR01 WHERE "
			" MAT_STATUS IN('03','13')) "
			" WHERE 1=1 ";

		CString cmd_query_cr = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 WHERE MAT_STATUS = @mat_status ";

		CString cmd_query_cr_xj = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 WHERE MAT_STATUS = @mat_status ";

		CString cmd_query_cr_count1 = "";
		CString cmd_query_cr1 = "";
		CString cmd_query_cr_xj1 = "";
#pragma endregion

		if (v_order_div.Trim() == "1")//有合同
		{
			cmd_query_sm_count += " AND ORDER_NO > ' ' ";
			cmd_query_sm += " AND ORDER_NO > ' ' ";
			cmd_query_sm_xj += " AND ORDER_NO > ' ' ";

			cmd_query_hr_count += " AND ORDER_NO > ' ' ";
			cmd_query_hr += " AND ORDER_NO > ' ' ";
			cmd_query_hr_xj += " AND ORDER_NO > ' ' ";

			cmd_query_cr_count += " AND ORDER_NO > ' ' ";
			cmd_query_cr += " AND ORDER_NO > ' ' ";
			cmd_query_cr_xj += " AND ORDER_NO > ' ' ";
		}

		if (v_order_div.Trim() == "0")//无合同
		{
			cmd_query_sm_count += " AND ORDER_NO = ' ' ";
			cmd_query_sm += " AND ORDER_NO = ' ' ";
			cmd_query_sm_xj += " AND ORDER_NO = ' ' ";

			cmd_query_hr_count += " AND ORDER_NO = ' ' ";
			cmd_query_hr += " AND ORDER_NO = ' ' ";
			cmd_query_hr_xj += " AND ORDER_NO = ' ' ";

			cmd_query_cr_count += " AND ORDER_NO = ' ' ";
			cmd_query_cr += " AND ORDER_NO = ' ' ";
			cmd_query_cr_xj += " AND ORDER_NO = ' ' ";
		}

		if (v_hold_time_from.ToDouble() > 0)
		{
			cmd_query_sm_count += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_sm += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_sm_xj += " AND HOLD_TIME < @sta_chg_time_from ";

			cmd_query_hr_count += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_hr += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_hr_xj += " AND HOLD_TIME < @sta_chg_time_from ";

			cmd_query_cr_count += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_cr += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd_query_cr_xj += " AND HOLD_TIME < @sta_chg_time_from ";

			cmd_count.Parameters.Set("sta_chg_time_from", BM2::CDateTime::Now().AddDays(-v_hold_time_from.ToDouble()).ToString("yyyyMMddHHmmss"));
			cmd.Parameters.Set("sta_chg_time_from", BM2::CDateTime::Now().AddDays(-v_hold_time_from.ToDouble()).ToString("yyyyMMddHHmmss"));
		}
		if (v_hold_time_to.ToDouble() > 0)
		{
			cmd_query_sm_count += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd_query_sm += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd_query_sm_xj += " AND HOLD_TIME > @sta_chg_time_to ";

			cmd_query_hr_count += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd_query_hr += " AND HOLD_TIME > @sta_chg_time_to ";;
			cmd_query_hr_xj += " AND HOLD_TIME > @sta_chg_time_to ";

			cmd_query_cr_count += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd_query_cr += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd_query_cr_xj += " AND HOLD_TIME > @sta_chg_time_to ";

			cmd_count.Parameters.Set("sta_chg_time_to", BM2::CDateTime::Now().AddDays(-v_hold_time_to.ToDouble()).ToString("yyyyMMddHHmmss"));
			cmd.Parameters.Set("sta_chg_time_to", BM2::CDateTime::Now().AddDays(-v_hold_time_to.ToDouble()).ToString("yyyyMMddHHmmss"));
		}

		int i = 0;
		for (loop_factory_div = 0; loop_factory_div < 10; loop_factory_div++)
		{
			Log::Info("", __FUNCTION__, "loop_factory_div=[{0}]", loop_factory_div);
			// 炼钢  板材炼钢
			//if (loop_factory_div == 0)//板坯
			if (loop_factory_div == 0 || loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//板坯
			{
				cmd_count.SetCommandText(cmd_query_sm_count);
				Log::Info("", __FUNCTION__, "DestSql=[{0}],dest=[{1}],loop_factory_div=[{2}]",
					cmd_query_sm_count, v_factory_div[loop_factory_div], loop_factory_div);
			}
			//热轧		一热轧                  二热轧                  三热轧                  北营热轧
			//else if (loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//热卷
			else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7 || loop_factory_div == 8)//热卷
			{
				cmd_count.SetCommandText(cmd_query_hr_count);
				Log::Info("", __FUNCTION__, "DestSql=[{0}],dest=[{1}],loop_factory_div=[{2}]",
					cmd_query_hr_count, v_factory_div[loop_factory_div], loop_factory_div);
			}
			//冷轧	    一冷轧                  二冷轧                  三冷轧        
			//else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7)//冷卷
			else if (loop_factory_div == 9)//冷卷
			{
				//if (loop_factory_div == 5)
				//{
				//	cmd_query_cr_count1 = cmd_query_cr_count;
				//	cmd_query_cr1 = cmd_query_cr;
				//	cmd_query_cr_xj1 = cmd_query_cr_xj;

				//	cmd_query_cr_count += " AND FACTORY_STORE IN ('L51','L52') ";
				//	cmd_query_cr += " AND FACTORY_STORE IN ('L51','L52') ";
				//	cmd_query_cr_xj += " AND FACTORY_STORE IN ('L51','L52') ";

				//	Log::Trace("", __FUNCTION__, "loop_factory_div[5].....cmd_query_cr = [{0}]", cmd_query_cr);
				//}
				//if (loop_factory_div == 7)
				//{
				//	cmd_query_cr_count = cmd_query_cr_count1;
				//	cmd_query_cr = cmd_query_cr1;
				//	cmd_query_cr_xj = cmd_query_cr_xj1;

				//	cmd_query_cr_count += " AND FACTORY_STORE ='L54' ";
				//	cmd_query_cr += " AND FACTORY_STORE ='L54' ";
				//	cmd_query_cr_xj += " AND FACTORY_STORE ='L54' ";

				//	Log::Trace("", __FUNCTION__, "loop_factory_div[7].....cmd_query_cr = [{0}]", cmd_query_cr);
				//}

				cmd_count.SetCommandText(cmd_query_cr_count);
				Log::Info("", __FUNCTION__, "DestSql=[{0}],dest=[{1}],loop_factory_div=[{2}]",
					cmd_query_cr_count, v_factory_div[loop_factory_div], loop_factory_div);
			}

			//if (loop_factory_div != 5 || loop_factory_div != 6 || loop_factory_div != 7)
			//{
			//	Log::Info("", __FUNCTION__, " 251 v_factory_div=[{0}]", v_factory_div[loop_factory_div]);
			//	cmd_count.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
			//}

			Log::Info("", __FUNCTION__, " 255 v_factory_div=[{0}]", v_factory_div[loop_factory_div]);
			cmd_count.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
			cmd_count.ExecuteReader();
			while (cmd_count.Read())
			{
				v_dest = cmd_count.GetString(1);
				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[i][0] = v_factory_div_desc[loop_factory_div * 2];
				bcls_ret->Tables[0].Rows[i][1] = v_factory_div_desc[loop_factory_div * 2 + 1];
				bcls_ret->Tables[0].Rows[i][2] = v_dest;

				//if (loop_factory_div != 5 || loop_factory_div != 6 || loop_factory_div != 7)
				//{
				//	cmd.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
				//}
				cmd.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
				cmd.Parameters.Set("dest", v_dest);
				for (loop_mat_status = 0; loop_mat_status < 7; loop_mat_status++)
				{
					if (loop_mat_status == 2 || loop_mat_status == 3)
					{

						// 炼钢  板材炼钢
						//if (loop_factory_div == 0)//板坯
						if (loop_factory_div == 0 || loop_factory_div == 1 || loop_factory_div == 2)//板坯
						{
							cmd_query = cmd_query_sm;
						}
						//热轧		一热轧                  二热轧                  三热轧                  北营热轧
						//else if (loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//热卷
						else if (loop_factory_div == 3 || loop_factory_div == 4 || loop_factory_div == 5 || loop_factory_div == 6)//热卷
						{
							//cmd_query = cmd_query_hr + " AND HSF_UNIT_REPAIR = @dest ";//热卷
							cmd_query = cmd_query_hr + " AND REPAIR_FLAG = '1' ";//热卷
						}
						//冷轧	    一冷轧                  二冷轧                  三冷轧        
						//else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7)//冷卷
						else if (loop_factory_div == 7)//冷卷
						{
							//cmd_query = cmd_query_cr + " AND REPAIR_UNIT_CODE = @dest ";//冷卷
							cmd_query = cmd_query_cr + " AND REPAIR_FLAG = '1' ";//冷卷
						}
					}
					else
					{

						// 炼钢  板材炼钢
						//if (loop_factory_div == 0)//板坯
						if (loop_factory_div == 0 || loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//板坯
						{
							//cmd_query = cmd_query_sm;
							cmd_query = cmd_query_sm + " AND FACTORY_STORE = '" + v_factory_div[loop_factory_div] + "'";
						}
						//热轧		一热轧                  二热轧                  三热轧                  北营热轧
						//else if (loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//热卷
						else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7 || loop_factory_div == 8)//热卷
						{
							cmd_query = cmd_query_hr + " AND NEXT_UNIT_CODE = @dest ";//热卷
						}
						//冷轧	    一冷轧                  二冷轧                  三冷轧        
						//else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7)//冷卷
						else if (loop_factory_div == 9)//冷卷
						{
							cmd_query = cmd_query_cr + " AND NEXT_UNIT_CODE = @dest "; // 冷卷
						}
					}
					Log::Info("", __FUNCTION__, "factory_div=[{0}];mat_kind=[{1}];v_dest=[{2}];MAT_STATUS=[{3}]", v_factory_div_desc[loop_factory_div * 2],
						v_factory_div_desc[loop_factory_div * 2 + 1], v_dest, v_mat_status[loop_mat_status]);

					cmd.SetCommandText(cmd_query);

					if (loop_mat_status != 4)
					{
						cmd.Parameters.Set("mat_status", v_mat_status[loop_mat_status]);
						cmd.ExecuteReader();
						if (cmd.Read())
						{
							v_mat_act_wt[i][loop_mat_status + 5] = cmd.GetDecimal(1).Floor();
							bcls_ret->Tables[0].Rows[i][loop_mat_status + 5] = v_mat_act_wt[i][loop_mat_status + 5];
						}
						cmd.Close();
					}
				}

				v_mat_act_wt[i][4] = v_mat_act_wt[i][5] + v_mat_act_wt[i][6] + v_mat_act_wt[i][7] + v_mat_act_wt[i][8];//在制品小计
				v_mat_act_wt[i][9] = v_mat_act_wt[i][10] + v_mat_act_wt[i][11];//成品小计
				v_mat_act_wt[i][3] = v_mat_act_wt[i][4] + v_mat_act_wt[i][9];//总计

				bcls_ret->Tables[0].Rows[i][4] = v_mat_act_wt[i][4];
				bcls_ret->Tables[0].Rows[i][9] = v_mat_act_wt[i][9];
				bcls_ret->Tables[0].Rows[i][3] = v_mat_act_wt[i][3];

				i++;
			}
			cmd_count.Close();


			//小计行
			bcls_ret->Tables[0].Rows.Add();//增加小计行
			bcls_ret->Tables[0].Rows[i][2] = "小计";

			bcls_ret->Tables[0].Rows[i][0] = v_factory_div_desc[loop_factory_div * 2];
			bcls_ret->Tables[0].Rows[i][1] = v_factory_div_desc[loop_factory_div * 2 + 1];

			//if (loop_factory_div == 0)//板坯
			if (loop_factory_div == 0 || loop_factory_div == 1 || loop_factory_div == 2 || loop_factory_div == 3 || loop_factory_div == 4)//板坯
			{
				cmd.SetCommandText(cmd_query_sm_xj);
			}
			//else if (loop_factory_div == 1 || loop_factory_div == 2)//热卷
			else if (loop_factory_div == 5 || loop_factory_div == 6 || loop_factory_div == 7 || loop_factory_div == 8)//热卷
			{
				cmd.SetCommandText(cmd_query_hr_xj);
			}
			//else if (loop_factory_div == 3 || loop_factory_div == 4)//冷卷
			else if (loop_factory_div == 9)//冷卷
			{
				cmd.SetCommandText(cmd_query_cr_xj);
			}

			//if (loop_factory_div != 5 || loop_factory_div != 6 || loop_factory_div != 7)
			//{
				//cmd.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
			//}

			cmd.Parameters.Set("factory_div", v_factory_div[loop_factory_div]);
			for (loop_mat_status = 0; loop_mat_status < 7; loop_mat_status++)
			{
				if (loop_mat_status != 4)
				{
					cmd.Parameters.Set("mat_status", v_mat_status[loop_mat_status]);
					cmd.ExecuteReader();
					if (cmd.Read())
					{
						v_mat_act_wt[i][loop_mat_status + 5] = cmd.GetDecimal(1).Floor();//状态单列小计
						bcls_ret->Tables[0].Rows[i][loop_mat_status + 5] = v_mat_act_wt[i][loop_mat_status + 5];
					}
					cmd.Close();
				}
			}

			v_mat_act_wt[i][4] = v_mat_act_wt[i][5] + v_mat_act_wt[i][6] + v_mat_act_wt[i][7] + v_mat_act_wt[i][8];//在制品小计
			v_mat_act_wt[i][9] = v_mat_act_wt[i][10] + v_mat_act_wt[i][11];//成品小计
			v_mat_act_wt[i][3] = v_mat_act_wt[i][4] + v_mat_act_wt[i][9];//总计

			bcls_ret->Tables[0].Rows[i][4] = v_mat_act_wt[i][4];
			bcls_ret->Tables[0].Rows[i][9] = v_mat_act_wt[i][9];
			bcls_ret->Tables[0].Rows[i][3] = v_mat_act_wt[i][3];

			i++;
		}


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
