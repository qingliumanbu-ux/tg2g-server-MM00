/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      KYE110
Version:     1.0
Date:        2018-04-25 16:55:33
Description: 板坯热卷按流向厂别汇总
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/

#include "stdafx.h"
//#include "tmmhrma01.h"  //业务头
//#include "tmmsmma01.h"
#include "mmhrma_common.h"

//int f_mmhrma_hstock(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 热轧当前库存汇总
/// <para>根据查询条件查询热轧物料主档TMMHRMA01</para>
/// </summary>
/// <returns>满足查询条件的热轧物料主档TMMHRMA01的总量数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mm00su47a1_q)


int f_mm00su47a1_q(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	//int i = 0;
	//  int j = 0;
	CString sqlstr = "";
	/* 实体类定义 */
	//CTMMHRMA01 tmmhrma01(conn);
	//CTMMSMMA01 tmmsmma01(conn);
	CModel tmmhr01("TMMHR01");
	CModel tmmsm01("TMMSM01");
	try
	{
		CDbCommand cmdcinq(conn);
		CDbCommand cmdhinq(conn);
		CString strSql1 = "";
		CString strSql2 = "";
		CString str = "";
		int product_row = 17;	//成品行号
		int order_type = 0;

		//有委托条件
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_TYPE"))
		{
			order_type = bcls_rec->Tables[0].Rows[0]["ORDER_TYPE"];
			switch (order_type)
			{
			case 1:
				str = " and order_no > ' ' ";
				break;
			case 2:
				str = " and order_no <= ' ' ";
				break;
			}
		}

		const int row_count = 23; /*画面总行数*/
		CString v_table_row_name[row_count][4] = {
			/*第一列是行号，第二，三列画面显示内容，第四列是程序判断用的机组号*/
			//2018-6-21:H031 CSP轧机，H032 2250轧机，H033 1580轧机
			//2018-6-21:2250直发A6现改为1700直发D1、2250直发D2、1580直发D3、CSP直发D4
			{ "0", "板坯", "H031", "00" },
			{ "1", "", "H032", "01" },
			{ "2", "", "H033", "02" },
			{ "3", "", "外供", "" },
			{ "4", "", "小计", "" },
			{ "5", "", "其中无委托", "" },
			{ "6", "热卷(在制)", "H040", "H040" },
			{ "7", "", "H041", "H041" },
			{ "8", "", "H042", "H042" },
			{ "9", "", "H069", "H069" },
			{ "10", "", "H051", "H051" },
			{ "11", "", "CSP直发", "D1" },
			{ "12", "", "2250直发", "D2" },
			{ "13", "", "1580直发", "D3" },
			{ "14", "", "小计", "" },
			{ "15", "", "其中无委托", "" },
			{ "16", "", "其中返回卷", "" },
			{ "17", "成品", "有委托", "" },
			{ "18", "", "无委托", "" },
			{ "19", "", "报现货", "" },
			{ "20", "", "小计", "" },
			{ "21", "中间坯", "小计", "" },
			{ "22", "合计", "", "" }
		};
		const int col_count = 16; /*画面总列数*/
		CString v_table_col_name[col_count][3] =
		{
			//2018-6-20将X01改成H21 H22 H23 H25;将S6 S7合并为H10，FXX自用 FXX外购不用了
			//EPED54的MMHRMA47A1_Q3按照以下来定
			{ "0", "", "MAT_TYPE" },
			{ "1", "", "NEXT_UNIT_CODE" },
			{ "2", "", "SUM_WT" },				//总重量
			{ "3", "", "NO_ORDER" },			//无委托（其中）
			{ "4", "", "DUMMY_COIL" },	        //返回卷（其中）
			//{ "5", "", "SUM_NUM" },			    //总数
			{ "5", "H11", "H11" },              //CSP热轧成品库 
			{ "6", "H21", "H21" },              //2250后库
			{ "7", "H22", "H22" },              //横切成品库 
			{ "8", "H31", "H31" },              //1580后库
			{ "9", "S41", "S41" },	        	//2250板坯库
			{ "10", "S42", "S42" },	        	//1580板坯库
			{ "11", "S43", "S43" },	        	//板坯清理前库
			{ "12", "S45", "S45" },	        	//未清理板坯1580前库
			{ "13", "S4B", "S4B" },	        	//线外板坯库
			{ "14", "", "STOCK_99" },			//在途
			{ "15", "", "STOCK_COLD" }			//冷轧原料库
		};

		/*画面列标题赋值*/

		bcls_ret->Tables.Add();
		for (int i = 0; i < 2; i++)
		{
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "MAT_TYPE");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "SUM_WT");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "NO_ORDER");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "DUMMY_COIL");
			//bcls_ret->Tables[i].Columns.Add(DT_STRING, "SUM_NUM");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "H11");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "H21");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "H22");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "H31");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "S41");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "S42");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "S43");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "S45");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "S4B");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "STOCK_99");
			bcls_ret->Tables[i].Columns.Add(DT_STRING, "STOCK_COLD");
		}
		/*画面左边的行标题赋值*/
		for (int i = 0; i<row_count; i++)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[i]["MAT_TYPE"] = v_table_row_name[i][1];
			bcls_ret->Tables[0].Rows[i]["NEXT_UNIT_CODE"] = v_table_row_name[i][2];
			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[i]["MAT_TYPE"] = v_table_row_name[i][1];
			bcls_ret->Tables[1].Rows[i]["NEXT_UNIT_CODE"] = v_table_row_name[i][2];
		}

		int row_i = 0, col_j = 0;
		bool find_row;
		bool find_col;
		//计算结果存放的数组
		CDecimal table_result[row_count][col_count];
		CDecimal table_count[row_count][col_count];
		//赋初值
		for (int i = 0; i<row_count; i++)
		{
			for (int j = 0; j<col_count; j++)
			{
				table_result[i][j] = 0;
				table_count[i][j] = 0;
			}
		}

		Log::Trace("", __FUNCTION__, "**************** 板坯库 ****************");
		strSql1 = " SELECT	NEXT_UNIT_CODE "
			",ORDER_NO "
			",STOCK_NO "
			",FACTORY_DIV "
			",MAT_ACT_WT "
			",APP_DECIDE_NO "
			",PRODUCT_FLAG "
			",DEST_FIN "
			",MAT_NO "
			",RAW_ORIGIN "  //20140529新增外购来源，判断F库区自用和外购的条件
			" FROM TMMSMMA01 WHERE 1=1 ";
		strSql1 += str;
		cmdhinq.SetCommandText(strSql1);
		sqlstr = strSql1;
		Log::Trace("", __FUNCTION__, "strSql1 = [{0}]", (const char*)sqlstr);
		cmdhinq.ExecuteReader();
		while (cmdhinq.Read())
		{
			find_row = false;
			find_col = false;
			tmmsmma01.Reset();
			tmmsmma01.NEXT_UNIT_CODE = cmdhinq.GetString(1).Trim();
			tmmsmma01.ORDER_NO = cmdhinq.GetString(2).Trim();
			tmmsmma01.STOCK_NO = cmdhinq.GetString(3).Trim();
			tmmsmma01.FACTORY_DIV = cmdhinq.GetString(4).Trim();
			tmmsmma01.MAT_ACT_WT = cmdhinq.GetDecimal(5);
			tmmsmma01.APP_DECIDE_NO = cmdhinq.GetString(6).Trim();
			tmmsmma01.PRODUCT_FLAG = cmdhinq.GetString(7).Trim();
			tmmsmma01.DEST_FIN = cmdhinq.GetString(8).Trim();
			tmmsmma01.MAT_NO = cmdhinq.GetString(9).Trim();
			tmmsmma01.RAW_ORIGIN = cmdhinq.GetString(10).Trim();
			if (tmmsmma01.DEST_FIN == "00")
			{
				row_i = 0;
				find_row = true;
			}
			if (tmmsmma01.DEST_FIN == "01")
			{
				row_i = 1;
				find_row = true;
			}
			if (tmmsmma01.DEST_FIN == "02")
			{
				row_i = 2;
				find_row = true;
			}
			else if (tmmsmma01.DEST_FIN == "20" || tmmsmma01.DEST_FIN == "09" || tmmsmma01.DEST_FIN == "10")	//外供
			{
				row_i = 3;
				find_row = true;
			}
			//如果记录不是要统计的信息跳过本循环取下一条数据
			if (!find_row)
			{
				continue;
			}
			if (tmmsmma01.STOCK_NO.GetLength() == 0)//在途（无库区的算做在途）
			{
				col_j = 14;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO == "S41") //2250板坯库
			{
				//板坯库
				col_j = 9;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO == "S42") //1580板坯库
			{
				//板坯库
				col_j = 10;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO == "S43") //板坯清理前库
			{
				//板坯库
				col_j = 11;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO == "S45") //未清理板坯1580前库
			{
				//板坯库
				col_j = 12;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO == "S4B") //线外板坯库
			{
				//板坯库
				col_j = 13;
				find_col = true;
			}
			else if (tmmsmma01.STOCK_NO.GetLength() > 2
				&& (tmmsmma01.STOCK_NO.Substring(1, 2) == "99" || tmmsmma01.STOCK_NO.Substring(1, 2) == "00")
				&& tmmsmma01.STOCK_NO != "S99")//在途
			{
				col_j = 14;
				find_col = true;
			}

			//如果记录不是要统计的信息跳过本循环取下一条数据
			if (!find_col)
			{
				continue;
			}
			//结果进行累计
			if (find_col && find_row)
			{
				table_result[row_i][col_j] = table_result[row_i][col_j] + tmmsmma01.MAT_ACT_WT;
				table_count[row_i][col_j] = table_count[row_i][col_j] + 1;
			}
			//无委托列
			if (tmmsmma01.ORDER_NO == "")
			{
				table_result[row_i][3] = table_result[row_i][3] + tmmsmma01.MAT_ACT_WT;
				table_count[row_i][3] = table_count[row_i][3] + 1;
			}
			//其中无委托行（5行）
			if (tmmsmma01.ORDER_NO == "")
			{
				table_result[5][col_j] = table_result[5][col_j] + tmmsmma01.MAT_ACT_WT;
				table_count[5][col_j] = table_count[5][col_j] + 1;
				//其中无委托行，无委托列
				if (tmmsmma01.ORDER_NO == "")
				{
					table_result[5][3] = table_result[5][3] + tmmsmma01.MAT_ACT_WT;
					table_count[5][3] = table_count[5][3] + 1;
				}
			}
		}
		cmdhinq.Close();

		Log::Trace("", __FUNCTION__, "**************** 热轧库 ****************");
		strSql1 = " SELECT	NEXT_UNIT_CODE "
			",ORDER_NO "
			",STORE_AREA "
			",FACTORY_STORE "
			",MAT_WT "
			",APP_DECIDE_NO "
			",PLATE_OR_COIL "
			",PRODUCT_FLAG "
			",DUMMY_COIL_FLAG "
			",MAT_ORIGIN_CODE "
			",MAT_STATUS "
			",PLAN_NO "
			",PRE_UNIT_CODE "
			",MAT_DESTION "
			",COMPLEX_DECIDE_CODE "
			" FROM TMMHRMA01 WHERE 1=1 ";
		strSql1 += str;
		strSql1 += "ORDER BY ORDER_NO";

		Log::Trace("", __FUNCTION__, "strSql1 = [{0}]", strSql1);
		cmdhinq.SetCommandText(strSql1);
		sqlstr = strSql1;
		cmdhinq.ExecuteReader();

		while (cmdhinq.Read())
		{
			tmmhrma01.Reset();
			tmmhrma01.NEXT_UNIT_CODE = cmdhinq.GetString(1).Trim();
			tmmhrma01.ORDER_NO = cmdhinq.GetString(2).Trim();
			tmmhrma01.STORE_AREA = cmdhinq.GetString(3).Trim();
			tmmhrma01.FACTORY_STORE = cmdhinq.GetString(4).Trim();
			tmmhrma01.MAT_WT = cmdhinq.GetDecimal(5);
			tmmhrma01.APP_DECIDE_NO = cmdhinq.GetString(6).Trim();
			tmmhrma01.PLATE_OR_COIL = cmdhinq.GetString(7).Trim();
			tmmhrma01.PRODUCT_FLAG = cmdhinq.GetString(8).Trim();
			tmmhrma01.DUMMY_COIL_FLAG = cmdhinq.GetString(9).Trim();
			tmmhrma01.MAT_ORIGIN_CODE = cmdhinq.GetString(10).Trim();
			tmmhrma01.MAT_STATUS = cmdhinq.GetString(11).Trim();
			tmmhrma01.PLAN_NO = cmdhinq.GetString(12).Trim();
			tmmhrma01.PRE_UNIT_CODE = cmdhinq.GetString(13).Trim();
			tmmhrma01.MAT_DESTION = cmdhinq.GetString(14).Trim();
			tmmhrma01.COMPLEX_DECIDE_CODE = cmdhinq.GetString(15).Trim(); //20150917新增中间坯判断标志（综合判定代码9）

			find_row = false;
			find_col = false;


			if (tmmhrma01.PRODUCT_FLAG == "0")		//热卷在制
			{
				//返回卷优先根据计划号判断，其次是产出机组
				if (tmmhrma01.DUMMY_COIL_FLAG == "1" || tmmhrma01.DUMMY_COIL_FLAG == "A")
				{
					if (tmmhrma01.PLAN_NO.GetLength() >= 4)
					{
						tmmhrma01.NEXT_UNIT_CODE = tmmhrma01.PLAN_NO.Substring(0, 4);
					}
					else
					{
						tmmhrma01.NEXT_UNIT_CODE = tmmhrma01.PRE_UNIT_CODE;
					}
				}
				if (tmmhrma01.NEXT_UNIT_CODE != "")
				{
					if (tmmhrma01.NEXT_UNIT_CODE.Substring(0, 1) == "H")
					{
						for (int i = 6; i < 11; i++)
						{
							Log::Trace("", __FUNCTION__, "tmmhrma01.NEXT_UNIT_CODE = [{0}],v_table_row_name[i][3]= [{1}]", tmmhrma01.NEXT_UNIT_CODE, v_table_row_name[i][3]);
							//机组号能直接确认的（6-10行）
							if (tmmhrma01.NEXT_UNIT_CODE == v_table_row_name[i][3])
							{
								if (tmmhrma01.NEXT_UNIT_CODE == "H040")
								{
									Log::Trace("", __FUNCTION__, "tmmhrma01.NEXT_UNIT_CODE = [{0}]", tmmhrma01.NEXT_UNIT_CODE);
									Log::Trace("", __FUNCTION__, "tmmhrma01.STORE_AREA = [{0}]", tmmhrma01.STORE_AREA);
								}
								row_i = i;
								find_row = true;
								break;
							}
						}
					}
				}
				if (tmmhrma01.MAT_DESTION.Trim() != "")
				{
					//2018-6-21:2250直发A6现改为1700直发D1、2250直发D2、1580直发D3、CSP直发D4
					if (tmmhrma01.MAT_DESTION.Substring(0, 2) == "P1" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "S1" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "T1")
					{
						row_i = 11;
						find_row = true;
					}
					if (tmmhrma01.MAT_DESTION.Substring(0, 2) == "P2" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "S2" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "T2")
					{
						row_i = 12;
						find_row = true;
					}
					if (tmmhrma01.MAT_DESTION.Substring(0, 2) == "P3" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "S3" ||
						tmmhrma01.MAT_DESTION.Substring(0, 2) == "T3")
					{
						row_i = 13;
						find_row = true;
					}
				}
			}
			else if (tmmhrma01.PRODUCT_FLAG == "1")	//成品(20151231号黄帮明提出把材态27的也统计进成品)
			{
				if (tmmhrma01.ORDER_NO > " ")		//成品有委托
				{
					row_i = product_row;
					find_row = true;
				}
				else if (tmmhrma01.APP_DECIDE_NO > " ")	//成品报现货
				{
					row_i = product_row + 2;
					find_row = true;
				}
				else
				{
					row_i = product_row + 1;
					find_row = true;                   //成品无委托
				}
			}

			//20170330中间坯单列，不分在制和成品，显示在17行，库区为X88
			//2018-6-20库区没有X88，中间坯单列对应的条件现还没定
			if (tmmhrma01.STORE_AREA == "X88")
			{
				row_i = product_row + 4;
				find_row = true;
			}

			//如果记录不是要统计的信息跳过本循环取下一条数据
			if (!find_row)
			{
				continue;
			}

			//获取列号&无直接判断库号
			if (tmmhrma01.STORE_AREA.GetLength() == 0)  //在途（无库号）
			{
				col_j = 14;
				find_col = true;
			}
			else if (tmmhrma01.STORE_AREA.GetLength() > 2
				&& (tmmhrma01.STORE_AREA.Substring(1, 2) == "99"
				|| tmmhrma01.STORE_AREA.Substring(1, 2) == "00"))//在途
			{
				col_j = 14;
				find_col = true;
			}
			//2018-6-20冷轧原料库的STORE_AREA不是H，现还未定
			if (tmmhrma01.STORE_AREA.GetLength()>0)
			{
				if (tmmhrma01.STORE_AREA.Substring(0, 1) == "C" || tmmhrma01.STORE_AREA.Substring(0, 1) == "Q"/*
																											  && (tmmhrma01.STORE_AREA == "H01" || tmmhrma01.STORE_AREA == "H11")*/)		//冷轧原料库(只统计冷轧原料前库的)
				{
					Log::Trace("", __FUNCTION__, "tmmhrma01.STORE_AREA = [{0}]", tmmhrma01.STORE_AREA);
					col_j = 15;
					find_col = true;
				}
			}
			if (tmmhrma01.STORE_AREA == "H11")
			{
				//CSP热轧成品库 
				col_j = 5;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "H21")
			{
				//2250后库
				col_j = 6;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "H22")
			{
				//横切成品库 
				col_j = 7;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "H31")
			{
				//1580后库
				col_j = 8;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "S41")
			{
				//2250板坯库
				col_j = 9;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "S42")
			{
				//1580板坯库
				col_j = 10;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "S43")
			{
				//板坯清理前库
				col_j = 11;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "S45")
			{
				//未清理板坯1580前库
				col_j = 12;
				find_col = true;
			}
			if (tmmhrma01.STORE_AREA == "S4B")
			{
				//线外板坯库
				col_j = 13;
				find_col = true;
			}
			//如果记录不是要统计的信息跳过本循环取下一条数据
			if (!find_col)
			{
				continue;
			}
			//结果进行累计
			if (find_col && find_row)
			{
				table_result[row_i][col_j] = table_result[row_i][col_j] + tmmhrma01.MAT_WT;
				table_count[row_i][col_j] = table_count[row_i][col_j] + 1;
			}


			if (tmmhrma01.DUMMY_COIL_FLAG == "1" || tmmhrma01.DUMMY_COIL_FLAG == "A") //其中返回卷
			{
				//返回卷列
				table_result[row_i][4] = table_result[row_i][4] + tmmhrma01.MAT_WT;
				table_count[row_i][4] = table_count[row_i][4] + 1;
				if (tmmhrma01.PRODUCT_FLAG == "0")
				{
					//在制
					table_result[product_row - 1][col_j] = table_result[product_row - 1][col_j] + tmmhrma01.MAT_WT;
					table_count[product_row - 1][col_j] = table_count[product_row - 1][col_j] + 1;

					table_result[product_row - 1][4] = table_result[product_row - 1][4] + tmmhrma01.MAT_WT;
					table_count[product_row - 1][4] = table_count[product_row - 1][4] + 1;

					if (tmmhrma01.ORDER_NO == "")	//“其中返回卷”行，“无委托”列
					{
						table_result[product_row - 1][3] = table_result[product_row - 1][3] + tmmhrma01.MAT_WT;
						table_count[product_row - 1][3] = table_count[product_row - 1][3] + 1;
					}
				}
			}
			if (tmmhrma01.ORDER_NO == "")	//其中无委托
			{
				//无委托列
				table_result[row_i][3] = table_result[row_i][3] + tmmhrma01.MAT_WT;
				table_count[row_i][3] = table_count[row_i][3] + 1;
				if (tmmhrma01.PRODUCT_FLAG == "0")
				{
					//无委托行各列
					table_result[product_row - 2][col_j] = table_result[product_row - 2][col_j] + tmmhrma01.MAT_WT;
					table_count[product_row - 2][col_j] = table_count[product_row - 2][col_j] + 1;

					if (tmmhrma01.ORDER_NO == "")
					{
						//无委托行无委托列
						table_result[product_row - 2][3] = table_result[product_row - 2][3] + tmmhrma01.MAT_WT;
						table_count[product_row - 2][3] = table_count[product_row - 2][3] + 1;
					}

					//“其中无委托”行，“返回卷”列
					if (tmmhrma01.DUMMY_COIL_FLAG == "1" || tmmhrma01.DUMMY_COIL_FLAG == "A")
					{
						table_result[product_row - 2][4] = table_result[product_row - 2][4] + tmmhrma01.MAT_WT;
						table_count[product_row - 2][4] = table_count[product_row - 2][4] + 1;
					}
				}
			}
		}
		cmdcinq.Close();

		Log::Trace("", __FUNCTION__, "**************** 合计重量 ****************");
		//计算合计重量列
		for (int i = 0; i < row_count; i++)
		{
			for (int j = 6; j < col_count; j++)
			{
				table_result[i][2] = table_result[i][2] + table_result[i][j];
				table_count[i][2] = table_count[i][2] + table_count[i][j];
			}
		}

		Log::Trace("", __FUNCTION__, "**************** 合计 ****************");

		//计算小计，合计
		for (int i = 0; i < row_count - 1; i++)
		{
			for (int j = 2; j < col_count; j++)
			{
				//板坯小计行 i=4，热卷在制小计行14，成品小计行原来i=20
				if (i < 4)     //板坯小计行     
				{
					table_result[4][j] = table_result[4][j] + table_result[i][j];
					table_count[4][j] = table_count[4][j] + table_count[i][j];
				}
				if (i > 5 && i < product_row - 3)//热卷在制小计行
				{
					table_result[product_row - 3][j] = table_result[product_row - 3][j] + table_result[i][j];
					table_count[product_row - 3][j] = table_count[product_row - 3][j] + table_count[i][j];
				}
				if (i > product_row - 1 && i < product_row + 3) //成品小计行
				{
					table_result[product_row + 3][j] = table_result[product_row + 3][j] + table_result[i][j];
					table_count[product_row + 3][j] = table_count[product_row + 3][j] + table_count[i][j];
				}
				if (i != 4 && i != 5 && i != product_row - 3 && i != product_row - 2 && i != product_row - 1 && i != product_row + 3)//最后一行
				{
					table_result[product_row + 5][j] = table_result[product_row + 5][j] + table_result[i][j];
					table_count[product_row + 5][j] = table_count[product_row + 5][j] + table_count[i][j];
				}
			}
		}


		Log::Trace("", __FUNCTION__, "**************** 赋值bcls_ret开始 ****************");
		for (int i = 0; i < row_count; i++)
		{
			for (int j = 2; j < col_count; j++)
			{
				if (i >= 0 && i <= 5 && j == 4)
				{
					bcls_ret->Tables[0].Rows[i][j] = "-";
					bcls_ret->Tables[1].Rows[i][j] = "-";
				}
				else
				{
					bcls_ret->Tables[0].Rows[i][j] = table_result[i][j].ToInt32();
					bcls_ret->Tables[1].Rows[i][j] = table_count[i][j].ToInt32();
				}
			}
		}
		Log::Trace("", __FUNCTION__, "**************** 历史库存存档 ****************");

		//历史库存存档
		if ((CString)s.userid == "batch")
		{
			int rowcount = bcls_ret->Tables[0].Rows.get_Count();
			int columncount = bcls_ret->Tables[0].Columns.get_Count();
			for (int i = 0; i < rowcount; i++)
			{
				for (int j = 2; j < columncount; j++)
				{
					bcls_ret->Tables[0].Rows[i][j] = bcls_ret->Tables[0].Rows[i][j].ToString() + " / "
						+ bcls_ret->Tables[1].Rows[i][j].ToString();
				}
			}

			bcls_ret->Tables.Add();
			bcls_ret->Tables[2].Columns.Add(DT_STRING, "FACTORY_STORE");
			bcls_ret->Tables[2].Columns.Add(DT_STRING, "FORM_NO");
			bcls_ret->Tables[2].Rows.Add();
			bcls_ret->Tables[2].Rows[0]["FACTORY_STORE"] = "MM";
			bcls_ret->Tables[2].Rows[0]["FORM_NO"] = "MM00SU47A1";

			Log::Trace("", __FUNCTION__, "**************** 调用库存存档函数 ****************");

			//doFlag = f_mmhrma_hstock(bcls_ret, bcls_ret, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
		}

		//Log::Trace("",__FUNCTION__,"****************赋值bcls_ret结束 ****************");
		/*返回处理信息*/
		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
	return(doFlag);
}