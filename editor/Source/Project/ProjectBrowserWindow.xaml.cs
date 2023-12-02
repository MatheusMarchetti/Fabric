using System.Windows;

namespace editor.Project
{
    public partial class ProjectBrowserWindow : Window
    {
        public ProjectBrowserWindow()
        {
            InitializeComponent();
        }

        private void radioButton_Check(object sender, RoutedEventArgs e)
        {
            var dc = DataContext as ProjectBrowser;

            if (dc != null)
            {
                if (sender == createProjectButton)
                    dc.CreateProjectViewCommand.Execute(this);

                if(sender == openProjectButton)
                    dc.OpenProjectViewCommand.Execute(this);
            }
        }
    }
}
