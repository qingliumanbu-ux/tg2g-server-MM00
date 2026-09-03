/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      KE1809
Version:     1.0
Date:        2022-07-05 16:33:43
Description: 查询各机组封锁量信息明细
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 查询各机组封锁量信息明细
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表         </para>
/// </summary>
/// <param name="TABLE_NAME">物料形态</param>
/// <param name="UNIT_CODE">产出机组</param>
/// <param name="MAT_STATUS">材料状态</param>
/// <returns>查询各机组封锁量信息明细</returns>
===========================================================</remark>*/


#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//名称空间引用
// Service 入口
BM2F_ENTERACE(mm00sud1f1_inq)

int f_mm00sud1f1_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
		CDecimal v_hold_time_from = bcls_rec->Tables[0].Rows[0]["HOLD_TIME_FROM"];//封锁时间起
		CDecimal v_hold_time_to = bcls_rec->Tables[0].Rows[0]["HOLD_TIME_TO"];//封锁时间止
		CString v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();//存储厂别
		CString v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();//物料形态
		CString v_unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim();//下游/返修机组
		CString v_mat_status = bcls_rec->Tables[0].Rows[0]["MAT_STATUS"].ToString().Trim();//材料状态

		if (v_table_name == "板坯")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMSM01 WHERE 1=1 ";
			strSql2 = " SELECT * FROM TMMSM01 WHERE 1=1 ";

			if (v_factory_div == "一炼钢")
			{
				strSql1 += " AND FACTORY_STORE = 'A10' ";
				strSql2 += " AND FACTORY_STORE = 'A10'  ";
			}
			else if (v_factory_div == "二炼钢")
			{
				strSql1 += " AND FACTORY_STORE = 'A20' ";
				strSql2 += " AND FACTORY_STORE = 'A20'  ";
			}
			else if (v_factory_div == "一热轧")
			{
				strSql1 += " AND FACTORY_STORE = 'H10' ";
				strSql2 += " AND FACTORY_STORE = 'H10'  ";
			}
			else if (v_factory_div == "二热轧")
			{
				strSql1 += " AND FACTORY_STORE = 'HA0' ";
				strSql2 += " AND FACTORY_STORE = 'HA0'  ";
			}
			else if (v_factory_div == "连铸连轧")
			{
				strSql1 += " AND FACTORY_STORE = 'E10' ";
				strSql2 += " AND FACTORY_STORE = 'E10'  ";
			}
			strSql1 += " AND DEST_FIN = @unit_code AND MAT_STATUS = @mat_status ";
			strSql2 += " AND DEST_FIN = @unit_code AND MAT_STATUS = @mat_status ";
		}
		else if (v_table_name == "热卷")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE 1=1 ";
			strSql2 = " SELECT * FROM TMMHR01 WHERE 1=1 ";

			if (v_factory_div == "一热轧")
			{
				strSql1 += " AND FACTORY_STORE = 'H10' ";
				strSql2 += " AND FACTORY_STORE = 'H10' ";
			}
			else if (v_factory_div == "二热轧")
			{
				strSql1 += " AND FACTORY_STORE = 'HA0' ";
				strSql2 += " AND FACTORY_STORE = 'HA0' ";
			}
			else if (v_factory_div == "连铸连轧")
			{
				strSql1 += " AND FACTORY_STORE = 'E10' ";
				strSql2 += " AND FACTORY_STORE = 'E10' ";
			}
			else if (v_factory_div == "冷轧厂")
			{
				strSql1 += " AND FACTORY_STORE = 'C10' ";
				strSql2 += " AND FACTORY_STORE = 'C10' ";
			}
			//if (v_mat_status == "03" || v_mat_status == "13")
			//{
			//	strSql1 += " AND HSF_UNIT_REPAIR = @unit_code ";
			//	strSql2 += " AND HSF_UNIT_REPAIR = @unit_code ";
			//}
			//else
			{
				strSql1 += " AND NEXT_UNIT_CODE = @unit_code ";
				strSql2 += " AND NEXT_UNIT_CODE = @unit_code ";
			}

			strSql1 += " AND MAT_STATUS = @mat_status ";
			strSql2 += " AND MAT_STATUS = @mat_status ";


		}
		else if (v_table_name == "冷卷")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMCR01 WHERE 1=1 ";
			strSql2 = " SELECT * FROM TMMCR01 WHERE 1=1 ";

			if (v_factory_div == "冷轧")
			{
				strSql1 += " AND FACTORY_STORE = 'C10' ";
				strSql2 += " AND FACTORY_STORE = 'C10' ";
			}
			//if (v_mat_status == "03" || v_mat_status == "13")
			//{
			//	strSql1 += " AND REPAIR_UNIT_CODE = @unit_code ";
			//	strSql2 += " AND REPAIR_UNIT_CODE = @unit_code ";
			//}
			//else
			{
				strSql1 += " AND NEXT_UNIT_CODE = @unit_code ";
				strSql2 += " AND NEXT_UNIT_CODE = @unit_code ";
			}

			strSql1 += " AND MAT_STATUS = @mat_status ";
			strSql2 += " AND MAT_STATUS = @mat_status ";

		}
		/*else if (v_table_name == "硅钢")
		{
			strSql1 = " SELECT COUNT(1) FROM TMMSI01 WHERE 1=1 ";
			strSql2 = " SELECT * FROM TMMSI01 WHERE 1=1 ";

			if (v_factory_div == "一热轧")
			{
				strSql1 += " AND FACTORY_DIV = 'RZ1' ";
				strSql2 += " AND FACTORY_DIV = 'RZ1' ";
			}
			else if (v_factory_div == "二热轧")
			{
				strSql1 += " AND FACTORY_DIV = 'RZ2' ";
				strSql2 += " AND FACTORY_DIV = 'RZ2' ";
			}
			else if (v_factory_div == "三热轧")
			{
				strSql1 += " AND FACTORY_DIV = 'RZ3' ";
				strSql2 += " AND FACTORY_DIV = 'RZ3' ";
			}
			else if (v_factory_div == "四热轧")
			{
				strSql1 += " AND FACTORY_DIV = 'RZ4' ";
				strSql2 += " AND FACTORY_DIV = 'RZ4' ";
			}
			if (v_mat_status == "03" || v_mat_status == "13")
			{
				strSql1 += " AND REPAIR_UNIT_CODE = @unit_code ";
				strSql2 += " AND REPAIR_UNIT_CODE = @unit_code ";
			}
			else
			{
				strSql1 += " AND NEXT_UNIT_CODE = @unit_code ";
				strSql2 += " AND NEXT_UNIT_CODE = @unit_code ";
			}

			strSql1 += " AND MAT_STATUS = @mat_status ";
			strSql2 += " AND MAT_STATUS = @mat_status ";
		}*/
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

		if (v_hold_time_from.ToDouble() > 0)
		{
			strSql1 += " AND HOLD_TIME < @sta_chg_time_from ";
			strSql2 += " AND HOLD_TIME < @sta_chg_time_from ";
			cmd.Parameters.Set("sta_chg_time_from", BM2::CDateTime::Now().AddDays(-v_hold_time_from.ToDouble()).ToString("yyyyMMddHHmmss"));
			cmdinq.Parameters.Set("sta_chg_time_from", BM2::CDateTime::Now().AddDays(-v_hold_time_from.ToDouble()).ToString("yyyyMMddHHmmss"));
		}

		if (v_hold_time_to.ToDouble() > 0)
		{
			strSql1 += " AND HOLD_TIME > @sta_chg_time_to ";
			strSql2 += " AND HOLD_TIME > @sta_chg_time_to ";
			cmd.Parameters.Set("sta_chg_time_to", BM2::CDateTime::Now().AddDays(-v_hold_time_to.ToDouble()).ToString("yyyyMMddHHmmss"));
			cmdinq.Parameters.Set("sta_chg_time_to", BM2::CDateTime::Now().AddDays(-v_hold_time_to.ToDouble()).ToString("yyyyMMddHHmmss"));
		}

		cmd.Parameters.Set("unit_code", v_unit_code);
		cmd.Parameters.Set("mat_status", v_mat_status);
		cmdinq.Parameters.Set("unit_code", v_unit_code);
		cmdinq.Parameters.Set("mat_status", v_mat_status);

		Log::Trace("", "mm00sud1f1_inq", "------strSql1{0}--------", (const char*)strSql1);
		cmd.SetCommandText(strSql1);
		CDecimal nRecCount = cmd.ExecuteScalar();//总记录数


		//查询数据并返回前台
		/*此方法返回单行单列数据*/
		//			strSql2 = "SELECT * FROM  ";

		//		cmdinq.Parameters.Set("", );

		cmdinq.SetCommandText(strSql2);
		Log::Trace("MM00", "mm00sud1f1_inq", "------nRecCount{0}--------", nRecCount.ToInt32());

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

	Log::Trace("MMCRMA", "mm00mad1_in2", "------time used{0}--------", ts.TotalSeconds());
	//sprintf(s.msg, "后台时间 %f 秒, 共%d条记录",  ts.TotalSeconds(), kk);

	return(doFlag);
}

