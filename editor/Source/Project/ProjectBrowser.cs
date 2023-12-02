using editor.Core;
using System.Windows.Controls;

namespace editor.Project
{
    class ProjectBrowser : ObservableObject
    {
        private UserControl _currentView;

        public RelayCommand<ProjectBrowserWindow> CreateProjectViewCommand { get; set; }
        public RelayCommand<ProjectBrowserWindow> OpenProjectViewCommand { get; set; }

        public CreateProject CreateProjectVM { get; set; }
        public OpenProject OpenProjectVM { get; set; }

        public UserControl CurrentView
        {
            get => _currentView;
            set
            {
                if(_currentView != value)
                {
                    _currentView = value;
                    OnPropertyChanged();
                }
            }
        }

        public ProjectBrowser()
        {
            CreateProjectVM = new CreateProject();
            OpenProjectVM = new OpenProject();

            CreateProjectViewCommand = new RelayCommand<ProjectBrowserWindow>(o =>
            {
                CurrentView = new CreateProjectView();
                CurrentView.DataContext = CreateProjectVM;
            });

            OpenProjectViewCommand = new RelayCommand<ProjectBrowserWindow>(o =>
            {
                CurrentView = new OpenProjectView();
                CurrentView.DataContext = OpenProjectVM;
            });
        }
    }
}
