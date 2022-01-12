# mb280
Modbus tcp slave server for BME280 on PI-4b


Support for 4 BME280 sensors use adr 76,76 both I2C busses

uses 
	libmodbus	5.1.0
	
	libwiringpi

target:
	slave:	PI4b
	
		make
	client: gcc
	
		make client
