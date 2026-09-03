/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 叶德清
日期: 2018-09-17
功能: 合同跟踪材料清单查询
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/

#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 合同跟踪材料清单画面
/// <para>根据查询条件查询冷轧主档TMMCR01-热轧物料主档TMMHR01</para>
/// </summary>
/// <returns>满足查询条件的薄板材料数据</returns>
===========================================================</remark>*/

BM2F_ENTERACE(mm00sud5f2_inq)

int f_mm00sud5f2_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义

	//程序用变量
	int doFlag = 0;
	int rowCount = 0;

	//业务用变量
	CString v_order_no_group = " ";
	CString v_backlog_point_group = " ";
	CString v_backlog = " ";
	CString v_order_no = " ";
	CString v_next_backlog_pos = " ";
	CString v_hot_coil_no = "";
	//数据库SQL操作字符串
	CString sqlstr = "";

	//数据库操作类定义 
	CDbCommand cmd_inq(conn);

	try
	{
		//获取传入参数
		v_order_no_group = bcls_rec->Tables[0].Rows[0]["ORDER_NO_GROUP"].ToString().Trim();
		v_backlog_point_group = bcls_rec->Tables[0].Rows[0]["BACKLOG_POINT_GROUP"].ToString().Trim();

		//查询冷轧物料档
		sqlstr = "SELECT MAT_NO, "						//材料号
			"		 MAT_STATUS, "					//材料状态
			"		 MAT_ACT_THICK, "				//材料实际厚度
			"		 MAT_ACT_WIDTH, "				//材料实际宽度
			"		 MAT_WT MAT_ACT_WT, "			//材料重量
			"		 STOCK_NO, "					//库区号
			"		 ORDER_NO, "					//合同号
			"		 PRE_UNIT_CODE, "				//前机组代码
			"		 NEXT_UNIT_CODE, "				//后机组代码
			"		 BACKLOG_PT, "					//工序指针
			"		 NEXT_BACKLOG_PT, "				//下工序指针
			"		 ST_NO, "						//出钢记号
			"		 SG_SIGN, "						//牌号（钢级）
			"		 PONO, "						//制造命令号
			"		 PLAN_NO, "						//计划号
			"		 CONFM_PLAN_NO, "				//准发计划号
			"		 TRANSFER_FLAG, "				//转库计划标记
			"		 REPAIR_UNIT_CODE, "			//返修机组代码
			"		 PROD_END_TIME, "				//生产结束时间
			"		 PACK_TYPE_CODE, "				//包装类型代码
			"		 MAT_KIND, "					//物料类型
			"		 MAT_ACT_INNER_DIA, "			//材料实际内径
			"		 PCH_JUDGE_CODE, "				//性能判定代码
			"       SURFACE_DECIDE_CODE, "			//表面判定代码
			"		 COMPLEX_DECIDE_CODE, "			//综合判定代码
			"		 BACKLOG, "
			"		HOT_COIL_NO1 AS HOT_COIL_NO, "
			"		'' AS HOT_ROLL_TIME,"
			"		'' AS IN_STOCK_COLD_TIME,"
			"       '' SAMPLE_LOT_STATUS,"
			"       '' SAMPLE_LOT_NO"
			"  FROM TMMCR01 "
			" WHERE ORDER_NO IN (" + v_order_no_group + ") ";
		if (!v_backlog_point_group.Trim().IsEmpty())
		{
			sqlstr += "	 AND LPAD(TO_CHAR(NEXT_BACKLOG_PT), 2, '0') IN (" + v_backlog_point_group + ")";
		}
		sqlstr += " ORDER BY MAT_NO ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		//查询热轧物料档
		rowCount = bcls_ret->Tables[0].Rows.get_Count();
		bcls_ret->Tables.Add();

		sqlstr = "SELECT A.MAT_NO, "						//材料号
			"		 A.MAT_STATUS, "					//材料状态
			"		 A.MAT_ACT_THICK, "				//材料实际厚度
			"		 A.MAT_ACT_WIDTH, "				//材料实际宽度
			"		 A.MAT_WT MAT_ACT_WT, "			//材料重量
			"		 A.STOCK_NO, "					//库区号
			"		 A.ORDER_NO, "					//合同号
			"		 A.PRE_UNIT_CODE, "				//前机组代码
			"		 A.NEXT_UNIT_CODE, "				//后机组代码
			"		 A.BACKLOG_PT, "					//工序指针
			"		 A.NEXT_BACKLOG_PT, "				//下工序指针
			"		 A.ST_NO, "						//出钢记号
			"		 A.SG_SIGN, "						//牌号（钢级）
			"		 A.PONO, "						//制造命令号
			"		 A.PLAN_NO, "						//计划号
			"		 A.CONFM_PLAN_NO, "				//准发计划号
			"		 A.TRANSFER_FLAG, "				//转库计划标记
			"		 A.HSF_UNIT_REPAIR REPAIR_UNIT_CODE, "	//精整返修机组代码——冷轧：REPAIR_UNIT_CODE			//返修机组代码
			"		 A.PROD_END_TIME, "				//生产结束时间
			"		 A.PACK_TYPE_CODE, "				//包装类型代码
			"		 A.MAT_KIND, "					//物料类型
			"		 A.MAT_ACT_INNER_DIA, "			//材料实际内径
			"		 A.PCH_JUDGE_CODE, "				//性能判定代码
			"       A.SURFACE_DECIDE_CODE, "			//表面判定代码
			"		 A.COMPLEX_DECIDE_CODE, "			//综合判定代码
			"		 A.BACKLOG, "
			"		HOT_COIL_NO, "
			"		PROD_END_TIME AS HOT_ROLL_TIME,"
			"		IN_STOCK_COLD_TIME,"
			"       C.SAMPLE_LOT_STATUS,"				//20131022杜毅明要求增加试样状态
			"       C.SAMPLE_LOT_NO"				//20180510杜毅明要求增加试样状态
			"  FROM TMMHR01 A LEFT JOIN TQMTQMAV2 B ON A.MAT_NO = B.MAT_NO LEFT JOIN TQMTQMAV1 C ON B.SAMPLE_LOT_NO = C.SAMPLE_LOT_NO "
			" WHERE A.ORDER_NO IN (" + v_order_no_group + ") ";
		if (!v_backlog_point_group.Trim().IsEmpty())
		{
			sqlstr += "	 AND LPAD(TO_CHAR(A.NEXT_BACKLOG_PT), 2, '0') IN (" + v_backlog_point_group + ")";
		}
		sqlstr += " ORDER BY A.MAT_NO ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);

		//将取出的热轧数据与之前的表合起来
		for (int i = 0; i < bcls_ret->Tables[1].Rows.get_Count(); i++)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[rowCount].Merge(bcls_ret->Tables[1].Rows[i]);
			rowCount++;
		}

		//清空热轧数据表
		bcls_ret->Tables[1].Clear();

		//查询板坯物料档
		sqlstr = "SELECT MAT_NO, "						//材料号
			"		 MAT_STATUS, "					//材料状态
			"		 MAT_ACT_THICK, "				//材料实际厚度
			"		 MAT_ACT_WIDTH, "				//材料实际宽度
			"		 MAT_ACT_WT, "					//材料实际重量
			"		 STOCK_NO, "					//库区号
			"		 ORDER_NO, "					//合同号
			"		 PRE_UNIT_CODE, "				//前机组代码
			"		 NEXT_UNIT_CODE, "				//后机组代码
			"		 BACKLOG_PT, "					//工序指针
			"		 NEXT_BACKLOG_PT, "				//下工序指针
			"		 ST_NO, "						//出钢记号
			"		 SG_SIGN, "						//牌号（钢级）
			"		 PONO, "						//制造命令号
			"		 PLAN_NO, "						//计划号
			"		 CONFM_PLAN_NO, "				//准发计划号
			"		 TRANSFER_FLAG, "				//转库计划标记
			"		 ' ' REPAIR_UNIT_CODE, "		//返修机组代码——冷轧：REPAIR_UNIT_CODE			//返修机组代码
			"		 SLAB_CUT_TIME PROD_END_TIME, "	//板坯切断时刻——冷轧：PROD_END_TIME				//生产结束时间
			"		 ' ' PACK_TYPE_CODE, "			//包装类型代码
			"		 MAT_KIND, "					//物料类型
			"		 0 MAT_ACT_INNER_DIA, "			//材料实际内径
			"		 ' ' PCH_JUDGE_CODE, "			//性能判定代码
			"       SURFACE_DECIDE_CODE, "			//表面判定代码
			"		 COMPLEX_DECIDE_CODE, "			//综合判定代码
			"		 BACKLOG, "
			"		'' AS HOT_COIL_NO, "
			"		'' AS HOT_ROLL_TIME,"
			"		'' AS IN_STOCK_COLD_TIME,"
			"       '' SAMPLE_LOT_STATUS,"
			"       '' SAMPLE_LOT_NO"
			"  FROM TMMSM01 "
			" WHERE ORDER_NO IN (" + v_order_no_group + ") ";
		if (!v_backlog_point_group.Trim().IsEmpty())
		{
			sqlstr += "	 AND LPAD(TO_CHAR(NEXT_BACKLOG_PT), 2, '0') IN (" + v_backlog_point_group + ")";
		}
		sqlstr += " ORDER BY MAT_NO ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);

		//将取出的板坯数据与之前的表合起来
		for (int i = 0; i < bcls_ret->Tables[1].Rows.get_Count(); i++)
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[rowCount].Merge(bcls_ret->Tables[1].Rows[i]);
			rowCount++;
		}

		//清空板坯数据表
		bcls_ret->Tables[1].Clear();

		//取合同主档数据
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DELIVY_WEEK_FLAG");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ORDER_DELIVERY_DATE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SORT_GRADE_CODE_F");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LACK_WT");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LACK_WT_19");

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			v_order_no = bcls_ret->Tables[0].Rows[i]["ORDER_NO"].ToString().Trim();
			v_backlog = bcls_ret->Tables[0].Rows[i]["BACKLOG"].ToString().Trim();
			v_next_backlog_pos = bcls_ret->Tables[0].Rows[i]["NEXT_BACKLOG_PT"].ToString().Trim();
			//崔先雄要求增加热轧产出时间和热卷入冷轧库时间显示列
			v_hot_coil_no = bcls_ret->Tables[0].Rows[i]["HOT_COIL_NO"].ToString().Trim();
			//查询合同主档
			sqlstr = "SELECT DELIVY_WEEK_FLAG, "			//按周交货标记
				"		 DELIVY_DATE, "			//合同交货期
				"		 SORT_GRADE_CODE_F "			//材料分选度
				"  FROM TOMPO01 "
				" WHERE ORDER_NO = @v_order_no ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_order_no", v_order_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[i]["DELIVY_WEEK_FLAG"] = cmd_inq.GetString(1);	//按周交货标记
				bcls_ret->Tables[0].Rows[i]["ORDER_DELIVERY_DATE"] = cmd_inq.GetString(2);	//交货期
				bcls_ret->Tables[0].Rows[i]["SORT_GRADE_CODE_F"] = cmd_inq.GetString(3);	//分选度
			}
			cmd_inq.Close();

			//查询下工序欠量
			sqlstr = "SELECT NVL(LACK_WT, 0) "
				"  FROM TPMOF02 "
				" WHERE ORDER_NO = @v_order_no "
				"   AND BACKLOG  = @v_backlog "
				"   AND BACKLOG_POS = LPAD(@v_next_backlog_pos, 2, '0') ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_order_no", v_order_no);
			cmd_inq.Parameters.Set("v_backlog", v_backlog);
			cmd_inq.Parameters.Set("v_next_backlog_pos", v_next_backlog_pos);
			bcls_ret->Tables[0].Rows[i]["LACK_WT"] = cmd_inq.ExecuteScalar();
			//查询准发确认工序欠量
			sqlstr = "SELECT NVL(LACK_WT, 0) "
				"  FROM TPMOF02 "
				" WHERE ORDER_NO = @v_order_no "
				"   AND BACKLOG_POS = '19' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_order_no", v_order_no);
			bcls_ret->Tables[0].Rows[i]["LACK_WT_19"] = cmd_inq.ExecuteScalar();
			//查热卷历史表
			if (v_hot_coil_no > "")
			{
				sqlstr = "SELECT PROD_END_TIME, "
					"		 IN_STOCK_COLD_TIME "
					"  FROM HMMHR01 "
					" WHERE MAT_NO = @hot_coil_no ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("hot_coil_no", v_hot_coil_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_ret->Tables[0].Rows[i]["HOT_ROLL_TIME"] = cmd_inq.GetString(1);	//按周
					bcls_ret->Tables[0].Rows[i]["IN_STOCK_COLD_TIME"] = cmd_inq.GetString(2);	//交货期
				}
				cmd_inq.Close();
			}

		}

		bcls_ret->Tables[0].set_TableName("MM0005A1_INQ");
		CFormattable arguments[] = { bcls_ret->Tables[0].Rows.get_Count() };// 定义参数列表的数组
		CMessageFormat::Format(s.msg, _RES("GCRSS0000004")/*查询到[{0}]条记录。*/, arguments, 1);//格式化字符串
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