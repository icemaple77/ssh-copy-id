using System;
using System.Diagnostics;
using System.IO;

class Program
{
    static void Main(string[] args)
    {
        string userAtMachine = args[0];
        string sshArgs = string.Join(" ", args, 1, args.Length - 1);

        string publicKey = Environment.GetEnvironmentVariable("USERPROFILE") + "/.ssh/id_rsa.pub";
        if (!File.Exists(publicKey))
        {
            Console.WriteLine("ERROR: failed to open ID file '{0}': No such file", publicKey);
            return;
        }

        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = "ssh",
            Arguments = string.Format("{0} \"umask 077; test -d .ssh || mkdir .ssh ; cat >> .ssh/authorized_keys || exit 1\"", sshArgs),
            RedirectStandardInput = true
        };

        using (Process process = Process.Start(startInfo))
        {
            using (StreamWriter writer = process.StandardInput)
            {
                using (StreamReader reader = new StreamReader(publicKey))
                {
                    writer.Write(reader.ReadToEnd());
                }
            }

            process.WaitForExit();

            if (process.ExitCode == 0)
            {
                Console.WriteLine("Successfully copied public key to {0}", userAtMachine);
            }
            else
            {
                Console.WriteLine("Failed to copy public key");
            }
        }
    }
}
