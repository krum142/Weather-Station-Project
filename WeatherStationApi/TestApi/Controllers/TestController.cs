using Microsoft.AspNetCore.Mvc;

namespace TestApi.Controllers
{
    public class TestController : ControllerBase
    {
        public IActionResult Index()
        {
            return this.Ok("test ok message");
        }
        [HttpGet("test")]
        public string Test(string ID,string Name)
        {
            Console.WriteLine(ID + "-" + Name);
            return "test";
        }
    }
}
