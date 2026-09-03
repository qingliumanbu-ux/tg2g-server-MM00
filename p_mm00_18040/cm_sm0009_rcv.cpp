/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     178149
Version:    1.0
Date:       2019-08-27
Description: 发货码单接收
**************************************************/
//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmtc_mat_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送质保书界面档
int f_qmtc_zbs(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);     //质保书写界面档
int f_pmof99_v3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2F_ENTERACE_TELE(cm_sm0009_rcv)

int f_cm_sm0009_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CString matNo = "";
	CString matKind = "";
	CDecimal matWt = 0;
	CDecimal matDispWt = 0; //磅差重量
	CString prodClassCode = "";
	CString stackingType = "";
	CString stackingNo = "";
	CString orderNo = "";
	CString certiTypeCode = "";
	CString sqlstr = "";

	//产成品字段项
	CString privateRouteCode = "";
	CString privateRouteName = "";
	CString billOfLadingNo = "";
	CString trncCodeAct = "";
	CString carryCompanyCode = "";
	CString carryCompanyName = "";
	CDecimal stackingNum = 0;
	CDecimal stackingWt = 0;
	CDecimal stackingGrossWt = 0;
	CDecimal stackingDiscrepWt = 0;
	CDecimal stackingPiece = 0;
	CString delivyTime = "";
	CString outStockCode = "";
	CString inStockCode = "";
	CString delivyPlaceCode = "";
	CString delivyPlaceName = "";
	CString consignUserCode = "";
	CString consignUserName = "";
	CString balanceUserCode = "";
	CString balanceUserName = "";
	CString prodCode = "";
	CString prodName = "";
	CString vehicleNo = "";
	CString sgSign = "";
	CString wtMode = "";
	CString exportFlag = "";
	///////////////////

	CString aimSysCode = "";
	CString oldSysCode = "";

	CModel tsmsw51("TSMSW51");
	CModel tsmsw52("TSMSW52");
	CModel tom01("TOM01");

	int qm = 0;
	int mm = 0;
	int pm = 0;

	CModel tpmof03("TPMOF03");
	CModel tpmof02("TPMOF02");

	EIClass  qmtc_rec;  //抛质保书界面档
	EIClass  pmof_rec;  //合同跟踪
	EIClass  mm00_rec;  //物料跟踪

	CDbCommand cmd(conn);

	try
	{
		blkNum = mm00_rec.Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			mm00_rec.Tables.Add("MM0099");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");

			//转库用字段项
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "OUT_STOCK_TIME");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_NO");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "AIM_STORE");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "TRNP_MODE_CODE");
			mm00_rec.Tables["MM0099"].Columns.Add(DT_STRING, "SYS_CODE");
		}

		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "op_flag");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "stacking_no");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "mat_no");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "prod_class_code");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "stacking_type");

		pmof_rec.Tables.Add("PMOF99");
		pmof_rec.Tables["PMOF99"].Columns.Add(tpmof03);

		Log::Trace("", __FUNCTION__, "LINGGU TRACE 开始获取码单数据...");

		stackingNo = bcls_rec->Tables["BODY"].Rows[0]["STACKING_NO"].ToString().Trim();
		prodClassCode = bcls_rec->Tables["BODY"].Rows[0]["PROD_CLASS_CODE"].ToString().Trim();
		stackingType = bcls_rec->Tables["BODY"].Rows[0]["STACKING_TYPE"].ToString().Trim();
		orderNo = bcls_rec->Tables["BODY"].Rows[0]["ORDER_NO"].ToString().Trim();
		billOfLadingNo = bcls_rec->Tables["BODY"].Rows[0]["BILL_OF_LADING_NO"].ToString().Trim();
		if (bcls_rec->Tables["BODY"].Columns.Contains("TRNC_CODE_ACT"))
		{
			trncCodeAct = bcls_rec->Tables["BODY"].Rows[0]["TRNC_CODE_ACT"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("CARRY_COMPANY_CODE"))
		{
			carryCompanyCode = bcls_rec->Tables["BODY"].Rows[0]["CARRY_COMPANY_CODE"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("CARRY_COMPANY_NAME"))
		{
			carryCompanyName = bcls_rec->Tables["BODY"].Rows[0]["CARRY_COMPANY_NAME"].ToString().Trim();
		}
		stackingNum = bcls_rec->Tables["BODY"].Rows[0]["STACKING_NUM"];
		stackingWt = bcls_rec->Tables["BODY"].Rows[0]["STACKING_WT"];
		if (bcls_rec->Tables["BODY"].Columns.Contains("STACKING_GROSS_WT"))
		{
			stackingGrossWt = bcls_rec->Tables["BODY"].Rows[0]["STACKING_GROSS_WT"];
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("STACKING_DISCREP_WT"))
		{
			stackingDiscrepWt = bcls_rec->Tables["BODY"].Rows[0]["STACKING_DISCREP_WT"];
		}
		stackingPiece = stackingNum; //暂时同NUM，有需要再定制
		delivyTime = bcls_rec->Tables["BODY"].Rows[0]["DELIVY_TIME"].ToString().Trim();
		outStockCode = bcls_rec->Tables["BODY"].Rows[0]["OUT_STOCK_CODE"].ToString().Trim();
		inStockCode = bcls_rec->Tables["BODY"].Rows[0]["IN_STOCK_CODE"].ToString().Trim();

		if (bcls_rec->Tables["BODY"].Columns.Contains("PRIVATE_ROUTE_CODE"))
		{
			privateRouteCode = bcls_rec->Tables["BODY"].Rows[0]["PRIVATE_ROUTE_CODE"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("PRIVATE_ROUTE_NAME"))
		{
			privateRouteName = bcls_rec->Tables["BODY"].Rows[0]["PRIVATE_ROUTE_NAME"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("DELIVY_PLACE_CODE"))
		{
			delivyPlaceCode = bcls_rec->Tables["BODY"].Rows[0]["DELIVY_PLACE_CODE"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("DELIVY_PLACE_NAME"))
		{
			delivyPlaceName = bcls_rec->Tables["BODY"].Rows[0]["DELIVY_PLACE_NAME"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("CONSIGN_USER_CODE"))
		{
			consignUserCode = bcls_rec->Tables["BODY"].Rows[0]["CONSIGN_USER_CODE"].ToString().Trim();
		}
		if (bcls_rec->Tables["BODY"].Columns.Contains("CONSIGN_USER_NAME"))
		{
			consignUserName = bcls_rec->Tables["BODY"].Rows[0]["CONSIGN_USER_NAME"].ToString().Trim();
		}
		prodCode = bcls_rec->Tables["BODY"].Rows[0]["PROD_CODE"].ToString().Trim();
		prodName = bcls_rec->Tables["BODY"].Rows[0]["PROD_CNAME"].ToString().Trim();
		vehicleNo = bcls_rec->Tables["BODY"].Rows[0]["VEHICLE_NO"].ToString().Trim();
		sgSign = bcls_rec->Tables["BODY"].Rows[0]["SG_SIGN"].ToString().Trim();
		if (bcls_rec->Tables["BODY"].Columns.Contains("BALANCE_USER_CODE"))
		{
			balanceUserCode = bcls_rec->Tables["BODY"].Rows[0]["BALANCE_USER_CODE"].ToString().Trim();
		}
		else balanceUserCode = " ";
		if (bcls_rec->Tables["BODY"].Columns.Contains("BALANCE_USER_NAME"))
		{
			balanceUserName = bcls_rec->Tables["BODY"].Rows[0]["BALANCE_USER_NAME"].ToString().Trim();
		}
		else balanceUserName = " ";
		if (bcls_rec->Tables["BODY"].Columns.Contains("WT_MODE"))
		{
			wtMode = bcls_rec->Tables["BODY"].Rows[0]["WT_MODE"].ToString().Trim();
		}
		else wtMode = " ";
		if (bcls_rec->Tables["BODY"].Columns.Contains("EXPORT_FLAG"))
		{
			exportFlag = bcls_rec->Tables["BODY"].Rows[0]["EXPORT_FLAG"].ToString().Trim();
		}
		else exportFlag = " ";

		Log::Trace("", __FUNCTION__, "LINGGU TRACE 读取合同主表数据...[{0}]", orderNo);
		tom01["ORDER_NO"] = orderNo;
		tom01.Query("ORDER_NO");

		cmd.SetCommandText("SELECT TC_MARK FROM TSI0021 WHERE STOCK_NO = @stockCode");
		cmd.Parameters.Set("stockCode", outStockCode);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			oldSysCode = cmd.GetString(1);
		}
		cmd.Close();

		cmd.SetCommandText("SELECT TC_MARK FROM TSI0021 WHERE STOCK_NO = @instockCode");
		cmd.Parameters.Set("stockCode", inStockCode);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			aimSysCode = cmd.GetString(1);
		}
		cmd.Close();


		if (stackingType == "0" || stackingType == "3" || stackingType == "T")
		{
			//linggu 插入51表
			tsmsw51["REC_CREATOR"] = "SM0009";
			tsmsw51["REC_CREATE_TIME"] = delivyTime;
			tsmsw51["COMPANY_CODE"] = tom01["COMPANY_CODE"];
			tsmsw51["COMPANY_NAME"] = tom01["COMPANY_NAME"]; //根据材料信息写入
			tsmsw51["BASE_CODE"] = tom01["BASE_CODE"]; //基地代码，根据材料档写入
			tsmsw51["BASE_NAME"] = tom01["BASE_NAME"]; //基地代码，根据材料档写入
			tsmsw51["STACKING_NO"] = stackingNo; //码单号

			//删除原有码单数据
			Log::Trace("", __FUNCTION__, "LINGGU TRACE 清除已有码单信息...[{0}]", stackingNo);

			tsmsw51.Delete("STACKING_NO");
			tsmsw52["STACKING_NO"] = stackingNo;
			tsmsw52.Delete("STACKING_NO");

			tsmsw51["STACKING_TYPE"] = stackingType;
			tsmsw51["BILL_OF_LADING_NO"] = billOfLadingNo;

			tsmsw51["ORDER_NO_ERP"] = orderNo;
			tsmsw51["ORDER_NO"] = orderNo;
			tsmsw51["TRNP_CODE_ACT"] = trncCodeAct;
			tsmsw51["CARRY_COMPANY_CODE"] = carryCompanyCode;
			tsmsw51["CARRY_COMPANY_NAME"] = carryCompanyName;
			tsmsw51["STACKING_WT"] = stackingWt;
			tsmsw51["STACKING_GROSS_WT"] = stackingGrossWt;
			tsmsw51["STACKING_DISCREP_WT"] = stackingDiscrepWt;

			tsmsw51["STACKING_NUM"] = stackingNum;
			tsmsw51["STACKING_PIECE"] = stackingPiece;
			tsmsw51["OUT_STOCK_CODE"] = outStockCode;
			tsmsw51["DELIVY_PLACE_CODE_ACT"] = delivyPlaceCode;
			tsmsw51["DELIVY_PLACE_NAME_ACT"] = delivyPlaceName;
			tsmsw51["PRIVATE_ROUTE_CODE"] = privateRouteCode;
			tsmsw51["PRIVATE_ROUTE_NAME"] = privateRouteName;
			tsmsw51["CONSIGN_USER_CODE"] = consignUserCode;
			tsmsw51["CONSIGN_USER_NAME"] = consignUserName;
			tsmsw51["BALANCE_USER_CODE"] = balanceUserCode;
			tsmsw51["BALANCE_USER_NAME"] = balanceUserName;

			tsmsw51["DELIVY_TIME"] = delivyTime;
			tsmsw51["VEHICLE_NO"] = vehicleNo;
			tsmsw51["EXPORT_FLAG"] = exportFlag;
			tsmsw51["PROD_CODE"] = prodCode;
			tsmsw51["PROD_CNAME"] = prodName;

			tsmsw51["SG_SIGN"] = sgSign;
			tsmsw51["THREE_READY_NO"] = " "; //no get
			tsmsw51["PONDER_NO"] = " "; // no use
			tsmsw51["WT_MODE"] = wtMode;
			tsmsw51["CARRIER_NO"] = " "; //no use承运单号
			tsmsw51["MOVE_PLACE"] = " "; //no use
			tsmsw51["SHIP_LOT_NO"] = " "; //no use
			tsmsw51["DELIVY_QTY_FLAG"] = " "; //no use 交货量标识

			tsmsw51["ORDER_THICK"] = tom01["ORDER_THICK"];
			tsmsw51["ORDER_WIDTH"] = tom01["ORDER_WIDTH"];
			tsmsw51["ORDER_MIN_LEN"] = tom01["ORDER_MIN_LEN"];
			tsmsw51["ORDER_MAX_LEN"] = tom01["ORDER_MAX_LEN"];

			Log::Trace("", __FUNCTION__, "LINGGU TRACE 形成TSMSW51数据...[{0}]", stackingNo);

			tsmsw51.Insert();
		}

		Log::Trace("", __FUNCTION__, "LINGGU TRACE 开始获取码单明细数据...记录数{0}", bcls_rec->Tables["DETAIL"].Rows.get_Count());

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables["DETAIL"].Rows.get_Count(); i++)
		{
			matNo = bcls_rec->Tables["DETAIL"].Rows[i]["CUST_MAT_NO"].ToString().Trim();
			matKind = bcls_rec->Tables["DETAIL"].Rows[i]["MAT_KIND"].ToString().Trim();
			matWt = bcls_rec->Tables["DETAIL"].Rows[i]["MAT_NET_WT"];
			matDispWt = bcls_rec->Tables["DETAIL"].Rows[i]["MAT_DISCREP_WT"];
			
			Log::Trace("", __FUNCTION__, "LINGGU TRACE 开始处理...第{0}个 材料号[{1}]",i+1,matNo);

			if (matNo == "")
			{
				break;
			}

			if (stackingType == "0" || stackingType == "3" || stackingType == "T")
			{
				qmtc_rec.Tables[0].Rows.Add();
				qmtc_rec.Tables[0].Rows[qm]["op_flag"] = "1"; //1-码单生成，3-码单红冲
				qmtc_rec.Tables[0].Rows[qm]["stacking_no"] = stackingNo;
				qmtc_rec.Tables[0].Rows[qm]["mat_no"] = matNo;//Modified tyr  2016.6.23 原本传空值
				qmtc_rec.Tables[0].Rows[qm]["prod_class_code"] = prodClassCode;
				qmtc_rec.Tables[0].Rows[qm]["stacking_type"] = stackingType;//Modified tyr  2016.6.23 原本传空值
				
				//插入52表
				tsmsw52["REC_CREATOR"] = "SM0009";
				tsmsw52["REC_CREATE_TIME"] = delivyTime;
				tsmsw52["COMPANY_CODE"] = tom01["COMPANY_CODE"];
				tsmsw52["COMPANY_NAME"] = tom01["COMPANY_NAME"]; //根据材料信息写入
				tsmsw52["BASE_CODE"] = tom01["BASE_CODE"]; //基地代码，根据材料档写入
				tsmsw52["BASE_NAME"] = tom01["BASE_NAME"]; //基地代码，根据材料档写入

				tsmsw52["CUST_MAT_NO"] = matNo; //材料号
				tsmsw52["MAT_NO"] = matNo; //材料号
				tsmsw52["PACK_NO"] = matNo; //捆包号
				tsmsw52["CUST_MAT_NO"] = matNo; //材料号
				tsmsw52["CUST_MAT_NO"] = matNo; //材料号

				tsmsw52["STACKING_NO"] = stackingNo; //码单号
				tsmsw52["STOCK_CODE"] = outStockCode; 
				tsmsw52["OLD_STOCK_CODE"] = " ";
				tsmsw52["DST_STOCK_CODE"] = outStockCode;
				tsmsw52["STOCK_ROOM_NO"] = " "; //NO GET
				tsmsw52["ORDER_NO"] = orderNo;
				tsmsw52["ORDER_NO_ERP"] = orderNo;
				tsmsw52["BILL_OF_LADING_NO"] = billOfLadingNo;
				tsmsw52["DELIVY_TIME"] = delivyTime;
				tsmsw52["VEHICLE_NO"] = vehicleNo;
				tsmsw52["WT_MODE"] = wtMode;
				tsmsw52["EXPORT_FLAG"] = exportFlag;

				tsmsw52["TRNP_CODE_ACT"] = trncCodeAct;
				tsmsw52["DELIVY_PLACE_CODE"] = delivyPlaceCode;
				tsmsw52["DELIVY_PLACE_NAME"] = delivyPlaceName;

				tsmsw52["PRIVATE_ROUTE_CODE"] = privateRouteCode;
				tsmsw52["PRIVATE_ROUTE_NAME"] = privateRouteName;

				tsmsw52["CONSIGN_USER_CODE"] = consignUserCode;
				tsmsw52["CONSIGN_USER_NAME"] = consignUserName;
				tsmsw52["ORDER_CUST_CODE"] = tom01["ORDER_CUST_CODE"];
				tsmsw52["ORDER_CUST_CNAME"] = tom01["ORDER_CUST_CNAME"];
				tsmsw52["BALANCE_USER_CODE"] = balanceUserCode;
				tsmsw52["BALANCE_USER_NAME"] = balanceUserName;

				tsmsw52["PROD_CODE"] = prodCode;
				tsmsw52["PROD_CNAME"] = prodName;

				tsmsw52["SG_SIGN"] = sgSign;

				tsmsw52["CARRY_COMPANY_CODE"] = carryCompanyCode;
				tsmsw52["CARRY_COMPANY_NAME"] = carryCompanyName;

				tsmsw52["PROD_CLASS_CODE"] = tom01["PROD_CLASS_CODE"];
				tsmsw52["PROD_CLASS_DESC"] = tom01["PROD_CLASS_DESC"];

				tsmsw52["TRIM_FLAG"] = tom01["TRIM_FLAG"];
				
				//再补充上原52表数据项
				sqlstr = "SELECT SAMPLE_LOT_NO,COMPANY_CODE,COMPANY_NAME,MAT_LINE_TYPE,MAT_NO,PACK_NO,HEAT_NO,ST_NO,MAT_SHAPE_FLAG, "
					" MAT_THEORY_WT,MAT_ACT_WT,MAT_NUM,MAT_THICK,MAT_WIDTH,MAT_LEN,MAT_STATUS,CONFM_PLAN_NO,PSC,APN,MSC,MSC_LINE_NO,SG_STD, "
					" COMPLEX_DECIDE_CODE,COMPLEX_DECIDE_MAKER,COMPLEX_DECIDE_TIME,OLD_ORDER_NO, "
					" WHOLE_BACKLOG_CODE,WHOLE_BACKLOG,CROSS_CODE,PRODUCT_CODE "
					" FROM TMM" + matKind + "01 WHERE MAT_NO = @matNo";
				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("matNo", matNo);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					tsmsw52["SAMPLE_LOT_NO"] = cmd.GetString(1);
					tsmsw52["MAT_LINE_TYPE"] = cmd.GetString(4);
					tsmsw52["PACK_NO"] = cmd.GetString(6);
					tsmsw52["HEAT_NO"] = cmd.GetString(7);
					tsmsw52["ST_NO"] = cmd.GetString(8);

					tsmsw52["MAT_SHAPE_FLAG"] = cmd.GetString(9);
					tsmsw52["MAT_THEORY_WT"] = cmd.GetDecimal(10);
					tsmsw52["MAT_ACT_WT"] = cmd.GetDecimal(11);
					tsmsw52["MAT_NET_WT"] = matWt;
					tsmsw52["MAT_DISCREP_WT"] = matDispWt;
					tsmsw52["MAT_GROSS_WT"] = matWt + matDispWt;
					tsmsw52["MAT_NUM"] = cmd.GetDecimal(12);
					tsmsw52["MAT_THICK"] = cmd.GetDecimal(13);
					tsmsw52["MAT_WIDTH"] = cmd.GetDecimal(14);
					tsmsw52["MAT_LEN"] = cmd.GetDecimal(15);
					tsmsw52["MAT_STATUS"] = cmd.GetString(16);

					tsmsw52["MAT_KIND"] = matKind;
					tsmsw52["CONFM_PLAN_NO"] = cmd.GetString(17);
					tsmsw52["PSC"] = cmd.GetString(18);
					tsmsw52["APN"] = cmd.GetString(19);

					tsmsw52["MSC"] = cmd.GetString(20);

					Log::Trace("", __FUNCTION__, "LINGGU TRACE 111");

					//tsmsw52["MSC_LINE_NO"] = cmd.GetDecimal(21);
					//tsmsw52["SG_STD"] = cmd.GetString(22);

					tsmsw52["COMPLEX_DECIDE_CODE"] = cmd.GetString(23);

					Log::Trace("", __FUNCTION__, "LINGGU TRACE 222");

					tsmsw52["OLD_ORDER_NO"] = cmd.GetString(26);
					tsmsw52["WHOLE_BACKLOG_CODE"] = cmd.GetString(27);
					tsmsw52["WHOLE_BACKLOG"] = cmd.GetString(28);
					tsmsw52["CROSS_CODE"] = cmd.GetString(29);
					tsmsw52["PRODUCT_CODE"] = cmd.GetString(30);

					Log::Trace("", __FUNCTION__, "LINGGU TRACE 333");

					if (matKind == "BW")
					{
						tsmsw52["CUST_MAT_SPECS"] = "φ" || cmd.GetDecimal(13).ToString();
					}
					else if (matKind == "HP")
					{
						tsmsw52["CUST_MAT_SPECS"] = cmd.GetDecimal(13).ToString() + "*" + cmd.GetDecimal(14).ToString() + "*" + cmd.GetDecimal(15).ToString();
					}
					else
					{
						//TODO:
					}
				}
				cmd.Close();
				
				tsmsw52.Insert();

				Log::Trace("", __FUNCTION__, "LINGGU TRACE 插入...第{0}个 材料号[{1}]", i + 1, matNo);

				qm++;

				/* 设置物料跟踪参数*/
				mm00_rec.Tables["MM0099"].Rows.Add();
				mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_ID"] = "SM01";
				mm00_rec.Tables["MM0099"].Rows[mm]["SYSTEM_ID"] = "MM" + matKind;
				mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_LINE_TYPE"] = "00";
				mm00_rec.Tables["MM0099"].Rows[mm]["FUNC_ID"] = s.svc_name;
				mm00_rec.Tables["MM0099"].Rows[mm]["MAT_KIND"] = matKind;
				mm00_rec.Tables["MM0099"].Rows[mm]["MAT_NO"] = matNo;
				mm++;
				
				/*调用生产合同跟踪BEGIN*/
				CString wholeBacklog = "";
				tpmof02["ORDER_NO"] = orderNo;
				tpmof02["BACKLOG_FLAG"] = "0";
				tpmof02["WHOLE_BACKLOG_CODE"] = "9B";
				if (!tpmof02.Query("ORDER_NO,BACKLOG_FLAG,WHOLE_BACKLOG_CODE"))
				{
					sprintf(s.msg, _RES("合同全程途径码读取失败")/*合同全程途径码读取失败*/);
					throw	CApplicationException(-1, s.msg, s.svc_name);
				}

				Log::Trace("", __FUNCTION__, "LINGGU TRACE 抛合同跟踪...9B工序 whole_backlog_seq = {0}", tpmof02["WHOLE_BACKLOG_SEQ"]);

				tpmof03["EVENT_ID"] = "72";
				tpmof03["SYSTEM_ID"] = "MM";								//子系统标识
				tpmof03["FUNC_ID"] = "cm_sm0009_rcv"; 				//功能标识
				tpmof03["ORDER_NO"] = orderNo;	//合同号
				tpmof03["BACKLOG_FLAG"] = "0";	                    //主副制程标记
				tpmof03["WHOLE_BACKLOG"] = tpmof02["WHOLE_BACKLOG"];	//全程途径码
				tpmof03["WHOLE_BACKLOG_NO"] = tpmof02["WHOLE_BACKLOG_NO"];

				wholeBacklog = tpmof02["WHOLE_BACKLOG"];
				tpmof03["WHOLE_BACKLOG_SEQ"] = tpmof02["WHOLE_BACKLOG_SEQ"];
				tpmof03["WHOLE_BACKLOG_CODE"] = "9B";
				tpmof03["MAT_NO"] = matNo;
				tpmof03["WT"] = matWt;

				pmof_rec.Tables["PMOF99"].Rows.Add();
				pmof_rec.Tables["PMOF99"].Rows[pm].Merge(tpmof03);
				pm++;

				if (matDispWt > 0)
				{
					tpmof03["EVENT_ID"] = "75";
					tpmof03["WT"] = matDispWt;
					pmof_rec.Tables["PMOF99"].Rows.Add();
					pmof_rec.Tables["PMOF99"].Rows[0].Merge(tpmof03);
					pm++;
				}
			}

			//linggu add 2023年2月8日
			//当销售过来是转库的码单类型时，需做判断
			//如果是厂内库转厂内库、厂内库转厂外库的情况，仅做产销更新（pes由发货模块调用）
			//如果是厂外库转回厂内库，则需要通过同步电文通知PES做厂内库的入库（pes已归档）
			if (stackingType == "2" )
			{
				Log::Trace("", __FUNCTION__, "LINGGU TRACE 转库码单从[{0}]转到[{1}]", oldSysCode, aimSysCode);

				if (aimSysCode != oldSysCode && aimSysCode == "00")
				{
					Log::Trace("", __FUNCTION__, "LINGGU TRACE 厂外转厂内，需同步PES!");

					mm00_rec.Tables["MM0099"].Rows.Add();
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_ID"] = "SMR2";  //相同分区时，同步			
					mm00_rec.Tables["MM0099"].Rows[mm]["MAT_KIND"] = matKind;
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_LINE_TYPE"] = "00";
					mm00_rec.Tables["MM0099"].Rows[mm]["SYSTEM_ID"] = "MM" + matKind;
					mm00_rec.Tables["MM0099"].Rows[mm]["FUNC_ID"] = s.svc_name;
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_DESC"] = "厂外库转回厂内库";
					mm00_rec.Tables["MM0099"].Rows[mm]["MAT_NO"] = matNo;
					mm00_rec.Tables["MM0099"].Rows[mm]["OUT_STOCK_TIME"] = s.datetime;
					mm00_rec.Tables["MM0099"].Rows[mm]["STOCK_NO"] = inStockCode;
					mm00_rec.Tables["MM0099"].Rows[mm]["STOCK_OPER_ORDER"] = "1N";
					mm00_rec.Tables["MM0099"].Rows[mm]["AIM_STORE"] = inStockCode;
					mm00_rec.Tables["MM0099"].Rows[mm]["TRNP_MODE_CODE"] = "";
					mm00_rec.Tables["MM0099"].Rows[mm]["SYS_CODE"] = aimSysCode;

					mm++;
				}
				else
				{
					//产成品转库出库
					Log::Trace("", __FUNCTION__, "LINGGU TRACE 非厂外转厂内，仅更新产销!");

					mm00_rec.Tables["MM0099"].Rows.Add();
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_ID"] = "SMRC";  //相同分区时，同步			
					mm00_rec.Tables["MM0099"].Rows[mm]["MAT_KIND"] = matKind;
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_LINE_TYPE"] = "00";
					mm00_rec.Tables["MM0099"].Rows[mm]["SYSTEM_ID"] = "MM" + matKind;
					mm00_rec.Tables["MM0099"].Rows[mm]["FUNC_ID"] = s.svc_name;
					mm00_rec.Tables["MM0099"].Rows[mm]["EVENT_DESC"] = "厂外库转回厂内库";
					mm00_rec.Tables["MM0099"].Rows[mm]["MAT_NO"] = matNo;
					mm00_rec.Tables["MM0099"].Rows[mm]["OUT_STOCK_TIME"] = s.datetime;
					mm00_rec.Tables["MM0099"].Rows[mm]["STOCK_NO"] = outStockCode;
					mm00_rec.Tables["MM0099"].Rows[mm]["AIM_STORE"] = inStockCode;

					mm++;

				}
			}
			
		}

		if (mm > 0)
		{
			Log::Trace("", __FUNCTION__, "调用物料函数BEGIN...传入记录数[{0}]", mm);
			doFlag = f_mm0099(&mm00_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(doFlag, s.msg, log.Location);
			}
		}

		//抛合同跟踪
		if (pm > 0)
		{
			Log::Trace("", __FUNCTION__, "调用合同跟踪函数BEGIN...");
			doFlag = f_pmof99_v3(&pmof_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}


		if (stackingType == "0" || stackingType == "3" || stackingType == "T")
		{
			sqlstr = "select CERTI_TYPE_CODE from tqmtp01 where psc= (select psc from tom01 where order_no = @orderNo)";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("orderNo", orderNo);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				certiTypeCode = cmd.GetString(1);
			}
			cmd.Close();

			if (qm>0 && certiTypeCode != "00")
			{
				doFlag = f_qmtc_mat_rcv(&qmtc_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "调用质保书界面档函数！");
					strcat(s.msg, "调用f_qmtc_mat_rcv出错！");
					s.flag = -1;
					doFlag = -1;
					return -1;
				}
				CTransactionManager::Commit(0);
				CTransactionManager::Begin(0, 0);
				doFlag = f_qmtc_zbs(&qmtc_rec, bcls_ret, conn);
				if (doFlag < 0 || s.flag < 0)  //Modified  lx 2016-03-07
				{
					Log::Trace("", __FUNCTION__, "调用质量函数报错");
					strcat(s.msg, "调用f_qmtc_zbs出错！,请找质量质保书负责人");
					CTransactionManager::Abort(0);
					CTransactionManager::Begin(0, 0);
					s.flag = 0;
					doFlag = 0;
				}
			}
		}		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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

	return doFlag;
}
