namespace WeatherStation.Entities
{
    public static class Config
    {
        public static string ConnectionString { get; set; } = string.Empty;
        public static int ReceiveWeatherDataKey { get; set; }
    }
}
