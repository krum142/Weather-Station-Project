using WeatherStation.Entities;

var builder = WebApplication.CreateBuilder(args);


Config.ConnectionString = builder.Configuration.GetConnectionString("SqlConnection");
Config.ReceiveWeatherDataKey = int.Parse(builder.Configuration["ReceiveWeatherDataKey"]);

// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();
builder.Services.AddControllersWithViews();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

//app.UseHttpsRedirection();

app.UseAuthorization();

app.MapControllers();

app.Run();
