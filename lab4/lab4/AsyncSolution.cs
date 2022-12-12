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

        private static void start(object objId)
        {
            int id = (int)objId;

            StartClientAsync(hostnames[id], id).Wait();
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
            var taskCompletion = new TaskCompletionSource<bool>();

            state.clientSocket!.BeginConnect(state.remoteEndpoint!, (IAsyncResult ar) =>
            {
                state.clientSocket!.EndConnect(ar);
                state.LogConnect();
                taskCompletion.TrySetResult(true);

            }, state);

            await taskCompletion.Task;
        }

        private static async Task SendWrapper(State state, string data)
        {
            var taskCompletion = new TaskCompletionSource<int>();

            var byteData = Encoding.ASCII.GetBytes(data);

            state.clientSocket!.BeginSend(byteData, 0, byteData.Length, 0, (IAsyncResult ar) =>
            {
                var bytesSent = state.clientSocket!.EndSend(ar);
                state.LogSend(bytesSent);
                taskCompletion.TrySetResult(bytesSent);

            }, state);

            await taskCompletion.Task;
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
