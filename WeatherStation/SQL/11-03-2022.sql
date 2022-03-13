Create Database WeatherStation
--WDirection=918 - 755, WSpeed=0.00, SHumidityAndTemp=19.00 - 27.70, SPressureAndTemp=102847 - 26.80
Create Table WeatherData(
	Id int primary key identity,
	CreatedOn datetime default(getdate()),
	WindDirection varchar(255),
	WindSpeed float,
	Humidity float,
	TemperatureS1 float,
	TemperatureS2 float,
	Pressure float
)