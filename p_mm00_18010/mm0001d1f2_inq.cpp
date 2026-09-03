/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2016-08-03
Description: 材料信息查询(通用)
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料信息查询(通用)
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

//从字符串中根据指定分隔符拆分数据
// 入口字符，分隔字符，函数是返回字符信息。
CString f_get_multi_value(CString v_in_str, CString v_spilit_flag, CDbConnection * conn);

BM2F_ENTERACE(mm0001d1f2_inq) 

int f_mm0001d1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	cs_mat_kind("");
	CString	cs_table_ename("");
	CString	cs_mat_line_type("");
	CString	cs_tmp("");

	CString	cs_mat_no("");
	CString	cs_order_no("");
	CString	cs_heat_no("");
	CString	cs_mat_status("");
	CString	cs_factory_store("");
	CString	cs_factory_prod("");
	CString	cs_product_flag("");
	CString cs_mng_hold_cause_code("");
	CString cs_hold_flag("");
	CString cs_function_name("");

	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    
	/* 实体类定义 */ 

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();			//材料号
		Log::Trace("", __FUNCTION__, "cs_mat_no				= [{0}]", cs_mat_no);

		cs_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();		//合同号
		Log::Trace("", __FUNCTION__, "cs_order_no				= [{0}]", cs_order_no);

		cs_mat_status = bcls_rec->Tables[0].Rows[0]["MAT_STATUS"].ToString().Trim();      //材料状态
		Log::Trace("", __FUNCTION__, "cs_mat_status			= [{0}]", cs_mat_status);

		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO") == true)
		{
			cs_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();			//炉号
			Log::Trace("", __FUNCTION__, "cs_heat_no				= [{0}]", cs_heat_no);
		}
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_STORE") == true)
		{
			cs_factory_store = bcls_rec->Tables[0].Rows[0]["FACTORY_STORE"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "cs_factory_store	        = [{0}]", cs_factory_store);
		}
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_PROD") == true)
		{
			cs_factory_prod = bcls_rec->Tables[0].Rows[0]["FACTORY_PROD"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "cs_factory_prod	        = [{0}]", cs_factory_prod);
		}
		if (bcls_rec->Tables[0].Columns.Contains("PRODUCT_FLAG") == true)
		{
			cs_product_flag = bcls_rec->Tables[0].Rows[0]["PRODUCT_FLAG"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "cs_product_flag			= [{0}]", cs_product_flag);
		}
		if (bcls_rec->Tables[0].Columns.Contains("MNG_HOLD_CAUSE_CODE") == true)
		{
			cs_mng_hold_cause_code = bcls_rec->Tables[0].Rows[0]["MNG_HOLD_CAUSE_CODE"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "cs_mng_hold_cause_code			= [{0}]", cs_mng_hold_cause_code);
		}
		if (bcls_rec->Tables[0].Columns.Contains("HOLD_FLAG") == true)
		{
			cs_hold_flag = bcls_rec->Tables[0].Rows[0]["HOLD_FLAG"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "cs_hold_flag			= [{0}]", cs_hold_flag);
		}

		record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	
		current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];	
		
		Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);

		cs_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		cs_table_ename = bcls_rec->Tables[0].Rows[0]["TABLE_ENAME"].ToString().Trim();
		cs_mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		cs_function_name = bcls_rec->Tables[0].Rows[0]["FUNCTION_NAME"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "cs_mat_kind,cs_table_ename,cs_mat_line_type = [{0}],[{1}],[{2}]",
			(const char*)cs_mat_kind, (const char*)cs_table_ename, (const char*)cs_mat_line_type);

		/* 查询材料信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
			sqlstr_count = " SELECT COUNT(1) FROM " + cs_table_ename + " WHERE "
				"  MAT_LINE_TYPE = @cs_mat_line_type AND 1 = 1 ";
			sqlstr = " SELECT * FROM " + cs_table_ename + " WHERE "
				"  MAT_LINE_TYPE = @cs_mat_line_type AND 1 = 1 ";

			if (cs_function_name.Trim() == "PTF_CH")
			{
				sqlstr_temp += " AND MAT_STATUS IN ('29','39') ";
			}

			if(cs_mat_no.Trim() != "")
			{
				//根据指定分隔符，拆分字符信息。
				cs_tmp = f_get_multi_value(cs_mat_no, "\n", conn);

				if (cs_tmp.Trim() != "")
				{//返回的信息，不为空。
					if (cs_mat_no.Trim().GetLength() <= 8)
					{//进行模糊查询==LIKE .
						sqlstr_temp += " AND MAT_NO LIKE @cs_mat_no ||'%' ";
					}
					else
					{
						//进行字符拆分处理。 ===f_get_multi_value() 
						sqlstr_temp += " AND MAT_NO in ( " + cs_tmp + " ) ";
					}
				}
			}
			if (cs_order_no.Trim() != "")
			{
				//根据指定分隔符，拆分字符信息。
				cs_tmp = f_get_multi_value(cs_order_no, "\n", conn);

				if (cs_tmp.Trim() != "")
				{//返回的信息，不为空。
					if (cs_order_no.Trim().GetLength() <= 4)
					{//进行模糊查询==LIKE .
						sqlstr_temp += " AND ORDER_NO LIKE @cs_order_no ||'%' ";
					}
					else
					{
						//进行字符拆分处理。 ===f_get_multi_value() 
						sqlstr_temp += " AND ORDER_NO in ( " + cs_tmp + " ) ";
					}
				}
			}
			if (cs_heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO LIKE @cs_heat_no ||'%' ";
			}
			if(cs_mat_status.Trim() != "")
			{
				sqlstr_temp	+= " AND MAT_STATUS IN (" + cs_mat_status + ") "; 
			}					
			if (cs_factory_store.Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_STORE = @cs_factory_store ";
			}
			if (cs_factory_prod.Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_PROD = @cs_factory_prod ";
			}
			if (cs_product_flag.Trim() != "")
			{
				sqlstr_temp += " AND PRODUCT_FLAG = @cs_product_flag ";
			}
			if (cs_product_flag.Trim() != "")
			{
				sqlstr_temp += " AND PRODUCT_FLAG = @cs_product_flag ";
			}
			if (cs_mng_hold_cause_code.Trim() != "")
			{
				sqlstr_temp += " AND MNG_HOLD_CAUSE_CODE IN @cs_mng_hold_cause_code ";
			}
			if (cs_hold_flag.Trim() != "")
			{
				sqlstr_temp += " AND HOLD_FLAG IN @cs_hold_flag ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY MAT_NO ASC";
			sqlstr		 = sqlstr + sqlstr_temp;
			break;
		}     
		Log::Trace("",__FUNCTION__,"sqlstr_temp			= [{0}]",(const char*)sqlstr_temp);
		Log::Trace("",__FUNCTION__,"sqlstr_count		= [{0}]",(const char*)sqlstr_count);
		Log::Trace("",__FUNCTION__,"sqlstr				= [{0}]",(const char*)sqlstr);
		cmd_inq.Parameters.Clear();

		if(cs_mat_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_mat_no",cs_mat_no); 
		}	
		if (cs_order_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_order_no", cs_order_no);
		}
		if (cs_heat_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_heat_no", cs_heat_no);
		}
		if(cs_mat_status.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_mat_status",cs_mat_status);
		}
		if (cs_factory_store.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_factory_store", cs_factory_store);
		}
		if (cs_factory_prod.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_factory_prod", cs_factory_prod);
		}
		if (cs_product_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_product_flag", cs_product_flag);
		}
		if (cs_mng_hold_cause_code.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_mng_hold_cause_code", cs_mng_hold_cause_code);
		}
		if (cs_hold_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_hold_flag", cs_hold_flag);
		}
		cmd_inq.Parameters.Set("cs_mat_line_type", cs_mat_line_type);		
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if(start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0]	= cd_count.ToInt32();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		