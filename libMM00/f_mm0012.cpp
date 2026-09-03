/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-07-24
Description: 计算材料重量
**************************************************/
//框架头文件
#include "stdafx.h" 
#include <math.h>

/*<remark>=========================================================
/// <summary>
/// 计算材料重量
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2_FUNCTION_EXPORT
int f_mm0012(CDecimal w_length,				/* Length		    (mm)  长度   */
				CDecimal w_width,					/* Width		    (mm)  宽度  */
				CDecimal w_thick,					/* Thickness	    (mm)  厚度 */
				CDecimal w_density,			    /* Material density (g/cm3)   密度 */
				CDecimal w_ctwg,					/* Coating weight   (g/m2) 涂层 */
				CDecimal& w_weight,				/* Weight 		理重    (t)    */
				CDbConnection * conn)
{
	int doFlag = 0;
	CDecimal w_dub1;
	CString sqlstr = " ";

	try
	{
		if (w_width <= 0 || w_thick <= 0 || w_density <= 0 || w_ctwg    < 0)
		{
			sprintf(s.msg, "输入参数：厚、宽、密度不能为0");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		w_dub1 = w_length * w_width * (w_thick * w_density + w_ctwg / 1000.0) / 1000 / 1000/1000;
		w_weight = w_dub1.Round(3);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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


