using editor.Project;
using System;
using System.IO;
using System.Windows;

namespace editor
{
    public partial class App : Application
    {
        App()
        {
            var currentDir = Directory.GetCurrentDirectory();
            Environment.SetEnvironmentVariable("FABRIC_PATH", currentDir, EnvironmentVariableTarget.User);

            Startup += App_Startup;
        }

        private void App_Startup(object sender, StartupEventArgs e)
        {
            Startup -= App_Startup;

            var startupWindow = new ProjectBrowserWindow();

            startupWindow.Show();
        }
    }
}
