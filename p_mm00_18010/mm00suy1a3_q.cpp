/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      KYE110
Version:     1.0
Date:        2018-04-26 14:02:01
Description: 热轧钢卷信息查询
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/

#include "stdafx.h"

int f_mmhrma_hstock(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/*<remark>=========================================================
/// <summary>
/// 钢卷库库存查询分析（按流向、时间）
/// <para>TMMHR01</para>
/// </summary>
/// <returns>根据下机组号统计主档数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mm00suy1a3_q)


int f_mm00suy1a3_q(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;

	CString sqlstr = "";
	/* 实体类定义 */
	CModel tmmhr01("TMMHR01");
	try
	{
		CDbCommand cmdinq(conn);
		CDbCommand cmdcode(conn);
		CString sqlcode = "";
		CString strSql1 = "";
		CString str = "";
		CString delivery_date = "";
		CString v_factory_div = "";
		CString date_flag = "0";
		CString v_order_delivery_date_1 = "";
		CString v_order_delivery_date_2 = "";

		if ((CString)s.userid != "batch")
		{
			v_factory_div = bcls_rec->Tables[0].Rows[0][0].ToString().Trim();
			date_flag = bcls_rec->Tables[0].Rows[0][1].ToString().Trim();
			v_order_delivery_date_1 = bcls_rec->Tables[0].Rows[0][2].ToString().Trim();
			v_order_delivery_date_2 = bcls_rec->Tables[0].Rows[0][3].ToString().Trim();
		}
		else
		{
			CString v_factory_div = ((CString)bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"]).Trim();
		}

		Log::Trace("", "_FUNCTION_", "v_factory_div={0}", v_factory_div);
		Log::Trace("", "_FUNCTION_", "date={0}", v_order_delivery_date_1);
		Log::Trace("", "_FUNCTION_", "date={0}", v_order_delivery_date_2);

		if (v_factory_div != "")
		{
			//str = "  AND CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END =@factory_div  ";
		}
		//str = " and FACTORY_DIV like 'RZ%' ";
#pragma region/*向结果表添加行标题和列标题，并将数据区数据初始化为0*/
		const int row_count = 14;				/*画面总行数*/
		CString table_row_list[row_count][3] =
		{
			{ "无委托", "释放待冷", "0" },//0
			{ "", "封锁", "1" },//1
			{ "", "返回卷", "2" },//2
			{ "", "小计", "3" },//3
			{ "有委托", "计划中(12)", "4" },//4
			{ "", "预计划(11)", "5" },//5
			{ "", "无效计划", "6" },//6
			{ "", "可计划(10)", "7" },//7
			{ "", "封锁返修(03、13)", "8" },//8
			{ "", "封锁(01、02)", "9" },//9
			{ "", "等待(00)", "10" },//10
			{ "", "小计", "11" },//11
			{ "合计", "总计", "12" },//12
			{ "", "其中封锁", "13" },//13
			//{ "", "FXX", "14" } //14
		};

		//从主档读取下机组号，作为列名
		sqlcode = "SELECT DISTINCT next_unit_code FROM TMMHR01 "
			"WHERE 1=1 "
			"AND CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = @factory_div "
			//"AND next_unit_code <> '' AND next_unit_code <> '0000'"
			"AND next_unit_code <> '' AND product_flag <> '1' "
			"UNION "
			"SELECT DISTINCT hsf_unit_repair as next_unit_code FROM TMMHR01 "
			"WHERE 1=1 "
			"AND CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = @factory_div "
			"AND hsf_unit_repair <> '' AND hsf_unit_repair <> '0000'"
			"ORDER BY next_unit_code ";
		cmdcode.SetCommandText(sqlcode);
		cmdcode.Parameters.Set("factory_div", v_factory_div);
		bcls_rec->Tables.Add();
		cmdcode.ExecuteQuery(bcls_rec->Tables[1]);

		Log::Trace("", __FUNCTION__, "sqlcode = [{0}]", sqlcode);

		Log::Trace("", __FUNCTION__, "bcls_rec->Tables[1].Rows.get_Count() = [{0}]", bcls_rec->Tables[1].Rows.get_Count());
		int column_count = bcls_rec->Tables[1].Rows.get_Count() + 3;	//画面总列数

		Log::Trace("", __FUNCTION__, "column_count = [{0}]", column_count);

		if (!column_count > 3)
		{
			strcpy(s.msg, "未查询到机组");
			return -1;
		}
		CString table_column_list[200];
		table_column_list[0] = " ";
		table_column_list[1] = "状态";

		//添加列名，下机组
		for (int i = 2; i < column_count - 1; i++)
		{
			table_column_list[i] = bcls_rec->Tables[1].Rows[i - 2][0].ToString().Trim();
		}

		//column_count = column_count + 6;

		int k = 2 + bcls_rec->Tables[1].Rows.get_Count();

		//table_column_list[k] = "供2030";    // D1
		//table_column_list[k + 1] = "供1550";   // D2
		//table_column_list[k + 2] = "供酸洗";    // D9
		//table_column_list[k + 3] = "2250直发";  // A6
		//table_column_list[k + 4] = "2250平整";  // A1
		//table_column_list[k + 5] = "中间坯"; // 非（D1/D2/D9/A6/A1）

		table_column_list[column_count - 1] = "合计";

		CDecimal table_result[row_count + 1][200];		//存储重量统计
		CDecimal table_count[row_count + 1][200];		//存储块数统计

		Log::Trace("", "", "添加列标题");
		for (int i = 0; i < column_count; i++)									//初始化
		{
			//添加列标题
			bcls_ret->Tables[0].Columns.Add(DT_STRING, table_column_list[i]);

		}

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Clone(bcls_ret->Tables[0]);

		Log::Trace("", "", "添加行标题");
		for (int i = 0; i < row_count; i++)
		{
			//添加行标题
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[i][0] = table_row_list[i][0];
			bcls_ret->Tables[0].Rows[i][1] = table_row_list[i][1];

			bcls_ret->Tables[1].Rows.Add();
			bcls_ret->Tables[1].Rows[i][0] = table_row_list[i][0];
			bcls_ret->Tables[1].Rows[i][1] = table_row_list[i][1];
		}

		Log::Trace("", "", "初始化数据区数据为0");
		for (int i = 0; i < row_count; i++)
		{
			//初始化数据区数据为0
			for (int j = 2; j < column_count; j++)
			{
				table_result[i][j] = 0;
				table_count[i][j] = 0;
			}
		}
#pragma endregion

		for (int i = 2; i < column_count; i++)
		{
			Log::Trace("", "", table_column_list[i].Trim());
		}

		bool find_row;		//行标题匹配
		int row_i = 0;
		bool find_column;	//列标题匹配						
		int column_i = 0;

		/*从数据表取数据*/
		strSql1 = "SELECT t1.MAT_STATUS,"
			"t1.ORDER_NO,"
			"t1.MAT_WT,"
			"t1.NEXT_UNIT_CODE,"
			"t1.WHOLE_BACKLOG_CODE,"
			"t1.STORE_AREA,"
			"t1.DUMMY_COIL_FLAG,"
			"t1.FACTORY_STORE,"
			"t1.COIL_PACK_FLAG,"
			"t1.COMPLEX_DECIDE_CODE,"
			"t1.MAT_DESTION,"
			"t1.PRE_UNIT_CODE,"
			"t2.ORDER_DELIVERY_DATE, "
			"t1.FACTORY_DIV, "
			"t1.PLAN_NO, "
			"t1.HSF_UNIT_REPAIR "
			"FROM TMMHR01 t1 "
			"LEFT OUTER JOIN OMPOMA.TOMPOMA01 t2 ON t1.ORDER_NO = t2.ORDER_NO "
			//"WHERE t1.FACTORY_STORE = @factory_div  AND t1.next_unit_code <> ' ' AND t1.next_unit_code <> '0000' ";
			"WHERE t1.FACTORY_STORE = @factory_div  AND t1.next_unit_code <> ' ' AND t1.product_flag <> '1' ";
		//strSql1 += str;
		cmdinq.SetCommandText(strSql1);
		cmdinq.Parameters.Set("factory_div", v_factory_div);
		cmdinq.ExecuteReader();
		Log::Trace("", __FUNCTION__, "strSql1 = [{0}]", strSql1);
		while (cmdinq.Read())									//循环验证，插入结果表
		{
			tmmhr01.MAT_STATUS = cmdinq.GetString(1).Trim();
			tmmhr01.ORDER_NO = cmdinq.GetString(2).Trim();
			tmmhr01.MAT_WT = cmdinq.GetDecimal(3).Round(3);
			tmmhr01.NEXT_UNIT_CODE = cmdinq.GetString(4).Trim();
			tmmhr01.WHOLE_BACKLOG_CODE = cmdinq.GetString(5).Trim();
			tmmhr01.STORE_AREA = cmdinq.GetString(6).Trim();
			tmmhr01.DUMMY_COIL_FLAG = cmdinq.GetString(7).Trim();
			tmmhr01.FACTORY_STORE = cmdinq.GetString(8).Trim();
			tmmhr01.COIL_PACK_FLAG = cmdinq.GetString(9).Trim();
			tmmhr01.COMPLEX_DECIDE_CODE = cmdinq.GetString(10).Trim();
			tmmhr01.MAT_DESTION = cmdinq.GetString(11).Trim();
			tmmhr01.PRE_UNIT_CODE = cmdinq.GetString(12).Trim();
			tmmhr01.FACTORY_DIV = cmdinq.GetString(14).Trim();
			tmmhr01.PLAN_NO = cmdinq.GetString(15).Trim();
			tmmhr01.HSF_UNIT_REPAIR = cmdinq.GetString(16).Trim();
			delivery_date = cmdinq.GetString(13).Trim();

			find_row = false;
			find_column = false;

			//if (tmmhr01.STORE_AREA.GetLength() > 0 && tmmhr01.STORE_AREA.Substring(0, 1) == 'F')	//FXX
			//{
			//	row_i = 14;
			//	find_row = true;
			//}
			//else 
			if (tmmhr01.ORDER_NO != "")		//有委托
			{
				if (date_flag == "1")
				{
					if (!(delivery_date >= v_order_delivery_date_1
						&& delivery_date <= v_order_delivery_date_2)
						)
					{
						continue;
					}
				}
				if (tmmhr01.MAT_STATUS == "12")			//计划中(12)
				{
					row_i = 4;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "11"
					&& ((tmmhr01.PLAN_NO.GetLength() > 6 && tmmhr01.PLAN_NO[6] != 'A' && tmmhr01.PLAN_NO[6] != 'a')
					|| tmmhr01.PLAN_NO.GetLength() <= 6))//预计划(11)
				{
					row_i = 5;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "11")	//无效计划(11)
				{
					row_i = 6;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "10")	//可计划(10)
				{
					row_i = 7;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "03" || tmmhr01.MAT_STATUS == "13")	//封锁返修
				{
					row_i = 8;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "01" || tmmhr01.MAT_STATUS == "02")	//封锁
				{
					row_i = 9;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "00")									//等待
				{
					row_i = 10;
					find_row = true;
				}
			}
			else if (tmmhr01.ORDER_NO == "")		//无委托
			{
				if (tmmhr01.DUMMY_COIL_FLAG != "0" && tmmhr01.DUMMY_COIL_FLAG != "")	//返回卷
				{
					row_i = 2;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "01" || tmmhr01.MAT_STATUS == "02"
					|| tmmhr01.MAT_STATUS == "03" || tmmhr01.MAT_STATUS == "13")	//封锁
				{
					row_i = 1;
					find_row = true;
				}
				else if (tmmhr01.MAT_STATUS == "00" || tmmhr01.MAT_STATUS == "09"
					|| tmmhr01.MAT_STATUS == "10" || tmmhr01.MAT_STATUS == "11"
					|| tmmhr01.MAT_STATUS == "12")	//释放
				{
					if ((tmmhr01.PLAN_NO.GetLength() > 3
						&& tmmhr01.PLAN_NO.Substring(3, 1) != "9"
						)
						|| tmmhr01.PLAN_NO.GetLength() < 3)
					{
						row_i = 0;
						find_row = true;
					}
				}
			}
			if (!find_row)
			{
				continue;
			}

			for (int j = 2; j < column_count; j++)	//根据下机组号确定对应列标题
			{
				//if (tmmhr01.NEXT_UNIT_CODE == "0000" || tmmhr01.NEXT_UNIT_CODE.Trim() == "")
				//{
				//	if (tmmhr01.MAT_DESTION.Trim() == "D1")
				//	{
				//		column_i = k;
				//		find_column = true;
				//	}
				//	else if (tmmhr01.MAT_DESTION.Trim() == "D2")
				//	{
				//		column_i = k + 1;
				//		find_column = true;
				//	}
				//	else if (tmmhr01.MAT_DESTION.Trim() == "D9")
				//	{
				//		column_i = k + 2;
				//		find_column = true;
				//	}
				//	else if (tmmhr01.MAT_DESTION.Trim() == "A6")
				//	{
				//		column_i = k + 3;
				//		find_column = true;
				//	}
				//	else if (tmmhr01.MAT_DESTION.Trim() == "A1")
				//	{
				//		column_i = k + 4;
				//		find_column = true;
				//	}
				//	else if (tmmhr01.MAT_DESTION.Trim() != "D1" && tmmhr01.MAT_DESTION.Trim() != "D2" && tmmhr01.MAT_DESTION.Trim() != "D9"
				//		&& tmmhr01.MAT_DESTION.Trim() != "A6" && tmmhr01.MAT_DESTION.Trim() != "A1")
				//	{
				//		column_i = k + 5;
				//		find_column = true;
				//	}
				//}
				//else if (tmmhr01.NEXT_UNIT_CODE != "0000" &&  tmmhr01.NEXT_UNIT_CODE.Trim() != "")
				{
					if (tmmhr01.NEXT_UNIT_CODE == table_column_list[j].Trim())
					{
						column_i = j;
						find_column = true;
						break;
					}
				}
			}

			Log::Trace("", __FUNCTION__, "row_i = [{0}]", row_i);
			Log::Trace("", __FUNCTION__, "column_i = [{0}]", column_i);
			Log::Trace("", __FUNCTION__, "tmmhr01.MAT_WT = [{0}]", tmmhr01.MAT_WT);
			if (find_row && find_column)		//将对应数据压入结果表
			{
				table_result[row_i][column_i] = table_result[row_i][column_i] + tmmhr01.MAT_WT;
				table_count[row_i][column_i] = table_count[row_i][column_i] + 1;
				table_result[row_i][column_count - 1] = table_result[row_i][column_count - 1]+  tmmhr01.MAT_WT;
				table_count[row_i][column_count - 1] = table_count[row_i][column_count - 1] + 1;
			}


		}
		cmdinq.Close();

		Log::Trace("", "", "合计");

		for (int i = 2; i < column_count; i++)
		{
			//无委托小计
			for (int j = 0; j < 3; j++)
			{
				table_result[3][i] = table_result[3][i] + table_result[j][i];
				table_count[3][i] = table_count[3][i] + table_count[j][i];
			}
			//有委托小计
			for (int j = 4; j < 11; j++)
			{
				table_result[11][i] = table_result[11][i] + table_result[j][i];
				table_count[11][i] = table_count[11][i] + table_count[j][i];
			}
			//总计
			table_result[12][i] = table_result[3][i] + table_result[11][i];
			table_count[12][i] = table_count[3][i] + table_count[11][i];
			//其中封锁
			table_result[13][i] = table_result[1][i] + table_result[9][i];
			table_count[13][i] = table_count[1][i] + table_count[9][i];
		}

		for (int i = 0; i < row_count; i++)
		{
			for (int j = 2; j < column_count; j++)
			{
				bcls_ret->Tables[0].Rows[i][j] = table_result[i][j].ToInt32();
				bcls_ret->Tables[1].Rows[i][j] = table_count[i][j].ToInt32();
			}
		}

		Log::Trace("", "", "删除空白列");

		//for (int i = 0; i < bcls_ret->Tables[0].Columns.get_Count() - 1; i++)
		//{
		//	if (bcls_ret->Tables[0].Rows[row_count - 3][i].ToString().Trim() == "0")
		//	{
		//		bcls_ret->Tables[0].Columns[i].Delete();
		//		bcls_ret->Tables[1].Columns[i].Delete();
		//		i--;
		//	}
		//}

		//删除最后一列“其他”列
		/*bcls_ret->Tables[0].Columns[bcls_ret->Tables[0].Columns.get_Count() - 1].Delete();
		bcls_ret->Tables[1].Columns[bcls_ret->Tables[1].Columns.get_Count() - 1].Delete();*/

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
			bcls_ret->Tables[2].Rows[0]["FACTORY_STORE"] = v_factory_div;
			bcls_ret->Tables[2].Rows[0]["FORM_NO"] = "MMHRMAY1A3";

			doFlag = f_mmhrma_hstock(bcls_ret, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		/*返回处理信息*/
		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")
			/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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