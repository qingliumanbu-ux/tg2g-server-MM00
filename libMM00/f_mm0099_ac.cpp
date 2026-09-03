/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-08-23 08:47:19
Description: 物料跟踪抛成本函数
**************************************************/

#include "CDynaTable.h"
BM2_FUNCTION_IMPORT
int f_mm009b(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT 
int f_mm0099_ac(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	try
	{

		/* 根据抛账事件表, 过滤不需抛成本账的数据 */
		doFlag = f_mm009b(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获取输入参数 */
		CDataTable dtEventData = bcls_rec->Tables["EVENT_DATA"];
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];
		CString matKind = bcls_rec->Tables["NEWMM_TABLE"].Rows[0]["MAT_KIND"].ToString();

		CDynaTable matAcjc("TMM" + matKind + "AC", conn);

		if (dtEventData.Rows[0]["EVENT_PROC_WAY_3"].ToString() == "1")
		{

			for (int i = 0; i < dtOldMat.Rows.get_Count(); i++)
			{
				Log::Info("", __FUNCTION__, "根据事件抛账表的字段新旧数据相同 tmm009b.ARCHIVE_FLAG = [{0}]", dtOldMat.Rows[i]["ARCHIVE_FLAG"].ToString());

				/* 根据事件抛账表的字段新旧数据相同,则不抛成本账 */
				if (dtOldMat.Rows[i]["ARCHIVE_FLAG"].ToString() == "0")
				{
					continue;
				}
				Log::Info("", __FUNCTION__, "根据事件抛账表的字段新旧数据相同 matAcjc.Insert = [{0}]", dtOldMat.Rows[i]["ARCHIVE_FLAG"].ToString());

				matAcjc.MergeFrom(dtOldMat.Rows[i]);
				matAcjc.CopyColVal("EVENT_ID", dtNewMat.Rows[i]);
				matAcjc.CopyColVal("EVENT_DESC", dtNewMat.Rows[i]);
				matAcjc.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMAC_RESUME_SEQ_NO", 20, conn));
				matAcjc.SetColVal("IN_OUT_DIV", "2");
				matAcjc.SetColVal("EVENT_DATETIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
				matAcjc.SetColVal("REMARK", matAcjc.GetColValString("EVENT_ID") + "(" + matAcjc.GetColValString("EVENT_DESC") + ")");
				matAcjc.SetColVal("MAT_ACT_WT", -matAcjc.GetColValDecimal("MAT_ACT_WT"));
				matAcjc.SetColVal("MAT_THEORY_WT", -matAcjc.GetColValDecimal("MAT_THEORY_WT"));
				matAcjc.CopyColVal("ACJC_RELATION_ID", "RESUME_SEQ_NO");

				if (matAcjc.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				matAcjc.MergeFrom(dtNewMat.Rows[i]);
				matAcjc.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMAC_RESUME_SEQ_NO", 20, conn));

				if (matAcjc.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		else
		{
			for (int i = 0; i < dtNewMat.Rows.get_Count(); i++)
			{
				if (dtNewMat.Rows[i]["EVENT_ID"].ToString() == "MM06")
				{
					matAcjc.MergeFrom(dtNewMat.Rows[i]);
					matAcjc.CopyColVal("EVENT_ID", dtNewMat.Rows[i]);
					matAcjc.CopyColVal("EVENT_DESC", dtNewMat.Rows[i]);
					matAcjc.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMAC_RESUME_SEQ_NO", 20, conn));
					matAcjc.SetColVal("EVENT_DATETIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
					matAcjc.SetColVal("REMARK", matAcjc.GetColValString("EVENT_ID") + "(" + matAcjc.GetColValString("EVENT_DESC") + ")");
					matAcjc.SetColVal("MAT_ACT_WT", -matAcjc.GetColValDecimal("MAT_ACT_WT"));
					matAcjc.SetColVal("MAT_THEORY_WT", -matAcjc.GetColValDecimal("MAT_THEORY_WT"));
					matAcjc.SetColVal("IN_OUT_DIV", "1");
					matAcjc.CopyColVal("SLAB_WT", "MAT_ACT_WT");
					matAcjc.CopyColVal("ACJC_RELATION_ID", "RESUME_SEQ_NO");
				}
				else
				{
					matAcjc.MergeFrom(dtNewMat.Rows[i]);
					matAcjc.CopyColVal("EVENT_ID", dtNewMat.Rows[i]);
					matAcjc.CopyColVal("EVENT_DESC", dtNewMat.Rows[i]);
					matAcjc.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMAC_RESUME_SEQ_NO", 20, conn));
					matAcjc.SetColVal("EVENT_DATETIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
					matAcjc.SetColVal("REMARK", matAcjc.GetColValString("EVENT_ID") + "(" + matAcjc.GetColValString("EVENT_DESC") + ")");
					matAcjc.SetColVal("IN_OUT_DIV", "2");
					matAcjc.CopyColVal("SLAB_WT", "MAT_ACT_WT");
					matAcjc.CopyColVal("ACJC_RELATION_ID", "RESUME_SEQ_NO");
				}

				if (matAcjc.Insert() < 0)
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


