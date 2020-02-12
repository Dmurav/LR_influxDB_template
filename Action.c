Action()
{
	// Вызов функции извлечения параметров из возвращаемого ответа
	get_response();
	// Старт основной транзакции. Замер времени старта транзакции
	start_transaction("Main_transaction");

	UC_01();
	
	// Завершение основной транзакции. Запись параметров транзакции в InfluxDB
	end_transaction("Main_transaction");
	
	return 0;
}

UC_01() 
{
	
	//======================Первая подтранзакция========================
	// Старт вложенной транзакции. Замер времени старта транзакции
	start_sub_transaction("Main_transaction_1", "Main_transaction");
	// Вызов функции извлечения параметров из возвращаемого ответа
	get_response();
	// Основное действие замеряемой транзакции
	status = web_custom_request();
	// Вызов функции для проверки статуса операции
	status = check_response(status);
	// Завершение подтранзакции. Запись параметров транзакции в InfluxDB
	end_subtrunsaction("Main_transaction_1", status);
	
	//======================Вторая подтранзакция=========================
	// Старт вложенной транзакции. Замер времени старта транзакции
	start_sub_transaction("Main_transaction_2", "Main_transaction");
	// Вызов функции извлечения параметров из возвращаемого ответа
	get_response();
	// Основное действие замеряемой транзакции
	status = web_custom_request();
	// Вызов функции для проверки статуса операции
	status = check_response(status);
	// Завершение подтранзакции. Запись параметров транзакции в InfluxDB
	end_subtrunsaction("Main_transaction_2", status);
	
	return 0;
}