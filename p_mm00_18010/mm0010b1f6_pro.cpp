/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-07-11
Description: 外购料进料计划管理_计划确认
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料进料计划管理_计划确认
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件





//#include "tmm0005.h"


//外部函数声明 
int f_mm0099(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_qmtj_kz41(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn);
//int f_mm000501_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined _SYS_PES || defined _SYS_MES 
int f_wm00_queue(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#endif

BM2F_ENTERACE(mm0010b1f6_pro)
int f_mm0010b1f6_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)  
{

	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0; 

	/* 业务变量 */
	CString	datetime("");    
	CString	cs_pch_judge_code("");    
	CString	cs_mm00_mat_track_no("");			//材料跟踪号的后4位流水号

	/* 实体类定义 */
	CModel tmm0010("TMM0010");
	CModel tmm0011("TMM0011");
	CModel tmmsm96("TMMSM96");
	CModel tmmhr96("TMMHR96");
	CModel tmmcr96("TMMCR96");
	//CTMM0005 tmm0005(conn);
	CModel tqmtqb0("TQMTQB0");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");
		

		/* 添加并设置块名 */ 
		blkNum = bcls_rec->Tables.IndexOf("MM0099");				
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
		}
		//blkNum = bcls_rec->Tables.IndexOf("MM000501");
		//if (blkNum < 0)
		//{
		//	bcls_rec->Tables.Add("MM000501");
		//}
		blkNum = bcls_rec->Tables.IndexOf("QMTJBLOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QMTJBLOCK"); 
			bcls_rec->Tables["QMTJBLOCK"].Columns.Add(DT_STRING,"JUDGE_FLAG");  //判定标志
			bcls_rec->Tables["QMTJBLOCK"].Columns.Add(DT_STRING,"PONO");        //制造命令号
			bcls_rec->Tables["QMTJBLOCK"].Rows.Add();
		}
		
        #if defined _SYS_PES || defined _SYS_MES 
		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");  //库业务类型
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");          //材料号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");       //材料件数
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");     //机组代码
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");     //操作标志
		}
        #endif
		
		/* 获取输入参数 */
		tmm0011["DEMAND_PLAN_NO"]	= bcls_rec->Tables[0].Rows[0]["DEMAND_PLAN_NO"].ToString().Trim();

		/* 打印传入参数 */
		Log::Trace("", __FUNCTION__, "打印传入参数 tmm0011.DEMAND_PLAN_NO	= [{0}]", tmm0011["DEMAND_PLAN_NO"].ToString());

		/* 查询进料计划表 */
		tmm0011.Query("DEMAND_PLAN_NO"); 
		tmm0011.TrimOrBlank();

		/* 校验进料计划是否已确认 */
		if(tmm0011["PLAN_STATUS"].ToString().Trim() == "30") //30-计划已确认
		{
			sprintf(s.msg,"进料计划[%s]已确认!",(const char*)tmm0011["DEMAND_PLAN_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		/* 更新进料计划表的计划状态等信息 */
		tmm0011["PLAN_STATUS"]		= "30";		//30-计划已确认
		tmm0011["PLAN_SEND_TIME"]	= datetime; //计划发送时刻
		tmm0011["REC_REVISOR"]		= s.userid;
		tmm0011["REC_REVISE_TIME"]	= datetime;
		tmm0011.TrimOrBlank();
		tmm0011.Update( "PLAN_STATUS,"
						"PLAN_SEND_TIME,"
						"REC_REVISOR,"
						"REC_REVISE_TIME","DEMAND_PLAN_NO");

		/* 按进料计划号查询外购料信息 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "  FROM TMM0010 "
						 " WHERE DEMAND_PLAN_NO = @tmm0011.DEMAND_PLAN_NO "
						 " ORDER BY REC_CREATE_TIME ASC ";	
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0011.DEMAND_PLAN_NO", tmm0011["DEMAND_PLAN_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm0010);
			tmm0010.TrimOrBlank();

			/* 校验外购材料信息是否已确认 */
			if(tmm0010["AFFIRM_FLAG"].ToString().Trim() == "Y")
			{
				sprintf(s.msg,"材料号[%s]已确认!",(const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 校验外购料炉次信息 */
			tqmtqb0["PONO"]	= tmm0010["PONO"];
			tqmtqb0.Query("PONO");
			tqmtqb0.TrimOrBlank();
			if(tqmtqb0["HEAT_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg,"外购材料[%s]的炉次[%s]信息没有，不能进行确认操作!",
						(const char*)tmm0010["MAT_NO"].ToString(),(const char*)tmm0010["PONO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			Log::Trace("", __FUNCTION__, "传入参数 tqmtqb0.PONO	= [{0}]", tmm0010["PONO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tqmtqb0.HEAT_NO	= [{0}]", tmm0010["HEAT_NO"].ToString());

			/* 设置出钢记号和标准 */
			if(tqmtqb0["SG_STD"].ToString().Trim() == "")    //SG_STD 标准
			{
				tqmtqb0["SG_STD"]	= "WAIGOU";
			}				
			if(tqmtqb0["ST_NO"].ToString().Trim() == "")
			{
				tqmtqb0["ST_NO"]	= tmm0010["SG_SIGN"];
			}
			
			/* 判断外购成分是否合格 */
			bcls_rec->Tables["QMTJBLOCK"].Rows[0]["JUDGE_FLAG"]		= "5";
			bcls_rec->Tables["QMTJBLOCK"].Rows[0]["PONO"]			= tmm0010["PONO"];
			doFlag = f_qmtj_kz41(bcls_rec, bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			//传出参数 说明
			//0 成分不合格,某些成分,有标准但实绩没有录入
			//2 成分不合格,某些成分,标准和实绩不合
			//1 成分合格  
			cs_pch_judge_code	= bcls_ret->Tables["QMTJBLOCK"].Rows[0]["PCH_JUDGE_CODE"].ToString().Trim();	
			if(cs_pch_judge_code.Trim() != "1")
			{
				sprintf(s.msg,"外购材料号[%s]的成分不合格,不能进行确认操作!",(const char*)tmm0010["ORIGIN_MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("",__FUNCTION__,"新增材料主档表 tmm0010.ORIGIN_MAT_NO	= [{0}]",(const char*)tmm0010["ORIGIN_MAT_NO"].ToString());	  				
			Log::Trace("",__FUNCTION__,"新增材料主档表 tmm0010.MAT_NO	= [{0}]",(const char*)tmm0010["MAT_NO"].ToString());	  				
			Log::Trace("",__FUNCTION__,"新增材料主档表 tmm0010.MAT_KIND	= [{0}]",(const char*)tmm0010["MAT_KIND"].ToString());	  				

			/* 新增材料主档 设置初始值 */
			if (tmm0010["MAT_KIND"].ToString().Trim() == "SM")
			{							
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmm0010);

				//生成物料材料跟踪号后4位流水号
				doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_mm00_mat_track_no, conn);
				if (doFlag < 0 || cs_mm00_mat_track_no.Trim() == "")
				{
					strcpy(s.msg,"物料材料跟踪号后4位流水号生成错误！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("",__FUNCTION__,"cs_mm00_mat_track_no	= [{0}]",cs_mm00_mat_track_no);	  

				tmmsm96["MAT_TRACK_NO"]		= datetime + cs_mm00_mat_track_no; 
				tmmsm96["PASS_BACKLOG_SEQ_NO"] = 1;
				tmmsm96["PROD_MAKER"]			= s.userid;
				tmmsm96["ST_NO"]				= tqmtqb0["ST_NO"];		// 出钢记号
				tmmsm96["PREC_ST_NO"]			= tqmtqb0["ST_NO"];		// 预定出钢记号
				tmmsm96["FIN_ST_NO"]			= tqmtqb0["ST_NO"];		// 最终出钢记号
				tmmsm96["SG_STD"]				= tqmtqb0["SG_STD"];
				tmmsm96["PRODUCT_FLAG"]		= "0";
				tmmsm96["MEASURE_WT_FLAG"]		= "0";
				tmmsm96["HOLD_FLAG"]			= "0";					//封锁标记
				tmmsm96["SURFACE_DECIDE_CODE"]	= "1";					//表面判定代码(默认合格)     
				tmmsm96["SURFACE_DECIDE_TIME"]	= datetime;				//表面判定时间     
				tmmsm96["SURFACE_DECIDE_MAKER"]= s.userid;				//表面判定责任者     
				tmmsm96["PCH_JUDGE_ABN"]		= "0";					//性能判定代码     
				tmmsm96["COMPLEX_DECIDE_CODE"]	= "0";					//综合判定代码  
				tmmsm96["PRODUCT_PACK_FLAG"]	= "0";					//成品包装标志 
				tmmsm96["IN_FLAG"]				= "0";					//入库标记   
				tmmsm96["TRANSFER_FLAG"]		= "0";					//转库计划标记        
				tmmsm96["CONFM_FLAG"]			= "0";					//准发确认标记        	 
				tmmsm96["APP_DECIDE_FLAG"]		= "0";					//现货申报标记       	
				tmmsm96["PLAN_NO"]				= " ";					//计划号                 
				tmmsm96["STOCK_PLACE_NO"]		= " ";					//库号                
				tmmsm96["LAYERNO"]				= 0;					//层号                
				//tmmsm96["MAT_DESTION"]			= "11";					//去向    
				tmmsm96["REPAIR_FLAG"]			= "0";					//返修标记
				tmmsm96["CMD_FLAG"]			= "0";					//吊车命令标志
				tmmsm96["HOT_CHARGE_FLAG"]		= "0";                  //热装标记
				tmmsm96["HOT_SEND_FLAG"]		= "0";					//热送标记
				tmmsm96["SLABTOP_FLAG"]		= "0";					//板坯TOP点确认标志
				tmmsm96["ADJUST_WIDTH_MARK"]	= "0";					//调宽标记
				tmmsm96["MACH_CLEAR_FLAG"]		= "0";					//机清标志
				tmmsm96["RHEAT_SLAB_FLAG"]		= "0";					//回炉板坯标志
				tmmsm96["BD_FLAG"]				= "0";					//BD材标记

				tmmsm96["REC_CREATE_TIME"]		= datetime;			//记录创建时刻         
				tmmsm96["REC_CREATOR"]			= s.userid;			//记录创建责任者      

				/* 设置调用物料跟踪参数值 */
				tmmsm96["EVENT_ID"]		= "MM27";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"]		= "MM00";
				tmmsm96["FUNC_ID"]			= "mm0010b1f6_pro";
				tmmsm96.TrimOrBlank();
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
                
			}
			if (tmm0010["MAT_KIND"].ToString().Trim() == "HR")
			{							
				tmmhr96.Reset();
				tmmhr96.CopyFrom(tmm0010);

				//生成物料材料跟踪号后4位流水号
				doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_mm00_mat_track_no, conn);
				if (doFlag < 0 || cs_mm00_mat_track_no.Trim() == "")
				{
					strcpy(s.msg, "物料材料跟踪号后4位流水号生成错误！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", __FUNCTION__, "cs_mm00_mat_track_no	= [{0}]", cs_mm00_mat_track_no);
				tmmhr96["MAT_TRACK_NO"]		= datetime + cs_mm00_mat_track_no; 
				tmmhr96["PASS_BACKLOG_SEQ_NO"]	= 1; 
				tmmhr96["PROD_MAKER"]			= s.userid;
				tmmhr96["ST_NO"]				= tqmtqb0["ST_NO"];		// 出钢记号
				tmmhr96["SG_STD"]				= tqmtqb0["SG_STD"];
				tmmhr96["PRODUCT_FLAG"]		= "0";
				tmmhr96["MEASURE_WT_FLAG"]		= "0";
				tmmhr96["HOLD_FLAG"]			= "0";					//封锁标记
				tmmhr96["SURFACE_DECIDE_CODE"]	= "1";					//表面判定代码(默认合格)     
				tmmhr96["SURFACE_DECIDE_TIME"]	= datetime;				//表面判定时间     
				tmmhr96["SURFACE_DECIDE_MAKER"]= s.userid;				//表面判定责任者     
				tmmhr96["PCH_JUDGE_ABN"]		= "0";					//性能判定代码     
				tmmhr96["COMPLEX_DECIDE_CODE"]	= "0";					//综合判定代码  
				tmmhr96["PRODUCT_PACK_FLAG"]	= "0";					//成品包装标志 
				tmmhr96["IN_FLAG"]				= "0";					//入库标记   
				tmmhr96["TRANSFER_FLAG"]		= "0";					//转库计划标记        
				tmmhr96["CONFM_FLAG"]			= "0";					//准发确认标记        	 
				tmmhr96["APP_DECIDE_FLAG"]		= "0";					//现货申报标记       	
				tmmhr96["PLAN_NO"]				= " ";					//计划号                 
				tmmhr96["STOCK_PLACE_NO"]		= " ";					//库号                
				tmmhr96["LAYERNO"]				= 0;					//层号                
				tmmhr96["REPAIR_FLAG"]			= "0";					//返修标记
				tmmhr96["SAMPLE_TAKEN_FLAG"]	= "0";					//已取样标记
				tmmhr96["COLD_HOT_FLAG"]		= "0";					//冷热标志 

				tmmhr96["REC_CREATE_TIME"]		= datetime;			//记录创建时刻         
				tmmhr96["REC_CREATOR"]			= s.userid;			//记录创建责任者      

				///* 设置调用物料跟踪参数值 */
				//tmmhr96["EVENT_ID"]		= "MM27";
				//tmmhr96["EVENT_LINE_TYPE"]	= "00";
				//tmmhr96["SYSTEM_ID"]		= "MM00";
				//tmmhr96["FUNC_ID"]			= "mm0010b1f6_pro";
				//tmmhr96.TrimOrBlank();
				//tmmhr96.MergeTo(bcls_rec->Tables["MM0099"], false);

			}
			if (tmm0010["MAT_KIND"].ToString().Trim() == "CR")
			{
				tmmcr96.Reset();
				tmmcr96.CopyFrom(tmm0010);

				//生成物料材料跟踪号后4位流水号
				doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_mm00_mat_track_no, conn);
				if (doFlag < 0 || cs_mm00_mat_track_no.Trim() == "")
				{
					strcpy(s.msg, "物料材料跟踪号后4位流水号生成错误！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", __FUNCTION__, "cs_mm00_mat_track_no	= [{0}]", cs_mm00_mat_track_no);
				tmmcr96["MAT_TRACK_NO"]		= datetime + cs_mm00_mat_track_no;
				tmmcr96["PASS_BACKLOG_SEQ_NO"] = 1;
				tmmcr96["PROD_MAKER"]			= s.userid;
				tmmcr96["ST_NO"]				= tqmtqb0["ST_NO"];		// 出钢记号
				tmmcr96["SG_STD"]				= tqmtqb0["SG_STD"];
				tmmcr96["PRODUCT_FLAG"]		= "0";
				tmmcr96["MEASURE_WT_FLAG"]		= "0";
				tmmcr96["HOLD_FLAG"]			= "0";					//封锁标记
				tmmcr96["SURFACE_DECIDE_CODE"] = "1";					//表面判定代码(默认合格)     
				tmmcr96["SURFACE_DECIDE_TIME"] = datetime;				//表面判定时间     
				tmmcr96["SURFACE_DECIDE_MAKER"] = s.userid;				//表面判定责任者     
				tmmcr96["PCH_JUDGE_ABN"]		= "0";					//性能判定代码     
				tmmcr96["COMPLEX_DECIDE_CODE"] = "0";					//综合判定代码  
				tmmcr96["PRODUCT_PACK_FLAG"]	= "0";					//成品包装标志 
				tmmcr96["IN_FLAG"]				= "0";					//入库标记   
				tmmcr96["TRANSFER_FLAG"]		= "0";					//转库计划标记        
				tmmcr96["CONFM_FLAG"]			= "0";					//准发确认标记        	 
				tmmcr96["APP_DECIDE_FLAG"]		= "0";					//现货申报标记       	
				tmmcr96["PLAN_NO"]				= " ";					//计划号                 
				tmmcr96["STOCK_PLACE_NO"]		= " ";					//库号                
				tmmcr96["LAYERNO"]				= 0;					//层号                
				tmmcr96["REPAIR_FLAG"]			= "0";					//返修标记
				//tmmcr96.SAMPLE_TAKEN_FLAG = "0";					//已取样标记
				//tmmcr96.COLD_HOT_FLAG = "0";					//冷热标志 

				tmmcr96["REC_CREATE_TIME"] = datetime;			//记录创建时刻         
				tmmcr96["REC_CREATOR"] = s.userid;			//记录创建责任者      

				///* 设置调用物料跟踪参数值 */
				//tmmcr96["EVENT_ID"]		= "MM27";
				//tmmcr96["EVENT_LINE_TYPE"] = "00";
				//tmmcr96["SYSTEM_ID"]		= "MM00";
				//tmmcr96["FUNC_ID"]			= "mm0010b1f6_pro";
				//tmmcr96.TrimOrBlank();
				//tmmcr96.MergeTo(bcls_rec->Tables["MM0099"], false);

			}

			///* 设置物料路径接口参数 */
			//tmm0005.Reset();
			//tmm0005.MAT_NO			= tmm0010["MAT_NO"];
			//tmm0005.MAT_KIND		= tmm0010["MAT_KIND"];
			//tmm0005.MAT_PROD_FLAG	= "10";
			//tmm0005.MAIN_MAT_FLAG	= "1";	//主材料为1,非主材料为0;针对并卷,标识产出材料的跟踪号跟着此入口材料号
			//tmm0005.SPECAIL_FLAG	= "0";	//特殊标记 0-默认值,1-并卷同时分卷
			//tmm0005.TrimOrBlank();
			//tmm0005.MergeTo(bcls_rec->Tables["MM000501"], false);

			#if defined _SYS_PES || defined _SYS_MES 
			/* 调用入出库队列生成函数 */
			bcls_rec->Tables["WM00QUE"].Rows.Add(); // 创建一行
			bcls_rec->Tables["WM00QUE"].Rows[bcls_rec->Tables["WM00QUE"].Rows.get_Count() - 1]["STOCK_OPER_ORDER"]	= "1E"; //1E-外购入库
			bcls_rec->Tables["WM00QUE"].Rows[bcls_rec->Tables["WM00QUE"].Rows.get_Count() - 1]["MAT_NO"]			= tmm0010["MAT_NO"];
			bcls_rec->Tables["WM00QUE"].Rows[bcls_rec->Tables["WM00QUE"].Rows.get_Count() - 1]["MAT_NUM"]			= tmm0010["MAT_NUM"];
			bcls_rec->Tables["WM00QUE"].Rows[bcls_rec->Tables["WM00QUE"].Rows.get_Count() - 1]["UNIT_CODE"]			= tmm0010["MAT_LINE_TYPE"].ToString() + tmm0010["MAT_KIND"].ToString();
			bcls_rec->Tables["WM00QUE"].Rows[bcls_rec->Tables["WM00QUE"].Rows.get_Count() - 1]["OPER_FLAG"]			= "I"; //"I":新增  "D":删除
			#endif

			/* 更新外购料表的确认信息 */
			tmm0010["AFFIRM_FLAG"]		= "Y";
			tmm0010["AFFIRM_BY"]		= s.userid;
			tmm0010["AFFIRM_TIME"]		= datetime;
			tmm0010["REC_REVISOR"]		= s.userid;
			tmm0010["REC_REVISE_TIME"]	= datetime;
			tmm0010.TrimOrBlank();
			tmm0010.Update("AFFIRM_FLAG,"
						   "AFFIRM_TIME,"
						   "AFFIRM_BY,"
						   "REC_REVISOR,"
						   "REC_REVISE_TIME","MAT_NO");			
		}	
		cmd_inq.Close();

		/* 调用物料跟踪函数 */
		doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//#if defined _SYS_MMS || defined _SYS_MES 
		///* 调用路径跟踪函数 */
		//doFlag = f_mm000501_proc(bcls_rec, bcls_ret,conn);
		//if (doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//#endif

		/* 调用仓库接口：入库队列生成 */
		#if defined _SYS_PES || defined _SYS_MES 
		doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		#endif

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错,sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台,与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1,事务将被回滚
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
	//返回-1时事务将回滚,返回为0是事务将提交
	return doFlag;

}
