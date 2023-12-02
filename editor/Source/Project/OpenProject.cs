using editor.Core;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;

namespace editor.Project
{
    [DataContract]
    public class ProjectData
    {
        [DataMember]
        public string ProjectName { get; set; }

        [DataMember]
        public string ProjectPath { get; set; }

        [DataMember]
        public DateTime LastModifiedDate { get; set; }

        public string FullPath { get => $"{ProjectPath}{ProjectName}.fabric"; }
        public byte[] Icon { get; set; }
        public byte[] Screenshot { get; set; }
    }

    [DataContract]
    public class ProjectDataList
    {
        [DataMember]
        public List<ProjectData> ExistingProjects { get; set; }
    }

    class OpenProject : ObservableObject
    {
        private readonly static string _appDataPath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}\Loom\";

        private readonly static string _projectDataPath = $"{_appDataPath}ProjectData.json";

        private static readonly ObservableCollection<ProjectData> _projects = new ObservableCollection<ProjectData>();

        public ReadOnlyObservableCollection<ProjectData> Projects { get; }

        public bool HasProjects { get => _projects.Any(); }

        public OpenProject()
        {
            try
            {
                if(!Directory.Exists(_appDataPath))
                    Directory.CreateDirectory(_appDataPath);

                Projects = new ReadOnlyObservableCollection<ProjectData>(_projects);

                ReadProjectData();
            }
            catch(Exception ex)
            {
                Debug.WriteLine(ex);
                throw;
            }
        }

        private static void ReadProjectData()
        {
            if(File.Exists(_projectDataPath))
            {
                var projects = Serializer.Deserialize<ProjectDataList>(_projectDataPath).ExistingProjects.OrderByDescending(x => x.LastModifiedDate);

                _projects.Clear();

                foreach(var project in projects)
                {
                    if(File.Exists(project.FullPath))
                    {
                        project.Icon = File.ReadAllBytes($@"{project.ProjectPath}\.fabric\icon.png");
                        project.Screenshot = File.ReadAllBytes($@"{project.ProjectPath}\.fabric\screenshot.png");

                        _projects.Add(project);
                    }
                }
            }
        }

        private static void WriteProjectData()
        {
            var projects = _projects.OrderBy(x=>x.LastModifiedDate).ToList();

            Serializer.Serialize(new ProjectDataList { ExistingProjects = projects }, _projectDataPath);
        }

        public static GameProject Open(ProjectData data)
        {
            ReadProjectData();

            var project = _projects.FirstOrDefault(x=> x.FullPath == data.FullPath);

            if(project != null)
            {
                project.LastModifiedDate = DateTime.Now;
            }
            else
            {
                project = data;
                project.LastModifiedDate = DateTime.Now;
                _projects.Add(project);
            }

            WriteProjectData();

            return GameProject.Load(project.FullPath);
        }
    }
}
