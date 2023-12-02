using editor.Core;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;

namespace editor.Project
{
    // Project file structure (v. 0.0.1):
    // Project file version
    // Project name
    // Project path
    // Folders to observe

    [DataContract]
    class GameProject : ObservableObject
    {
        [DataMember(Name = "Version", Order = 0)]
        private readonly string _version = "0.0.1";

        private string _name;

        [DataMember(Name = "Project Path", Order = 2)]
        private string _projectPath;

        private bool _needsToSave = false;

        [DataMember(Name = "Project Name", Order = 1)]
        public string Name
        {
            get => _name;
            set
            {
                if(_name != value)
                {
                    _name = value;
                    OnPropertyChanged();
                }
            }
        }

        [DataMember(Order = 3)]
        public List<string> Folders { get; set; }

        public string ProjectPath
        {
            get => _projectPath;
            set
                {
                    if(value != _projectPath)
                        _projectPath = value;
                }
        }

        public GameProject()
        {
            _name = "Invalid project";
            _projectPath = "Invalid project";
            Folders = new List<string>();
        }

        public GameProject(string name, string path) 
        {
            _name = name;
            _projectPath = Path.Combine(path, name);
            Folders = new List<string>();
        }

        public void AddFolder(string name)
        {
            var folderName = Path.Combine(_projectPath, name);

            if(!Directory.Exists(folderName))
            {
                Directory.CreateDirectory(folderName);
            }

            Folders.Add(name);
            _needsToSave = true;
        }

        public void Save()
        {
            Serializer.Serialize(this, Path.Combine(_projectPath, string.Concat(_name, ".fabric")));
            _needsToSave = false;
        }

        public static GameProject Load(string file)
        {
            return Serializer.Deserialize<GameProject>(file);
        }
    }
}
