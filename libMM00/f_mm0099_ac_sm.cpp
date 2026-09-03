/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      179648
Version:     1.0
Date:        2023年2月14日
Description: 物料跟踪抛成本函数（三明模式）

基地
TCBMMIS1_SM	生产实绩数据 - 炼钢 - 三明	f_cbmmis1_sm_rcv
TCBMMIW1_SM	生产实绩数据 - 棒线 - 三明	f_cbmmIw1_sm_rcv
TCBMMIP1_SM	生产实绩数据 - 中板 - 三明	f_cbmmip1_sm_rcv
罗源
TCBMMIS1_LY	生产实绩数据-炼钢-罗源	f_cbmmis1_ly_rcv
TCBMMIW1_LY	生产实绩数据-棒线-罗源	f_cbmmIw1_ly_rcv
TCBMMIP1_LY	生产实绩数据-中板-罗源	f_cbmmip1_ly_rcv
泉州
TCBMMIS1_QZ	生产实绩数据-炼钢-泉州	f_cbmmis1_qz_rcv
TCBMMIW1_QZ	生产实绩数据-棒线-泉州	f_cbmmIw1_qz_rcv
TCBMMIP1_QZ	生产实绩数据-中板-泉州	f_cbmmip1_qz_rcv

**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_IMPORT
int f_cbmmis1_sm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_cbmmiw1_sm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_cbmmip1_sm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

//BM2_FUNCTION_IMPORT
//int f_cbmmis1_ly_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//BM2_FUNCTION_IMPORT
//int f_cbmmiw1_ly_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//BM2_FUNCTION_IMPORT
//int f_cbmmip1_ly_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//
//BM2_FUNCTION_IMPORT
//int f_cbmmis1_qz_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//BM2_FUNCTION_IMPORT
//int f_cbmmiw1_qz_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//BM2_FUNCTION_IMPORT
//int f_cbmmip1_qz_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT 
int f_mm0099_ac_sm(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	EIClass cbmm_rec;

	try
	{
		/* 获取输入参数 */
		CDataTable dtEventData = bcls_rec->Tables["EVENT_DATA"];
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];
		CString baseCode = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["BASE_CODE"].ToString(); //基地代码
		CString matKind = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["MAT_KIND"].ToString();   //物料类型
		CString matLineType = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["MAT_LINE_TYPE"].ToString();   //产线类型
		
		Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数oldmat_records[{0}]", dtOldMat.Rows.get_Count());
		Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数newmat_records[{0}]", dtNewMat.Rows.get_Count());

		if (dtOldMat.Rows.get_Count() > 0)
		{
			Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数basecode_old[{0}]", dtOldMat.Rows[0]["BASE_CODE"].ToString());
		}
		if (dtNewMat.Rows.get_Count() > 0)
		{
			Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数basecode_NEW[{0}]", dtNewMat.Rows[0]["BASE_CODE"].ToString());
			Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数mat_no_NEW[{0}]", dtNewMat.Rows[0]["MAT_NO"].ToString());
		}

		Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数basecode[{0}]", baseCode);
		Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数matkind[{0}]", matKind);
		Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入参数matlinetype[{0}]", matLineType);

		CString acTable = ""; //成本抛账表
		CString mmTable = ""; //物料主档表 
		CString acFunc = "";  //要求成本统一函数

		if (matKind == "SM")
		{
			mmTable = "TMMSM01";
		}
		else if (matKind == "HP")
		{
			mmTable = "TMMHP01";
		}
		else if (matKind == "BW")
		{
			mmTable = "TMMBW01";
		}
		else
		{
			sprintf(s.msg, "材料类型MAT_KIND错误，不在三钢产品范围内(SM/HP/BW)!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		CString systemId = "";
		if (matLineType == "SM")
		{
			systemId = "IS";
		}
		else if (matLineType == "HP")
		{
			systemId = "IP";
		}
		else if (matLineType == "BW")
		{
			systemId = "IW";
		}

		if (baseCode == "1")
		{
			//总部
			acTable = "TCBMM" + systemId + "1_SM";
		}
		else if (baseCode == "2")
		{
			//罗源
			acTable = "TCBMM" + systemId + "1_LY";
		}
		else if (baseCode == "3")
		{
			//泉州
			acTable = "TCBMM" + systemId + "1_QZ";
		}
		else
		{
			sprintf(s.msg, "基地代码有误，请检查材料主档BASE_CODE字段值!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		Log::Trace("", __FUNCTION__, "基本校验完成...acTable = [{0}]",acTable);

		//CModel定义抛账表字段
		CModel matAcjc(acTable);
		CModel tmmxx01(mmTable);
		CModel tom01("TOM01");

		cbmm_rec.Tables[0].Columns.Add(matAcjc);

		CString acFlag = dtEventData.Rows[0]["EVENT_PROC_WAY_AC"].ToString();
		CString acMode = dtEventData.Rows[0]["KEYVALUE_1_AC"].ToString();

		Log::Trace("", __FUNCTION__, "获取事件抛账配置event_proc_way_3 [{0}] keyvalue_1_ac = [{1}]...",acFlag,acMode);

		if (acFlag != "0")
		{
			if (acMode == "" || acMode.GetLength() < 2)
			{
				sprintf(s.msg, "物料事件成本规则配置有误，请检查MM0097A1对应事件的成本抛账字段项!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			CString acModeFunc = acMode.Substring(0, 1); //成本功能代码 (详见成本接口文档)
			CString acModeFunc2 = acMode.Substring(1, 1); 

			Log::Trace("", __FUNCTION__, "拆分抛账功能 acModeFunc [{0}] acModeFunc2 = [{1}]...", acModeFunc, acModeFunc2);

			//0:抛单笔数据
			//1:(固定组合31, 产出带投入的模式)
			//G : (固定组合GH, 改判)
			//M : (固定组合MN, 重量修正)
			//T : (固定组合TR, 跨产线转库)
			CString acMark = " ";  //D:代表负账
			if (acMode.GetLength() > 2)
			{
				acMark = acMode.Substring(2, 1);
			}
			else acMark = " ";

			Log::Trace("", __FUNCTION__, "拆分抛账功能 acMark [{0}]...", acMark);

			int acRecNum = 0;

			//循环处理每一条NEW的处理（因为无论什么事件，都会有NEW的记录）
			//PS: 三钢这边LOT_NO字段记录投入产出关系
			//炼钢侧用HEAT_NO关联；棒线用ROLL_PLAN_NO; 中板用SLAB_NO;对应的原料库用
			for (int i = 0; i < dtNewMat.Rows.get_Count(); i++)
			{
				//由程序判断是否抛账或特殊赋值在此处添加逻辑
				//1.linggu添加2023年2月15日,针对转库入库的配置，当跨产线才抛
				if (acMode == "RT")  //对应入库点
				{
					//此判断如确定每次入库都是相同的操作，也可搬到外头执行
					CString fromFactoryDiv = dtOldMat.Rows[i]["FACTORY_DIV"].ToString();
					CString ToFactoryDiv = dtNewMat.Rows[i]["FACTORY_DIV"].ToString();

					if (fromFactoryDiv == ToFactoryDiv)
					{
						//以第一个材料来判断，直接跳出处理
						Log::Trace("", __FUNCTION__, "产线内倒运，不抛调拨!");
						break;
					}
				}

				Log::Trace("", __FUNCTION__, "linggu trace 111");
				CString cs_seq_no = "";

				if (acModeFunc2 != "0")
				{
					//插入OLD记录 以下只赋值必须项
					doFlag = f_mm0011("MMAC_RESUME_SEQ_NO", 6, cs_seq_no, conn);
					if (doFlag < 0 || cs_seq_no.Trim() == "")
					{
						sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MMAC_RESUME_SEQ_NO】是否正常!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					matAcjc["SEQ_ID"] = systemId+s.datetime+cs_seq_no; //由成本自己生成

					matAcjc["COMPANY_CODE"] = dtOldMat.Rows[i]["COMPANY_CODE"].ToString(); 
					matAcjc["SYSTEM_ID"] = systemId;
					matAcjc["OPER_TIME"] = s.datetime;
					matAcjc["FINA_POST_DATE"] = s.datetime; //TODO:此项暂时按抛账处理时间来,如需按业务点执行,再做定制参数传入
					matAcjc["FACTORY_NO"] = dtOldMat.Rows[i]["FACTORY_DIV"].ToString();
					matAcjc["UNIT_CODE"] = dtOldMat.Rows[i]["UNIT_CODE"].ToString(); //TODO:机组维度与成本不同，可能需转换一下
					matAcjc["UNIT_NAME"] = " ";

					Log::Trace("", __FUNCTION__, "linggu trace 111111");

					if (dtOldMat.Rows[i]["MAT_STATUS"].ToString() == "36")
					{
						matAcjc["PRODUCT_TYPE"] = "2";
						matAcjc["PRODUCT_TYPE_NAME"] = "产成品";
					}
					else if (dtOldMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString() == "3") //TODO:需按最终的码确定可利用材
					{
						matAcjc["PRODUCT_TYPE"] = "3";
						matAcjc["PRODUCT_TYPE_NAME"] = "可利用材";
					}
					else
					{
						matAcjc["PRODUCT_TYPE"] = "1";
						matAcjc["PRODUCT_TYPE_NAME"] = "半成品";
					}

					Log::Trace("", __FUNCTION__, "linggu trace 111222");

					matAcjc["PRODUCT_FLAG"] = dtOldMat.Rows[i]["MAT_ORIGIN"].ToString(); //来源标记
					matAcjc["PROD_CLASS_CODE"] = dtOldMat.Rows[i]["PROD_CLASS_CODE"].ToString(); //产品大类
					matAcjc["PROD_CODE"] = dtOldMat.Rows[i]["PROD_CODE"].ToString(); //品名
					matAcjc["PROD_CNAME"] = " "; //品名中文 
					matAcjc["SG_SIGN"] = dtOldMat.Rows[i]["SG_SIGN"].ToString(); //钢种
					matAcjc["SG_SIGN_NAME"] = " "; //钢种描述
					matAcjc["SPEC_REMARK"] = " "; //规格组距
					matAcjc["SPEC_1"] = dtOldMat.Rows[i]["MAT_THICK"];
					matAcjc["SPEC_2"] = dtOldMat.Rows[i]["MAT_WIDTH"];
					matAcjc["SPEC_3"] = dtOldMat.Rows[i]["MAT_LEN"];
				
					if (acMark == "D")
					{
						matAcjc["MAT_WT"] = -1 * dtOldMat.Rows[i]["MAT_WT"].ToDecimal();
					}
					else matAcjc["MAT_WT"] = dtOldMat.Rows[i]["MAT_WT"];

					Log::Trace("", __FUNCTION__, "linggu trace 111333");

					//matAcjc["SPEC_4"] = dtOldMat.Rows[i]["XX"]; H型钢 翼缘
					//matAcjc["SPEC_5"] = dtOldMat.Rows[i]["YY"]; H型钢 腹板

					matAcjc["STOCK_NO"] = dtOldMat.Rows[i]["STOCK_NO"].ToString();
					matAcjc["ORDER_NO"] = dtOldMat.Rows[i]["ORDER_NO"].ToString();

					//matAcjc["BTH_NO"] = dtOldMat.Rows[i]["ORDER_NO"].ToString(); //TODO:批次号

					matAcjc["PONO_NO"] = dtOldMat.Rows[i]["PONO"].ToString();
					matAcjc["HEAT_NO"] = dtOldMat.Rows[i]["HEAT_NO"].ToString();
					matAcjc["MAT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString();

					Log::Trace("", __FUNCTION__, "linggu trace 111444");

					if (dtOldMat.Rows[i]["ORDER_NO"].ToString() > " ")
					{
						tom01["ORDER_NO"] = dtOldMat.Rows[i]["ORDER_NO"].ToString();
						tom01.Query("ORDER_NO");

						matAcjc["DELIVY_STATUS_CODE"] = tom01["DELIVY_STATUS_CODE"].ToString();
						//matAcjc["DELIVY_STATUS_NAME"] = " ";
					}
					else
					{
						matAcjc["DELIVY_STATUS_CODE"] = " ";
						matAcjc["DELIVY_STATUS_NAME"] = " ";
					}
					
					matAcjc["REAL_WORK_TIME"] = 0; //TODO:目前无法提供
					matAcjc["PLAN_WORK_TIME"] = 0; //TODO:目前无法提供

					matAcjc["PLUNGE_FLAG"] = acModeFunc2;
					matAcjc["QUALITY_CODE"] = dtOldMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString();

					Log::Trace("", __FUNCTION__, "linggu trace 111555");

					if (systemId == "IS")
					{
						//炼钢侧，投入产出关系都用heat_no
						matAcjc["LOT_NO"] = dtOldMat.Rows[i]["HEAT_NO"].ToString(); //投入产出对应关系
					}
					else if (systemId == "IW")
					{
						if (matKind == "BW")
						{
							matAcjc["LOT_NO"] = dtOldMat.Rows[i]["ROLL_PLAN_NO"].ToString(); //材用轧制计划号
						}
						else
						{
							if (acModeFunc2 == "1")
							{
								//如果是投入类型的，则抛计划号
								matAcjc["LOT_NO"] = dtOldMat.Rows[i]["PLAN_NO"].ToString(); //投入产出用计划号
							}
							else matAcjc["LOT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString(); //原料库用方坯号
						}
					}
					else if (systemId == "IP")
					{
						if (matKind == "HP")
						{
							matAcjc["LOT_NO"] = dtOldMat.Rows[i]["SLAB_NO"].ToString(); //材用板坯号
						}
						else
						{
							matAcjc["LOT_NO"] = dtOldMat.Rows[i]["MAT_NO"].ToString(); //原料库用板坯号
						}
					}

					matAcjc.TrimOrBlank();
					//压入函数块
					cbmm_rec.Tables[0].Rows.Add();
					cbmm_rec.Tables[0].Rows[acRecNum].Merge(matAcjc);

					acRecNum++;

					matAcjc.Print();

				}


				//以下为特殊事件处理
				if (matKind == "SM" && acModeFunc == "3")
				{
					//炼钢产出和产出撤销时，抛铁水的投入
					doFlag = f_mm0011("MMAC_RESUME_SEQ_NO", 6, cs_seq_no, conn);
					if (doFlag < 0 || cs_seq_no.Trim() == "")
					{
						sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MMAC_RESUME_SEQ_NO】是否正常!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					matAcjc["SEQ_ID"] = systemId + s.datetime + cs_seq_no; 

					matAcjc["COMPANY_CODE"] = dtNewMat.Rows[i]["COMPANY_CODE"].ToString();
					matAcjc["SYSTEM_ID"] = systemId;
					matAcjc["OPER_TIME"] = s.datetime;
					matAcjc["FINA_POST_DATE"] = s.datetime; //TODO:此项暂时按抛账处理时间来,如需按业务点执行,再做定制参数传入
					matAcjc["FACTORY_NO"] = dtNewMat.Rows[i]["FACTORY_DIV"].ToString();
					matAcjc["UNIT_CODE"] = dtNewMat.Rows[i]["UNIT_CODE"].ToString(); //TODO:机组维度与成本不同，可能需转换一下
					matAcjc["UNIT_NAME"] = " ";
					matAcjc["PRODUCT_TYPE"] = "1";
					matAcjc["PRODUCT_TYPE_NAME"] = "半成品";
					matAcjc["PRODUCT_FLAG"] = dtNewMat.Rows[i]["MAT_ORIGIN"].ToString(); //来源标记
					matAcjc["PROD_CLASS_CODE"] = "T"; //产品大类-铁水
					matAcjc["PROD_CODE"] = "TS"; //品名-铁水临时
					matAcjc["PROD_CNAME"] = "铁水"; //品名中文 
					matAcjc["SG_SIGN"] = dtNewMat.Rows[i]["SG_SIGN"].ToString(); //钢种
					matAcjc["SG_SIGN_NAME"] = " "; //钢种描述
					matAcjc["SPEC_REMARK"] = " "; //规格组距
					matAcjc["SPEC_1"] = dtNewMat.Rows[i]["MAT_THICK"];
					matAcjc["SPEC_2"] = dtNewMat.Rows[i]["MAT_WIDTH"];
					matAcjc["SPEC_3"] = dtNewMat.Rows[i]["MAT_LEN"];

					if (acMark == "D")
					{
						matAcjc["MAT_WT"] = -1 * dtNewMat.Rows[i]["MAT_WT"].ToDecimal();
					}
					else matAcjc["MAT_WT"] = dtNewMat.Rows[i]["MAT_WT"];

					matAcjc["STOCK_NO"] = dtNewMat.Rows[i]["STOCK_NO"].ToString();
					matAcjc["ORDER_NO"] = dtNewMat.Rows[i]["ORDER_NO"].ToString();
					//matAcjc["BTH_NO"] = " "; //TODO:批次号
					matAcjc["PONO_NO"] = dtNewMat.Rows[i]["PONO"].ToString();
					matAcjc["HEAT_NO"] = dtNewMat.Rows[i]["HEAT_NO"].ToString();
					matAcjc["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();

					if (dtNewMat.Rows[i]["ORDER_NO"].ToString() > " ")
					{
						tom01["ORDER_NO"] = dtNewMat.Rows[i]["ORDER_NO"].ToString();
						tom01.Query("ORDER_NO");

						matAcjc["DELIVY_STATUS_CODE"] = tom01["DELIVY_STATUS_CODE"].ToString();
						//matAcjc["DELIVY_STATUS_NAME"] = " ";
					}
					else
					{
						matAcjc["DELIVY_STATUS_CODE"] = " ";
						matAcjc["DELIVY_STATUS_NAME"] = " ";
					}

					matAcjc["REAL_WORK_TIME"] = 0; //TODO:目前无法提供
					matAcjc["PLAN_WORK_TIME"] = 0; //TODO:目前无法提供
					matAcjc["PLUNGE_FLAG"] = "1"; //铁水投入
					matAcjc["QUALITY_CODE"] = dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString();
					matAcjc["LOT_NO"] = dtNewMat.Rows[i]["HEAT_NO"].ToString(); //投入产出对应关系--炼钢用炉号
					
					matAcjc.TrimOrBlank();
					//压入函数块
					cbmm_rec.Tables[0].Rows.Add();
					cbmm_rec.Tables[0].Rows[acRecNum].Merge(matAcjc);

					acRecNum++;
				}

				//特殊处理结束


				//插入NEW记录
				//插入OLD记录 以下只赋值必须项
				//插入OLD记录 以下只赋值必须项
				doFlag = f_mm0011("MMAC_RESUME_SEQ_NO", 6, cs_seq_no, conn);
				if (doFlag < 0 || cs_seq_no.Trim() == "")
				{
					sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MMAC_RESUME_SEQ_NO】是否正常!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				matAcjc["SEQ_ID"] = systemId + s.datetime + cs_seq_no; //由成本自己生成

				matAcjc["COMPANY_CODE"] = dtNewMat.Rows[i]["COMPANY_CODE"].ToString();
				matAcjc["SYSTEM_ID"] = systemId;
				matAcjc["OPER_TIME"] = s.datetime;
				matAcjc["FINA_POST_DATE"] = s.datetime; //TODO:此项暂时按抛账处理时间来,如需按业务点执行,再做定制参数传入
				matAcjc["FACTORY_NO"] = dtNewMat.Rows[i]["FACTORY_DIV"].ToString();
				matAcjc["UNIT_CODE"] = dtNewMat.Rows[i]["UNIT_CODE"].ToString(); //TODO:机组维度与成本不同，可能需转换一下
				matAcjc["UNIT_NAME"] = " ";

				Log::Trace("", __FUNCTION__, "linggu trace 222111");

				if (dtNewMat.Rows[i]["MAT_STATUS"].ToString() == "36")
				{
					matAcjc["PRODUCT_TYPE"] = "2";
					matAcjc["PRODUCT_TYPE_NAME"] = "产成品";
				}
				else if (dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString() == "3") //TODO:需按最终的码确定可利用材
				{
					matAcjc["PRODUCT_TYPE"] = "3";
					matAcjc["PRODUCT_TYPE_NAME"] = "可利用材";
				}
				else
				{
					matAcjc["PRODUCT_TYPE"] = "1";
					matAcjc["PRODUCT_TYPE_NAME"] = "半成品";
				}

				Log::Trace("", __FUNCTION__, "linggu trace 222222");

				matAcjc["PRODUCT_FLAG"] = dtNewMat.Rows[i]["MAT_ORIGIN"].ToString(); //来源标记
				matAcjc["PROD_CLASS_CODE"] = dtNewMat.Rows[i]["PROD_CLASS_CODE"].ToString(); //产品大类
				matAcjc["PROD_CODE"] = dtNewMat.Rows[i]["PROD_CODE"].ToString(); //品名
				matAcjc["PROD_CNAME"] = " "; //品名中文 
				matAcjc["SG_SIGN"] = dtNewMat.Rows[i]["SG_SIGN"].ToString(); //钢种
				matAcjc["SG_SIGN_NAME"] = " "; //钢种描述
				matAcjc["SPEC_REMARK"] = " "; //规格组距
				matAcjc["SPEC_1"] = dtNewMat.Rows[i]["MAT_THICK"];
				matAcjc["SPEC_2"] = dtNewMat.Rows[i]["MAT_WIDTH"];
				matAcjc["SPEC_3"] = dtNewMat.Rows[i]["MAT_LEN"];

				if (acMark == "D")
				{
					matAcjc["MAT_WT"] = -1 * dtNewMat.Rows[i]["MAT_WT"].ToDecimal();
				}
				else matAcjc["MAT_WT"] = dtNewMat.Rows[i]["MAT_WT"];

				Log::Trace("", __FUNCTION__, "linggu trace 222333");

				//matAcjc["SPEC_4"] = dtOldMat.Rows[i]["XX"]; H型钢 翼缘
				//matAcjc["SPEC_5"] = dtOldMat.Rows[i]["YY"]; H型钢 腹板

				matAcjc["STOCK_NO"] = dtNewMat.Rows[i]["STOCK_NO"].ToString();
				matAcjc["ORDER_NO"] = dtNewMat.Rows[i]["ORDER_NO"].ToString();

				//matAcjc["BTH_NO"] = dtOldMat.Rows[i]["ORDER_NO"].ToString(); //TODO:批次号

				matAcjc["PONO_NO"] = dtNewMat.Rows[i]["PONO"].ToString();
				matAcjc["HEAT_NO"] = dtNewMat.Rows[i]["HEAT_NO"].ToString();
				matAcjc["MAT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString();

				Log::Trace("", __FUNCTION__, "linggu trace 222444");

				if (dtNewMat.Rows[i]["ORDER_NO"].ToString() > " ")
				{
					tom01["ORDER_NO"] = dtNewMat.Rows[i]["ORDER_NO"].ToString();
					tom01.Query("ORDER_NO");

					matAcjc["DELIVY_STATUS_CODE"] = tom01["DELIVY_STATUS_CODE"].ToString();
					//matAcjc["DELIVY_STATUS_NAME"] = " ";
				}
				else
				{
					matAcjc["DELIVY_STATUS_CODE"] = " ";
					matAcjc["DELIVY_STATUS_NAME"] = " ";
				}

				matAcjc["REAL_WORK_TIME"] = 0; //TODO:目前无法提供
				matAcjc["PLAN_WORK_TIME"] = 0; //TODO:目前无法提供

				matAcjc["PLUNGE_FLAG"] = acModeFunc;
				matAcjc["QUALITY_CODE"] = dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString();

				if (systemId == "IS")
				{
					//炼钢侧，投入产出关系都用heat_no
					matAcjc["LOT_NO"] = dtNewMat.Rows[i]["HEAT_NO"].ToString(); //投入产出对应关系
				}
				else if (systemId == "IW")
				{
					if (matKind == "BW")
					{
						matAcjc["LOT_NO"] = dtNewMat.Rows[i]["ROLL_PLAN_NO"].ToString(); //材用轧制计划号
					}
					else
					{
						if (acModeFunc == "1")
						{
							//如果是投入类型的，则抛计划号
							matAcjc["LOT_NO"] = dtNewMat.Rows[i]["PLAN_NO"].ToString(); //投入产出用计划号
						}
						else matAcjc["LOT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString(); //原料库用方坯号
					}
				}
				else if (systemId == "IP")
				{
					if (matKind == "HP")
					{
						matAcjc["LOT_NO"] = dtNewMat.Rows[i]["SLAB_NO"].ToString(); //材用板坯号
					}
					else
					{
						matAcjc["LOT_NO"] = dtNewMat.Rows[i]["MAT_NO"].ToString(); //原料库用板坯号
					}
				}

				matAcjc.TrimOrBlank();

				//压入函数块
				cbmm_rec.Tables[0].Rows.Add();
				cbmm_rec.Tables[0].Rows[acRecNum].Merge(matAcjc);

				acRecNum++;

				matAcjc.Print();
			}

			if (acRecNum > 0)
			{
				//调用成本函数
				Log::Trace("", __FUNCTION__, "调用成本函数BEGIN...传入记录数[{0}] baseCode[{1}] matlinetype [{2}]", acRecNum,baseCode,matLineType);
				
				if (baseCode == "1" && matLineType == "SM")	doFlag = f_cbmmis1_sm_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "1" && matLineType == "HP")	doFlag = f_cbmmip1_sm_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "1" && matLineType == "BW")	doFlag = f_cbmmiw1_sm_rcv(&cbmm_rec, bcls_ret, conn);
				/*if (baseCode == "2" && matLineType == "SM")	doFlag = f_cbmmis1_ly_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "2" && matLineType == "HP")	doFlag = f_cbmmip1_ly_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "2" && matLineType == "BW")	doFlag = f_cbmmiw1_ly_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "3" && matLineType == "SM")	doFlag = f_cbmmis1_qz_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "3" && matLineType == "HP")	doFlag = f_cbmmip1_qz_rcv(&cbmm_rec, bcls_ret, conn);
				if (baseCode == "3" && matLineType == "BW")	doFlag = f_cbmmiw1_qz_rcv(&cbmm_rec, bcls_ret, conn);
*/
				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
			}
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


