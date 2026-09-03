/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-07-24 09:19:02
Description: 物料跟踪初始数据校验
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_mm0099_00(CDynaTable * matData, CString eventId)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	try
	{
		if (matData->GetColValString("MAT_KIND") == "CR")
		{
			if (eventId == "MM02")
			{
				if (matData->GetColValString("MAT_NO").Trim() == "")
				{
					strcpy(s.msg, "材料号不能为空!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("PONO").Trim() == "")
				{
					strcpy(s.msg, "制造命令号不能为空!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("MEASURE_WT_FLAG").Trim() == "")
				{
					strcpy(s.msg, "称重标记不能为空!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (eventId == "MM03")
			{
				//物料修改
				if (matData->GetColValString("HOLD_FLAG") == "0")
				{
					sprintf(s.msg, "材料[%s]没有封锁,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("MAT_STATUS") == "24" ||
					matData->GetColValString("MAT_STATUS") == "34" ||
					matData->GetColValString("MAT_STATUS") == "36" ||
					matData->GetColValString("MAT_STATUS") == "38")
				{
					sprintf(s.msg, "材料[%s]编入计划,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("REPAIR_FLAG") == "1")
				{
					sprintf(s.msg, "材料[%s]目前正在返修中,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("TRANSFER_FLAG") == "1")
				{
					sprintf(s.msg, "材料[%s]已经编入转库计划中,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("RETURN_MAT_FLAG") == "1")
				{
					sprintf(s.msg, "材料[%s]是返品,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("PRODUCT_FLAG") == "1")
				{
					if (matData->GetColValString("CONFM_FLAG") == "1")
					{
						sprintf(s.msg, "材料[%s]已在准发计划中,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					if (matData->GetColValString("COMPLEX_DECIDE_CODE").Trim() != "" &&
						matData->GetColValString("COMPLEX_DECIDE_CODE") != "0")
					{
						sprintf(s.msg, "材料[%s]已综判,不允许修改!", (const char*)matData->GetColValString("MAT_NO").Trim());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
			else if (eventId == "MM04")
			{
				//物料删除
				if (matData->GetColValString("HOLD_FLAG") != "2")
				{
					sprintf(s.msg, "材料[%s]没有质量封锁,不允许删除!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("REPAIR_FLAG") == "1")
				{
					sprintf(s.msg, "材料[%s]目前正在返修中,不允许删除!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("TRANSFER_FLAG") == "1")
				{
					sprintf(s.msg, "材料[%s]已经编入转库计划中,不允许删除!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("ORDER_NO").Trim() != "")
				{
					sprintf(s.msg, "材料[%s]不是余材,不允许删除!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (eventId == "MM07")
			{
				//物料管理封锁
				if (matData->GetColValString("PLAN_NO").Trim() != "")
				{
					sprintf(s.msg, "材料号[%s]在作业计划[%s]中,不能管理封锁!", (const char*)matData->GetColValString("MAT_NO"),
						(const char*)matData->GetColValString("PLAN_NO"));
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (matData->GetColValString("CONFM_FLAG") != "0")
				{
					sprintf(s.msg, "材料号[%s]材料状态[%s]是准发,不能管理封锁!", (const char*)matData->GetColValString("MAT_NO"),
						(const char*)matData->GetColValString("MAT_STATUS"));
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (matData->GetColValString("TRANSFER_FLAG") != "0")
				{
					sprintf(s.msg, "材料号[%s]在转库计划中,转库状态是[%s],不能管理封锁!", (const char*)matData->GetColValString("MAT_NO"),
						(const char*)matData->GetColValString("TRANSFER_FLAG"));
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (matData->GetColValString("APP_DECIDE_FLAG") != "0")
				{
					sprintf(s.msg, "材料号[%s]在现货申报计划中,现货申报标记是[%s],不能管理封锁!", (const char*)matData->GetColValString("MAT_NO"),
						(const char*)matData->GetColValString("APP_DECIDE_FLAG"));
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (eventId == "MM08")
			{
				//物料管理释放
				if (matData->GetColValString("HOLD_FLAG") != "1" && matData->GetColValString("HOLD_FLAG") != "3")
				{
					sprintf(s.msg, "材料号[%s]的封锁标记是[%s]中,未被管理封锁，不能进行释放!", (const char*)matData->GetColValString("MAT_NO"),
						(const char*)matData->GetColValString("HOLD_FLAG"));
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (eventId == "MM21")
			{
				//物料在制品转成品
				if (matData->GetColValString("PRODUCT_FLAG") != "0")
				{
					sprintf(s.msg, "材料[%s]不是在制品!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("ORDER_NO").Trim() != "")
				{
					sprintf(s.msg, "材料[%s]不是余材,不允许在制品转成品!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (eventId == "MM22")
			{
				//物料成品转在制品
				if (matData->GetColValString("PRODUCT_FLAG") != "1")
				{
					sprintf(s.msg, "材料[%s]不是成品!", (const char*)matData->GetColValString("MAT_NO"));
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (matData->GetColValString("ORDER_NO").Trim() != "")
				{
					sprintf(s.msg, "材料[%s]不是余材,不允许成品转在制品!", (const char*)matData->GetColValString("MAT_NO"));
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


