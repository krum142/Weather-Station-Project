using Dapper.Contrib.Extensions;
using Dapper;
using Microsoft.AspNetCore.Mvc;
using WeatherStation.Context;
using WeatherStation.Entities;
using System.Data.SqlClient;

namespace WeatherStation.Controllers;

[ApiController]
[Route("[controller]")]
public class HomeController : Controller
{
    //private DapperContext dapper;
    //public HomeController(DapperContext dapper)
    //{
    //    this.dapper = dapper;
    //    dapper.CreateConnection();
    //}

    [HttpGet("/")]
    public async Task<IActionResult> Index()
    {
        using var db = new SqlConnection(Config.ConnectionString);
        db.Open();
        var result = await db.QueryAsync<WeatherData>("select top 5 * from WeatherData order by CreatedOn desc");
        return View(result);
    }

    [HttpPost("/Send")]
    public IActionResult Send(WeatherData data)
    {
        if(data.Id != Config.ReceiveWeatherDataKey)
        {
            return this.NotFound();
        }
        using var db = new SqlConnection(Config.ConnectionString);
        Console.WriteLine(data);



        db.Open();
        data.CreatedOn = DateTime.Now;
        db.Insert(data);
        return this.Ok("yes");
        //return this.Created("", "$123,15000$");
    }
}
