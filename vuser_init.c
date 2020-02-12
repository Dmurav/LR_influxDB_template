vuser_init()
{
	char host1[64];
	
	// Значения удобно редактировать из PC (Runtime Settings-> Additional Atributes)
	lr_save_string(lr_get_attrib_string("InfluxHost"), "InfluxHost");
	lr_save_string(lr_get_attrib_string("Telegraf"), "InfluxHost");
	lr_save_string(lr_get_attrib_string("InfluxMeasurements"), "InfluxMeasurements");
	=
	return 0;
}
