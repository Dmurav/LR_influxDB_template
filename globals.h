#ifndef _GLOBALS_H
#define _GLOBALS_H

//--------------------------------------------------------------------
// Include Files
#include "lrun.h"
#include "web_api.h"
#include "lrw_custom_body.h"
#include "stats.h"

//--------------------------------------------------------------------
// Global Variables

// Извлечение ключевых параметров из ответа для проверки статуса транзакции
get_response(){
	web_reg_save_param_json(
				"ParamName=Success_status",
				"QueryString=$.success",
				"NotFound=warning",
				SEARCH_FILTERS,
				"Scope=Body",
				"LAST");
	web_reg_save_param_json(
				"ParamName=Error_text",
				"QueryString=$.error",
				"NotFound=warning",
				SEARCH_FILTERS,
				"Scope=Body",
				"LAST");	
	web_reg_save_param_json(
				"ParamName=Errors",
				"QueryString=$.errors",
				"NotFound=warning",
				SEARCH_FILTERS,
				"Scope=Body",
				"LAST");	
	web_reg_save_param_ex(
				"ParamName=Response",
				"LB/IC=",
				"RB/IC=",
				SEARCH_FILTERS,
				"Scope=Body",
				LAST);
	web_reg_save_param_ex(
				"ParamName=ResponseContentType",
				"LB/IC=Content-Type",
				"RB/IC=;",
				SEARCH_FILTERS,
				"Scope=headers",
				LAST);
}

// Проверка полученных параметров и возврат статуса транзакции - 1 или 0;
check_response(int status){
	int status_errors;
	if(strcmp((lr_eval_string("{ResponseContentType}")),lr_eval_string("application/json"))==0){
		lr_eval_json("Buffer={Response}","JsonObject=json_obj_response");
		status_errors = lr_json_get_values("JsonObject=json_obj_response",
		                                  "ValueParam=Status_errors",
		                                 "QueryString=$.status.errors",
		                                "NotFound=Continue", LAST);
	}
	if(strcmp(lr_eval_string("{Saccess_status}")),lr_eval_string("false"))==0){
		lr_error_message("Response Success status: False!\r\n\Error message: %s",lr_eval_string("{Error_text}"));
		status=1;
	}
	if(status_errors == 1){
		lr_error_message("We get Error status!\r\n\Error message: %s",lr_eval_string("{Status_errors}"));
		status=1;
	}
	if(strcmp(lr_eval_string("{Errors}")),lr_eval_string(""))==1){
		lr_error_message("We get Error status!\r\n\Error message: %s",lr_eval_string("{Errors}"));
		status=1;
	}
	return status;
}


#endif // _GLOBALS_H
