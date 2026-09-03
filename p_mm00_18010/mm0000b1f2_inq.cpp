/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-20 14:33:55
Description: L3L4材料对账信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// L3L4材料对账信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明
BM2_FUNCTION_IMPORT
void f_epex_call_cgi_svc(CDbConnection * conn, const CString& system_code, const CString& svc_name, EIClass * blks_in, EIClass * blks_out, int timeout);

BM2F_ENTERACE(mm0000b1f2_inq)


int f_mm0000b1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_pes_part("");
	CString cs_factory_div("");
	CString cs_mat_kind("");
	EIClass inBlock;
	EIClass outBlock;

	CString cs_part_name("BSPES");
	CString cs_svc_name("mm0000b1f2_pes_inq");
	int cs_timeout = 30;

	/* 实体类定义 */
	CModel tmm00si02("TMM00SI02");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_factory_div	= bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		cs_mat_kind		= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		cs_pes_part		= bcls_rec->Tables[0].Rows[0]["PES_PART"].ToString().Trim();

		/* 打印传入参数 */
		Log::Trace("", __FUNCTION__, "传入参数FACTORY_DIV		= [{0}]", cs_factory_div);
		Log::Trace("", __FUNCTION__, "传入参数MAT_KIND		= [{0}]", cs_mat_kind);
		Log::Trace("", __FUNCTION__, "传入参数PES_PART		= [{0}]", cs_pes_part);

		/* 检查输入参数合法性 */
		if (cs_mat_kind.Trim() == "")
		{
			strcpy(s.msg, "物料种类不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (cs_pes_part.Trim() == "")
		{
			strcpy(s.msg, "分区标记不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 调用PES的service 获取PES的材料信息 */
		inBlock.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		inBlock.Tables[0].Columns.Add(DT_STRING, "MAT_KIND");
		inBlock.Tables[0].Rows.Add();
		inBlock.Tables[0].Rows[0]["FACTORY_DIV"] = cs_factory_div;
		inBlock.Tables[0].Rows[0]["MAT_KIND"] = cs_mat_kind;

		Log::Trace("", __FUNCTION__, "调用PES的service!");
		f_epex_call_cgi_svc(conn, cs_part_name, cs_svc_name, &inBlock, &outBlock, cs_timeout);
		Log::Trace("", __FUNCTION__, "获取PES的材料信息!");

		struct ei_sys s_tmp;
		outBlock.GetSYS(&s_tmp);
		if (s_tmp.flag < 0)
		{
			Log::Trace("", "", "调用失败. s.flag=[%d] s.msg =[%s] s.sysmsg=[%s]", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg);
		}

		/* 设置返回块MMS、PES */
		bcls_ret->Tables.SetTableName(0, "MMS");
		bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, "MAT_NO", "材料号");
		bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, "MAT_LINE_TYPE", "产线类型");
		bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, "MAT_KIND", "物料种类");
		bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, "FACTORY_DIV", "厂别区分");
		

		bcls_ret->Tables.Add("PES");
		bcls_ret->Tables["PES"].Columns.Add(DT_STRING, "MAT_NO", "材料号");
		bcls_ret->Tables["PES"].Columns.Add(DT_STRING, "MAT_LINE_TYPE", "产线类型");
		bcls_ret->Tables["PES"].Columns.Add(DT_STRING, "MAT_KIND", "物料种类");
		bcls_ret->Tables["PES"].Columns.Add(DT_STRING, "FACTORY_DIV", "厂别区分");
		

		/* 设置块存放MMS和PES比较的数据 */
		bcls_rec->Tables.Add("MMS");
		bcls_rec->Tables.Add("PES");
		bcls_rec->Tables["PES"].Copy(outBlock.Tables["PES"]);

		/* 获取MMS的物料信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM" + cs_mat_kind + "01 "
						 "  WHERE MAT_KIND = @cs_mat_kind ";
				if (cs_factory_div.Trim() != "")
				{
					sqlstr_temp = " AND FACTORY_DIV = @cs_factory_div";
				}
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("cs_mat_kind", cs_mat_kind);
		if (cs_factory_div.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_factory_div", cs_factory_div);
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec->Tables["MMS"]);
		cmd_inq.Close();

		/* 查询参数配置表 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM00SI02 "
						 "  WHERE MAT_KIND = @cs_mat_kind "
						 "    AND SYS_CODE = @cs_pes_part ";
				if (cs_factory_div.Trim() != "")
				{
					sqlstr_temp = " AND FACTORY_DIV = @cs_factory_div";
				}
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("cs_mat_kind", cs_mat_kind);
		cmd_inq.Parameters.Set("cs_pes_part", cs_pes_part);
		if (cs_factory_div.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_factory_div", cs_factory_div);
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm00si02);
			tmm00si02.TrimOrBlank();

			if (tmm00si02["ITEM_KIND"].ToString().Trim() == "S")
			{
				bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, tmm00si02["ITEM_ENAME"].ToString(), "MMS" + tmm00si02["ITEM_CNAME"].ToString());
				bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, tmm00si02["ITEM_ENAME"].ToString() + "_PES", "PES" + tmm00si02["ITEM_CNAME"].ToString());

				bcls_ret->Tables["PES"].Columns.Add(DT_STRING, tmm00si02["ITEM_ENAME"].ToString(), tmm00si02["ITEM_CNAME"].ToString());
			}
			else if (tmm00si02["ITEM_KIND"].ToString().Trim() == "D")
			{
				bcls_ret->Tables["MMS"].Columns.Add(DT_DECIMAL, tmm00si02["ITEM_ENAME"].ToString(), "MMS" + tmm00si02["ITEM_CNAME"].ToString());
				bcls_ret->Tables["MMS"].Columns.Add(DT_DECIMAL, tmm00si02["ITEM_ENAME"].ToString() + "_PES", "PES" + tmm00si02["ITEM_CNAME"].ToString());

				bcls_ret->Tables["PES"].Columns.Add(DT_DECIMAL, tmm00si02["ITEM_ENAME"].ToString(), tmm00si02["ITEM_CNAME"].ToString());
			}
			bcls_ret->Tables["MMS"].Columns.Add(DT_STRING, tmm00si02["ITEM_ENAME"].ToString() + "_FLAG", tmm00si02["ITEM_CNAME"].ToString() + "描述");
			bcls_ret->Tables["PES"].Columns.Add(DT_STRING, tmm00si02["ITEM_ENAME"].ToString() + "_FLAG", tmm00si02["ITEM_CNAME"].ToString() + "描述");

			Log::Trace("", __FUNCTION__, "比较MMS和PES的[{0}]数据", (const char*)tmm00si02["ITEM_ENAME"].ToString());
			
			/* 比较MMS和PES的数据 */
			for (int i = 0; i < bcls_rec->Tables["MMS"].Rows.get_Count(); i++)
			{
				if (bcls_ret->Tables["MMS"].Rows.get_Count() < bcls_rec->Tables["MMS"].Rows.get_Count())
				{
					bcls_ret->Tables["MMS"].Rows.Add();
				}
				bcls_ret->Tables["MMS"].Rows[i]["MAT_NO"]				= bcls_rec->Tables["MMS"].Rows[i]["MAT_NO"];
				bcls_ret->Tables["MMS"].Rows[i]["MAT_LINE_TYPE"]		= bcls_rec->Tables["MMS"].Rows[i]["MAT_LINE_TYPE"];
				bcls_ret->Tables["MMS"].Rows[i]["MAT_KIND"]				= bcls_rec->Tables["MMS"].Rows[i]["MAT_KIND"];
				bcls_ret->Tables["MMS"].Rows[i]["FACTORY_DIV"]			= bcls_rec->Tables["MMS"].Rows[i]["FACTORY_DIV"];
				bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString()]	= bcls_rec->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString()];

				for (int j = 0; j < bcls_rec->Tables["PES"].Rows.get_Count(); j++)
				{
					if (bcls_ret->Tables["PES"].Rows.get_Count() < bcls_rec->Tables["PES"].Rows.get_Count())
					{
						bcls_ret->Tables["PES"].Rows.Add();
					}			
					bcls_ret->Tables["PES"].Rows[j]["MAT_NO"]				= bcls_rec->Tables["PES"].Rows[j]["MAT_NO"];
					bcls_ret->Tables["PES"].Rows[j]["MAT_LINE_TYPE"]		= bcls_rec->Tables["PES"].Rows[j]["MAT_LINE_TYPE"];
					bcls_ret->Tables["PES"].Rows[j]["MAT_KIND"]				= bcls_rec->Tables["PES"].Rows[j]["MAT_KIND"];
					bcls_ret->Tables["PES"].Rows[j]["FACTORY_DIV"]			= bcls_rec->Tables["PES"].Rows[j]["FACTORY_DIV"];
					bcls_ret->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString()]	= bcls_rec->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString()];

					if (bcls_rec->Tables["MMS"].Rows[i]["MAT_NO"].ToString() == bcls_rec->Tables["PES"].Rows[j]["MAT_NO"].ToString())
					{
						bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString() + "_PES"] = bcls_rec->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString()];
						if (tmm00si02["ITEM_KIND"].ToString().Trim() == "S")
						{					
							if (bcls_rec->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString()].ToString() == bcls_rec->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString()].ToString())
							{
								bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "一致";
								bcls_ret->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "一致";
							}
							else
							{
								bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "不一致";
								bcls_ret->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "不一致";
							}
						}
						else if (tmm00si02["ITEM_KIND"].ToString().Trim() == "D")
						{
							if (bcls_rec->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString()].ToDecimal() == bcls_rec->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString()].ToDecimal())
							{
								bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "一致";
								bcls_ret->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "一致";
							}
							else
							{
								bcls_ret->Tables["MMS"].Rows[i][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "不一致";
								bcls_ret->Tables["PES"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = tmm00si02["ITEM_CNAME"].ToString() + "不一致";
							}
						}
					}
				}
			}

			for (int j = 0; j < bcls_ret->Tables["MMS"].Rows.get_Count(); j++)
			{
				if (bcls_ret->Tables["MMS"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"].ToString().Trim() == "")
				{
					bcls_ret->Tables["MMS"].Rows[j][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = "MMS层有PES层没有";
				}
			}
			for (int k = 0; k < bcls_ret->Tables["PES"].Rows.get_Count(); k++)
			{
				if (bcls_ret->Tables["PES"].Rows[k][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"].ToString().Trim() == "")
				{
					bcls_ret->Tables["PES"].Rows[k][tmm00si02["ITEM_ENAME"].ToString() + "_FLAG"] = "PES层有MMS层没有";
				}
			}
		}
		cmd_inq.Close();
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


