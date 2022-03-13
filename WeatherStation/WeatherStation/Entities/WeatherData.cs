using Dapper.Contrib.Extensions;

namespace WeatherStation.Entities
{
    [Table("WeatherData")]
    public record WeatherData
    {
        [Computed]
        public int Id { get; set; }
        public string? WindDirection { get; set; }
        public double WindSpeed { get; set; }
        public double Humidity { get; set; }
        public double TemperatureS1 { get; set; }
        public double TemperatureS2 { get; set; }
        public double Pressure { get; set; }
        public DateTime? CreatedOn { get; set; }
    }
}
