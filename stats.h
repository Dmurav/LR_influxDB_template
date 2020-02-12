#ifdef _STATS_
#define _STATS_


typedef long_t;
// ��������� ��� �������� ��������� �������
struct _timeb {
	time_t time;
	unsigned short millitm;
	short timezone;
	short dstflag;
}

//=======================��������������� �������======================================================================================================================

// �������� ��������� �����
long get_timestamp(){
	struct _timeb timestamp;
	return timestamp.time * 1000 + timestamp.millitm;
}

// �������� ��������� �� �� ...
char* substr(char *input, int from, int length){
	int i = 0;
	int l = length;
	char output[8];
	for (i=0; i < strlen(input); i++){
		if (i > (from - 2) && i < (from + length - 1){
		    	output[i - from + 1] = input[i];
		    }
	}
	return output;
}
// �������� ���������, ������� ����� ������
char* string_replace(char* capValue, char* replace, char* replacewith){
	char* pos;
	int offset;
	char output[1000];
	
	pos = (char*)strstr(capValue, replace);
	strcpy(output, "");
	while(pos!=0){
		offset = (int)(pos -capValue);
		strncat(output, capValue, offset);
		strcar(output, replacewith);
		capValue = (char*) (pos + strlen(replace));
		pos = (char*)strstr(capValue, replace);
	}
	strcat(output, capValue);
	return output;
}

//=======================������� ������ � ������ ���������� � INFLUXDB================================================================================================

// ��������� �������. ����� ������� ������ ���������� � ���������� ����� �������
start_transaction(char *name){
	lr_start_transaction(name);
	save_start_transaction_time(name);
	return 0;
}

// ���������� �������� ����������. ����� ������� ������ ���������� � ���� InfluxDB
end_transaction(char *name, int status){
	int realStatus = end_transaction_commons(name, status);
	if (realStatus == LR_FAIL){
		lr_exit(LR_EXIT_MAIN_ITERATION_AND_CONTINUE, LR_FAIL);
	}
	return realStatus;
}

// ���� ������������ ���������� ���������, �� ������������ ���������� ����������� �� �������� FAIL, � ����� �������� �������������
start_sub_transaction(char *subtransaction_name, char *parent_transaction_name){
	char parent_transaction_status = lr_get_transaction_status(parent_transaction_name);
	if (parent_transaction_status == LR_FAIL)
		end_transaction(parent_transaction_name, LR_FAIL);
	else
		start_transaction(sub_transaction_name);
	return 0;
}

// ���������� �������������. ����� ������� ������ ���������� � ���� InfluxDB
end_sub_transaction(char *name, int status){
	return end_transaction_commons(name, status);
}

// ���������� �������� ������ � ��������� LR. ������ ���������� ���������� � ���� InfluxDB
end_transactions_commons(char *name, int status){
	int real_status;
	// ���������� ������������� ����� ���������� � �������� LR - "STATS_TRANSACTION_NAME"
	stats_save_transaction_encoded_name_for_influxdb(name);
	// ���������� ������������ ����������� ������� � �������� LR - "STATS_WASTED_TIME"
	stats_save_transaction_wasted_time(name);
	// ���������� ������� think-time � �������� LR - "STATS_THINK_TIME"
	stats_save_transaction_think_time(name);
	// ���������� ������� ��������� � ���������� � �������� LR - "STATS_TRUNSACTION_STATUS"
	real_status = stats_save_transaction_status(name, status);
	// ���������� ����������
	lr_end_transaction(name, status);
	// ���������� ��������� ������� �� ��������� ���������� � �������� LR - "STATS_TRANSACTION_ENDED"
	stats_save_transaction_end_time(name);
	// ���������� ��������� ������� � ������ ���������� � �������� LR - "STATS_TRANSACTION_STARTED"
	stats_save_transaction_started_time(name);
	// ���������� ������������ ������ � �������� LR - "STATS_RESPONSE_TIME"
	stats_save_transaction_response_time(name);
	// ���������� ��������� ������� � �������� LR - "STATS_TIMESTAMP"
	stats_save_timestamp();
	
	// ������ � ���� ������ Influx ���������� LR, �� ������ ����
	web_custom_request("/write",
	                   "URL={InfluxHost}",
	                   "Method=POST",
	                   "Body={InfluxMeasurements},name={STATS_TRANSACTION_NAME},host={APP_SERVER},vusr={VuserID},status={STATS_TRUNSACTION_STATUS} response_time={STATS_RESPONSE_TIME}i,think_time={STATS_THINK_TIME}000i,wasted_time={STATS_WASTED_TIME}000i, {STATS_TIMESTAMP}000000",
	                   LAST);
	
	// �������� ����� ��������� ���������� �� run-time, ������������ ������
	stats_free_transaction_encoded_name_for_influxdb();
	stats_free_transaction_wasted_time();
	stats_free_transaction_status();
	stats_free_transaction_think_time();
	stats_free_transaction_end_time();
	stats_free_transaction_started_time();
	stats_free_transaction_response_time();
	// ������� ������� ����������
	return real_status;
}

//=============================������� ���������� ���������� LR ��� ������ � INFLUXDB=================================================================================

// ��������� ����� ������ ���������� �� � ����� � �������� LR
char* save_start_transaction_time(char* transaction_name){
	char startTimeVariableName[250];
	sprintf(startTimeVariableName, "start%s", transaction_name);
	lr_save_int(get_timestamp(), startTimeVariableName);
	return startTimeVariableName;
}

// ������������ �������� ���������� � UTF-8
int stats_save_transaction_encoded_name_for_influxdb(char* transaction_name){
	lr_convert_string_encoding(transaction_name, NULL, LR_ENC_UTF8, "STATS_TRANSACTION_NAME");
	lr_save_string(string_replace(lr_eval_string("{STATS_TRANSACTION_NAME}"), " ", "\x5C\x20"), "STATS_TRANSACTION_NAME");
	lr_save_string(string_replace(lr_eval_string("{STATS_TRANSACTION_NAME}"), ",", "\x5C,"), "STATS_TRANSACTION_NAME");
	return 0;
}

int stats_save_transaction_wasted_time(char* transaction_name){
	return lr_save_string(lr_get_transaction_wasted_time(transaction_name), "STATS_WASTED_TIME");
}

int stats_save_transaction_status(char* transaction_name, int status){
	int real_status;
	if (status == LR_AUTO){
		real_status = lr_get_transaction_status(transaction_name);
	}else{
		real_status = status;
	}
	lr_save_int(real_status, "STATS_TRUNSACTION_STATUS");
	return real_status;
}

int stats_save_transaction_think_time(char* transaction_name){
	lr_save_int(lr_get_transaction_think_time(transaction_name), "STATS_THINK_TIME");
	return 0;
}

int stats_save_transaction_started_time(char *transaction_name){
	char startVariableName[250];
	sprintf(startVariableName, "{start%s}", transaction_name);
	lr_save_int(atoi(lr_eval_string(startVariableName)), "STATS_TRANSACTION_STARTED");
	return 0;
}

int stats_save_transaction_end_time(char *transaction_name){
	lr_save_int(get_timestamp(), "STATS_TRANSACTION_ENDED");
	return 0;
}

int stats_save_transaction_response_time(char* transaction_name){
	int started;
	int ended;
	int response_time;
	started = atoi(lr_eval_string("{STATS_TRANSACTION_STARTED}"));
	ended = atoi(lr_eval_string("{STATS_TRANSACTION_ENDED}"));
	response_time = ended -started;
	lr_save_int(response_time, "STATS_RESPONSE_TIME");
}

int stats_save_timestamp(){
	return lr_save_timestamp("STATS_TIMESTAMP", "DIGITS=13", LAST);
}

//============================������� ��� �������� ���������� LR======================================================================================================

int free_start_transaction_time(char* transaction_name){
	char startTimeVariableName[250];
	sprintf(startTimeVariableName, "start%s", transaction_name);
	return lr_free_parameter(startTimeVariableName);
}

int stats_free_transaction_encoded_name_for_influxdb(){
	return lr_free_parameter("STATS_TRANSACTION_NAME");
}

int stats_free_transaction_wasted_time(){
	return lr_free_parameter("STATS_WASTED_TIME");
}

int stats_free_transaction_status(){
	return lr_free_parameter("STATS_TRANSACTION_STATUS");
}

int stats_free_transaction_think_time(){
	return lr_free_parameter("STATS_THINK_TIME");
}

int stats_free_transaction_started_time(){
	return lr_free_parameter("STATS_TRANSACTION_STARTED");
}

int stats_free_transaction_end_time(){
	return lr_free_parameter("STATS_TRANSACTION_ENDED");
}

int stats_free_transaction_response_time(){
	return lr_free_parameter("STATS_RESPONSE_TIME");
}

int stats_free_timestamp(){
	return lr_free_parameter("STATS_TIMESTAMP");
}


#endif _STATS_