/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-03-27 16:46:52
Description: 物料通用_机组工序对照基本信息同步
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 物料通用_机组工序对照基本信息同步
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE(mm00si16f1_pro)


int f_mm00si16f1_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmm00si16("TMM00SI16");
	CModel tsi0015("TSI0015");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("TSI0015");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TSI0015");
		}


		/* 查询公用机组工序配置TSI0015表中不在TMM00SI16表中的机组 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "	 FROM TSI0015 "
						 "  WHERE UNIT_CODE "
						 " NOT IN (SELECT UNIT_CODE FROM TMM00SI16 )";
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec->Tables["TSI0015"]);
		cmd_inq.Close();

		/* 新增不在 TMM00SI16表中的机组 */
		for (int i = 0; i < bcls_rec->Tables["TSI0015"].Rows.get_Count(); i++)
		{
			tsi0015.MergeFrom(bcls_rec->Tables["TSI0015"].Rows[i]);
			tsi0015.TrimOrBlank();

			/* 设置TMM00SI16表中字段 */
			tmm00si16["UNIT_CODE"] = tsi0015["UNIT_CODE"];
			tmm00si16["UNIT_CNAME"] = tsi0015["UNIT_CNAME"];
			tmm00si16["PS_BACKLOG_TYPE_CODE"] = tsi0015["PS_BACKLOG_TYPE_CODE"];
			tmm00si16["PS_PLAN_SORT"] = tsi0015["PS_PLAN_SORT"];
			tmm00si16["FACTORY_DIV"] = tsi0015["FACTORY_DIV"];
			//tmm00si16["MAT_LINE_TYPE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["MAT_KIND"] = tsi0015["UNIT_CODE"];
			//tmm00si16["MAT_SHAPE_FLAG"] = tsi0015["UNIT_CODE"];
			//tmm00si16["IN_MAT_KIND"] = tsi0015["UNIT_CODE"];
			//tmm00si16["CUT_FLAG"] = tsi0015["UNIT_CODE"];
			//tmm00si16["CUT_MODE_CODE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["CUT_POS"] = tsi0015["UNIT_CODE"];
			//tmm00si16["CUT_NUM"] = tsi0015["UNIT_CODE"];
			//tmm00si16["STRIP_DIV"] = tsi0015["UNIT_CODE"];
			//tmm00si16["STRIP_NUM"] = tsi0015["UNIT_CODE"];
			//tmm00si16["RULE_CODE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["IF_PLAN"] = tsi0015["UNIT_CODE"];
			//tmm00si16["PROD_TABLE_NAME"] = tsi0015["UNIT_CODE"];
			//tmm00si16["PLAN_TABLE_NAME"] = tsi0015["UNIT_CODE"];
			//tmm00si16["RETURN_PROD_TABLE_NAME"] = tsi0015["UNIT_CODE"];
			//tmm00si16["FORM_NO"] = tsi0015["UNIT_CODE"];
			//tmm00si16["MESSAGEID"] = tsi0015["UNIT_CODE"];
			//tmm00si16["TC_NO"] = tsi0015["UNIT_CODE"];
			//tmm00si16["OPER_TYPE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["SHIFT_CLASS"] = tsi0015["UNIT_CODE"];
			//tmm00si16["OUT_BACKLOG_TYPE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["SVC_NAME"] = tsi0015["UNIT_CODE"];
			//tmm00si16["CFGITM_NAME"] = tsi0015["UNIT_CODE"];
			//tmm00si16["PRFM_DIV"] = tsi0015["UNIT_CODE"];
			//tmm00si16["FURNACE_TYPE"] = tsi0015["UNIT_CODE"];
			//tmm00si16["PRACT_COPY_INDICATE"] = tsi0015["UNIT_CODE"];

			/* 根据事件抛账表，校验新旧数据的字段是否一致 */
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT WHOLE_BACKLOG_CODE, "
							 "        WHOLE_BACKLOG_NAME "
							 "   FROM TMM00SI16 "
							 "  WHERE UNIT_CODE			= @tsi0015.UNIT_CODE ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tsi0015.UNIT_CODE", tsi0015["UNIT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmm00si16["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(1);
				tmm00si16["WHOLE_BACKLOG_NAME"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			/* 新增信息 */
			tmm00si16["REC_CREATE_TIME"] = datetime;
			tmm00si16["REC_CREATOR"] = s.userid;
			tmm00si16.TrimOrBlank();
			tmm00si16.Insert();


		}

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


