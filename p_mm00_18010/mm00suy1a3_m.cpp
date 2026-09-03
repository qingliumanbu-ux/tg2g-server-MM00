/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      KYE110
Version:     1.0
Date:        2018-04-26 14:05:47
Description: 在制热卷明细信息查询
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/

#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 钢卷库库存查询分析（按流向、时间）明细查询
/// </summary>
/// <param name="ROW">表示行号</param>
/// <param name="NEXT_UNIT_CODE">下道机组代码</param>    TOMPOMA01
/// <param name="QUERY_TYPE">翻页标记</param>
/// <param name="POSITION">翻页起始位置</param>
/// <returns>满足查询条件的热轧物料主表</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mm00suy1a3_m)


int f_mm00suy1a3_m(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	CString sqlstr = "";	//数据库SQL操作字符串，用于捕获数据库操作异常情况

	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);
		CDbCommand cmd_tompoma01(conn);

		CString strSql1 = "";
		CString strSql1_where = "";
		CString strSql1_table = "";
		CString sqlstr = "";
		CString strSql2 = "";
		CString strSql3 = "";
		CString strSql2_where = "";
		CString strSql2_table = "";
		CString strPosition = "";
		CString v_order_no = "";
		CString v_mat_destion = "";

		int v_row = (int)bcls_rec->Tables[0].Rows[0]["ROW"];
		bool mat_destion = false;

		CString v_next_unit_code = bcls_rec->Tables[0].Rows[0]["NEXT_UNIT_CODE"].ToString().Trim();
		CString v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		CString date_flag = bcls_rec->Tables[0].Rows[0]["DATE_FLAG"].ToString();
		CString v_order_delivery_date_1 = bcls_rec->Tables[0].Rows[0]["ORDER_DELIVERY_DATE_1"].ToString().Trim();
		CString v_order_delivery_date_2 = bcls_rec->Tables[0].Rows[0]["ORDER_DELIVERY_DATE_2"].ToString().Trim();

		//2018-6-21去向和下机组现分成2个
		if (v_next_unit_code == "供2030")
		{
			//v_next_unit_code = " = 'D1' ";//2018-6-21类似这种统计类的先不改
			v_mat_destion = " = 'D1' ";
			mat_destion = true;
		}
		if (v_next_unit_code == "供1550")
		{
			v_next_unit_code = " = 'D2' ";
			mat_destion = true;
		}
		if (v_next_unit_code == "供酸洗")
		{
			v_next_unit_code = " = 'D9' ";
			mat_destion = true;
		}
		else if (v_next_unit_code == "2250直发")
		{
			v_next_unit_code = " = 'A6' ";
			mat_destion = true;
		}
		else if (v_next_unit_code == "2250平整")
		{
			v_next_unit_code = " = 'A1' ";
			mat_destion = true;
		}
		else if (v_next_unit_code == "中间坯")
		{
			v_next_unit_code = " not in ('D1','D2','D9','A6','A1') ";
			mat_destion = true;
		}

		if (mat_destion && v_row != 8)
		{
			strSql2_where = " AND MAT_DESTION " + v_next_unit_code + " "
				" AND NEXT_UNIT_CODE NOT IN "
				"( SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
				/*"  CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
				+ v_factory_div + "' "*/
				"  FACTORY_STORE = @factory_div "
				" and next_unit_code != '0000' and next_unit_code != '' "
				") ";
		}
		else if (v_row == 8 && !mat_destion)
		{
			strSql2_where = " AND ("
				" NEXT_UNIT_CODE = '" + v_next_unit_code + "' "
				" OR ((HSF_UNIT_REPAIR = '" + v_next_unit_code + "') "
				" AND (NEXT_UNIT_CODE = '0000' OR NEXT_UNIT_CODE <= ' ') )"
				" )";
		}
		else if (mat_destion && v_row == 8)
		{
			strSql2_where = " AND MAT_DESTION " + v_next_unit_code + " "
				" AND NEXT_UNIT_CODE NOT IN "
				"( SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
				/*"  CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
				+ v_factory_div + "' "*/
				"  FACTORY_STORE = @factory_div "
				" and next_unit_code != '0000' and next_unit_code != '' "
				") ";
		}
		else if (!mat_destion)
		{
			strSql2_where = " AND NEXT_UNIT_CODE = '" + v_next_unit_code + "' ";
		}
		if (date_flag == "1")
		{
			if (v_row > 3 && v_row < 11)
			{
				if (v_order_delivery_date_1 != "")
				{
					sqlstr += " AND t2.ORDER_DELIVERY_DATE  >= '" + v_order_delivery_date_1 + "' ";
				}
				if (v_order_delivery_date_2 != "")
				{
					sqlstr += " AND t2.ORDER_DELIVERY_DATE  <= '" + v_order_delivery_date_2 + "'";
				}
			}
			strSql2_where += sqlstr;
		}
		if (v_row != 14)
		{
			strSql2_where += "AND STORE_AREA NOT LIKE 'F%' ";
		}

		switch (v_row)
		{
		//case 14: strSql1_where = " WHERE   FACTORY_DIV like 'RZ%'  AND STORE_AREA  LIKE 'F%' ";
		//	break;

		case 2: strSql1_where = " WHERE (DUMMY_COIL_FLAG = '1' OR DUMMY_COIL_FLAG = 'A') AND t1.ORDER_NO <= ' ' "
			" AND FACTORY_STORE = @factory_div ";
			if (v_next_unit_code.GetLength() > 2)
			{
				if (!mat_destion)
				{
					strSql1_where +=
						"AND ( "
						"(DUMMY_COIL_FLAG = 'A' "
						//"PLAN_NO LIKE '" + v_next_unit_code + "%' "
						"OR (NEXT_UNIT_CODE = '" + v_next_unit_code + "') "
						") "
						"OR "
						"(DUMMY_COIL_FLAG = '1' AND NEXT_UNIT_CODE = '" + v_next_unit_code + "') "
						")";
				}
			}
			else
			{
				strSql1_where +=
					"AND "
					"(("
					"DUMMY_COIL_FLAG = 'A' "
					//2018-8-8计划号不按机组号编，现在规则不一样
					//"DUMMY_COIL_FLAG = 'A' AND "
					//"SUBSTR(PLAN_NO,1,4) NOT IN (SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
					///*"CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
					//+ v_factory_div + "' "*/
					//" FACTORY_DIV like 'RZ%' "
					//" and next_unit_code != '0000' and next_unit_code != '' ) "
					"AND NEXT_UNIT_CODE NOT IN (SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
					/*"  CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
					+ v_factory_div + "' "*/
					" FACTORY_DIV = @factory_div "
					" and next_unit_code != '0000' and next_unit_code != '' ) "
					") "
					"OR ("
					"(DUMMY_COIL_FLAG = '1' AND NEXT_UNIT_CODE NOT IN (SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
					/*"  CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
					+ v_factory_div + "' "*/
					" FACTORY_DIV = @factory_div "
					" and next_unit_code != '0000' and next_unit_code != '' ) "
					")))"
					"AND NEXT_UNIT_CODE NOT IN (SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
					/*"CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
					+ v_factory_div + "' "*/
					" FACTORY_STORE = @factory_div "
					" and next_unit_code != '0000' and next_unit_code != '' ) "
					"AND MAT_DESTION " + v_next_unit_code + " ";
			}

			strSql2_where = "";
			if (v_next_unit_code == "其他")
			{
				strSql2_where = "WHERE (DUMMY_COIL_FLAG = '1' OR DUMMY_COIL_FLAG = 'A') "
					" AND (NEXT_UNIT_CODE = '0000' OR NEXT_UNIT_CODE <= ' ') "
					" AND MAT_DESTION = '' "
					//" AND SUBSTR(PLAN_NO,1,4) NOT IN "
					//"( SELECT DISTINCT NEXT_UNIT_CODE FROM TMMHR01 WHERE "
					///*"  CASE WHEN FACTORY_STORE = '' THEN FACTORY_DIV ELSE FACTORY_STORE END = '"
					//+ v_factory_div + "' "*/
					//" FACTORY_DIV like 'RZ%' "
					//") "
					;
				strSql1_where = "";
			}
			break;
			//NOT LIKE , NOT IN 有问题，最好不用
		case 0: strSql1_where = " WHERE  t1.ORDER_NO <= ' ' AND FACTORY_STORE = @factory_div AND (DUMMY_COIL_FLAG = '0' OR DUMMY_COIL_FLAG = '') AND PLAN_NO NOT LIKE '___9%' AND MAT_STATUS IN ('00','09','10','11','12')  ";
			break;
		case 1: strSql1_where = " WHERE  t1.ORDER_NO <= ' ' AND FACTORY_STORE = @factory_div AND (DUMMY_COIL_FLAG = '0' OR DUMMY_COIL_FLAG = '') AND PLAN_NO NOT LIKE '___9%' AND MAT_STATUS IN('01','02','03','13')  ";
			break;
		case 4: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('12')  ";
			break;
		case 5: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('11') AND substr(mat_no,7,1) != 'A' and substr(mat_no,7,1) != 'a' ";
			break;
		case 6: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('11') AND (substr(mat_no,7,1) = 'A' or substr(mat_no,7,1) = 'a') ";
			break;
		case 7: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('10') ";
			break;
		case 8: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('13','03') ";
			break;
		case 9: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('01','02') ";
			break;
		case 10: strSql1_where = " WHERE  t1.ORDER_NO > ' ' AND FACTORY_STORE = @factory_div AND MAT_STATUS IN('00') ";
			break;
		case 13: strSql1_where = " WHERE  FACTORY_STORE = @factory_div AND MAT_STATUS IN('01','02','03','13') AND ((t1.ORDER_NO <= ' ' AND DUMMY_COIL_FLAG = '0' OR DUMMY_COIL_FLAG = '' AND PLAN_NO NOT LIKE '___9%' ) OR"
			"( MAT_STATUS IN('01','02') AND  t1.ORDER_NO > ' '"
			+ sqlstr + ")) ";
			break;

		default: strSql1_where = " WHERE 1 = 2 ";
		}

		strSql2 = " SELECT  t1.* "
			" ,t2.ORDER_STATUS"
			" ,t2.ORDER_DELIVERY_DATE"
			" ,t2.DELIVY_WEEK_FLAG "
			" ,t2.DELIVERY_PLACE_NAME "  //20160104沈楚伟提出新增终到站港描述显示
			" FROM TMMHR01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
			" ON t1.ORDER_NO = t2.ORDER_NO ";

		strSql2 = strSql2
			+ strSql1_where + strSql2_where
			+ " ORDER BY t1.ORDER_NO ";


		sqlstr = strSql2;
		cmdinq.SetCommandText(strSql2);
		cmdinq.Parameters.Set("factory_div", v_factory_div);

		int nResultRecordCount = cmdinq.ExecuteQuery(bcls_ret->Tables[0]);

		Log::Trace("", "strSql1_where", strSql1_where);
		Log::Trace("", "strSql2_where", strSql2_where);
		Log::Trace("", "strSql2", strSql2);

		/*返回处理信息*/
		if (bcls_ret->Tables[0].Rows.get_Count() > 0)
		{
			strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);	//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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

