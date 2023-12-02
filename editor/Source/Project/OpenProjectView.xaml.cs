using editor.Editors;
using System.Windows;
using System.Windows.Controls;

namespace editor.Project
{
    public partial class OpenProjectView : UserControl
    {
        public OpenProjectView()
        {
            InitializeComponent();
        }

        private void openButton_Click(object sender, RoutedEventArgs e)
        {
            var win = Window.GetWindow(this);

            var project = OpenProject.Open((ProjectData)projectsListBox.SelectedItem);

            if (project != null)
            {
                var mainWindow = Application.Current.MainWindow;
                mainWindow = new MainWindow();
                mainWindow.DataContext = project;

                mainWindow.Show();
            }

            win.Close();
        }
    }
}
