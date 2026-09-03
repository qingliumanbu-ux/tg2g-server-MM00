/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-07-11
Description: 外购料信息管理_材料查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料信息管理_材料查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(mm0010a1f2_inq)

int f_mm0010a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CDecimal cd_mat_thick_from	= 0;	//厚度FROM
	CDecimal cd_mat_thick_to	= 0;	//厚度TO
	CDecimal cd_mat_width_from	= 0;	//宽度FROM
	CDecimal cd_mat_width_to	= 0;    //宽度TO
	CDecimal cd_mat_wt_from		= 0;    //重量FROM
	CDecimal cd_mat_wt_to		= 0;    //重量TO
	CDecimal cd_count	= 0  ;	
	int	record_count_per_page	= 0;	/* 每页记录数 */
	int	current_page_no			= 0;	/* 需查询的页号,从0开始计数 */
	int	start_row				= 0;	/* 将要压入outBlock的起始行 */

	/* 实体类定义 */ 	
	CModel tmm0010("TMM0010");

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
		tmm0010.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmm0010.TrimOrBlank();
		cd_mat_thick_from	= bcls_rec->Tables[0].Rows[0]["MAT_THICK_FROM"];	//厚度FROM
		cd_mat_thick_to		= bcls_rec->Tables[0].Rows[0]["MAT_TIHCK_TO"];		//厚度TO
		cd_mat_width_from	= bcls_rec->Tables[0].Rows[0]["MAT_WIDTH_FROM"];	//宽度FROM
		cd_mat_width_to		= bcls_rec->Tables[0].Rows[0]["MAT_WIDTH_TO"];		//宽度TO
		cd_mat_wt_from		= bcls_rec->Tables[0].Rows[0]["MAT_WT_FROM"];		//重量FROM
		cd_mat_wt_to		= bcls_rec->Tables[0].Rows[0]["MAT_WT_TO"];	        //重量TO

		record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	/* 每页记录数 */
		current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];		/* 需查询的页号 */
		
		/* 打印输入参数 */
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.RAW_ORIGIN		= [{0}]", tmm0010["RAW_ORIGIN"].ToString());       //原料来源
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.MAT_KIND			= [{0}]", tmm0010["MAT_KIND"].ToString());         //材料类型
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.MAT_LINE_TYPE	= [{0}]", tmm0010["MAT_LINE_TYPE"].ToString());    //材料去向
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.MAT_NO  	        = [{0}]", tmm0010["MAT_NO"].ToString());           //材料号
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.ST_NO  	        = [{0}]", tmm0010["ST_NO"].ToString());            //出钢记号
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.SHIP_LOT_NO  	= [{0}]", tmm0010["SHIP_LOT_NO"].ToString());      //船批号
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_thick_from  	    = [{0}]", cd_mat_thick_from);        //厚度FROM
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_thick_to  	    = [{0}]", cd_mat_thick_to);          //厚度TO
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_width_from  	    = [{0}]", cd_mat_width_from);        //宽度FROM
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_width_to  	    = [{0}]", cd_mat_width_to);          //宽度TO
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_wt_from  	        = [{0}]", cd_mat_wt_from);           //重量FROM
		Log::Trace("", __FUNCTION__, "打印输入参数 cd_mat_wt_to  	        = [{0}]", cd_mat_wt_to);             //重量TO
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.AFFIRM_FLAG		= [{0}]", tmm0010["AFFIRM_FLAG"].ToString());      //确认标记
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.DEMAND_PLAN_NO	= [{0}]", tmm0010["DEMAND_PLAN_NO"].ToString());   //进料计划
		Log::Trace("", __FUNCTION__, "打印输入参数 tmm0010.STOCK_NO_TO	    = [{0}]", tmm0010["STOCK_NO_TO"].ToString());      //目标库区

		Log::Trace("", __FUNCTION__, "record_count_per_page		= [{0}]", record_count_per_page);
		Log::Trace("",__FUNCTION__,"current_page_no				= [{0}]",current_page_no);

		/* 查询外购料信息表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count =	"SELECT COUNT(1) "
								"  FROM TMM0010 "
								" WHERE 1 = 1 ";
				sqlstr =	"SELECT * "
							"  FROM TMM0010 "
							" WHERE 1 = 1 "; 
				if (tmm0010["RAW_ORIGIN"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND RAW_ORIGIN = @tmm0010.RAW_ORIGIN ";
				}
				if (tmm0010["MAT_KIND"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_KIND = @tmm0010.MAT_KIND ";
				}
				if (tmm0010["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_LINE_TYPE = @tmm0010.MAT_LINE_TYPE ";
				}
				if(tmm0010["AFFIRM_FLAG"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND AFFIRM_FLAG = @tmm0010.AFFIRM_FLAG ";
				}			
				if (tmm0010["MAT_NO"].ToString().Trim() != "")     //材料号
				{
					sqlstr_temp += " AND MAT_NO LIKE @tmm0010.MAT_NO||'%'  ";
				}
				if (tmm0010["ST_NO"].ToString().Trim() != "")    //出钢记号
				{
					sqlstr_temp += " AND ST_NO LIKE @tmm0010.ST_NO ||'%' ";
				}
				if (tmm0010["SHIP_LOT_NO"].ToString().Trim() != "")  //船批号
				{
					sqlstr_temp += " AND SHIP_LOT_NO LIKE @tmm0010.SHIP_LOT_NO ||'%' ";
				}
				if (tmm0010["STOCK_NO_TO"].ToString().Trim() != "")  //目标库区
				{
					sqlstr_temp += " AND STOCK_NO_TO LIKE @tmm0010.STOCK_NO_TO ||'%' ";
				}
				if (cd_mat_thick_from > 0)      //厚度
				{
					sqlstr_temp += " AND MAT_THICK >= @cd_mat_thick_from";
				}
				if (cd_mat_thick_to > 0)
				{
					sqlstr_temp += " AND MAT_THICK <= @cd_mat_thick_to";
				}
				if (cd_mat_width_from > 0)    //宽度
				{
					sqlstr_temp += " AND MAT_WIDTH >= @cd_mat_width_from";
				}
				if (cd_mat_width_to > 0)
				{
					sqlstr_temp += " AND MAT_WIDTH <= @cd_mat_width_to";
				}
				if (cd_mat_wt_from > 0)    //重量
				{
					sqlstr_temp += " AND MAT_WT >= @cd_mat_wt_from";
				}
				if (cd_mat_wt_to > 0)
				{
					sqlstr_temp += " AND MAT_WT <= @cd_mat_wt_to";
				}
				if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() == "Y")       //有计划号
				{
					sqlstr_temp += " AND DEMAND_PLAN_NO <> ' ' ";
				}
				else if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() == "N")  //无计划号
				{
					sqlstr_temp += " AND DEMAND_PLAN_NO = ' '  ";
				}
				else if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() != "") //有计划号具体值，可以录入值得时候
				{
					sqlstr_temp += " AND DEMAND_PLAN_NO  LIKE @tmm0010.DEMAND_PLAN_NO ||'%'  ";
				}
				else //计划号的值为空
				{
					//不需要加计划号的查询条件
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr = sqlstr + sqlstr_temp + " ORDER BY MAT_NO ASC";
				break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count			= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (tmm0010["RAW_ORIGIN"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.RAW_ORIGIN", tmm0010["RAW_ORIGIN"].ToString());
		}
		if (tmm0010["MAT_KIND"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.MAT_KIND",tmm0010["MAT_KIND"].ToString()); 
		}
		if(tmm0010["MAT_LINE_TYPE"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.MAT_LINE_TYPE",tmm0010["MAT_LINE_TYPE"].ToString()); 
		}
		if(tmm0010["AFFIRM_FLAG"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.AFFIRM_FLAG",tmm0010["AFFIRM_FLAG"].ToString()); 
		}
		if (tmm0010["MAT_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.MAT_NO", tmm0010["MAT_NO"].ToString());
		}
		if (tmm0010["ST_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.ST_NO", tmm0010["ST_NO"].ToString());
		}
		if (tmm0010["SHIP_LOT_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.SHIP_LOT_NO", tmm0010["SHIP_LOT_NO"].ToString());
		}
		if (tmm0010["STOCK_NO_TO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.STOCK_NO_TO", tmm0010["STOCK_NO_TO"].ToString());
		}
		if (cd_mat_thick_from > 0)      //厚度
		{
			cmd_inq.Parameters.Set("cd_mat_thick_from", cd_mat_thick_from);
		}
		if (cd_mat_thick_to > 0)
		{
			cmd_inq.Parameters.Set("cd_mat_thick_to", cd_mat_thick_to);
		}
		if (cd_mat_width_from > 0)    //宽度
		{
			cmd_inq.Parameters.Set("cd_mat_width_from", cd_mat_width_from);
		}
		if (cd_mat_width_to > 0)
		{
			cmd_inq.Parameters.Set("cd_mat_width_to", cd_mat_width_to);
		}
		if (cd_mat_wt_from > 0)   //重量
		{
			cmd_inq.Parameters.Set("cd_mat_wt_from", cd_mat_wt_from);
		}
		if (cd_mat_wt_to > 0)
		{
			cmd_inq.Parameters.Set("cd_mat_wt_to", cd_mat_wt_to);
		}
		if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmm0010.DEMAND_PLAN_NO", tmm0010["DEMAND_PLAN_NO"].ToString());
		}	
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count	= cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		Log::Trace("",__FUNCTION__,"总记录数cd_count = [{0}]",cd_count.ToInt32());

		start_row = record_count_per_page * (current_page_no-1) ;
		if(start_row > cd_count.ToInt32())
		{
			start_row = 0;
		}
		Log::Trace("",__FUNCTION__,"start_row = [{0}]",start_row);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");//增加块
		blkNum	= bcls_ret->Tables.IndexOf("PAGEINFO"); 
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");		//总记录数
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
		
