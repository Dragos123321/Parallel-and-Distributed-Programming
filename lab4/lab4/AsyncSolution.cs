using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace lab4
{
    internal class AsyncSolution
    {
        private static List<string> hostnames;

        public static void run(List<string> urls)
        {
            hostnames = urls;

            var tasks = new List<Task>();

            for (int i = 0; i < hostnames.Count; i++)
            {
                tasks.Add(Task.Factory.StartNew(start, i));
            }

            Task.WaitAll(tasks.ToArray());
        }

        private static async void start(object objId)
        {
            int id = (int)objId;

            StartClientAsync(hostnames[id], id);
        }

        private static async Task StartClientAsync(string host, int id)
        {
            var state = State.getNewState(host, id);

            await ConnectWrapper(state);

            await SendWrapper(state, HttpHelper.getRequestString(state.serverHostname!, state.endpoint!));

            await ReceiveWrapper(state);

            state.LogReceive();
            state.clientSocket!.Shutdown(SocketShutdown.Both);
            state.clientSocket.Close();
        }

        private static async Task ConnectWrapper(State state)
        {
            state.clientSocket!.BeginConnect(state.remoteEndpoint!, ConnectCallback, state);

            await Task.FromResult(state.connectDone.WaitOne());
        }

        private static void ConnectCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState!;

            state.clientSocket!.EndConnect(ar);

            state.LogConnect();
            state.connectDone.Set();
        }

        private static async Task SendWrapper(State state, string data)
        {
            var byteData = Encoding.ASCII.GetBytes(data);

            state.clientSocket!.BeginSend(byteData, 0, byteData.Length, 0, SendCallback, state);

            await Task.FromResult(state.sendDone.WaitOne());
        }

        private static void SendCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState!;

            var bytesSent = state.clientSocket!.EndSend(ar);

            state.LogSend(bytesSent);
            state.sendDone.Set();
        }

        private static async Task ReceiveWrapper(State state)
        {
            state.clientSocket!.BeginReceive(state.recvBuffer, 0, State.BUFFER_SIZE, 0, ReceiveCallback, state);

            await Task.FromResult(state.receiveDone.WaitOne());
        }

        public static void ReceiveCallback(IAsyncResult ar)
        {
            var state = (State)ar.AsyncState!;
            var client = state.clientSocket;

            try
            {
                var bytesRead = client!.EndReceive(ar);

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
