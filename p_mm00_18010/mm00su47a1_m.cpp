/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      KYE110
Version:     1.0
Date:        2018-04-25 17:00:18
Description: 板坯热卷明细信息查询
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/

#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 热轧物料当前库存信息查询
/// <para>根据查询条件查询热轧物料主表TMMHRMA01</para>
/// </summary>
/// <param name="ROW">表示物料类别的行号</param>
/// <param name="NEXT_UNIT_CODE">下道机组代码</param>
/// <param name="FACTORY_STORE">厂别或库区别</param>
/// <param name="STORE_AREA">厂别或库区别</param>
/// <param name="QUERY_TYPE">翻页标记</param>
/// <param name="POSITION">翻页起始位置</param>
/// <returns>满足查询条件的热轧物料主表TMMHRMA01或热轧物料主表TMMHRMA01数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mm00su47a1_m)


int f_mm00su47a1_m(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	CString sqlstr = "";	//数据库SQL操作字符串，用于捕获数据库操作异常情况

	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);

		CString strSql1 = "";
		CString strSql2 = "";
		CString store_area = "";
		CString sqlwhere = "";
		CString strPosition = "";
		CString mat_destion = "";
		int order_type = 0;

		CDecimal v_row = bcls_rec->Tables[0].Rows[0]["ROW"];
		CString v_next_unit_code = bcls_rec->Tables[0].Rows[0]["NEXT_UNIT_CODE"].ToString().Trim();
		CString v_factory_store = bcls_rec->Tables[0].Rows[0]["FACTORY_STORE"].ToString().Trim();
		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "v_row = [{0}]", v_row);
		Log::Trace("", __FUNCTION__, "v_next_unit_code = [{0}]", v_next_unit_code);
		Log::Trace("", __FUNCTION__, "v_factory_store = [{0}]", v_factory_store);
		Log::Trace("", __FUNCTION__, "v_stock_no = [{0}]", v_stock_no);

		int rowcount = 22;	//从前台获取行数,默认23行
		if (bcls_rec->Tables[0].Columns.Contains("ROWCOUNT"))
		{
			rowcount = bcls_rec->Tables[0].Rows[0]["ROWCOUNT"];
			rowcount = rowcount - 1;
		}
		//2018-6-21:2250直发A6现改为1700直发D1、2250直发D1、1580直发D1、CSP直发D1
		//1700直发对应的下机组不一定是H031，2250直发对应的下机组不一定是H032
		if (v_next_unit_code == "CSP直发")
		{
			v_next_unit_code = "H031";
			mat_destion = " IN ('P1','S2','T1') ";
		}
		else if (v_next_unit_code == "2250直发")
		{
			v_next_unit_code = "H032";
			mat_destion = " IN ('P2','S2','T2') ";
		}
		else if (v_next_unit_code == "1580直发")
		{
			v_next_unit_code = "H033";
			mat_destion = " IN ('P3','S3','T3') ";
		}
		else if (v_next_unit_code == "H031")
		{
			mat_destion = "00";
		}
		else if (v_next_unit_code == "H032")
		{
			mat_destion = "01";
		}
		else if (v_next_unit_code == "H033")
		{
			mat_destion = "02";
		}
		Log::Trace("", __FUNCTION__, "mat_destion = [{0}]", mat_destion);

		if (bcls_rec->Tables[0].Columns.Contains("ORDER_TYPE"))
		{
			order_type = bcls_rec->Tables[0].Rows[0]["ORDER_TYPE"];
			switch (order_type)
			{
			case 1:
				sqlwhere += " and  t1.ORDER_NO > ' ' ";
				break;
			case 2:
				sqlwhere += " and  t1.ORDER_NO <= ' ' ";
				break;
			}
		}
		int nQueryType = bcls_rec->Tables[0].Rows[0]["QUERY_TYPE"];

		strSql1 = "SELECT COUNT(1) ";
		strSql1 += " FROM TMMHRMA01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
			" ON t1.ORDER_NO = t2.ORDER_NO  where 1 = 1 ";

		/*根据标记行号查询热轧主档*/
		//2018-6-20板坯原(0到3行)，板坯现(0到5行)
		if (v_row >= 0 && v_row < 6)
		{
			strSql2 = " SELECT  t1.MAT_NO "
				",t1.MAT_STATUS "
				",t1.STOCK_PLACE_NO "
				",t1.LG_ST "
				",t1.MAT_ACT_THICK "
				",t1.MAT_ACT_WIDTH "
				",t1.MAT_ACT_LEN "
				",t1.MAT_ACT_WT AS MAT_WT "
				",t1.ST_NO "
				",t1.STOCK_NO "
				",t1.STORE_AREA "
				",t1.PLAN_NO "
				",t1.SLAB_CUT_TIME "
				",t1.IN_STOCK_HOT_TIME "
				",t1.HSF_END_TIME "
				",t1.ORDER_NO AS ORDER_NO "
				",t1.BACKLOG "
				",t1.TRANSFER_PLAN_NO"
				",t2.ORDER_DELIVERY_DATE "
				",t2.DELIVY_WEEK_FLAG ";
			//在库时间计算
			strSql2 += ",CASE WHEN LENGTH(TRIM(t1.SLAB_CUT_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(16,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.SLAB_CUT_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE '' "
				"END AS IN_STOCK_DURA"	//产出时间天（系统当前时间 - 板坯切断时间）
				",CASE WHEN LENGTH(TRIM(t1.SLAB_CUT_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(8,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.SLAB_CUT_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE ''  "
				"END AS IN_STOCK_HOUR"	//产出时间时（系统当前时间 - 板坯切断时间）
				",CASE WHEN LENGTH(TRIM(t1.IN_STOCK_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(16,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.IN_STOCK_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE '' "
				"END AS IN_STOCK_TIME_DURA" //在库时间天（系统当前时间 - 入库时间）
				",CASE WHEN LENGTH(TRIM(t1.IN_STOCK_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(8,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.IN_STOCK_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE '' "
				"END AS IN_STOCK_TIME_HOUR"; //在库时间时（系统当前时间 - 入库时间） 

			strSql2 += " FROM TMMSMMA01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
				" ON t1.ORDER_NO = t2.ORDER_NO  where 1 = 1 ";

			store_area = "t1.STOCK_NO";
			strSql1 = "SELECT COUNT(1) ";
			strSql1 += " FROM TMMSMMA01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
				" ON t1.ORDER_NO = t2.ORDER_NO  where 1 = 1 ";
		}
		//2018-6-20热卷在制(6-16行)
		//查询热卷档，暂时不区分在制和成品（20170419修改，6-16行是热卷）
		else if (v_row >= 6 && v_row <= rowcount - 2)
		{
			strSql2 = " SELECT  t1.MAT_NO "
				",t1.MAT_STATUS "
				",t1.STOCK_PLACE_NO "
				",t1.LG_ST "
				",t1.MAT_ACT_THICK "
				",t1.MAT_ACT_WIDTH "
				",t1.MAT_ACT_LEN "
				",t1.MAT_WT "
				",t1.ST_NO "
				",t1.STOCK_NO "
				",t1.STORE_AREA "
				",t1.ORDER_NO AS ORDER_NO "
				",t1.PLAN_NO "
				",t1.PRE_UNIT_CODE "
				",t1.NEXT_UNIT_CODE "
				",t1.MAT_DESTION "
				",t1.CONFM_PLAN_NO "
				",t1.COMPLEX_DECIDE_CODE " //20150917新增综合判定代码‘9’
				",t2.ORDER_DELIVERY_DATE "
				",t2.DELIVY_WEEK_FLAG ";
			//轧制时间计算（系统当前时间 - 钢卷卷曲时间）
			strSql2 += ",CASE WHEN LENGTH(TRIM(t1.COILED_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(16,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.COILED_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE '' "
				"END AS ROLL_TIME_DURA" //轧制时间天
				",CASE WHEN LENGTH(TRIM(t1.COILED_TIME)) = 14 "
				"THEN TO_CHAR(TIMESTAMPDIFF(8,TO_CHAR(CURRENT TIMESTAMP-TO_DATE(t1.COILED_TIME,'YYYYMMDDHH24MISS') ))) "
				"ELSE '' "
				"END AS ROLL_TIME_HOUR";	//轧制时间时

			strSql2 += " FROM TMMHRMA01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
				" ON t1.ORDER_NO = t2.ORDER_NO  where 1 = 1 ";

			store_area = "t1.STORE_AREA";
			strSql1 = "SELECT COUNT(1) ";
			strSql1 += " FROM TMMHRMA01 t1 LEFT OUTER JOIN TOMPOMA01 t2 "
				" ON t1.ORDER_NO = t2.ORDER_NO  where 1 = 1 ";
		}

		//2018-6-20库区没有X88，中间坯单列对应的条件现还没定
		if (v_row >= 6 && v_row < rowcount - 6) //在制
		{
			sqlwhere += " AND t1.PRODUCT_FLAG = '0' AND t1.STORE_AREA  != 'X88'";
		}
		else if (v_row >= rowcount - 6 && v_row < rowcount - 1) //成品
		{
			sqlwhere += " AND t1.PRODUCT_FLAG = '1' AND t1.STORE_AREA  != 'X88' ";
		}
		else if (v_row == rowcount - 1) //中间坯行
		{
			sqlwhere += " AND t1.STORE_AREA = 'X88' ";
		}

		//确定行条件
		if (v_next_unit_code == "无委托")  //成品无委托行  
		{
			sqlwhere += " AND t1.ORDER_NO <= ' ' AND t1.APP_DECIDE_NO <= ' ' ";
		}
		else if (v_next_unit_code == "小计"  &&  v_row < 6)  //板坯小计行
		{
			sqlwhere += " AND t1.DEST_FIN IN('00','01','02','20','09','10')  ";
		}
		else if (v_next_unit_code == "其中无委托"  && v_row < 6)  //板坯无委托行
		{
			sqlwhere += " AND t1.ORDER_NO <= ' ' AND  t1.DEST_FIN IN('00','01','02','20','09','10')  ";
		}
		else if (v_next_unit_code == "外供" && v_row < 6) //板坯外供行
		{
			sqlwhere += " AND t1.DEST_FIN IN('20','09','10') ";
		}
		else if (v_next_unit_code == "小计" &&  v_row >= 6 && v_row < rowcount - 6) //在制品小计行
		{  
			sqlwhere += " AND (t1.MAT_DESTION IN('P1','P2','P3','S1','S2','S3','T1','T2','T3') "
				" OR( CASE WHEN t1.DUMMY_COIL_FLAG NOT IN ('A','1') THEN t1.NEXT_UNIT_CODE  "
				" ELSE CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END END "
				" in('H040','H041','H042','H069','H051')) )";
		}
		else if (v_next_unit_code == "其中无委托" &&  v_row >= 7 && v_row < rowcount - 6) //在制品其中无委托行
		{
			sqlwhere += " AND (t1.ORDER_NO <= ' ' "
				" AND t1.MAT_DESTION IN('P1','P2','P3','S1','S2','S3','T1','T2','T3') "
				" OR (CASE WHEN t1.DUMMY_COIL_FLAG NOT IN ('A','1') THEN t1.NEXT_UNIT_CODE  "
				" ELSE CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END END "
				" in('H040','H041','H042','H069','H051')) )";
		}
		else if (v_next_unit_code == "其中返回卷"  &&  v_row >= 7 && v_row < rowcount - 6)//在制品其中返回卷行
		{
			sqlwhere += " AND (t1.DUMMY_COIL_FLAG = '1' OR t1.DUMMY_COIL_FLAG = 'A') "
				" AND CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END "
				" in('H040','H041','H042','H069','H051')";
		}
		else if (v_next_unit_code == "报现货") //成品报现货行
		{
			sqlwhere += " AND t1.ORDER_NO <= ' ' AND t1.APP_DECIDE_NO > ' ' ";
		}
		else if (v_next_unit_code == "有委托") //成品有委托行
		{
			sqlwhere += " AND t1.ORDER_NO > ' ' ";
		}
		else if (v_next_unit_code == "小计"  && v_row == rowcount - 1) //中间坯小计行
		{
			sqlwhere += " AND  t1.STOCK_NO IN ('X88')";
		}
		else if (v_next_unit_code != "小计" && v_next_unit_code != "合计")
		{
			if (v_row < 6) //板坯，除小计外
			{
				sqlwhere += " AND DEST_FIN ='" + mat_destion + "' ";
			}			
			else if (v_row > 10 && v_row < 14) //CSP直发、2250直发、1580直发
			{
				if (v_factory_store == "DUMMY_COIL") //其中返回卷列
				{
					sqlwhere += " AND CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END = '0000' AND t1.MAT_DESTION " + mat_destion + " ";
				}
				else //除“其中返回卷”列的其他列
				{
					if (v_next_unit_code == "0000")
					{
						sqlwhere += " AND ((t1.NEXT_UNIT_CODE = '0000'  and t1.DUMMY_COIL_FLAG NOT IN('A','1')) "
							"OR t1.DUMMY_COIL_FLAG IN('A','1') AND "
							"CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END "
							"= '0000' ) ";
					}
					else
					{
						sqlwhere += " AND ((t1.NEXT_UNIT_CODE = '0000' AND t1.MAT_DESTION " + mat_destion + " AND t1.DUMMY_COIL_FLAG NOT IN('A','1')) "
							"OR t1.DUMMY_COIL_FLAG IN('A','1') AND "
							"CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END "
							"= '0000' AND t1.MAT_DESTION  " + mat_destion + ") ";
					}
				}
			}
			else
			{
				if (v_factory_store == "DUMMY_COIL")
				{
					sqlwhere += " AND CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END = '" + v_next_unit_code + "' ";
					if (v_next_unit_code == "0000")
					{
						sqlwhere += " AND 1=2 ";
					}
				}
				else
				{
					sqlwhere += " AND ((t1.NEXT_UNIT_CODE = '" + v_next_unit_code + "' and t1.DUMMY_COIL_FLAG NOT IN('A','1')) "
						"OR t1.DUMMY_COIL_FLAG IN('A','1') AND "
						"CASE WHEN t1.PLAN_NO != '' THEN SUBSTR(t1.PLAN_NO,1,4) ELSE t1.PRE_UNIT_CODE END "
						"= '" + v_next_unit_code + "') ";
					if (v_next_unit_code == "0000")
					{
						sqlwhere += " AND 1=2 ";
					}
				}
			}
		}

		//确定列条件
		if (v_factory_store == "DUMMY_COIL") //其中返回卷列
		{
			sqlwhere += " AND (t1.DUMMY_COIL_FLAG = '1' OR t1.DUMMY_COIL_FLAG = 'A') ";
		}
		else if (v_factory_store == "NO_ORDER") //其中无委托列
		{
			sqlwhere += " AND t1.ORDER_NO <= '' ";
		}
		else if (v_factory_store == "STOCK_99") //在途列
		{
			if (v_row >= 0 && v_row < 6) //板坯在途
			{
				sqlwhere += " AND (" + store_area + " LIKE '_99' "
					"OR " + store_area + " LIKE '_00' "
					"OR " + store_area + "= '' ) "
					"AND " + store_area + " NOT LIKE 'S99'";
			}
			else //热卷在途
			{
				//没有下机组Y201和存储区域Y00的材料
				sqlwhere += " AND (" + store_area + " LIKE '_99' "
					"OR " + store_area + " LIKE '_00' "
					"OR " + store_area + "= '' )";
			}
		}
		else if (v_factory_store == "STOCK_COLD") //冷轧原料库列
		{
			sqlwhere += " AND " + store_area + " != ' ' AND " + store_area + " NOT LIKE '_99' "
				" AND (" + store_area + " NOT LIKE '_00') ";
			//冷轧原料前库中,存储区域为C和Q开头
			sqlwhere += " AND SUBSTR(" + store_area + ",1,1) IN ('C,Q') " ;
		}
		else if (v_factory_store == "H11") //CSP热轧成品库
		{
			/*if (v_row < 6)
			{
				sqlwhere += " AND " + store_area + " = 'H11' ";
			}
			else
			{
				sqlwhere += " and 1 = 2 ";
			}*/
			sqlwhere += " AND " + store_area + " = 'H11' ";
		}
		else if (v_factory_store != "SUM_WT") //四钢轧库区
		{
			//if (v_row >= 0 && v_row < 6)
			//{
			//	sqlwhere += " AND t1.STOCK_NO ='" + v_stock_no + "'";
			//}
			//else
			//{
			//	//2018-6-20冷轧原料库的STORE_AREA不是H，现还未定,现暂时只有RZ2
			//	sqlwhere += " AND ((" + store_area + " "
			//		" NOT LIKE '_99' AND " + store_area + " NOT LIKE '_00' AND "
			//		 + store_area + " > ' ' "
			//		" and substr(" + store_area + ",1,1) not in ('C','Q')) ";
			//	sqlwhere += ") ";
			//}

			sqlwhere += " AND " + store_area +"='"+ v_stock_no + "'";
		}

		//无委托列和返回卷列，都含于其他列中
		if (v_factory_store == "DUMMY_COIL" || v_factory_store == "NO_ORDER" || v_factory_store == "SUM_WT")
		{
			sqlwhere += " AND ("
				"	((" + store_area + " LIKE '_99' ) "
				"		or (" + store_area + " LIKE '_00' ) or " + store_area + "= '' ) "
				"	or	" + store_area + " LIKE 'F%' "
				"	or	" + store_area + " LIKE 'S6%' "
				"	or 	" + store_area + " LIKE 'S7%' "
				"   or (substr(" + store_area + ",1,1) in ('C,Q') ";
			if (v_row >= 0 && v_row < 6)
			{
				sqlwhere += " or t1.STOCK_NO  IN ('H11','H21','H22','H31','S41','S42','S43','S45','S4B')) ";
			}
			else
			{
				sqlwhere += " or t1.FACTORY_STORE = 'RZ4') ";
			}
			sqlwhere += ") ";
		}

		strSql1 += sqlwhere;

		Log::Trace("", __FUNCTION__, "strSql1 = [{0}]", (const char*)strSql1);


		if (nQueryType == 0)
		{
			sqlstr = strSql1;

			cmdcount.SetCommandText(strSql1);

			/*此方法返回单行单列数据*/
			CDecimal nRecCount = cmdcount.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "nRecCount = [{0}]", nRecCount.ToInt32());
			/*返回记录数，分页显示用*/
			bcls_ret->ExtendedProperties.Add("REC_COUNT", nRecCount.ToString());
		}
		else
		{
			strPosition = bcls_rec->Tables[0].Rows[0]["POSITION"];
			Log::Trace("", __FUNCTION__, "strPosition = [{0}]", (const char*)strPosition);
		}

		strSql2 += sqlwhere;

		Log::Trace("", __FUNCTION__, "strSql2 = [{0}]", (const char*)strSql2);

		if (nQueryType != 0)
		{
			strSql2 += "  AND MAT_NO >  @position ";
		}

		strSql2 += " ORDER BY MAT_NO";

		if (nQueryType != 2)
		{
			strSql2 += " FETCH FIRST  1000 ROWS ONLY ";
		}

		sqlstr = strSql2;
		cmdinq.SetCommandText(strSql2);

		if (nQueryType != 0)
		{
			cmdinq.Parameters.Set("position", strPosition);
		}

		int nResultRecordCount = cmdinq.ExecuteQuery(bcls_ret->Tables[0]);


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
		CString str = sqlstr + "\r\n" + ex.GetMsg();
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
