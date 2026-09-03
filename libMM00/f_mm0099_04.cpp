/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-07-13 16:01:36
Description: 通用物料状态计算
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_mm0099_04(CDynaTable * matData, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString	datetime("");
	CString packTypeCode = "";

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		Log::Info("", __FUNCTION__, "材料状态 MAT_KIND = [{0}]", matData->GetColValString("MAT_KIND"));
		Log::Info("", __FUNCTION__, "材料状态 COLD_HOT_FLAG = [{0}]", matData->GetColValString("COLD_HOT_FLAG"));
		Log::Info("", __FUNCTION__, "材料状态 PRODUCT_FLAG = [{0}]", matData->GetColValString("PRODUCT_FLAG"));
		Log::Info("", __FUNCTION__, "材料状态 HOLD_FLAG = [{0}]", matData->GetColValString("HOLD_FLAG"));
		Log::Info("", __FUNCTION__, "材料状态 TRANSFER_FLAG = [{0}]", matData->GetColValString("TRANSFER_FLAG"));
		Log::Info("", __FUNCTION__, "材料状态 CONFM_FLAG = [{0}]", matData->GetColValString("CONFM_FLAG"));
		Log::Info("", __FUNCTION__, "材料状态 APP_DECIDE_FLAG = [{0}]", matData->GetColValString("APP_DECIDE_FLAG"));

		if (matData->GetColValString("MAT_KIND") == "HR" &&
			matData->GetColValString("COLD_HOT_FLAG") != "0" && matData->GetColValString("COLD_HOT_FLAG") != "1")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的冷热标志[" +
				matData->GetColValString("COLD_HOT_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (matData->GetColValString("PRODUCT_FLAG") != "0" && matData->GetColValString("PRODUCT_FLAG") != "1"
			&& matData->GetColValString("PRODUCT_FLAG").Trim() != "")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的成品标记[" +
				matData->GetColValString("PRODUCT_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (matData->GetColValString("HOLD_FLAG") != "0" && matData->GetColValString("HOLD_FLAG") != "1" &&
			matData->GetColValString("HOLD_FLAG") != "2" && matData->GetColValString("HOLD_FLAG") != "3")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的封锁标记[" +
				matData->GetColValString("HOLD_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (matData->GetColValString("TRANSFER_FLAG") != "0" && matData->GetColValString("TRANSFER_FLAG") != "1" &&
			matData->GetColValString("TRANSFER_FLAG") != "2")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的转库计划标记[" +
				matData->GetColValString("TRANSFER_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (matData->GetColValString("CONFM_FLAG") != "0" && matData->GetColValString("CONFM_FLAG") != "1" &&
			matData->GetColValString("CONFM_FLAG") != "2")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的准发标记[" +
				matData->GetColValString("CONFM_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (matData->GetColValString("APP_DECIDE_FLAG") != "0" && matData->GetColValString("APP_DECIDE_FLAG") != "1" &&
			matData->GetColValString("APP_DECIDE_FLAG") != "2")
		{
			strcpy(s.msg, "材料[" + matData->GetColValString("MAT_NO") + "]的现货申报标记[" +
				matData->GetColValString("APP_DECIDE_FLAG") + "]错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* 设置材料状态码 MAT_STATUS */

		if (matData->GetColValString("PRODUCT_FLAG") == "0")
		{
			//在制品
			if (matData->GetColValString("HOLD_FLAG") == "1")
			{
				//管理封闭 
				matData->SetColVal("MAT_STATUS", "21");
				//设置材料状态修改时刻
				matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);

			}
			else if (matData->GetColValString("HOLD_FLAG") == "2" || matData->GetColValString("HOLD_FLAG") == "3")					//质量封闭
			{
				matData->SetColVal("MAT_STATUS", "22");
				//设置材料状态修改时刻
				matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
			}
			else if (matData->GetColValString("HOLD_FLAG") == "0")
			{
				//不封闭
				if (matData->GetColValString("MAT_KIND") == "HP")
				{
					CDynaTable tmmhp03("TMMHP03", conn);
					tmmhp03.SetFilterColVal("MAT_NO", matData->GetColValString("MAT_NO"));
					tmmhp03.SetFilterColVal("SUB_BACKLOG_SEQ", matData->GetColValDecimal("SUB_BACKLOG_SEQ"));
					tmmhp03.Query();

					//CDynaTable tsi0011("TSI0011", conn);
					//tsi0011.SetFilterColVal("SUB_BACKLOG_CODE", matData->GetColValString("SUB_BACKLOG_CODE"));
					//tsi0011.Query();

					if (tmmhp03.GetColValString("BACKLOG_DECIDE_CODE") == "E" || tmmhp03.GetColValString("BACKLOG_DECIDE_CODE") == "0")
					{
						//0-未判,E-产出待判
						matData->SetColVal("MAT_STATUS", "20");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
					}
					else if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")					//有转库计划或有作业计划号
					{
						matData->SetColVal("MAT_STATUS", "24");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
					}
					else
					{
						if (matData->GetColValString("ORDER_NO").Trim() != "")
						{
							//有合同号
							matData->SetColVal("MAT_STATUS", "23");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
						else
						{
							//无合同号
							matData->SetColVal("MAT_STATUS", "29");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
					}
				}
				else if (matData->GetColValString("MAT_KIND") == "SM")
				{
					if (matData->GetColValString("HOT_CHARGE_FLAG") == "2") //2-DHCR直接热装  不判断钢种
					{
						//热装热送
						if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")					//有转库计划或有作业计划号
						{
							matData->SetColVal("MAT_STATUS", "24");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
						else
						{
							if (matData->GetColValString("ORDER_NO").Trim() != "")
							{
								//有合同号
								matData->SetColVal("MAT_STATUS", "23");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else
							{
								//无合同号
								matData->SetColVal("MAT_STATUS", "29");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
						}
					}
					else	//非热装热送
					{
						if (matData->GetColValString("MAT_ORIGIN") != "1" && (matData->GetColValString("FIN_ST_NO").Trim() == "" || matData->GetColValString("COMPLEX_DECIDE_CODE").Trim() == "" ||
							matData->GetColValString("COMPLEX_DECIDE_CODE") == "0" ||
							matData->GetColValString("COMPLEX_DECIDE_CODE") == "A" ||
							matData->GetColValString("COMPLEX_DECIDE_CODE") == "2"))
						{
							//材料来源是1-外购料,则外购料录入时没有出钢记号和最终出钢记号
							// 因此判定非外购料且最终出钢记号为空材料状态=20,李婧昊修改2013-09-18
							matData->SetColVal("MAT_STATUS", "20");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
						else if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")					//有转库计划或有作业计划号
						{
							matData->SetColVal("MAT_STATUS", "24");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
						else
						{
							if (matData->GetColValString("ORDER_NO").Trim() != "")
							{
								//有合同号
								matData->SetColVal("MAT_STATUS", "23");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else
							{
								//无合同号
								matData->SetColVal("MAT_STATUS", "29");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
						}
					}
				}
				else if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")					//有转库计划或有作业计划号
				{
					matData->SetColVal("MAT_STATUS", "24");
					//设置材料状态修改时刻
					matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
				}
				else
				{
					//无转库计划并无作业计划号
					if (matData->GetColValString("MAT_KIND") == "HR" && matData->GetColValString("COLD_HOT_FLAG") != "0")
					{
						//热轧未放冷
						matData->SetColVal("MAT_STATUS", "20");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
					}
					else if (matData->GetColValString("MAT_KIND") != "BW" && matData->GetColValString("SURFACE_DECIDE_CODE") != "1")
					{
						//表面判定代码非判合格
						matData->SetColVal("MAT_STATUS", "20");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
					}
					else
					{
						if (matData->GetColValString("ORDER_NO").Trim() != "")
						{
							//有合同号
							matData->SetColVal("MAT_STATUS", "23");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
						else
						{
							//无合同号
							matData->SetColVal("MAT_STATUS", "29");
							//设置材料状态修改时刻
							matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						}
					}
				}
			}
		}
		else if (matData->GetColValString("PRODUCT_FLAG") == "1")
		{
			//成品
			if (matData->GetColValString("HOLD_FLAG") == "1")
			{
				//管理封闭 
				matData->SetColVal("MAT_STATUS", "31");
				//设置材料状态修改时刻
				matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
			}
			else if (matData->GetColValString("HOLD_FLAG") == "2" || matData->GetColValString("HOLD_FLAG") == "3")
			{
				//质量封闭
				matData->SetColVal("MAT_STATUS", "32");
				//设置材料状态修改时刻
				matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
			}
			else if (matData->GetColValString("HOLD_FLAG") == "0")
			{
				//不封闭
				if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")
				{
					//有转库计划或有作业计划号
					matData->SetColVal("MAT_STATUS", "34");
					//设置材料状态修改时刻
					matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
				}
				else
				{
					//无转库计划并无作业计划号
					if (matData->GetColValString("MAT_KIND") == "HR" && matData->GetColValString("COLD_HOT_FLAG") != "0")
					{
						Log::Info("", __FUNCTION__, "材料状态 热轧未放冷 = [{0}]", matData->GetColValString("MAT_STATUS"));
						//热轧未放冷
						matData->SetColVal("MAT_STATUS", "30");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
					}
					else if (matData->GetColValString("COMPLEX_DECIDE_CODE").Trim() == "" ||
						matData->GetColValString("COMPLEX_DECIDE_CODE") == "0" ||
						matData->GetColValString("COMPLEX_DECIDE_CODE") == "A" ||
						matData->GetColValString("COMPLEX_DECIDE_CODE") == "2")
					{
						//未判定、待人工处置、不合格
						matData->SetColVal("MAT_STATUS", "30");
						//设置材料状态修改时刻
						matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
						Log::Info("", __FUNCTION__, "材料状态 不合格 = [{0}]", matData->GetColValString("MAT_STATUS"));
					}
					else
					{
						if (matData->GetColValString("ORDER_NO").Trim() != "")
						{
							//有合同号

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
							if (matData->GetColValString("PRODUCT_PACK_FLAG") != "1" && 
								(matData->GetColValString("MAT_KIND") == "CR" || matData->GetColValString("MAT_KIND") == "BW"))
							{
								//查询合同包装代码
								CDbCommand cmd_inq(conn);
								sqlstr = "SELECT pack_type_code FROM TOM01 WHERE order_no = '" + matData->GetColValString("ORDER_NO") + "'";
								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									packTypeCode = cmd_inq.GetString(1).Trim();
								}
								cmd_inq.Close();
							}

#endif
							Log::Info("", __FUNCTION__, "材料状态 材料未包装 packTypeCode= [{0}]", packTypeCode);
							//材料未包装， 但合同要求包装
							if (matData->GetColValString("PRODUCT_PACK_FLAG") != "1" &&	packTypeCode.Trim() != "")//孙羽田提出，packTypeCode为0也需要包装。
							{
								Log::Info("", __FUNCTION__, "材料状态 材料未包装 PRODUCT_PACK_FLAG= [{0}]", matData->GetColValString("PRODUCT_PACK_FLAG"));
								matData->SetColVal("MAT_STATUS", "30");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else	//材料已包装， 或合同未要求包装
							{
								if (matData->GetColValString("CONFM_FLAG") == "0")
								{
									//无准发计划
									matData->SetColVal("MAT_STATUS", "33");
									//设置材料状态修改时刻
									matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
								}
								else if (matData->GetColValString("CONFM_FLAG") == "1")
								{
									//编入准发计划
									matData->SetColVal("MAT_STATUS", "34");
									//设置材料状态修改时刻
									matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
								}
								else if (matData->GetColValString("CONFM_FLAG") == "2")
								{
									//准发计划确认
									matData->SetColVal("MAT_STATUS", "36");
									//设置材料状态修改时刻
									matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
								}
							}
						}
						else
						{
							//无合同号
							if (matData->GetColValString("APP_DECIDE_FLAG") == "0")
							{
								//无现货申报
								matData->SetColVal("MAT_STATUS", "39");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else if (matData->GetColValString("APP_DECIDE_FLAG") == "1")
							{
								//编入现货申报
								matData->SetColVal("MAT_STATUS", "34");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else if (matData->GetColValString("APP_DECIDE_FLAG") == "2")
							{
								//现货申报确认
								matData->SetColVal("MAT_STATUS", "38");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
							else if (matData->GetColValString("APP_DECIDE_FLAG") == "4")
							{
								//现货材料确认(西王新增)
								matData->SetColVal("MAT_STATUS", "36");
								//设置材料状态修改时刻
								matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
							}
						}
					}
				}
			}
		}
		//太钢定制   余材的成品标记人工选择    mfj  20240131
		else if (matData->GetColValString("PRODUCT_FLAG").Trim() == "")
		{
			//余材产出待确认成品标记
			matData->SetColVal("MAT_STATUS", "2A");
			//设置材料状态修改时刻
			matData->SetColVal("MAT_STATUS_UPDATE_TIME", datetime);
		}

		/* 设置定制材料状态码 CUST_MAT_STATUS */
		/*-------------------------------------------梅钢定制开始----------------------------------------*/
		if (matData->GetColValString("PRODUCT_FLAG") == "0")//0:在制品
		{
			//梅钢定制
			//Log::Info("", __FUNCTION__, "HOLD_FLAG = [{0}]", matData->GetColValString("HOLD_FLAG"));
			if (matData->GetColValString("HOLD_FLAG") != "0")//0:释放
			{
				matData->SetColVal("CUST_MAT_STATUS", "01");//01:待检验
				matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
			}
			else
			{
				Log::Info("", __FUNCTION__, "SURFACE_DECIDE_CODE = [{0}]", matData->GetColValString("SURFACE_DECIDE_CODE"));
				if (matData->GetColValString("SURFACE_DECIDE_CODE") != "1")//1:合格
				{
					matData->SetColVal("CUST_MAT_STATUS", "01");//01:待检验
					matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
				}
				else
				{
					//Log::Info("", __FUNCTION__, "MAT_DESTION = [{0}]", matData->GetColValString("MAT_DESTION"));
					//Log::Info("", __FUNCTION__, "ORDER_NO = [{0}]", matData->GetColValString("ORDER_NO"));
					//板坯去向（00：1热轧，01：2热轧）
					if (matData->GetColValString("MAT_DESTION") == "00" || matData->GetColValString("MAT_DESTION") == "01")
					{
						if (matData->GetColValString("ORDER_NO").Trim() == "")
						{
							matData->SetColVal("CUST_MAT_STATUS", "02");//02:待内供
							matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
						}
						else
						{
							matData->SetColVal("CUST_MAT_STATUS", "03");//03:待准发
							matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
						}
					}
				}
			}
			//转库计划
			//Log::Info("", __FUNCTION__, "TRANSFER_PLAN_NO = [{0}]", matData->GetColValString("TRANSFER_PLAN_NO"));
			//Log::Info("", __FUNCTION__, "TRANSFER_FLAG = [{0}]", matData->GetColValString("TRANSFER_FLAG"));
			if (matData->GetColValString("TRANSFER_PLAN_NO").Trim() != "" && matData->GetColValString("TRANSFER_BILL_NO").Trim() != "")
			{
				if (matData->GetColValString("TRANSFER_FLAG") == "1")
				{
					matData->SetColVal("CUST_MAT_STATUS", "07");//07:待转库
					matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
				}
				else if (matData->GetColValString("TRANSFER_FLAG") == "2")
				{
					matData->SetColVal("CUST_MAT_STATUS", "06");//06:已出厂
					matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
				}
			}
			//轧制计划
			//Log::Info("", __FUNCTION__, "ROLL_PLAN_NO = [{0}]", matData->GetColValString("ROLL_PLAN_NO"));
			//Log::Info("", __FUNCTION__, "IN_PLAN_FLAG = [{0}]", matData->GetColValString("IN_PLAN_FLAG"));
			if (matData->GetColValString("ROLL_PLAN_NO").Trim() != "")
			{
				if (matData->GetColValString("IN_PLAN_FLAG") == "1")
				{
					matData->SetColVal("CUST_MAT_STATUS", "09");//09:轧制计划中
					matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);
				}
			}
			/*-------------------------------------------梅钢定制结束----------------------------------------*/
			
			////在制品
			//if (matData->GetColValString("HOLD_FLAG") == "1")
			//{
			//	//L4封闭 
			//	matData->SetColVal("CUST_MAT_STATUS", "01");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("HOLD_FLAG") == "2" || matData->GetColValString("HOLD_FLAG") == "3")
			//{
			//	//L3封闭
			//	matData->SetColVal("CUST_MAT_STATUS", "02");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("HOLD_FLAG") == "0")
			//{
			//	//不封闭
			//	if (matData->GetColValString("TRANSFER_FLAG") != "0" || matData->GetColValString("PLAN_NO").Trim() != "")
			//	{
			//		//有转库计划或有作业计划号
			//		if (matData->GetColValString("REPAIR_FLAG") == "1")
			//		{
			//			//编入返修计划
			//			matData->SetColVal("CUST_MAT_STATUS", "13");
			//		}
			//		else if (matData->GetColValString("IN_PLAN_FLAG") == "1")
			//		{
			//			//编入预计划
			//			matData->SetColVal("CUST_MAT_STATUS", "11");
			//		}
			//		else
			//		{
			//			//编入终计划
			//			matData->SetColVal("CUST_MAT_STATUS", "12");
			//		}
			//	}
			//	else
			//	{
			//		//无转库计划并无作业计划号
			//		if (matData->GetColValString("COLD_HOT_FLAG") == "1")
			//		{
			//			//待冷
			//			matData->SetColVal("CUST_MAT_STATUS", "00");
			//		}
			//		else
			//		{
			//			if (matData->GetColValString("ORDER_NO").Trim() != "")
			//			{
			//				//有合同号
			//				matData->SetColVal("CUST_MAT_STATUS", "10");
			//			}
			//			else if (matData->GetColValString("APP_DECIDE_FLAG") != "0")
			//			{
			//				//现货申报
			//				matData->SetColVal("CUST_MAT_STATUS", "19");
			//			}
			//			else
			//			{
			//				matData->SetColVal("CUST_MAT_STATUS", "09");
			//			}
			//		}
			//	}
			//}
		}
		else if (matData->GetColValString("PRODUCT_FLAG") == "1")
		{
			////成品
			//if (matData->GetColValString("HOLD_FLAG") == "1")
			//{
			//	//L4封闭 
			//	matData->SetColVal("CUST_MAT_STATUS", "21");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("HOLD_FLAG") == "2" || matData->GetColValString("HOLD_FLAG") == "3")
			//{
			//	//L3封闭
			//	matData->SetColVal("CUST_MAT_STATUS", "22");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("ORDER_NO") == "")
			//{
			//	//无合同
			//	if (matData->GetColValString("APP_DECIDE_FLAG") != "0")
			//	{
			//		matData->SetColVal("CUST_MAT_STATUS", "29");
			//		//设置定制材料状态修改时刻
			//		matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//	}
			//	else
			//	{
			//		//现货申报
			//		matData->SetColVal("CUST_MAT_STATUS", "39");
			//		//设置定制材料状态修改时刻
			//		matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//	}
			//}
			//else if (matData->GetColValString("COMPLEX_DECIDE_CODE") != "1" && matData->GetColValString("PRODUCT_PACK_FLAG") != "1")
			//{
			//	//产品产出(未包装，未判定)
			//	matData->SetColVal("CUST_MAT_STATUS", "20");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("COMPLEX_DECIDE_CODE") != "1" && matData->GetColValString("PRODUCT_PACK_FLAG") == "1")
			//{
			//	//产品已包装，未判定
			//	matData->SetColVal("CUST_MAT_STATUS", "23");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("COMPLEX_DECIDE_CODE") == "1" && matData->GetColValString("PRODUCT_PACK_FLAG") != "1")
			//{
			//	//产品未包装，已判定
			//	matData->SetColVal("CUST_MAT_STATUS", "24");
			//	//设置定制材料状态修改时刻
			//	matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//}
			//else if (matData->GetColValString("COMPLEX_DECIDE_CODE") == "1" && matData->GetColValString("PRODUCT_PACK_FLAG") == "1")
			//{
			//	//产品已包装，已判定
			//	if (matData->GetColValString("CONFM_FLAG") == "0")
			//	{
			//		//未准发
			//		matData->SetColVal("CUST_MAT_STATUS", "25");
			//		//设置定制材料状态修改时刻
			//		matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//	}
			//	else if (matData->GetColValString("CONFM_FLAG") == "1")
			//	{
			//		//编入准发计划
			//		matData->SetColVal("CUST_MAT_STATUS", "26");
			//		//设置定制材料状态修改时刻
			//		matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//	}
			//	else if (matData->GetColValString("CONFM_FLAG") == "2")
			//	{
			//		//准发确认
			//		matData->SetColVal("CUST_MAT_STATUS", "27");
			//		//设置定制材料状态修改时刻
			//		matData->SetColVal("CUST_MAT_STATUS_UPDATE_TIME", datetime);

			//	}
			//}
		}
		Log::Info("", __FUNCTION__, "材料状态 CUST_MAT_STATUS = [{0}]", matData->GetColValString("CUST_MAT_STATUS"));
		Log::Info("", __FUNCTION__, "材料状态 MAT_STATUS = [{0}]", matData->GetColValString("MAT_STATUS"));

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


