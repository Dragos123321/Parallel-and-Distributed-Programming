using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace lab4
{
   internal class HttpHelper
   {
        public static readonly int HTTP_PORT = 80;

        public static bool checkHeaderObtained(string response)
        {
            return response.Contains("\r\n\r\n");
        }

        public static string getResponseBody(string response)
        {
            var responseSplit = response.Split(new[] { "\r\n\r\n" }, StringSplitOptions.RemoveEmptyEntries);

            return responseSplit.Length > 1 ? responseSplit[1] : "";
        }

        public static int getContentLength(string response)
        {
            var length = 0;
            var lines = response.Split('\r', '\n');

            foreach (var line in lines)
            {
                var headerDetails = line.Split(':');
                
                if (headerDetails[0].CompareTo("Content-Length") == 0)
                {
                    length = int.Parse(headerDetails[1]);
                }
            }

            return length;
        }

        public static string getRequestString(string hostname, string endpoint)
        {
            return "GET " + endpoint + " HTTP/1.1\r\n" +
                   "Host: " + hostname + "\r\n" +
                   "Content-Length: 0\r\n\r\n";
        }
    }
}
