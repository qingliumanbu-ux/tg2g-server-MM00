/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-08-23 08:48:32
Description: 物料跟踪抛收发存函数
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_mm0099_ym(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString csSfType = "";
	CString eventId = "";

	try
	{
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];
		CString matKind = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["MAT_KIND"].ToString();

		CDynaTable matStock("TMM" + matKind + "WM", conn);

		for (int i = 0; i < dtNewMat.Rows.get_Count(); i++)
		{
			eventId = dtNewMat.Rows[i]["EVENT_ID"];

			//判断收发事件类型逻辑
			//1收 2发
			//A:1-  B:1+  C:2-  D:2+

			if (eventId == "WM04" || eventId == "MM18" || eventId == "MM09")
			{
				csSfType = "B";
			}
			else if (eventId == "MM76" || eventId == "QM05" || eventId == "MM04")
			{
				csSfType = "A";
			}
			else if (eventId == "MM03" || eventId == "MM0B" || eventId == "MM1B" || eventId == "MM12" ||
				eventId == "QM04" || eventId == "PM02" || eventId == "SM08" || eventId == "PM51")
			{
				csSfType = "AB";
			}
			else if (eventId == "SM02")
			{
				csSfType = "C";
			}
			else if (eventId == "SM01" || eventId == "MM06")//钢坯侧不会出现轧废
			{
				csSfType = "D";
			}
			else if (eventId == "WM01")
			{
				if (dtOldMat.Rows[i]["MAT_LINE_TYPE"].ToString() != dtNewMat.Rows[i]["MAT_LINE_TYPE"].ToString())
				{
					matStock.SetColVal("SF_OPER_FLAG", "1A");
					csSfType = "B";
				}
				else
				{
					if (dtNewMat.Rows[i]["MAT_RETURN_FLAG"].ToString() == "1")
					{
						matStock.SetColVal("SF_OPER_FLAG", "1B");
						csSfType = "C";
					}
					if (dtNewMat.Rows[i]["STOCK_OPER_ORDER"].ToString() = "1C")
					{
						matStock.SetColVal("SF_OPER_FLAG", "1C");
						csSfType = "C";
					}
					else if (dtNewMat.Rows[i]["STOCK_OPER_ORDER"].ToString() = "1M")
					{
						matStock.SetColVal("SF_OPER_FLAG", "1D");
						csSfType = "B";
					}
				}
			}
			else if (eventId == "WM02")
			{
				if (dtNewMat.Rows[i]["COMPLEX_DECIDE_CODE"].ToString() != "4")
				{
					if (dtNewMat.Rows[i]["MAT_RETURN_FLAG"].ToString() == "1")
					{
						matStock.SetColVal("SF_OPER_FLAG", "2B");
						csSfType = "A";
					}
					else
					{
						matStock.SetColVal("SF_OPER_FLAG", "2A");
						csSfType = "D";
					}
				}
			}

			if (csSfType.Find("A") != -1 || csSfType.Find("B") != -1)
			{
				matStock.SetColVal("IN_OUT_DIV", "1");
			}
			else if (csSfType.Find("C") != -1 || csSfType.Find("D") != -1)
			{
				matStock.SetColVal("IN_OUT_DIV", "2");
			}

			if (csSfType.Find("A") != -1 || csSfType.Find("C") != -1)
			{
				matStock.SetColVal("MAT_ACT_WT", -dtNewMat.Rows[i]["MAT_ACT_WT"].ToDecimal());
				matStock.SetColVal("MAT_THEORY_WT", -dtNewMat.Rows[i]["MAT_THEORY_WT"].ToDecimal());
				matStock.SetColVal("MAT_ACT_THICK", dtNewMat.Rows[i]["MAT_ACT_THICK"].ToDecimal());
				matStock.SetColVal("MAT_ACT_WIDTH", dtNewMat.Rows[i]["MAT_ACT_WIDTH"].ToDecimal());
				matStock.SetColVal("MAT_ACT_LEN", dtNewMat.Rows[i]["MAT_ACT_LEN"].ToDecimal());
				matStock.SetColVal("SG_SIGN", dtNewMat.Rows[i]["SG_SIGN"].ToString());
			}

			matStock.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMYM_RESUME_SEQ_NO", 20, conn));
			matStock.SetColVal("EVENT_ID", eventId);
			matStock.SetColVal("EVENT_DESC", dtNewMat.Rows[i]["EVENT_DESC"].ToString());
			matStock.SetColVal("EVENT_DATETIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
			matStock.SetColVal("REMARK", eventId + "(" + dtNewMat.Rows[i]["EVENT_DESC"].ToString() + ")");

			if (matStock.Insert() < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (csSfType == "AB")
			{
				matStock.SetColVal("MAT_ACT_WT", dtNewMat.Rows[i]["MAT_ACT_WT"].ToDecimal());
				matStock.SetColVal("MAT_THEORY_WT", dtNewMat.Rows[i]["MAT_THEORY_WT"].ToDecimal());
				matStock.SetColVal("MAT_ACT_THICK", dtNewMat.Rows[i]["MAT_ACT_THICK"].ToDecimal());
				matStock.SetColVal("MAT_ACT_WIDTH", dtNewMat.Rows[i]["MAT_ACT_WIDTH"].ToDecimal());
				matStock.SetColVal("MAT_ACT_LEN", dtNewMat.Rows[i]["MAT_ACT_LEN"].ToDecimal());
				matStock.SetColVal("SG_SIGN", dtNewMat.Rows[i]["SG_SIGN"].ToString());
				matStock.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMYM_RESUME_SEQ_NO", 20, conn));

				if (matStock.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
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


