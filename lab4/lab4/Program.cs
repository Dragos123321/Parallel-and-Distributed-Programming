namespace lab4
{
    public static class Program
    {
        private static readonly List<string> urls = new List<string> {
            "www.cs.ubbcluj.ro/~rlupsa/edu/pdp",
            "google.com",
            "captive.apple.com"
        };

        public static void Main(string[] args)
        {
            Console.WriteLine("Menu:");
            Console.WriteLine("x - exit");
            Console.WriteLine("1 - run callback solution");
            Console.WriteLine("2 - run tasks solution");
            Console.WriteLine("3 - run async/await solution\n");

            bool done = false;

            while (!done)
            {
                Console.Write("Input command: ");
                var command = Console.ReadLine();

                switch(command)
                {
                    case "x":
                        done = true;
                        Console.WriteLine("Exiting...");
                        break;
                    case "1":
                        CallbacksSolution.run(urls);
                        break;
                    case "2":
                        TasksSolution.run(urls);
                        break;
                    case "3":
                        AsyncSolution.run(urls);
                        break;
                    default:
                        Console.WriteLine("Wrong input.");
                        break;
                }
            }
        }
    }
}