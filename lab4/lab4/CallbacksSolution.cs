using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace lab4
{
    internal class CallbacksSolution
    {
        private static List<string> hosts;

        public static void run(List<string> urls)
        {
            hosts = urls;

            for (int i = 0; i < hosts.Count; i++)
            {
                start(i);
                Thread.Sleep(1000);
            }
        }

        public static void start(int id)
        {
            StartClient(hosts[id], id);
        }

        public static void StartClient(string host, int id)
        {
            var state = State.getNewState(host, id);

            state.clientSocket.BeginConnect(state.remoteEndpoint, Connect, state);
        }

        public static void Connect(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;

            state.clientSocket.EndConnect(ar);
            state.LogConnect();

            var byteData = Encoding.ASCII.GetBytes(HttpHelper.getRequestString(state.serverHostname, state.endpoint));

            state.clientSocket.BeginSend(byteData, 0, byteData.Length, 0, Send, state);
        }

        public static void Send(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;

            var bytesSent = state.clientSocket.EndSend(ar);
            state.LogSend(bytesSent);

            state.clientSocket.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, Receive, state);
        }

        public static void Receive(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;
            var client = state.clientSocket;

            try
            {
                var bytesRead = client.EndReceive(ar);

                state.responseContent.Append(Encoding.ASCII.GetString(state.recvBuffer, 0, bytesRead));

                if (!HttpHelper.checkHeaderObtained(state.responseContent.ToString()))
                {
                    client.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, Receive, state);
                }
                else
                {
                    var responseBody = HttpHelper.getResponseBody(state.responseContent.ToString());

                    var headerContentLength = HttpHelper.getContentLength(state.responseContent.ToString());

                    if (responseBody.Length < headerContentLength)
                    {
                        client.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, Receive, state);
                    }
                    else
                    {
                        state.LogReceive();
                        client.Shutdown(SocketShutdown.Both);
                        client.Close();
                    }
                }
            } 
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }
    }
}
