/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      K17017
Version:     1.0
Date:        2018-06-11 16:33:43
Description: 薄板类合同超量材料清单查询
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 薄板类合同超量材料清单查询
/// <para>根据有超量的合同信息遍历冷轧主档表TMMCR01或热轧主档表TMMHR01的材料信息</para>
/// </summary>
/// <returns>冷轧主档表TMMCR01或热轧主档表TMMHR01的材料信息</returns>
===========================================================</remark>*/


#include "stdafx.h"

//名称空间引用
// Service 入口
BM2F_ENTERACE(mm00sue5f2_inq);

int f_mm00sue5f2_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int i = 0;
	int j = 0;

	CString sqlstr = "";
	CString strSql1 = "";
	CString strSql2 = "";
	CDateTime dt1 = CDateTime::Now();
	CDecimal MAT_WT = 0;
	CDecimal OVER_WT = 0;
	CDecimal MAT_WT1 = 0;

	try
	{
	CModel tmmcr01("TMMCR01");
	CModel tmmhr01("TMMHR01");
		CDbCommand cmd(conn);
		CDbCommand cmdinq(conn);

		CString flag = (CString)bcls_rec->Tables[0].Rows[0]["flag"];
		CString main_plant = (CString)bcls_rec->Tables[0].Rows[0]["main_plant"];
		CString order_nature = (CString)bcls_rec->Tables[0].Rows[0]["order_nature"];
		CString ht_flag = (CString)bcls_rec->Tables[0].Rows[0]["ht_flag"];
		Log::Trace("", __FUNCTION__, "main_plant =[{0}] ", main_plant);
		Log::Trace("", __FUNCTION__, "order_nature =[{0}] ", order_nature);
		Log::Trace("", __FUNCTION__, "ht_flag=[{0}] ", ht_flag);

		//有超量的合同信息


		sqlstr = "SELECT A.ORDER_TYPE_CODE,B.ORDER_NO, B.WHOLE_BACKLOG ,WHOLE_BACKLOG_CODE , (B.STOCK_WT - B.LACK_WT) AS OVER_WT "
			"FROM   OMPOMA.TOMPO01 A, PMOFMA.TPMOFMA02 B "
			"WHERE  A.ORDER_NO = B.ORDER_NO "
			"AND    A.PROD_CONFIG_CODE = 'CR' "
			"AND    A.ORDER_STATUS < '51' "
			"AND    B.STOCK_WT > B.LACK_WT ";
		if (order_nature.GetLength() > 0)
			sqlstr = sqlstr + "and A.ORDER_TYPE_CODE='" + order_nature + "'";
		if (ht_flag == "Y")
			sqlstr = sqlstr + "AND  A.CHAR_CODE IN (SELECT CHAR_CODE  FROM OMPOMA.TOMPO01 WHERE  PROD_CONFIG_CODE = 'CR' AND ORDER_STATUS < '51' AND  CHAR_CODE > ' ' GROUP BY CHAR_CODE HAVING  COUNT(ORDER_NO) > 1)";



		if (flag == "1") {
			//strSql1="SELECT C.MAIN_PLANT,C.MAT_NO, C.MAT_KIND,C.MAT_STATUS,C.MAT_ACT_THICK,C.MAT_ACT_WIDTH,C.MAT_ACT_WT,C.ST_NO,C.SG_SIGN,C.STOCK_NO,C.PRE_UNIT_CODE,"
			// "C.NEXT_UNIT_CODE,C.BACKLOG_PT,C.NEXT_BACKLOG_PT,C.MAT_ACT_INNER_DIA,C.PONO,C.PLAN_NO,C.CONFM_PLAN_NO,C.TRANSFER_FLAG,"
			// "C.ORDER_NO,C.WHOLE_BACKLOG,C.NEXT_WHOLE_BACKLOG_CODE, C.mat_wt,D.OVER_WT,D.ORDER_TYPE_CODE "
			strSql1 = "SELECT C.*,D.OVER_WT,D.ORDER_TYPE_CODE "
				"FROM MMCRMA.TMMCR01 C,(" + sqlstr + ") D"
				/*(SELECT B.ORDER_NO, B.WHOLE_BACKLOG ,WHOLE_BACKLOG_CODE , (B.STOCK_WT - B.LACK_WT) AS OVER_WT "
				"	    FROM   OMPOMA.TOMPO01 A, PMOFMA.TPMOFMA02 B "
				"		 WHERE  A.ORDER_NO = B.ORDER_NO "
				"		 AND    A.ORDER_STATUS < '51' "
				"AND    A.PROD_CONFIG_CODE = 'CR' "
				"		 AND    B.STOCK_WT > B.LACK_WT) D"*/
				"	WHERE C.ORDER_NO = D.ORDER_NO "
				"	AND  C.WHOLE_BACKLOG = D.WHOLE_BACKLOG"
				"	AND   C.NEXT_WHOLE_BACKLOG_CODE = D.WHOLE_BACKLOG_CODE  "
				"and C.mat_wt<= D.over_wt ";
			//" and  C.ORDER_NO ='L4BC009151'"
			//"ORDER BY C.ORDER_NO,C.WHOLE_BACKLOG,C.NEXT_WHOLE_BACKLOG_CODE, C.mat_wt"
			if (main_plant.GetLength() > 0)
				strSql1 = strSql1 + "and C.MAIN_PLANT = '" + main_plant + "' ";

		}



		if (flag == "0") {
			strSql1 = "SELECT  C.*,D.OVER_WT,D.ORDER_TYPE_CODE "
				"FROM MMHRMA.TMMHR01 C,(" + sqlstr + ") D"
				/*(SELECT B.ORDER_NO, B.WHOLE_BACKLOG ,WHOLE_BACKLOG_CODE , (B.STOCK_WT - B.LACK_WT) AS OVER_WT "
				"	    FROM   OMPOMA.TOMPO01 A, PMOFMA.TPMOFMA02 B "
				"		 WHERE  A.ORDER_NO = B.ORDER_NO "
				"		 AND    A.ORDER_STATUS < '51' "
				"AND    A.PROD_CONFIG_CODE = 'CR' "
				"		 AND    B.STOCK_WT > B.LACK_WT) D"*/
				"	WHERE C.ORDER_NO = D.ORDER_NO "
				"	AND  C.WHOLE_BACKLOG = D.WHOLE_BACKLOG"
				"	AND   C.NEXT_WHOLE_BACKLOG_CODE = D.WHOLE_BACKLOG_CODE  "
				"and C.mat_wt<= D.over_wt ";
			if (main_plant.GetLength() > 0)
				strSql1 = strSql1 + "and C.NEXT_UNIT_CODE = '" + main_plant + "' ";
			//" and  C.ORDER_NO in ('X6L0000111','X3A0000290','X3A0000289')"
			//"ORDER BY C.ORDER_NO,C.WHOLE_BACKLOG,C.NEXT_WHOLE_BACKLOG_CODE, C.mat_wt";

		}



		strSql1 = strSql1 + "ORDER BY C.ORDER_NO,C.WHOLE_BACKLOG,C.NEXT_WHOLE_BACKLOG_CODE, C.mat_wt";


		Log::Trace("", "mm00mae5_inq", "------strSql1= {0}--------", strSql1);
		cmd.SetCommandText(strSql1);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		Log::Trace("", "mm00mae5_inq", "------Table0.Count=[{0}]--------", bcls_ret->Tables[0].Rows.get_Count());
		bcls_ret->Tables.Add("table2");

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			if (i < bcls_ret->Tables[0].Rows.get_Count() - 1) {
				//Log::Trace("",__FUNCTION__,"----j=[{0}] " ,j);

				CString ORDER_NO = bcls_ret->Tables[0].Rows[i]["ORDER_NO"].ToString();
				CString ORDER_NO1 = bcls_ret->Tables[0].Rows[i + 1]["ORDER_NO"].ToString();
				CString WHOLE_BACKLOG = bcls_ret->Tables[0].Rows[i]["WHOLE_BACKLOG"].ToString();
				CString WHOLE_BACKLOG1 = bcls_ret->Tables[0].Rows[i + 1]["WHOLE_BACKLOG"].ToString();
				CString NEXT_WHOLE_BACKLOG_CODE = bcls_ret->Tables[0].Rows[i]["NEXT_WHOLE_BACKLOG_CODE"].ToString();
				CString NEXT_WHOLE_BACKLOG_CODE1 = bcls_ret->Tables[0].Rows[i + 1]["NEXT_WHOLE_BACKLOG_CODE"].ToString();
				if (ORDER_NO == ORDER_NO1 && WHOLE_BACKLOG == WHOLE_BACKLOG1 && NEXT_WHOLE_BACKLOG_CODE == NEXT_WHOLE_BACKLOG_CODE1) {
					//Log::Trace("",__FUNCTION__,"+++++++++j=[{0}] " ,j);

					MAT_WT = bcls_ret->Tables[0].Rows[i]["MAT_WT"].ToDecimal();

					if (j == 0) {
						OVER_WT = bcls_ret->Tables[0].Rows[i]["OVER_WT"].ToDecimal();

					}
					if (MAT_WT <= OVER_WT)
					{
						OVER_WT = OVER_WT - MAT_WT;

						//tmmcr01.Reset();
						tmmcr01.MergeFrom(bcls_ret->Tables[0].Rows[i]);
						tmmcr01.MergeTo(bcls_ret->Tables["table2"], false);//符合条件记录放到table2
					}
					j = 1;

				}
				else {
					//Log::Trace("",__FUNCTION__,"+++j=[{0}] " ,j);
					MAT_WT = bcls_ret->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
					if (j == 0)
						OVER_WT = bcls_ret->Tables[0].Rows[i]["OVER_WT"].ToDecimal();
					if (MAT_WT <= OVER_WT) {

						//tmmcr01.Reset();
						tmmcr01.MergeFrom(bcls_ret->Tables[0].Rows[i]);
						tmmcr01.MergeTo(bcls_ret->Tables["table2"], false);//符合条件记录放到table2
					}
					j = 0;
				}
			}
			else if (i == bcls_ret->Tables[0].Rows.get_Count() - 1) {
				MAT_WT = bcls_ret->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
				if (j == 0)
					OVER_WT = bcls_ret->Tables[0].Rows[i]["OVER_WT"].ToDecimal();
				if (MAT_WT <= OVER_WT) {


					//tmmcr01.Reset();
					tmmcr01.MergeFrom(bcls_ret->Tables[0].Rows[i]);
					tmmcr01.MergeTo(bcls_ret->Tables["table2"], false);//符合条件记录放到table2
				}
			}
		}


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		//热轧存货
		//SELECT * 
		//FROM MMHRMA.TMMHR01
		//WHERE ORDER_NO = @order_no 
		//AND   WHOLE_BACKLOG = @whole_backlog
		//AND   NEXT_WHOLE_BACKLOG_CODE = @whole_backlog_code
		//ORDER BY MAT_WT;

		//冷轧存货
		//SELECT * 
		//FROM MMCRMA.TMMCR01
		//WHERE ORDER_NO = @order_no 
		//AND   WHOLE_BACKLOG = @whole_backlog
		//AND   NEXT_WHOLE_BACKLOG_CODE = @whole_backlog_code
		//ORDER BY MAT_WT;

		//cmd.ExecuteNonQuery();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();

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

	CTimeSpan ts = CDateTime::Now() - dt1;

	Log::Trace("", "mm00mae5_inq", "------time used{0}--------", ts.TotalSeconds());
	//sprintf(s.msg, "后台时间 %f 秒, 共%d条记录",  ts.TotalSeconds(), kk);

	return(doFlag);
}

