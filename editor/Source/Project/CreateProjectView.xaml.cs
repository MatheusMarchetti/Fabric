using editor.Editors;
using Microsoft.Win32;
using System.Windows;
using System.Windows.Controls;

namespace editor.Project
{
    public partial class CreateProjectView : UserControl
    {
        public CreateProjectView()
        {
            InitializeComponent();
        }

        private void browseButton_Click(object sender, RoutedEventArgs e)
        {
            var dc = DataContext as CreateProject;
            var dlg = new OpenFolderDialog();
            if (dlg.ShowDialog() == true)
            {
                dc.ProjectPath = dlg.FolderName;
            }
        }

        private void createButton_Click(object sender, RoutedEventArgs e)
        {
            var dc = DataContext as CreateProject;
            var win = Window.GetWindow(this);

            var projectPath = dc.Create((ProjectTemplate)templateListBox.SelectedItem);

            if (!string.IsNullOrEmpty(projectPath))
            {
                var project = OpenProject.Open(new ProjectData { ProjectName = dc.ProjectName, ProjectPath = projectPath } );

                if (project != null)
                {
                    var mainWindow = Application.Current.MainWindow;
                    mainWindow = new MainWindow();
                    mainWindow.DataContext = project;

                    mainWindow.Show();
                }
            }

            win.Close();
        }
    }
}
