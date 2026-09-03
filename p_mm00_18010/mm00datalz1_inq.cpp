/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      971280
Version:     1.0
Date:        2021-5-24 14:59:50
Description: 数据切换后对账--冷轧存货对账
**************************************************/

#include "stdafx.h" 

BM2F_ENTERACE(mm00datalz1_inq)


int f_mm00datalz1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);


	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd(conn);
	CString v_tab_index = "";

	try
	{

		v_tab_index = bcls_rec->Tables[0].Rows[0][0].ToString();

		Log::Trace("", __FUNCTION__, "------v_tab_index = [{0}]-------", v_tab_index);

		if (v_tab_index == "1")
		{
			sqlstr = " SELECT t1.item_name, "
				" (sum(t1.num_1)+sum(t1.num_2)+sum(t1.num_3)+sum(t1.num_4)+sum(t1.num_5)+sum(t1.num_6)) as num_1,"
				" cast((sum(t1.wt_1)+sum(t1.wt_2)+sum(t1.wt_3)+sum(t1.wt_4)+sum(t1.wt_5)+sum(t1.wt_6)) as decimal(20,3)) as wt_1,"
				" (sum(t1.num_13)+sum(t1.num_14)+sum(t1.num_15)+sum(t1.num_16)+sum(t1.num_17)+sum(t1.num_18)) as num_8,"
				" cast((sum(t1.wt_13)+sum(t1.wt_14)+sum(t1.wt_15)+sum(t1.wt_16)+sum(t1.wt_17)+sum(t1.wt_18)) as decimal(20,3)) as wt_8,"
				" sum(t1.num_7) as num_2, cast(sum(t1.wt_7) as decimal(20,3)) as wt_2,"
				" sum(t1.num_12) as num_3, cast(sum(t1.wt_12) as decimal(20,3)) as wt_3 ,"
				" sum(t1.num_8) as num_4, cast(sum(t1.wt_8) as decimal(20,3)) as wt_4 ,"
				" sum(t1.num_9) as num_5, cast(sum(t1.wt_9) as decimal(20,3)) as wt_5 FROM "
				" (SELECT SG_SIGN ITEM_NAME,COUNT(1) NUM_1,SUM(MAT_WT) WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,"
				" 0 NUM_6,0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='1' GROUP BY SG_SIGN UNION ALL "

				" SELECT SG_SIGN ITEM_NAME, 0 NUM_1, 0 WT_1, 0 NUM_2, 0 WT_2, 0 NUM_3, 0 WT_3, 0 NUM_4, 0 WT_4, 0 NUM_5, 0"
				" WT_5, 0 NUM_6, 0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"COUNT(1) NUM_13,SUM(MAT_WT) WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='3' GROUP BY SG_SIGN UNION ALL "

				" SELECT SG_SIGN ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, COUNT(1)"
				" NUM_7, SUM(MAT_WT) WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12,"
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='3' GROUP BY SG_SIGN UNION ALL "

				" SELECT SG_SIGN ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,COUNT(1) NUM_8,SUM(MAT_WT) WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY SG_SIGN  UNION ALL "

				" SELECT SG_SIGN ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,0 NUM_8,0 WT_8,COUNT(1) NUM_9,SUM(MAT_WT) WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY SG_SIGN  UNION ALL "

				" SELECT SG_SIGN ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0 "
				" NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,COUNT(1) NUM_12,SUM(MAT_WT) WT_12,  "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='1'  GROUP BY SG_SIGN "
				") t1 group by t1.item_name ";
		}
		else if (v_tab_index == "2")
		{
			sqlstr = " SELECT t1.item_name, "
				" (sum(t1.num_1)+sum(t1.num_2)+sum(t1.num_3)+sum(t1.num_4)+sum(t1.num_5)+sum(t1.num_6)) as num_1,"
				" cast((sum(t1.wt_1)+sum(t1.wt_2)+sum(t1.wt_3)+sum(t1.wt_4)+sum(t1.wt_5)+sum(t1.wt_6)) as decimal(20,3)) as wt_1,"
				" (sum(t1.num_13)+sum(t1.num_14)+sum(t1.num_15)+sum(t1.num_16)+sum(t1.num_17)+sum(t1.num_18)) as num_8,"
				" cast((sum(t1.wt_13)+sum(t1.wt_14)+sum(t1.wt_15)+sum(t1.wt_16)+sum(t1.wt_17)+sum(t1.wt_18)) as decimal(20,3)) as wt_8,"
				" sum(t1.num_7) as num_2, cast(sum(t1.wt_7) as decimal(20,3)) as wt_2,"
				" sum(t1.num_12) as num_3, cast(sum(t1.wt_12) as decimal(20,3)) as wt_3 ,"
				" sum(t1.num_8) as num_4, cast(sum(t1.wt_8) as decimal(20,3)) as wt_4 ,"
				" sum(t1.num_9) as num_5, cast(sum(t1.wt_9) as decimal(20,3)) as wt_5 FROM "
				" (SELECT STOCK_NO ITEM_NAME,COUNT(1) NUM_1,SUM(MAT_WT) WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,"
				" 0 NUM_6,0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='1' GROUP BY STOCK_NO UNION ALL "

				" SELECT STOCK_NO ITEM_NAME, 0 NUM_1, 0 WT_1, 0 NUM_2, 0 WT_2, 0 NUM_3, 0 WT_3, 0 NUM_4, 0 WT_4, 0 NUM_5, 0"
				" WT_5, 0 NUM_6, 0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"COUNT(1) NUM_13,SUM(MAT_WT) WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='3' GROUP BY STOCK_NO UNION ALL "

				" SELECT STOCK_NO ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, COUNT(1)"
				" NUM_7, SUM(MAT_WT) WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12,"
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='3' GROUP BY STOCK_NO UNION ALL "

				" SELECT STOCK_NO ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,COUNT(1) NUM_8,SUM(MAT_WT) WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY STOCK_NO  UNION ALL "

				" SELECT STOCK_NO ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,0 NUM_8,0 WT_8,COUNT(1) NUM_9,SUM(MAT_WT) WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY STOCK_NO  UNION ALL "

				" SELECT STOCK_NO ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0 "
				" NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,COUNT(1) NUM_12,SUM(MAT_WT) WT_12,  "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='1'  GROUP BY STOCK_NO "
				") t1 group by t1.item_name ";
		}
		else if (v_tab_index == "3")
		{
			sqlstr = " SELECT t1.item_name, "
				" (sum(t1.num_1)+sum(t1.num_2)+sum(t1.num_3)+sum(t1.num_4)+sum(t1.num_5)+sum(t1.num_6)) as num_1,"
				" cast((sum(t1.wt_1)+sum(t1.wt_2)+sum(t1.wt_3)+sum(t1.wt_4)+sum(t1.wt_5)+sum(t1.wt_6)) as decimal(20,3)) as wt_1,"
				" (sum(t1.num_13)+sum(t1.num_14)+sum(t1.num_15)+sum(t1.num_16)+sum(t1.num_17)+sum(t1.num_18)) as num_8,"
				" cast((sum(t1.wt_13)+sum(t1.wt_14)+sum(t1.wt_15)+sum(t1.wt_16)+sum(t1.wt_17)+sum(t1.wt_18)) as decimal(20,3)) as wt_8,"
				" sum(t1.num_7) as num_2, cast(sum(t1.wt_7) as decimal(20,3)) as wt_2,"
				" sum(t1.num_12) as num_3, cast(sum(t1.wt_12) as decimal(20,3)) as wt_3 ,"
				" sum(t1.num_8) as num_4, cast(sum(t1.wt_8) as decimal(20,3)) as wt_4 ,"
				" sum(t1.num_9) as num_5, cast(sum(t1.wt_9) as decimal(20,3)) as wt_5 FROM "
				" (SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME,COUNT(1) NUM_1,SUM(MAT_WT) WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,"
				" 0 NUM_6,0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='1' GROUP BY NEXT_WHOLE_BACKLOG_CODE UNION ALL "

				" SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME, 0 NUM_1, 0 WT_1, 0 NUM_2, 0 WT_2, 0 NUM_3, 0 WT_3, 0 NUM_4, 0 WT_4, 0 NUM_5, 0"
				" WT_5, 0 NUM_6, 0 WT_6,0 NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"COUNT(1) NUM_13,SUM(MAT_WT) WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMM00CRQC WHERE MAT_SHAPE_FLAG ='3' GROUP BY NEXT_WHOLE_BACKLOG_CODE UNION ALL "

				" SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, COUNT(1)"
				" NUM_7, SUM(MAT_WT) WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12,"
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='3' GROUP BY NEXT_WHOLE_BACKLOG_CODE UNION ALL "

				" SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,COUNT(1) NUM_8,SUM(MAT_WT) WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY NEXT_WHOLE_BACKLOG_CODE  UNION ALL "

				" SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0"
				" NUM_7, 0 WT_7,0 NUM_8,0 WT_8,COUNT(1) NUM_9,SUM(MAT_WT) WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,0 NUM_12,0 WT_12, "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_PES WHERE MAT_SHAPE_FLAG ='3' GROUP BY NEXT_WHOLE_BACKLOG_CODE  UNION ALL "

				" SELECT NEXT_WHOLE_BACKLOG_CODE ITEM_NAME,0 NUM_1,0 WT_1,0 NUM_2,0 WT_2,0 NUM_3,0 WT_3,0 NUM_4,0 WT_4,0 NUM_5,0 WT_5,0 NUM_6,0 WT_6, 0 "
				" NUM_7,0 WT_7,0 NUM_8,0 WT_8,0 NUM_9,0 WT_9,0 NUM_10,0 WT_10,0 NUM_11,0 WT_11,COUNT(1) NUM_12,SUM(MAT_WT) WT_12,  "
				"0 NUM_13,0 WT_13,0 NUM_14,0 WT_14,0 NUM_15,0 WT_15,0 NUM_16,0 WT_16,0 NUM_17,0 WT_17,0 NUM_18,0 WT_18 "
				" FROM TMMCR01_QC WHERE MAT_SHAPE_FLAG ='1'  GROUP BY NEXT_WHOLE_BACKLOG_CODE "
				") t1 group by t1.item_name ";
		}

		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		cmd.Close();

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


