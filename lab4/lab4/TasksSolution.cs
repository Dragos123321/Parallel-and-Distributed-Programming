using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace lab4
{
    internal class TasksSolution
    {
        private static List<string> hosts;

        public static void run(List<string> urls)
        {
            hosts = urls;

            var tasks = new List<Task>();

            for (int i = 0; i < hosts.Count; i++)
            {
                tasks.Add(Task.Factory.StartNew(start, i));
            }

            Task.WaitAll(tasks.ToArray());
        }

        public static void start(object objId)
        {
            int id = (int)objId;

            StartClient(hosts[id], id);
        }

        public static void StartClient(string host, int id)
        {
            var state = State.getNewState(host, id);

            ConnectWrapper(state).Wait();

            SendWrapper(state, HttpHelper.getRequestString(state.serverHostname, state.endpoint)).Wait();

            ReceiveWrapper(state).Wait();

            state.LogReceive();

            state.clientSocket.Shutdown(SocketShutdown.Both);
            state.clientSocket.Close();
        }

        private static Task ConnectWrapper(State state)
        {
            state.clientSocket.BeginConnect(state.remoteEndpoint, ConnectCallback, state);

            return Task.FromResult(state.connectDone.WaitOne());
        }

        private static void ConnectCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;

            state.clientSocket.EndConnect(ar);
            state.LogConnect();
            state.connectDone.Set();
        }

        private static Task SendWrapper(State state, string data)
        {
            var byteData = Encoding.ASCII.GetBytes(data);

            state.clientSocket.BeginSend(byteData, 0, byteData.Length, 0, SendCallback, state);

            return Task.FromResult(state.sendDone.WaitOne());
        }

        private static void SendCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;
            var id = state.clientId;
            var bytesSent = state.clientSocket.EndSend(ar);

            state.LogSend(bytesSent);
            state.sendDone.Set();
        }

        private static Task ReceiveWrapper(State state)
        {
            state.clientSocket.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, ReceiveCallback, state);

            return Task.FromResult(state.receiveDone.WaitOne());
        }

        public static void ReceiveCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState;
            var client = state.clientSocket;

            try
            {
                var bytesRead = client.EndReceive(ar);

                state.responseContent.Append(Encoding.ASCII.GetString(state.recvBuffer, 0, bytesRead));

                if (!HttpHelper.checkHeaderObtained(state.responseContent.ToString()))
                {
                    client.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, ReceiveCallback, state);
                }
                else
                {
                    var responseBody = HttpHelper.getResponseBody(state.responseContent.ToString());

                    var headerContentLength = HttpHelper.getContentLength(state.responseContent.ToString());

                    if (responseBody.Length < headerContentLength)
                    {
                        client.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, ReceiveCallback, state);
                    }
                    else
                    {
                        state.receiveDone.Set();
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
