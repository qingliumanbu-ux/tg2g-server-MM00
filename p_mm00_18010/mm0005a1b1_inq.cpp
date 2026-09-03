/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-09
Description: 物料路径跟踪材料履历查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料路径跟踪材料履历查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm0005a1b1_inq)

int f_mm0005a1b1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_query_flag("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	/* 实体类定义 */
	CModel tmm0005("TMM0005");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_h(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0005["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		tmm0005["MAT_KIND"] = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		/* 打印输入参数 */
		Log::Info("", __FUNCTION__, "传入参数 tmm0005[\"MAT_NO\"] = [{0}]", tmm0005["MAT_NO"].ToString());
		Log::Info("", __FUNCTION__, "传入参数 tmm0005[\"MAT_KIND\"] = [{0}]", tmm0005["MAT_KIND"].ToString());
		Log::Info("", __FUNCTION__, "传入参数 record_count_per_page	= [{0}]", record_count_per_page);
		Log::Info("", __FUNCTION__, "传入参数 current_page_no = [{0}]", current_page_no);

		/* 检查输入参数合法性 */
		if (tmm0005["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (tmm0005["MAT_KIND"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 根据材料号,查询该材料在当前档还是历史档 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT MAT_ID FROM TMM" + tmm0005["MAT_KIND"].ToString() + "01 WHERE MAT_NO = '" + tmm0005["MAT_NO"].ToString() + "'";
		}

		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmm0005["MAT_ID"] = cmd_inq.GetString(1);
			tmm0005["ARCHIVE_FLAG"] = "T";
		}
		else
		{
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT MAT_ID FROM HMM" + tmm0005["MAT_KIND"].ToString() + "01 WHERE MAT_NO = '" + tmm0005["MAT_NO"].ToString() + "'";
			}

			cmd_inq_h.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq_h.ExecuteReader();
			if (cmd_inq_h.Read())
			{
				tmm0005["MAT_ID"] = cmd_inq_h.GetString(1);
				tmm0005["ARCHIVE_FLAG"] = "H";
			}
			else
			{
				strcpy(s.msg, "该材料在当前档和历史档都不存在!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq_h.Close();
		}
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "获取 tmm0005[\"ARCHIVE_FLAG\"] = [{0}]", tmm0005["ARCHIVE_FLAG"].ToString());
		Log::Info("", __FUNCTION__, "获取 tmm0005[\"MAT_ID\"] = [{0}]", tmm0005["MAT_ID"].ToString());

		//全产线覆盖再调用此查询
		//#if defined(_LINE_SM) && defined(_LINE_HR) && defined(_LINE_CR) &&  defined(_LINE_HP) && defined(_LINE_BW) 
		/* 查询材料履历信息 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = "SELECT COUNT(1) FROM " + tmm0005["ARCHIVE_FLAG"].ToString() + "MM" + tmm0005["MAT_KIND"].ToString() + "96"
				" WHERE MAT_ID = '" + tmm0005["MAT_ID"].ToString() + "'";

			sqlstr = "SELECT * FROM " + tmm0005["ARCHIVE_FLAG"].ToString() + "MM" + tmm0005["MAT_KIND"].ToString() + "96"
				" WHERE MAT_ID = '" + tmm0005["MAT_ID"].ToString() + "' ORDER BY RESUME_SEQ_NO ASC ";
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.SetCommandText(sqlstr_count);

		Log::Info("", __FUNCTION__, "sqlstr_count = [{0}]", sqlstr_count);
		Log::Info("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);

		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		bcls_ret->Tables.Add("PAGEINFO");
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
