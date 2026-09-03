/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2016-08-03
Description: 材料成品转在制品(通用)
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料成品转在制品(通用)
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
	
BM2F_ENTERACE(mm0001d1f8_pro)

int f_mm0001d1f8_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_mat_no("");
	CString	cs_mat_kind("");
	CString	cs_table_ename("");
	CString	cs_mat_line_type("");

	CString cs_mat_status("");
	CString cs_order_no("");
	CString cs_whole_backlog("");
	CString cs_next_whole_backlog_code("");
	CDecimal cd_next_whole_backlog_seq = 0;
	CDecimal cd_mat_act_wt = 0;
	/* 实体类定义 */
	CModel tpmof03("TPMOF03");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加与设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		}
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}

		cs_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		cs_table_ename = bcls_rec->Tables[0].Rows[0]["TABLE_ENAME"].ToString().Trim();
		cs_mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			cs_mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "cs_mat_no			= [{0}]", (const char*)cs_mat_no);

			/* 检查输入参数合法性 */
			if (cs_mat_no.Trim() == "")
			{
				strcpy(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 查询材料主档表 */
			/* 查询材料主档表 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT MAT_STATUS,ORDER_NO,WHOLE_BACKLOG,NEXT_WHOLE_BACKLOG_CODE, "
					"            NEXT_WHOLE_BACKLOG_SEQ,MAT_ACT_WT "
					"       FROM " + cs_table_ename + " WHERE MAT_NO = @cs_mat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("cs_mat_no", cs_mat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cs_mat_status = cmd_inq.GetString(1);
				cs_order_no = cmd_inq.GetString(2);
				cs_whole_backlog = cmd_inq.GetString(3);
				cs_next_whole_backlog_code = cmd_inq.GetString(4);
				cd_next_whole_backlog_seq = cmd_inq.GetDecimal(5);
				cd_mat_act_wt = cmd_inq.GetDecimal(6);
			}
			cmd_inq.Close();

			/* 校验逻辑合法性 */
			if (cs_mat_status.Trim().Substring(0, 1) != '2')
			{
				sprintf(s.msg, "材料号[%s]材料状态[%s]不是在制品!", (const char*)cs_mat_no, (const char*)cs_mat_status);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cs_mat_status.Trim() == "22")
			{
				sprintf(s.msg, "材料号[%s]材料状态[%s]是质量封锁,不能转成品!", (const char*)cs_mat_no, (const char*)cs_mat_status);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cs_mat_status.Trim() == "24")
			{
				sprintf(s.msg, "材料号[%s]材料状态[%s]计划中,不能转成品!", (const char*)cs_mat_no, (const char*)cs_mat_status);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 设置物料跟踪参数 */
			//--前台已控制只能余材转在制品,若为合同材,需调用2A事件,PES可做同步操作
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_KIND"] = cs_mat_kind;	//产线类型
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"] = "MM21";	//在制品转成品
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "00";
			bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "MM" + cs_mat_line_type;
			bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"] = "mm0001d1f8_pro";
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"] = cs_mat_no;

			/* 合同材,设置合同跟踪接口参数 */
			if (cs_order_no.Trim() != "")
			{
				tpmof03["CALL_FLAG"] = 1;
				tpmof03["EVENT_ID"] = "5B";
				tpmof03["ORDER_NO"] = cs_order_no;
				tpmof03["SYSTEM_ID"] = "MM";
				tpmof03["FUNC_ID"] = "mm0001d1f8_pro";
				tpmof03["WHOLE_BACKLOG"] = cs_whole_backlog;
				tpmof03["WHOLE_BACKLOG_CODE"] = cs_next_whole_backlog_code;
				tpmof03["WHOLE_BACKLOG_SEQ"] = cd_next_whole_backlog_seq;
				tpmof03["MAT_NO"] = cs_mat_no;
				tpmof03["WT"] = cd_mat_act_wt;
				//tpmof03.MergeTo(bcls_rec->Tables["PMOF99"],false);
			}
		}

		/* 调用物料跟踪 */
		doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 调用合同跟踪 */
		if (tpmof03["EVENT_ID"].ToString().Trim() == "5B")
		{
			//doFlag = f_pmof99_v3(bcls_rec, bcls_ret,conn);	
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
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
