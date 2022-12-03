using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

namespace lab4
{
    public class State
    {
        public const int BUFFER_SIZE = 512;
        public Socket? clientSocket = null;
        public byte[] recvBuffer = new byte[BUFFER_SIZE];
        public StringBuilder responseContent = new StringBuilder();
        public int clientId;
        public string? serverHostname = null;
        public string? endpoint = null;
        public IPEndPoint? remoteEndpoint = null;
        public ManualResetEvent connectDone = new ManualResetEvent(false);
        public ManualResetEvent sendDone = new ManualResetEvent(false);
        public ManualResetEvent receiveDone = new ManualResetEvent(false);

        public static State getNewState(string host, int id)
        {
            var ipHostInfo = Dns.GetHostEntry(host.Split('/')[0]);
            var ipAddress = ipHostInfo.AddressList[0];
            var endpoint = new IPEndPoint(ipAddress, HttpHelper.HTTP_PORT);

            var client = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            return new State
            {
                clientSocket = client,
                serverHostname = host.Split('/')[0],
                endpoint = host.Contains('/') ? host.Substring(host.IndexOf('/')) : "/",
                remoteEndpoint = endpoint,
                clientId = id
            };
        }

        public void LogConnect()
        {
            Console.WriteLine("{0}: Socket connected to {1}.", this.clientId, this.serverHostname);
        }

        public void LogSend(int bytesSent)
        {
            Console.WriteLine("{0}: Sent {1} bytes to server.", this.clientId, bytesSent);
        }

        public void LogReceive()
        {
            Console.WriteLine(
                "{0}: Received {1} bytes.",
                this.clientId, this.responseContent.Length);
        }
    }
}
