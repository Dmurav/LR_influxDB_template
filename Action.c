Action()
{
	// ����� ������� ���������� ���������� �� ������������� ������
	get_response();
	// ����� �������� ����������. ����� ������� ������ ����������
	start_transaction("Main_transaction");

	UC_01();
	
	// ���������� �������� ����������. ������ ���������� ���������� � InfluxDB
	end_transaction("Main_transaction");
	
	return 0;
}

UC_01() 
{
	
	//======================������ �������������========================
	// ����� ��������� ����������. ����� ������� ������ ����������
	start_sub_transaction("Main_transaction_1", "Main_transaction");
	// ����� ������� ���������� ���������� �� ������������� ������
	get_response();
	// �������� �������� ���������� ����������
	status = web_custom_request();
	// ����� ������� ��� �������� ������� ��������
	status = check_response(status);
	// ���������� �������������. ������ ���������� ���������� � InfluxDB
	end_subtrunsaction("Main_transaction_1", status);
	
	//======================������ �������������=========================
	// ����� ��������� ����������. ����� ������� ������ ����������
	start_sub_transaction("Main_transaction_2", "Main_transaction");
	// ����� ������� ���������� ���������� �� ������������� ������
	get_response();
	// �������� �������� ���������� ����������
	status = web_custom_request();
	// ����� ������� ��� �������� ������� ��������
	status = check_response(status);
	// ���������� �������������. ������ ���������� ���������� � InfluxDB
	end_subtrunsaction("Main_transaction_2", status);
	
	return 0;
}