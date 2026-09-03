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
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表
///					TMM0021 库区静态表         </para>
/// </summary>
/// <param name="TABLE_NAME">材料</param>
/// <param name="STOCK_NO">库号</param>
/// <param name="TIME">在库时间</param>
/// <returns>查询在库周期信息</returns>
===========================================================</remark>*/


#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//名称空间引用
// Service 入口
BM2F_ENTERACE(mm00sud4f1_inq)

int f_mm00sud4f1_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int i = 0;
	CString sqlstr = "";
	CString strSql1 = "";
	CString strSql2 = "";
	CDateTime dt1 = CDateTime::Now();
	//	int kk = 0;
	try
	{
		CDbCommand cmd(conn);
		CDbCommand cmdinq(conn);

		CPageInfo pageInfo;
		if (bcls_rec->Tables.Contains("PageInfo"))
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;
		}

		CString v_order_div = bcls_rec->Tables[0].Rows[0]["ORDER_DIV"].ToString().Trim();//有无合同区分
		CString v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		CString v_stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		CString v_time = bcls_rec->Tables[0].Rows[0]["TIME"].ToString().Trim();

		//v_table_name
		if (v_table_name == "板坯")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMSM01 WHERE SLAB_CUT_TIME > @time1 AND SLAB_CUT_TIME <= @time2 ";
			strSql2 = " SELECT * FROM TMMSM01 WHERE SLAB_CUT_TIME > @time1 AND SLAB_CUT_TIME <= @time2 ";

			if (v_stock_no == "板坯库")
			{
				strSql1 += " AND MAT_LINE_TYPE = 'SM' ";
				strSql2 += " AND MAT_LINE_TYPE = 'SM' ";
			}
		}

		else if (v_table_name == "热轧")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE DUMMY_COIL_FLAG = '0' "
				" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
			strSql2 = " SELECT * FROM TMMHR01 WHERE DUMMY_COIL_FLAG = '0' "
				" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";

			if (v_stock_no == "钢卷库")
			{
				strSql1 += " AND MAT_LINE_TYPE = 'HR' AND PRODUCT_FLAG = '0' ";
				strSql2 += " AND MAT_LINE_TYPE = 'HR' AND PRODUCT_FLAG = '0' ";
			}
			else if (v_stock_no == "成品库")
			{
				strSql1 += " AND MAT_LINE_TYPE = 'HR' AND PRODUCT_FLAG = '1' AND MAT_STATUS <> '36' ";
				strSql2 += " AND MAT_LINE_TYPE = 'HR' AND PRODUCT_FLAG = '1' AND MAT_STATUS <> '36' ";
			}
		}

		else if (v_table_name == "冷轧")
		{
			if (v_stock_no == "原料库")
			{
				strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE DUMMY_COIL_FLAG = '0' AND MAT_LINE_TYPE = 'CR' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
				strSql2 = " SELECT * FROM TMMHR01 WHERE DUMMY_COIL_FLAG = '0' AND MAT_LINE_TYPE = 'CR' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
			}

			else if (v_stock_no == "中间库")
			{
				strSql1 = " SELECT COUNT(1) FROM TMMCR01 WHERE DUMMY_COIL_FLAG = '0' "
					" AND MAT_LINE_TYPE = 'CR'  "
					" AND PRODUCT_FLAG = '0' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
				strSql2 = " SELECT * FROM TMMCR01 WHERE DUMMY_COIL_FLAG = '0' "
					" AND MAT_LINE_TYPE = 'CR' "
					" AND PRODUCT_FLAG = '0' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 ";
			}
			else if (v_stock_no == "成品库")
			{
				strSql1 = " SELECT COUNT(1) FROM TMMCR01 WHERE DUMMY_COIL_FLAG = '0' "
					" AND MAT_LINE_TYPE = 'CR'  "
					" AND PRODUCT_FLAG = '1' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 "
					" AND MAT_STATUS <> '27' ";
				strSql2 = " SELECT * FROM TMMCR01 WHERE DUMMY_COIL_FLAG = '0' "
					" AND MAT_LINE_TYPE = 'CR'  "
					" AND PRODUCT_FLAG = '1' "
					" AND PROD_TIME > @time1 AND PROD_TIME <= @time2 "
					" AND MAT_STATUS <> '36' ";
			}
		}

		//v_order_div
		if (v_order_div.Trim() == "1")//有合同
		{
			strSql1 += " AND ORDER_NO > ' ' ";
			strSql2 += " AND ORDER_NO > ' ' ";
		}
		if (v_order_div.Trim() == "0")//无合同
		{
			strSql1 += " AND ORDER_NO = ' ' ";
			strSql2 += " AND ORDER_NO = ' ' ";
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
		if (v_time == "1天")
		{
			cmd.Parameters.Set("time1", date_1);
			cmd.Parameters.Set("time2", date_0);
			cmdinq.Parameters.Set("time1", date_1);
			cmdinq.Parameters.Set("time2", date_0);
		}
		else if (v_time == "2天")
		{
			cmd.Parameters.Set("time1", date_2);
			cmd.Parameters.Set("time2", date_1);
			cmdinq.Parameters.Set("time1", date_2);
			cmdinq.Parameters.Set("time2", date_1);
		}
		else if (v_time == "3天")
		{
			cmd.Parameters.Set("time1", date_3);
			cmd.Parameters.Set("time2", date_2);
			cmdinq.Parameters.Set("time1", date_3);
			cmdinq.Parameters.Set("time2", date_2);
		}
		else if (v_time == "4天")
		{
			cmd.Parameters.Set("time1", date_4);
			cmd.Parameters.Set("time2", date_3);
			cmdinq.Parameters.Set("time1", date_4);
			cmdinq.Parameters.Set("time2", date_3);
		}
		else if (v_time == "5天")
		{
			cmd.Parameters.Set("time1", date_5);
			cmd.Parameters.Set("time2", date_4);
			cmdinq.Parameters.Set("time1", date_5);
			cmdinq.Parameters.Set("time2", date_4);
		}
		else if (v_time == "6天")
		{
			cmd.Parameters.Set("time1", date_6);
			cmd.Parameters.Set("time2", date_5);
			cmdinq.Parameters.Set("time1", date_6);
			cmdinq.Parameters.Set("time2", date_5);
		}
		else if (v_time == "7天")
		{
			cmd.Parameters.Set("time1", date_7);
			cmd.Parameters.Set("time2", date_6);
			cmdinq.Parameters.Set("time1", date_7);
			cmdinq.Parameters.Set("time2", date_6);
		}
		else if (v_time == "8-14天")
		{
			cmd.Parameters.Set("time1", date_14);
			cmd.Parameters.Set("time2", date_7);
			cmdinq.Parameters.Set("time1", date_14);
			cmdinq.Parameters.Set("time2", date_7);
		}
		else if (v_time == "15天-1月")
		{
			cmd.Parameters.Set("time1", date_30);
			cmd.Parameters.Set("time2", date_14);
			cmdinq.Parameters.Set("time1", date_30);
			cmdinq.Parameters.Set("time2", date_14);
		}
		else if (v_time == "1月-3月")
		{
			cmd.Parameters.Set("time1", date_90);
			cmd.Parameters.Set("time2", date_30);
			cmdinq.Parameters.Set("time1", date_90);
			cmdinq.Parameters.Set("time2", date_30);
		}
		else if (v_time == "3月-6月")
		{
			cmd.Parameters.Set("time1", date_180);
			cmd.Parameters.Set("time2", date_90);
			cmdinq.Parameters.Set("time1", date_180);
			cmdinq.Parameters.Set("time2", date_90);
		}
		else if (v_time == "6月以上")
		{
			cmd.Parameters.Set("time1", "00000000000000");
			cmd.Parameters.Set("time2", date_180);
			cmdinq.Parameters.Set("time1", "00000000000000");
			cmdinq.Parameters.Set("time2", date_180);
		}
		else if (v_time == "合计")
		{
			cmd.Parameters.Set("time1", "00000000000000");
			cmd.Parameters.Set("time2", date_0);
			cmdinq.Parameters.Set("time1", "00000000000000");
			cmdinq.Parameters.Set("time2", date_0);
		}


		Log::Trace("", "mm00sud4f1_inq", "------strSql1{0}--------", (const char*)strSql1);
		cmd.SetCommandText(strSql1);
		CDecimal nRecCount = cmd.ExecuteScalar();//总记录数
		cmd.Close();

		//查询数据并返回前台
		/*此方法返回单行单列数据*/
//			strSql2 = "SELECT * FROM  ";

//		cmdinq.Parameters.Set("", );


		cmdinq.SetCommandText(strSql2);

		Log::Trace("", "mm00sud4f1_inq", "------nRecCount{0}--------", nRecCount.ToInt32());

		cmdinq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmdinq.Close();

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "total_count");
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["total_count"] = nRecCount;

		{
			CFormattable arguments[] = { nRecCount };

			CMessageFormat::Format(s.msg, _RES("GCRSS0000004")/*查询到[{0}]条记录。*/, arguments, 1);
		}

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

	Log::Trace("MMCRMA", "mm00sud4f1_inq", "------time used{0}--------", ts.TotalSeconds());
	//sprintf(s.msg, "后台时间 %f 秒, 共%d条记录",  ts.TotalSeconds(), kk);

	return(doFlag);
}

