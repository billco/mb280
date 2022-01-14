# mb280
Modbus tcp slave server for BME280 on PI-4b


Support for 4 BME280 sensors use adr 76,76 both I2C busses

Requirements 
	libmodbus	5.1.0 for slave & clent
	
	libwiringpi	 for slave and rasberry PI(writen on PI-4b)

target systems:

    slave:    PI
	
		make
		
    client: gcc
	
        make client
